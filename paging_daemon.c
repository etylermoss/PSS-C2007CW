#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "hash_table.h"
#include "linkedlist_guard.h"
#include "log.h"
#include "process_queue.h"

#include "paging_daemon.h"

void* paging_daemon(void* paging_daemon_args)
{
	struct PagingDaemonArgs args = *(struct PagingDaemonArgs*) paging_daemon_args;

	FrameEntry* frames = calloc(NUMBER_OF_FRAMES, (sizeof(*frames)));
	FrameEntry* frame_head = &frames[0];

	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
		frames[i] = (FrameEntry) PAGE_TABLE_ENTRY_INITIALIZER;

	while (true) {
		pthread_mutex_lock(&args.pqueue_pfaulted->mutex);

		/* wait until processes to simulate */
		while (pqueue_is_empty(args.pqueue_pfaulted))
			pthread_cond_wait(&args.pqueue_pfaulted->cond_change, &args.pqueue_pfaulted->mutex);

		/* check for poisoned queue, i.e. no more items will enter it */
		if (pqueue_poisoned(args.pqueue_pfaulted)) {
			removeFirst(args.pqueue_pfaulted->list);
			pthread_mutex_unlock(&args.pqueue_pfaulted->mutex);
			break;
		}

		Process* proc = (Process*) getHead(*args.pqueue_pfaulted->list)->pData;

		/* remove process from ready queue; unlock ready queue */
		removeFirst(args.pqueue_pfaulted->list);
		pthread_mutex_unlock(&args.pqueue_pfaulted->mutex);

		/* NRG algorithm */
		reclaimFrame(frame_head);
		mapFrame(proc, frame_head);
		if (++frame_head > &frames[NUMBER_OF_FRAMES - 1])
			frame_head = &frames[0];

		/* mark ready & add to ready queue */
		proc->iStatus = READY;

		pthread_mutex_lock(&args.pqueue_ready->mutex);
		addLast(proc, args.pqueue_ready->list);
		pthread_cond_signal(&args.pqueue_ready->cond_change);
		pthread_mutex_unlock(&args.pqueue_ready->mutex);
	}

	free(frames);

	return NULL;
}