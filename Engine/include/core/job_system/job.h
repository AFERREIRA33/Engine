#ifndef VDE__CORE__JOB_SYSTEM__JOB_H
#define VDE__CORE__JOB_SYSTEM__JOB_H
#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace vde::core::job_system
{
	struct Job;

	// -------------------------------------------------------------------
	// JobHandle
	// -------------------------------------------------------------------
	// Handle opaque vers un job soumis. Utilise shared_ptr pour garantir
	// que le Job reste en vie tant qu'un handle le reference (la queue et
	// les dependants peuvent tous deux detenir un handle).
	// -------------------------------------------------------------------
	using JobHandle = std::shared_ptr<Job>;

	// -------------------------------------------------------------------
	// Job
	// -------------------------------------------------------------------
	// Represente une unite de travail dans le job system.
	//
	// m_work             -- Callable a executer (petit, rapide).
	// m_unfinishedCount  -- Nombre de prerequis restants avant execution.
	//                       Initialise a (nombre de dependances + 1).
	//                       Le +1 est un "sentinel" de setup : il empeche
	//                       le job d'etre schedule avant que toutes les
	//                       aretes du DAG soient connectees. Le soumetteur
	//                       decremente le sentinel une fois le setup fini.
	//
	//   CHOIX DE PRIMITIVE ATOMIQUE :
	//     std::atomic<uint32_t> car plusieurs workers peuvent decrementer
	//     ce compteur en meme temps (quand leurs jobs-dependances respectives
	//     se terminent). On utilise fetch_sub avec memory_order_acq_rel :
	//       - "release" : les ecritures du job qui vient de finir sont
	//         visibles par le thread qui observe le compteur tomber a zero.
	//       - "acquire" : le thread qui observe zero voit toutes les
	//         ecritures anterieures des autres threads qui ont aussi
	//         decremente.
	//     C'est le pattern standard "decrement-and-check-zero" utilise
	//     dans les compteurs de references.
	//
	// m_dependents       -- Jobs qui dependent de CE job. Quand ce job
	//                       se termine, chaque dependant voit son compteur
	//                       decremente. Ce vecteur est rempli uniquement
	//                       pendant le setup (thread principal) et lu
	//                       pendant la completion (worker), donc pas de
	//                       mutation concurrente.
	// -------------------------------------------------------------------
	struct Job
	{
		std::function<void()>      m_work;
		std::atomic<uint32_t>      m_unfinishedCount { 1 };
		std::vector<JobHandle>     m_dependents;
	};
}

#endif /* VDE__CORE__JOB_SYSTEM__JOB_H */
