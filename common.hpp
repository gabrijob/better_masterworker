#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <memory>
#include "job_info.hpp"

#define MAX_TASKS_PER_NODE 3
#define NUMBER_OF_COPIES 2
#define HEARTBEAT_INTERVAL 0.5


typedef struct data_struct {
  double comp_size;
  long wid;
  long chunk_id;
}DATA;

typedef struct worker_info_struct {
  long wid;
  aid_t pid;
  int available;
  std::string msg;
  std::vector<long> currently_executing[NUMBER_OF_COPIES];
}W_INFO;

typedef struct task_execution_info_struct {
  bool will_fail;
  int available_task_slots;
  std::vector<long> currently_executing[NUMBER_OF_COPIES];
}TASK_EXEC_INFO;



#endif