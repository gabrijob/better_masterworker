#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <string>
#include <queue>



typedef struct data_struct {
  double comp_size;
  long wid;
}DATA;

typedef struct worker_info_struct {
  long wid;
  aid_t pid;
  int available;
}W_INFO;

typedef struct hb_data_struct {
  W_INFO* p_worker_info;
  std::string hb = "HEARTBEAT";
}HEARTBEAT_DATA;

void heartbeat(long wid, aid_t pid, int* available_ptr);
void receive_task(long wid, int* available_task_slots);

#endif