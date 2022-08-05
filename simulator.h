#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "log.h"
#include "process_queue.h"

struct SimulatorArgs
{
	struct LogState* log_state;
	struct ProcessQueue* pqueue_ready;
	struct ProcessQueue* pqueue_terminated;
	struct ProcessQueue* pqueue_pfaulted;
};

struct SimulatorResult
{
	unsigned int total_page_faults;
};

void* simulator(void* simulator_args);

#endif