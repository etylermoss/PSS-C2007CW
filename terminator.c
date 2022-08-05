#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "hash_table.h"
#include "linkedlist_guard.h"
#include "log.h"
#include "process_queue.h"

#include "terminator.h"

void* terminator(void* terminator_args)
{
	struct TerminatorArgs args = *(struct TerminatorArgs*) terminator_args;

	int freed_frames = 0;
	int terminated_procs = 0;
	while (terminated_procs < NUMBER_OF_PROCESSES) {
		pthread_mutex_lock(&args.pqueue_terminated->mutex);

		/* wait until processes to terminate */
		while (pqueue_is_empty(args.pqueue_terminated))
			pthread_cond_wait(&args.pqueue_terminated->cond_change, &args.pqueue_terminated->mutex);

		/* check for poisoned queue, i.e. no more items will enter it */
		if (pqueue_poisoned(args.pqueue_terminated)) {
			removeFirst(args.pqueue_terminated->list);
			pthread_mutex_unlock(&args.pqueue_terminated->mutex);
			break;
		}

		Process* proc = (Process*) getHead(*args.pqueue_terminated->list)->pData;

		/* remove process from terminated queue; unlock terminated queue */
		removeFirst(args.pqueue_terminated->list);
		pthread_mutex_unlock(&args.pqueue_terminated->mutex);

		/* log, free page table & process */
		log_event(args.log_state, EVT_CLEARED, (struct EventData) { proc });
		int iHash = proc->iHash;
		free(proc);

		/* remove process from process table */
		hash_table_pthread_lock(args.hash_table);
		hash_table_remove(args.hash_table, iHash);
		hash_table_pthread_signal(args.hash_table);
		hash_table_pthread_unlock(args.hash_table);
		
		terminated_procs++;
	}

	/* poison ready & pfaulted queues */
	pthread_mutex_lock(&args.pqueue_ready->mutex);
	pqueue_poison(args.pqueue_ready);
	pthread_cond_signal(&args.pqueue_ready->cond_change);
	pthread_mutex_unlock(&args.pqueue_ready->mutex);
	pthread_mutex_lock(&args.pqueue_pfaulted->mutex);
	pqueue_poison(args.pqueue_pfaulted);
	pthread_cond_signal(&args.pqueue_pfaulted->cond_change);
	pthread_mutex_unlock(&args.pqueue_pfaulted->mutex);

	return NULL;
}