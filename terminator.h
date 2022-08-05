#ifndef __TERMINATOR_H__
#define __TERMINATOR_H__

#include "process_queue.h"

struct TerminatorArgs
{
	struct LogState* log_state;
	struct ProcessQueue* pqueue_ready;
	struct ProcessQueue* pqueue_terminated;
	struct ProcessQueue* pqueue_pfaulted;
	struct HashTable* hash_table;
};

void* terminator(void* terminator_args);

#endif