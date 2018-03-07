#ifndef MASTER_HEADER
#define MASTER_HEADER

#include "common.hpp"
#include "dfs.hpp"
#include "job_info.hpp"


typedef std::shared_ptr<std::list<DATA*>> data_queue_ptr_type;

/** Initializes the task queue with n tasks and the same computation size. */
void fill_task_queue(data_queue_ptr_type task_data_queue, long number_of_tasks, double comp_size);

/** Receives the heartbeats from workers and sends them a task from the task queue.
 *  If a worker fails the lost tasks will be replaced in the queue. */
long listen_heartbeats(data_queue_ptr_type task_data_queue, double comm_size, double comp_size, long workers_count);

/** Tells all the functioning workers that there are no more tasks. */
void send_finish_task_to_all_workers(long workers_count);

/** Receives confirmation from workers that they'll not execute more new tasks. */
void wait_finish_msg(long workers_count);


#endif
