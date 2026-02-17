#include <core/job_system/jobsystem.h>

#include <algorithm>
#include <cassert>
#include <chrono>

namespace vde::core::job_system
{
	// -------------------------------------------------------------------
	// WorkerMain
	// -------------------------------------------------------------------
	// Chaque worker thread execute cette fonction en boucle. Il bloque
	// sur la ready-queue via WaitAndPop (condition_variable) jusqu'a ce
	// qu'un job soit disponible ou que le shutdown soit demande.
	// -------------------------------------------------------------------
	void JobSystem::WorkerMain(uint32_t workerIndex)
	{
		while (true)
		{
			// WaitAndPop bloque via condition_variable jusqu'a ce qu'un
			// job soit push ou que m_shutdownRequested devienne true.
			auto job = m_readyQueue.WaitAndPop(m_shutdownRequested);

			if (!job.has_value())
			{
				// La queue a retourne nullopt => shutdown demande
				// et queue vide => on sort de la boucle.
				break;
			}

			// Executer le travail du job.
			if (job.value()->m_work)
			{
				job.value()->m_work();
			}

			// Signaler la completion : mettre a jour les dependants
			// et les compteurs.
			OnJobCompleted(job.value());
		}
	}

	// -------------------------------------------------------------------
	// OnJobCompleted
	// -------------------------------------------------------------------
	// Appelee apres que la fonction de travail d'un job a retourne.
	// Pour chaque job dependant dans la liste m_dependents du job fini :
	//   1. Decrementer atomiquement le m_unfinishedCount du dependant.
	//   2. Si m_unfinishedCount atteint 0, tous les prerequis sont
	//      satisfaits => push le dependant dans la ready queue.
	//
	// Puis decrementer le compteur global m_inFlightCount et notifier
	// tout thread en attente dans WaitForAll() ou WaitFor().
	//
	// JUSTIFICATION DE fetch_sub avec memory_order_acq_rel :
	//   - "release" : garantit que toutes les ecritures faites par le
	//     job qui vient de se terminer sont visibles par le thread qui
	//     observe le compteur tomber a zero.
	//   - "acquire" : garantit que quand on observe le compteur a zero,
	//     on voit toutes les ecritures anterieures des autres threads
	//     qui ont aussi decremente ce compteur.
	//   C'est le pattern standard pour un "decrement-and-check-zero"
	//   type compteur de references.
	// -------------------------------------------------------------------
	void JobSystem::OnJobCompleted(const JobHandle& job)
	{
		for (auto& dependent : job->m_dependents)
		{
			uint32_t prev = dependent->m_unfinishedCount.fetch_sub(
				1, std::memory_order_acq_rel);

			// Si on vient de decrementer de 1 a 0, ce dependant
			// est maintenant pret a s'executer.
			if (prev == 1)
			{
				m_readyQueue.Push(dependent);
			}
		}

		// Decrementer le compteur global de jobs en vol.
		uint32_t prevInFlight = m_inFlightCount.fetch_sub(
			1, std::memory_order_acq_rel);

		// Si c'etait le dernier job en vol, reveiller les waiters.
		if (prevInFlight == 1)
		{
			std::lock_guard<std::mutex> lock(m_completionMutex);
			m_completionCondition.notify_all();
		}
	}

	// -------------------------------------------------------------------
	// Initialize
	// -------------------------------------------------------------------
	// Lance N worker threads ou N = hardware_concurrency - 1.
	// On reserve un coeur pour le main thread qui joue le role
	// d'orchestrateur (et qui peut aussi aider via WaitFor/WaitForAll).
	// -------------------------------------------------------------------
	void JobSystem::Initialize()
	{
		uint32_t numCores = std::thread::hardware_concurrency();
		// Minimum 1 worker meme sur un single-core.
		uint32_t numWorkers = std::max(1u, numCores - 1);

		m_shutdownRequested.store(false, std::memory_order_relaxed);

		m_workers.reserve(numWorkers);
		for (uint32_t i = 0; i < numWorkers; ++i)
		{
			m_workers.emplace_back(
				[this, i]() { WorkerMain(i); });
		}
	}

	// -------------------------------------------------------------------
	// Shutdown
	// -------------------------------------------------------------------
	// Arret propre : signale a tous les workers de s'arreter, puis join.
	//
	// JUSTIFICATION DE memory_order_release pour le store :
	//   Garantit que toutes les ecritures anterieures (par ex. les
	//   derniers push dans la queue) sont visibles par les workers
	//   quand ils observent le flag.
	// -------------------------------------------------------------------
	void JobSystem::Shutdown()
	{
		m_shutdownRequested.store(true, std::memory_order_release);

		// Reveiller tous les workers qui dorment dans WaitAndPop.
		m_readyQueue.NotifyAll();

		// jthread::~jthread va auto-join, mais on clear le vecteur
		// explicitement pour join MAINTENANT plutot qu'au destructeur.
		m_workers.clear();

		m_shutdownRequested.store(false, std::memory_order_relaxed);
	}

	// -------------------------------------------------------------------
	// Destructeur
	// -------------------------------------------------------------------
	JobSystem::~JobSystem() noexcept
	{
		if (!m_workers.empty())
		{
			Shutdown();
		}
	}

	// -------------------------------------------------------------------
	// Schedule (sans dependances)
	// -------------------------------------------------------------------
	// Cree un job et le push immediatement dans la ready queue.
	// Le m_unfinishedCount demarre a 1 (sentinel de setup).
	// Pas de dependances => on decremente le sentinel => count = 0
	// => le job est immediatement pret.
	// -------------------------------------------------------------------
	JobHandle JobSystem::Schedule(std::function<void()> work)
	{
		auto job = std::make_shared<Job>();
		job->m_work = std::move(work);

		m_inFlightCount.fetch_add(1, std::memory_order_relaxed);

		// Decrementer le sentinel de setup. Sans dependances,
		// ca amene le count a 0 => push dans la ready queue.
		uint32_t prev = job->m_unfinishedCount.fetch_sub(
			1, std::memory_order_acq_rel);
		assert(prev == 1);

		m_readyQueue.Push(job);
		return job;
	}

	// -------------------------------------------------------------------
	// Schedule (avec dependances)
	// -------------------------------------------------------------------
	// Algorithme de cablage du DAG :
	//   1. Creer le Job J avec m_unfinishedCount = deps.size() + 1
	//      (un par dependance + le sentinel de setup).
	//   2. Pour chaque dependance D, ajouter J dans D->m_dependents.
	//   3. Decrementer le sentinel de setup.
	//   4. Si le count atteint 0 (toutes les deps sont deja finies),
	//      push J dans la ready queue.
	//
	// Securite contre les races :
	//   - Si une dep finit AVANT qu'on ajoute J : son OnJobCompleted
	//     a deja tourne, mais il ne peut pas decrementer J car J n'est
	//     pas encore dans ses dependents. Le sentinel empeche J d'etre
	//     schedule trop tot. IMPORTANT : les dependances doivent etre
	//     scheduleees dans l'ordre topologique depuis un seul thread.
	//   - Si une dep finit APRES qu'on ajoute J : son OnJobCompleted
	//     decremente le compteur de J normalement.
	// -------------------------------------------------------------------
	JobHandle JobSystem::Schedule(
		std::function<void()> work,
		std::span<const JobHandle> dependencies)
	{
		auto job = std::make_shared<Job>();
		job->m_work = std::move(work);

		// +1 pour le sentinel de setup.
		job->m_unfinishedCount.store(
			static_cast<uint32_t>(dependencies.size()) + 1,
			std::memory_order_relaxed);

		m_inFlightCount.fetch_add(1, std::memory_order_relaxed);

		// Cabler les aretes du DAG.
		for (const auto& dep : dependencies)
		{
			dep->m_dependents.push_back(job);
		}

		// Decrementer le sentinel de setup.
		uint32_t prev = job->m_unfinishedCount.fetch_sub(
			1, std::memory_order_acq_rel);

		// Si ca amene a 0, toutes les dependances etaient deja
		// terminees (elles ont chacune decremente) => push immediat.
		if (prev == 1)
		{
			m_readyQueue.Push(job);
		}

		return job;
	}

	// -------------------------------------------------------------------
	// WaitFor
	// -------------------------------------------------------------------
	// Attend la completion d'un job specifique. Le thread appelant aide
	// en "volant" du travail de la queue (work stealing cote main thread)
	// pour ne pas rester inactif.
	//
	// JUSTIFICATION :
	//   Pas de condition_variable par job (couterait trop de memoire par
	//   job). A la place, le thread aide en executant des jobs et yield
	//   quand la queue est vide. WaitFor est typiquement utilise pour
	//   le debug/test ; WaitForAll est la barriere de frame usuelle.
	// -------------------------------------------------------------------
	void JobSystem::WaitFor(const JobHandle& handle)
	{
		while (handle->m_unfinishedCount.load(std::memory_order_acquire) > 0)
		{
			// Essayer d'aider en executant un job pret.
			auto stolen = m_readyQueue.TryPop();
			if (stolen.has_value())
			{
				if (stolen.value()->m_work)
				{
					stolen.value()->m_work();
				}
				OnJobCompleted(stolen.value());
			}
			else
			{
				// Pas de travail disponible => yield pour ne pas
				// bruler le CPU.
				std::this_thread::yield();
			}
		}
	}

	// -------------------------------------------------------------------
	// WaitForAll
	// -------------------------------------------------------------------
	// Attend que TOUS les jobs en vol soient termines.
	// Le thread appelant aide en executant des jobs de la queue.
	//
	// JUSTIFICATION DE LA SYNCHRONISATION :
	//   Utilise m_completionMutex + m_completionCondition pour dormir
	//   quand il n'y a plus rien dans la queue mais que des jobs sont
	//   encore en cours d'execution par les workers. La condition_variable
	//   evite le busy-waiting du main thread.
	// -------------------------------------------------------------------
	void JobSystem::WaitForAll()
	{
		while (m_inFlightCount.load(std::memory_order_acquire) > 0)
		{
			// Aider en executant des jobs de la queue.
			auto stolen = m_readyQueue.TryPop();
			if (stolen.has_value())
			{
				if (stolen.value()->m_work)
				{
					stolen.value()->m_work();
				}
				OnJobCompleted(stolen.value());
			}
			else
			{
				// Rien dans la queue mais des jobs sont encore en vol
				// (en cours d'execution par des workers). Attendre la
				// notification de completion.
				std::unique_lock<std::mutex> lock(m_completionMutex);
				m_completionCondition.wait(lock, [this]()
				{
					return m_inFlightCount.load(
						std::memory_order_acquire) == 0;
				});
			}
		}
	}

	// -------------------------------------------------------------------
	// WorkerCount
	// -------------------------------------------------------------------
	uint32_t JobSystem::WorkerCount() const
	{
		return static_cast<uint32_t>(m_workers.size());
	}
}
