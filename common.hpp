#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <memory>


#define DEBUG 0

#define MAX_TASKS_PER_NODE 3
#define NUMBER_OF_COPIES 2
#define HEARTBEAT_INTERVAL 3
#define REMAINING_PERC_INTERVAL 1


enum{LOCAL, REMOTE, LOCAL_REC, NO_TASK};

//Sent by master to workers with data about the task
typedef struct data_struct {
  double comp_size;
  long wid;
  long chunk_id;
  int execution_type;
}DATA;

//Sent by workers to master on heartbeat
typedef struct worker_info_struct {
  long wid;
  aid_t pid;
  std::string msg;
}W_INFO;

//Used only(at the moment) by workers to know how it's execution will play out
typedef struct execution_info_struct {
  bool will_fail;
}EXEC_INFO;

typedef std::shared_ptr<EXEC_INFO> exec_info_ptr_type;

typedef std::shared_ptr<std::vector<long>> long_vector_ptr;


#endif