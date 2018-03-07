#ifndef WORKER_HEADER
#define WORKER_HEADER

#include "common.hpp"
#include "job_info.hpp"



/** Sends it's info to the master then sleeps for the pre-defined heartbeat interval amount of time.
 *  When a worker is supposed to fail, it'll tell the master and then shutdown.
 *  Will stop sending heartbeats when it's available task slots are set to a negative number.
*/
void heartbeat(long wid, aid_t pid, exec_info_ptr_type exec_info_ptr);

/** Receives the task data from the master and creates a new actor to execute it.
 *  If the computation size is a negative number it'll stop receiving tasks.
 *  If the task is a NO_TASK type it will not execute.
*/
void receive_task(long wid);


#endif