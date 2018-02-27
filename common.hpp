#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <string>
#include <list>
#include <vector>
#include <queue>

#define MAX_TASKS_PER_NODE 3
#define NUMBER_OF_COPIES 2
#define HEARTBEAT_INTERVAL 0.5


typedef std::list<long> chunk_list_type;
typedef std::vector<chunk_list_type*> chunk_to_worker_vector_type;


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


void heartbeat(long wid, aid_t pid, TASK_EXEC_INFO *exec_info);
void receive_task(long wid, TASK_EXEC_INFO *exec_info);

class Dfs
{
  long vector_size;
  chunk_to_worker_vector_type chunk_worker_v;

  public:
    explicit Dfs(long number_of_workers);
    void allocate_chunks(long);
    void add_chunk_to_worker(long, long);
    std::vector<long> find_workers_with_chunk(long);
    void print_distribution();
};

#endif