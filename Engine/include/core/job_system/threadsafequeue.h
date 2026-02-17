#ifndef VDE__CORE__JOB_SYSTEM__THREADSAFEQUEUE_H
#define VDE__CORE__JOB_SYSTEM__THREADSAFEQUEUE_H
#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

namespace vde::core::job_system
{
	// -------------------------------------------------------------------
	// ThreadSafeQueue<T>
	// -------------------------------------------------------------------
	// File FIFO thread-safe protegee par un mutex et notifiee via
	// une condition_variable.
	//
	// JUSTIFICATION DU CHOIX DE SYNCHRONISATION :
	//   std::mutex + std::condition_variable a ete choisi plutot qu'une
	//   queue lock-free pour les raisons suivantes :
	//     1. La correction est plus facile a verifier et documenter.
	//     2. condition_variable permet aux workers de dormir quand la
	//        queue est vide, evitant le busy-wait et l'usage CPU inutile.
	//     3. La fenetre de contention est tres petite (push/pop d'un
	//        shared_ptr), donc le mutex est tenu pendant quelques
	//        nanosecondes.
	//     4. Les queues lock-free necessitent un raisonnement complexe
	//        sur les memory orders (acquire/release fences) et sont
	//        plus difficiles a debugger.
	// -------------------------------------------------------------------
	template<typename T>
	class ThreadSafeQueue
	{
		std::deque<T>           m_data;
		mutable std::mutex      m_mutex;
		std::condition_variable m_condition;

	public:
		// Ajoute un element et reveille un thread en attente.
		void Push(T value)
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_data.push_back(std::move(value));
			}
			// notify_one est appele EN DEHORS du lock pour eviter le
			// pattern "hurry up and wait" ou le thread reveille bloque
			// immediatement sur le mutex.
			m_condition.notify_one();
		}

		// Tentative non-bloquante de pop. Retourne std::nullopt si vide.
		std::optional<T> TryPop()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_data.empty())
			{
				return std::nullopt;
			}
			T value = std::move(m_data.front());
			m_data.pop_front();
			return value;
		}

		// Pop bloquant. Attend qu'un element soit disponible OU que le
		// flag de shutdown soit active.
		// Retourne std::nullopt uniquement quand shutdownFlag est true
		// et que la queue est vide.
		//
		// JUSTIFICATION :
		//   condition_variable::wait bloque le thread sans consommer
		//   de CPU, contrairement a un spin-wait sur un atomic.
		//   Le predicat verifie a la fois la presence de donnees ET
		//   le flag de shutdown pour permettre un arret propre.
		std::optional<T> WaitAndPop(const std::atomic<bool>& shutdownFlag)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condition.wait(lock, [&]()
			{
				return !m_data.empty() || shutdownFlag.load(std::memory_order_acquire);
			});
			if (m_data.empty())
			{
				return std::nullopt;
			}
			T value = std::move(m_data.front());
			m_data.pop_front();
			return value;
		}

		// Reveille tous les threads en attente (utilise lors du shutdown).
		void NotifyAll()
		{
			m_condition.notify_all();
		}
	};
}

#endif /* VDE__CORE__JOB_SYSTEM__THREADSAFEQUEUE_H */
