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
  int* available_task_slots;
  simgrid::s4u::ExecPtr execution = nullptr;
  simgrid::s4u::ActorPtr printer_percentage = nullptr;

public:
  explicit Task(DATA* task_data, int* available)  //any this_actor method called on the contructor considers the creating actor "this_actor" not the created
  {
    comp_size = task_data->comp_size;
    worker_id = task_data->wid;
    free(task_data);
    available_task_slots = available;
  }

  void operator()()
  {
    execution = simgrid::s4u::this_actor::exec_init(comp_size);
    execution->start();  

    //create a percentage printer as another actor to tell the remanining percentage of the execution while it's running
    printer_percentage = simgrid::s4u::Actor::createActor("percentage printer", simgrid::s4u::Host::current(), printRemainingPercentage, worker_id, execution/*, is_executing*/);
  
    //start execution
    XBT_INFO("Starting execution");
    execution->wait();
    *available_task_slots = *available_task_slots + 1;
    XBT_INFO("Execution ended");
    printer_percentage->kill();
  }
};

void heartbeat(long wid, aid_t pid, int* available_ptr){
  W_INFO* w_info = (W_INFO*) malloc(sizeof(W_INFO));
  w_info->wid = wid;
  w_info->pid = pid;
  w_info->msg = "HEARTBEAT";

  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
  while(*available_ptr >= 0){
    XBT_INFO("TUM TUM");
    w_info->available = *available_ptr;
    mailbox->put(w_info, 0);
    simgrid::s4u::this_actor::sleep_for(HEARTBEAT_INTERVAL);
  }

  w_info->msg = "TERMINATED";
  mailbox->put(w_info, 0);  
}

void receive_task(long wid, int* available_task_slots){
  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(wid));
  while (1) { 
      DATA* task_data = (DATA*) mailbox->get();
      task_data->wid = wid;
      xbt_assert(task_data != nullptr, "mailbox->get() failed");
      if (task_data->comp_size < 0) { /* - Exit when -1.0 is received */
        *available_task_slots = - MAX_TASKS_PER_NODE - 1; //to make sure it is a negative number even when all tasks end at the same time
        XBT_INFO("I'm done receiving tasks. See you!");
        break;
      }
      if(*available_task_slots > 0){
        *available_task_slots = *available_task_slots - 1;     
        simgrid::s4u::ActorPtr task_exec = simgrid::s4u::Actor::createActor("task", simgrid::s4u::Host::current(), Task(task_data, available_task_slots));
      }
      simgrid::s4u::this_actor::yield();
    }
}

#endif