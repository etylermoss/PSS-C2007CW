#ifndef __PROCESS_QUEUE_H__
#define __PROCESS_QUEUE_H__

#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>

#include "coursework_guard.h"
#include "linkedlist_guard.h"

struct ProcessQueue
{
	pthread_mutex_t mutex;
	pthread_cond_t cond_change;
	LinkedList* list;
};

struct ProcessQueue* pqueue_init();
void pqueue_free(struct ProcessQueue* process_queue);
bool pqueue_is_empty(const struct ProcessQueue* process_queue);
size_t pqueue_len(const struct ProcessQueue* process_queue);
void pqueue_poison(const struct ProcessQueue* process_queue);
bool pqueue_poisoned(const struct ProcessQueue* process_queue);

#endif