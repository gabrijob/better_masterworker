#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <string>
#include <queue>

#define MAX_TASKS_PER_NODE 3
#define HEARTBEAT_INTERVAL 1

typedef struct data_struct {
  double comp_size;
  long wid;
}DATA;

typedef struct worker_info_struct {
  long wid;
  aid_t pid;
  int available;
  std::string msg;
}W_INFO;


void heartbeat(long wid, aid_t pid, int* available_ptr);
void receive_task(long wid, int* available_task_slots);

#endif