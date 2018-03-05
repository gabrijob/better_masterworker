#ifndef WORKER_HEADER
#define WORKER_HEADER

#include "common.hpp"
#include "job_info.hpp"


void heartbeat(long wid, aid_t pid, exec_info_ptr_type exec_info_ptr);
void receive_task(long wid);



#endif