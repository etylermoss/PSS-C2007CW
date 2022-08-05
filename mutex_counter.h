#ifndef __MUTEX_COUNTER_H__
#define __MUTEX_COUNTER_H__

#include <pthread.h>

#include "coursework_guard.h"

struct MutexCounter
{
	pthread_mutex_t mutex;
	pthread_cond_t cond_change;
	int count;
};

struct MutexCounter* mutex_counter_init();
void mutex_counter_free(struct MutexCounter* active_procs);

#endif