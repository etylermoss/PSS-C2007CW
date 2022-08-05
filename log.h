#ifndef __LOG_H__
#define __LOG_H__

#include <pthread.h>

#include "coursework_guard.h"
#include "util.h"

enum EventType
{
	EVT_ADMITTED,
	EVT_SIMULATING,
	EVT_READY,
	EVT_TERMINATED,
	EVT_CLEARED,
	EVT_PAGE_FAULTED,
};

struct LogState
{
	pthread_mutex_t mutex;
	enum SimulatorProgram sim;
};

struct EventData
{
	const Process* proc;
	const MemoryAddress* mem; // may be NULL except for EVT_PAGE_FAULTED
};

struct LogState* log_state_init(enum SimulatorProgram sim);
void log_state_free(struct LogState* log);
void log_event(struct LogState* log, enum EventType type, struct EventData data);
void log_finished(struct LogState* log, unsigned int total_page_faults, double average_page_faults);

#endif