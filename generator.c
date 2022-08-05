#include <pthread.h>
#include <sys/param.h>
#include <stddef.h>

#include "hash_table.h"
#include "util.h"
#include "log.h"

#include "generator.h"

void* generator(void* generator_args)
{
	struct GeneratorArgs args = *(struct GeneratorArgs*) generator_args;

	int generated_procs = 0;
	while (generated_procs < NUMBER_OF_PROCESSES) {
		hash_table_pthread_lock(args.hash_table);

		/* wait until we can generate more processes */
		int to_gen;
		while (true) {
			int procs_available = MAX_CONCURRENT_PROCESSES - hash_table_len(args.hash_table);
			int procs_left = NUMBER_OF_PROCESSES - generated_procs;
			if ((to_gen = MIN(procs_available, procs_left)) > 0)
				break;

			hash_table_pthread_wait(args.hash_table);
		}

		pthread_mutex_lock(&args.pqueue_ready->mutex);

		/* check for poisoned queue, i.e. no more items will enter it */
		if (pqueue_poisoned(args.pqueue_ready)) {
			removeFirst(args.pqueue_ready->list);
			pthread_mutex_unlock(&args.pqueue_ready->mutex);
			hash_table_pthread_unlock(args.hash_table);
			break;
		}

		/* generate processes; add them to ready queue */
		while (to_gen > 0) {
			Process* proc = generateProcess(generated_procs + 1);
			log_event(args.log_state, EVT_ADMITTED, (struct EventData) { proc });
			addLast(proc, args.pqueue_ready->list);
			if (!hash_table_insert(args.hash_table, proc->iHash, proc))
				fatal_error("Could not insert process into process table.");
			generated_procs++;
			to_gen--;
		}

		hash_table_pthread_signal(args.hash_table);
		hash_table_pthread_unlock(args.hash_table);

		pthread_cond_signal(&args.pqueue_ready->cond_change);
		pthread_mutex_unlock(&args.pqueue_ready->mutex);
	}

	return NULL;
}