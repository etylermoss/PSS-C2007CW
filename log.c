#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "print_list.h"
#include "util.h"

#include "log.h"

#define LOG_OUTPUT stderr

const char* event_to_string(const enum EventType type)
{
	struct EventTypeString { enum EventType type; char* str; };

	static const struct EventTypeString strs[] = {
		{ EVT_ADMITTED,     "ADMITTED" },
		{ EVT_SIMULATING,   "SIMULATING" },
		{ EVT_READY,        "READY" },
		{ EVT_TERMINATED,   "TERMINATED" },
		{ EVT_CLEARED,      "CLEARED" },
		{ EVT_PAGE_FAULTED, "PAGE_FAULTED" },
	};

	size_t strs_len = sizeof(strs) / sizeof(*strs);
	for (const struct EventTypeString* str = &strs[0]; str != &strs[strs_len]; str++)
		if (str->type == type)
			return str->str;

	return NULL;
};

struct LogState* log_state_init(const enum SimulatorProgram sim)
{
	struct LogState* log_state = calloc(1, sizeof(*log_state));

	pthread_mutex_init(&log_state->mutex, NULL);

	log_state->sim = sim;

	return log_state;
}

void log_state_free(struct LogState* log_state)
{
	pthread_mutex_destroy(&log_state->mutex);
	free(log_state);
}

void log_event(struct LogState* log_state, const enum EventType type, const struct EventData data)
{
	const char* event_str = event_to_string(type);

	if (event_str == NULL) // TODO: log fatal error
		return;

	const long int response_time = getDifferenceInMilliSeconds(data.proc->oTimeCreated, data.proc->oFirstTimeRunning);
	const long int turn_around_time = getDifferenceInMilliSeconds(data.proc->oFirstTimeRunning, data.proc->oLastTimeRunning);

	pthread_mutex_lock(&log_state->mutex);

	fprintf(LOG_OUTPUT, "%s: [", event_str);

	switch (type) {
	case EVT_ADMITTED:
		afprintf_list(LOG_OUTPUT,
			(struct ListOptions) { .items = 6 },
			(struct ListItem[]) {
				{ "PID", VAL_INT, &data.proc->iPID },
				{ "Hash", VAL_INT, &data.proc->iHash },
				{ "BurstTime", VAL_INT, &data.proc->iBurstTime },
				{ "RemainingBurstTime", VAL_INT, &data.proc->iRemainingBurstTime },
				{ "Locality", VAL_INT, &data.proc->iLocality },
				{ "Width", VAL_INT, &data.proc->iWidth },
			}
		);
		break;
	case EVT_SIMULATING:
		afprintf_list(LOG_OUTPUT,
			(struct ListOptions) { .items = 3 },
			(struct ListItem[]) {
				{ "PID", VAL_INT, &data.proc->iPID },
				{ "BurstTime", VAL_INT, &data.proc->iBurstTime },
				{ "RemainingBurstTime", VAL_INT, &data.proc->iRemainingBurstTime },
			}
		);
		break;
	case EVT_READY:
		afprintf_list(LOG_OUTPUT,
			(struct ListOptions) { .items = 5 },
			(struct ListItem[]) {
				{ "PID", VAL_INT, &data.proc->iPID },
				{ "BurstTime", VAL_INT, &data.proc->iBurstTime },
				{ "RemainingBurstTime", VAL_INT, &data.proc->iRemainingBurstTime },
				{ "Locality", VAL_INT, &data.proc->iLocality },
				{ "Width", VAL_INT, &data.proc->iWidth },
			}
		);
		break;
	case EVT_TERMINATED:
		switch (log_state->sim) {
		case SIM_1:
		case SIM_2:
		case SIM_3:
			afprintf_list(LOG_OUTPUT,
				(struct ListOptions) { .items = 3 },
				(struct ListItem[]) {
					{ "PID", VAL_INT, &data.proc->iPID },
					{ "ResponseTime", VAL_LONG, &response_time },
					{ "TurnAroundTime", VAL_LONG, &turn_around_time },
				}
			);
			break;
		case SIM_4:
		case SIM_5:
		case SIM_6:
		case SIM_7:
			afprintf_list(LOG_OUTPUT,
				(struct ListOptions) { .items = 2 },
				(struct ListItem[]) {
					{ "PID", VAL_INT, &data.proc->iPID },
					{ "RemainingBurstTime", VAL_INT, &data.proc->iRemainingBurstTime },
				}
			);
			break;
		}
		break;
	case EVT_PAGE_FAULTED:
		if (data.proc == NULL) // TODO: log fatal error
			return;

		afprintf_list(LOG_OUTPUT,
			(struct ListOptions) { .items = 7 },
			(struct ListItem[]) {
				{ "PID", VAL_INT, &data.proc->iPID },
				{ "BurstTime", VAL_INT, &data.proc->iBurstTime },
				{ "RemainingBurstTime", VAL_INT, &data.proc->iRemainingBurstTime },
				{ "Locality", VAL_INT, &data.proc->iLocality },
				{ "Width", VAL_INT, &data.proc->iWidth },
				{ "Page", VAL_INT, &data.mem->iPage },
				{ "Offset", VAL_INT, &data.mem->iOffset },
			}
		);
		break;
	case EVT_CLEARED:
		switch (log_state->sim) {
		case SIM_1:
		case SIM_2:
		case SIM_3:
		case SIM_4:
			afprintf_list(LOG_OUTPUT,
				(struct ListOptions) { .items = 3 },
				(struct ListItem[]) {
					{ "PID", VAL_INT, &data.proc->iPID },
					{ "ResponseTime", VAL_LONG, &response_time },
					{ "TurnAroundTime", VAL_LONG, &turn_around_time },
				}
			);
			break;
		case SIM_5:
		case SIM_6:
		case SIM_7:
			afprintf_list(LOG_OUTPUT,
				(struct ListOptions) { .items = 4 },
				(struct ListItem[]) {
					{ "PID", VAL_INT, &data.proc->iPID },
					{ "ResponseTime", VAL_LONG, &response_time },
					{ "TurnAroundTime", VAL_LONG, &turn_around_time },
					{ "PageFaults", VAL_INT, &data.proc->iPageFaults },
				}
			);
			break;
		}
		break;
	}

	fputs("]\n", LOG_OUTPUT);

	pthread_mutex_unlock(&log_state->mutex);
};

void log_finished(struct LogState* log_state, const unsigned int total_page_faults, const double average_page_faults)
{
	pthread_mutex_lock(&log_state->mutex);

	fputs("SIMULATION FINISHED: ", LOG_OUTPUT);

	fprintf_list(LOG_OUTPUT,
		(struct ListOptions) { .items = 2 },
		(struct ListItem) { "Total PageFaults", VAL_UINT, &total_page_faults },
		(struct ListItem) { "Average PageFaults", VAL_DOUBLE, &average_page_faults }
	);

	putc('\n', LOG_OUTPUT);

	pthread_mutex_unlock(&log_state->mutex);
};