#ifndef __PAGING_DAEMON_H__
#define __PAGING_DAEMON_H__

#include "process_queue.h"

struct PagingDaemonArgs
{
	struct LogState* log_state;
	struct ProcessQueue* pqueue_ready;
	struct ProcessQueue* pqueue_pfaulted;
	struct HashTable* hash_table;
};

void* paging_daemon(void* paging_daemon_args);

#endif