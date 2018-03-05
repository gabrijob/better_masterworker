#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <memory>


#define MAX_TASKS_PER_NODE 3
#define NUMBER_OF_COPIES 2
#define HEARTBEAT_INTERVAL 0.5

enum{LOCAL, REMOTE, NO_TASK};

typedef struct data_struct {
  double comp_size;
  long wid;
  long chunk_id;
  int execution_type;
}DATA;

typedef struct worker_info_struct {
  long wid;
  aid_t pid;
  std::string msg;
}W_INFO;

typedef struct execution_info_struct {
  bool will_fail;
}EXEC_INFO;

typedef std::shared_ptr<EXEC_INFO> exec_info_ptr_type;

typedef std::shared_ptr<std::vector<long>> long_vector_ptr;


#endif