#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist_guard.h"

#include "process_queue.h"

#define POISON_VAL NULL

struct ProcessQueue* pqueue_init(char* name)
{
	struct ProcessQueue* process_queue = calloc(1, sizeof(*process_queue));

	process_queue->list = calloc(1, sizeof(*process_queue->list));
	process_queue->list->sName = name;

	pthread_mutex_init(&process_queue->mutex, NULL);
	pthread_cond_init(&process_queue->cond_change, NULL);

	return process_queue;
}

void pqueue_free(struct ProcessQueue* process_queue)
{
	pthread_mutex_destroy(&process_queue->mutex);
	pthread_cond_destroy(&process_queue->cond_change);
	while (process_queue->list->pHead != NULL)
		removeFirst(process_queue->list);
	free(process_queue->list);
	free(process_queue);
}

bool pqueue_is_empty(const struct ProcessQueue* process_queue)
{
	return process_queue->list->pHead == NULL;
}

size_t pqueue_len(const struct ProcessQueue* process_queue)
{
	Element* element = process_queue->list->pHead;
	size_t i = 0;

	while (element != NULL) {
		element = getNext(element);
		i++;
	}

	return i;
}

void pqueue_poison(const struct ProcessQueue* process_queue)
{
	addLast(POISON_VAL, process_queue->list);
}

bool pqueue_poisoned(const struct ProcessQueue* process_queue)
{
	return process_queue->list->pHead && process_queue->list->pHead->pData == POISON_VAL;
}
