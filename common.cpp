#ifndef COMMON_CODE
#define COMMON_CODE

#include "common.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);

//print the remaining percentage of a task execution every 0.5 seconds
static void printRemainingPercentage(long worker_id, simgrid::s4u::ExecPtr execution){
  while(1){
    XBT_INFO("Remaining percentage: %.2f%% on worker-%ld", execution->getRemainingRatio()*100, worker_id);
    simgrid::s4u::this_actor::sleep_for(0.5);
  }
  return;
}

class Task {
  double comp_size;
  long worker_id;
  long chunk_id;
  TASK_EXEC_INFO *exec_info;
  simgrid::s4u::ExecPtr execution = nullptr;
  simgrid::s4u::ActorPtr printer_percentage = nullptr;

public:
  explicit Task(DATA* task_data, TASK_EXEC_INFO *task_exec_info)  //any this_actor method called on the contructor considers the creating actor "this_actor" not the created
  {
    comp_size = task_data->comp_size;
    worker_id = task_data->wid;
    chunk_id = task_data->chunk_id;
    free(task_data);
    exec_info = task_exec_info;
  }

  void operator()()
  {
    execution = simgrid::s4u::this_actor::exec_init(comp_size);
    execution->start();  

    //create a percentage printer as another actor to tell the remanining percentage of the execution while it's running
    printer_percentage = simgrid::s4u::Actor::createActor("percentage printer", simgrid::s4u::Host::current(), printRemainingPercentage, worker_id, execution/*, is_executing*/);
  
    //start execution
    //exec_info->currently_executing->push_back(chunk_id);
    XBT_INFO("Starting execution");
    execution->wait();
    XBT_INFO("Execution ended");
    exec_info->available_task_slots = exec_info->available_task_slots + 1;
    //exec_info->currently_executing->pop_back();
    printer_percentage->kill();
  }
};

void heartbeat(long wid, aid_t pid, TASK_EXEC_INFO *exec_info){
  W_INFO* w_info = (W_INFO*) malloc(sizeof(W_INFO));
  w_info->wid = wid;
  w_info->pid = pid;
  w_info->msg = "HEARTBEAT";

  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
  while(exec_info->available_task_slots >= 0){
    XBT_INFO("TUM TUM");
    w_info->available = exec_info->available_task_slots;
    mailbox->put(w_info, 0);
    simgrid::s4u::this_actor::sleep_for(HEARTBEAT_INTERVAL);

    if(exec_info->will_fail){
    //the worker will send a final heartbeat so that the master knows it is failing
      w_info->msg = "FAILING";
      mailbox->put(w_info, 0); 
      XBT_INFO("Host failing");
      simgrid::s4u::Host* my_host = simgrid::s4u::Host::current();
      my_host->turnOff();
    }
  }

  w_info->msg = "TERMINATED";
  mailbox->put(w_info, 0);  
}

void receive_task(long wid, TASK_EXEC_INFO *exec_info){
  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(wid));
  while (1) { 
      DATA* task_data = (DATA*) mailbox->get();
      xbt_assert(task_data != nullptr, "mailbox->get() failed");
      task_data->wid = wid;
      if (task_data->comp_size < 0) { /* - Exit when -1.0 is received */
        exec_info->available_task_slots = - MAX_TASKS_PER_NODE - 1; //to make sure it is a negative number even when all tasks end at the same time
        XBT_INFO("I'm done receiving tasks. See you!");
        break;
      }
      if(exec_info->available_task_slots > 0){
       exec_info->available_task_slots = exec_info->available_task_slots - 1;     
        simgrid::s4u::ActorPtr task_exec = simgrid::s4u::Actor::createActor("task", simgrid::s4u::Host::current(), Task(task_data, exec_info));
      }
      simgrid::s4u::this_actor::yield();
    }
}



Dfs::Dfs(long number_of_workers)
{
  vector_size = number_of_workers;

  for(long i = 0; i < vector_size; i++){
    chunk_list_type* chunk_list = new chunk_list_type;
    chunk_worker_v.push_back(chunk_list);
  }
}

void Dfs::allocate_chunks(long number_of_chunks){
  long chunk_id;
  long owner_id;

  if(NUMBER_OF_COPIES >= vector_size){
    for(chunk_id = 0; chunk_id < number_of_chunks; chunk_id++){
      for(owner_id = 0; owner_id < vector_size; owner_id++){
        add_chunk_to_worker(chunk_id, owner_id);
      }
    }
  }
  else{
    for(chunk_id = 0; chunk_id < number_of_chunks; chunk_id++){
      for(int copy_n = 0; copy_n < NUMBER_OF_COPIES; copy_n++){
        owner_id = ((chunk_id % vector_size) + ((vector_size / NUMBER_OF_COPIES) * copy_n)
                   ) % vector_size;
        add_chunk_to_worker(chunk_id, owner_id);
      }
    }
  }
}

void Dfs::add_chunk_to_worker(long chunk_id, long worker_id)
{
  chunk_list_type* chunk_list = chunk_worker_v[worker_id];
  chunk_list->push_back(chunk_id);
}

std::vector<long> Dfs::find_workers_with_chunk(long chunk_id)
{
  std::vector<long> workers_id_v;
  chunk_list_type* chunk_list;

  for(long worker = 0; worker < vector_size; worker++){
    chunk_list = chunk_worker_v[worker];
    for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
      if(*chunk_p == chunk_id)
        workers_id_v.push_back(worker);
    }
  }

  return workers_id_v; //must test if empty after
}

void Dfs::print_distribution(){
  chunk_list_type* chunk_list;

  for(long worker = 0; worker < vector_size; worker++){
    XBT_INFO("Worker-%ld has chunks:", worker);
    chunk_list = chunk_worker_v[worker];
    for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
      XBT_INFO(" %ld ", *chunk_p);
    }
  }
}


#endif