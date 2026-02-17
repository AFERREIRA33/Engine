#ifndef VDE__CORE__JOB_SYSTEM__JOBSYSTEM_H
#define VDE__CORE__JOB_SYSTEM__JOBSYSTEM_H
#pragma once

#include <core/job_system/job.h>
#include <core/job_system/threadsafequeue.h>
#include <util/globalinstance.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <span>
#include <thread>
#include <vector>

namespace vde::core::job_system
{
	// -------------------------------------------------------------------
	// JobSystem
	// -------------------------------------------------------------------
	// Gere un pool de worker threads persistants et une queue partagee
	// de jobs prets a executer. Supporte les graphes de dependances (DAG).
	//
	// Cycle de vie :
	//   JobSystem::Global()   -- acces au singleton
	//   Initialize()          -- lance les worker threads
	//   Schedule(...)         -- soumet des jobs avec dependances optionnelles
	//   WaitFor(handle)       -- bloque jusqu'a la fin d'un job specifique
	//   WaitForAll()          -- bloque jusqu'a la fin de tous les jobs
	//   Shutdown()            -- arrete et join les worker threads
	//
	// Herite de GlobalInstance<T> pour suivre le pattern singleton
	// existant dans le moteur (cf. PluginRegistry).
	// -------------------------------------------------------------------
	class JobSystem
		: public util::GlobalInstance<JobSystem>
	{
		// Worker threads.
		// CHOIX : std::jthread (C++20) plutot que std::thread car jthread
		// auto-join dans son destructeur, evitant les resource leaks si
		// Shutdown() n'est pas appele explicitement.
		std::vector<std::jthread>    m_workers;

		// File partagee de jobs prets a executer.
		ThreadSafeQueue<JobHandle>   m_readyQueue;

		// Flag pour signaler aux workers de s'arreter.
		// CHOIX : std::atomic<bool> avec memory_order_release (ecriture)
		// et memory_order_acquire (lecture). Un atomic suffit car c'est
		// un flag unidirectionnel (main -> workers) ecrit une seule fois.
		std::atomic<bool>            m_shutdownRequested { false };

		// Compteur global de jobs soumis mais pas encore termines.
		// CHOIX : std::atomic<uint32_t> car plusieurs workers decrementent
		// ce compteur en meme temps quand des jobs se terminent, et le
		// main thread le lit dans WaitForAll(). Un atomic evite de prendre
		// un mutex sur ce chemin critique (hot path).
		std::atomic<uint32_t>        m_inFlightCount { 0 };

		// Mutex + condition_variable pour implementer WaitForAll() et
		// WaitFor() sans busy-waiting.
		// CHOIX : condition_variable est le mecanisme standard C++ pour
		// "attendre qu'une condition soit vraie". Plus econome en energie
		// qu'un spin sur un atomic, et plus portable que les futex
		// specifiques a la plateforme.
		std::mutex                   m_completionMutex;
		std::condition_variable      m_completionCondition;

		// Fonction principale de chaque worker thread.
		void WorkerMain(uint32_t workerIndex);

		// Appelee quand un job finit : decremente les compteurs des
		// dependants et enqueue ceux qui deviennent prets.
		void OnJobCompleted(const JobHandle& job);

	public:
		JobSystem() = default;
		~JobSystem() noexcept;

		// Lance N worker threads (N = hardware_concurrency - 1,
		// on reserve un coeur pour le main thread).
		void Initialize();

		// Arret propre : signale aux workers de s'arreter, puis join.
		void Shutdown();

		// Soumet un job sans dependances.
		JobHandle Schedule(std::function<void()> work);

		// Soumet un job qui depend d'un ou plusieurs jobs prerequis.
		// Le nouveau job ne s'executera pas tant que TOUTES les
		// dependances n'auront pas termine.
		JobHandle Schedule(
			std::function<void()> work,
			std::span<const JobHandle> dependencies);

		// Bloque le thread appelant jusqu'a la fin du job donne.
		// Le thread appelant aide en executant des jobs de la queue.
		void WaitFor(const JobHandle& handle);

		// Bloque le thread appelant jusqu'a la fin de TOUS les jobs
		// en cours. Le thread appelant aide en executant des jobs.
		void WaitForAll();

		// Retourne le nombre de worker threads.
		uint32_t WorkerCount() const;
	};
}

#endif /* VDE__CORE__JOB_SYSTEM__JOBSYSTEM_H */
