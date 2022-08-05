#include <pthread.h>
#include <stdlib.h>

#include "mutex_counter.h"

struct MutexCounter* mutex_counter_init()
{
	struct MutexCounter* mutex_counter = calloc(1, sizeof(*mutex_counter));

	pthread_mutex_init(&mutex_counter->mutex, NULL);
	pthread_cond_init(&mutex_counter->cond_change, NULL);

	return mutex_counter;
}

void mutex_counter_free(struct MutexCounter* mutex_counter)
{
	pthread_mutex_destroy(&mutex_counter->mutex);
	pthread_cond_destroy(&mutex_counter->cond_change);
	free(mutex_counter);
}