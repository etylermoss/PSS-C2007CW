#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "linkedlist_guard.h"
#include "process_queue.h"
#include "log.h"

#include "simulator.h"

void* simulator(void* simulator_args)
{
	struct SimulatorArgs args = *(struct SimulatorArgs*) simulator_args;

	unsigned int total_page_faults = 0;

	while (true) {
		pthread_mutex_lock(&args.pqueue_ready->mutex);

		/* wait until processes to simulate */
		while (pqueue_is_empty(args.pqueue_ready))
			pthread_cond_wait(&args.pqueue_ready->cond_change, &args.pqueue_ready->mutex);

		/* check for poisoned queue, i.e. no more items will enter it */
		if (pqueue_poisoned(args.pqueue_ready)) {
			removeFirst(args.pqueue_ready->list);
			pthread_mutex_unlock(&args.pqueue_ready->mutex);
			break;
		}

		Process* proc = (Process*) getHead(*args.pqueue_ready->list)->pData;

		/* remove process from ready queue; unlock ready queue */
		removeFirst(args.pqueue_ready->list);
		pthread_mutex_unlock(&args.pqueue_ready->mutex);

		/* simulate process */
		log_event(args.log_state, EVT_SIMULATING, (struct EventData) { proc });
		runPreemptiveProcess(proc, true);

		/* handle process state */
		switch (proc->iStatus) {
		case READY:
			log_event(args.log_state, EVT_READY, (struct EventData) { proc });
			pthread_mutex_lock(&args.pqueue_ready->mutex);
			addLast(proc, args.pqueue_ready->list);
			pthread_cond_signal(&args.pqueue_ready->cond_change);
			pthread_mutex_unlock(&args.pqueue_ready->mutex);
			break;
		case TERMINATED:
			log_event(args.log_state, EVT_TERMINATED, (struct EventData) { proc });
			pthread_mutex_lock(&args.pqueue_terminated->mutex);
			addLast(proc, args.pqueue_terminated->list);
			pthread_cond_signal(&args.pqueue_terminated->cond_change);
			pthread_mutex_unlock(&args.pqueue_terminated->mutex);
			break;
		case PAGE_FAULTED:
			log_event(args.log_state, EVT_PAGE_FAULTED, (struct EventData) { proc, &proc->oLastRequested });
			pthread_mutex_lock(&args.pqueue_pfaulted->mutex);
			addLast(proc, args.pqueue_pfaulted->list);
			pthread_cond_signal(&args.pqueue_pfaulted->cond_change);
			pthread_mutex_unlock(&args.pqueue_pfaulted->mutex);
			total_page_faults++;
			break;
		default:
			fprintf(stderr, "Fatal error: Unexpected iStatus %d\n", proc->iStatus);
			exit(1);
		}
	}

	struct SimulatorResult* result = malloc(sizeof(*result));
	result->total_page_faults = total_page_faults;
	
	return result;
}