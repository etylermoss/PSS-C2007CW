#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "hash_table.h"
#include "process_queue.h"

struct GeneratorArgs
{
	struct LogState* log_state;
	struct ProcessQueue* pqueue_ready;
	struct HashTable* hash_table;
};

void* generator(void* generator_args);

#endif