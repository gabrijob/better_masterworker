#ifndef WORKER_HEADER
#define WORKER_HEADER

#include "common.hpp"

void heartbeat(long wid, aid_t pid, TASK_EXEC_INFO *exec_info);
void receive_task(long wid, TASK_EXEC_INFO *exec_info);



#endif