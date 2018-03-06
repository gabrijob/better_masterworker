#ifndef MASTER_HEADER
#define MASTER_HEADER

#include "common.hpp"
#include "dfs.hpp"
#include "job_info.hpp"


typedef std::shared_ptr<std::list<DATA*>> data_queue_ptr_type;

void fill_task_queue(data_queue_ptr_type task_data_queue, long number_of_tasks, double comp_size);
long listen_heartbeats(data_queue_ptr_type task_data_queue, double comm_size, double comp_size, long workers_count);
void send_finish_task_to_all_workers(long workers_count);
void wait_finish_msg(long workers_count);


#endif
