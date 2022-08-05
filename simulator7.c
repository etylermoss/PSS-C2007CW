#include <pthread.h>
#include <stdlib.h>

#include "util.h"
#include "log.h"
#include "hash_table.h"
#include "process_queue.h"
#include "generator.h"
#include "simulator.h"
#include "terminator.h"
#include "paging_daemon.h"

int main(int argc, char** argv)
{
	struct HashTable* hash_table;
	struct ProcessQueue* pqueue_ready,* pqueue_terminated,* pqueue_pfaulted;
	struct LogState* log_state;
	pthread_t thread_generator, thread_simulator, thread_terminator, thread_paging_daemon;

	hash_table = hash_table_init(SIZE_OF_PROCESS_TABLE);
	pqueue_ready = pqueue_init();
	pqueue_terminated = pqueue_init();
	pqueue_pfaulted = pqueue_init();
	log_state = log_state_init(SIM_7);
	hash_table_pthread_init(hash_table);

	struct GeneratorArgs generator_args = { log_state, pqueue_ready, hash_table };
	struct SimulatorArgs simulator_args = { log_state, pqueue_ready, pqueue_terminated, pqueue_pfaulted };
	struct TerminatorArgs terminator_args = { log_state, pqueue_ready, pqueue_terminated, pqueue_pfaulted, hash_table };
	struct PagingDaemonArgs paging_daemon_args = { log_state, pqueue_ready, pqueue_pfaulted, hash_table };

	pthread_create(&thread_generator, NULL, &generator, &generator_args);
	pthread_create(&thread_simulator, NULL, &simulator, &simulator_args);
	pthread_create(&thread_terminator, NULL, &terminator, &terminator_args);
	pthread_create(&thread_paging_daemon, NULL, &paging_daemon, &paging_daemon_args);

	struct SimulatorResult* simulator_result;

	pthread_join(thread_generator, NULL);
	pthread_join(thread_simulator, ((void*) &simulator_result));
	pthread_join(thread_terminator, NULL);
	pthread_join(thread_paging_daemon, NULL);

	log_finished(log_state, simulator_result->total_page_faults, ((double) (simulator_result)->total_page_faults) / NUMBER_OF_PROCESSES);
	free(simulator_result);

	hash_table_pthread_free(hash_table);
	log_state_free(log_state);
	pqueue_free(pqueue_terminated);
	pqueue_free(pqueue_ready);
	pqueue_free(pqueue_pfaulted);
	hash_table_free(hash_table);

	return 0;
}