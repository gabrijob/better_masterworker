#ifndef WORKER_CODE
#define WORKER_CODE

#include "worker.hpp"



XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);

using namespace job_info::available_task_slots;
using namespace job_info::chunk_execution;


static void printRemainingPercentage(long worker_id, simgrid::s4u::ExecPtr execution);


class Task {
  double comp_size;
  long worker_id;
  long chunk_id;
  simgrid::s4u::ExecPtr execution = nullptr;
  simgrid::s4u::ActorPtr printer_percentage = nullptr;

public:
  explicit Task(DATA* task_data)  //any this_actor method called on the contructor considers the creating actor "this_actor" not the created
  {
    comp_size = task_data->comp_size;
    worker_id = task_data->wid;
    chunk_id = task_data->chunk_id;
    free(task_data);
  }

  void operator()()
  {
    execution = simgrid::s4u::this_actor::exec_init(comp_size);
    execution->start();  

    //create a percentage printer as another actor to tell the remanining percentage of the execution while it's running
    printer_percentage = simgrid::s4u::Actor::createActor("percentage printer", simgrid::s4u::Host::current(), printRemainingPercentage, worker_id, execution);
  
    //start execution
    XBT_INFO("Starting execution");
    set_worker_executing_chunk(chunk_id, worker_id);
    execution->wait();
    XBT_INFO("Execution ended");
    //remove_chunk_from_executing_at_vec(chunk_id);
    inc_task_slots_at_worker(worker_id);
    printer_percentage->kill();
  }
};



void heartbeat(long wid, aid_t pid, exec_info_ptr_type exec_info_ptr){
  W_INFO* w_info = (W_INFO*) malloc(sizeof(W_INFO));
  w_info->wid = wid;
  w_info->pid = pid;

  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
  while(get_task_slots_at_worker(wid) >= 0){
    XBT_INFO("TUM TUM");
    w_info->msg = "HEARTBEAT";
    mailbox->put(w_info, 0);
    simgrid::s4u::this_actor::sleep_for(HEARTBEAT_INTERVAL);

    if(exec_info_ptr->will_fail){
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



void receive_task(long wid){
  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(wid));
  while (1) { 
      DATA* task_data = (DATA*) mailbox->get();
      xbt_assert(task_data != nullptr, "mailbox->get() failed");
      task_data->wid = wid;
      if (task_data->comp_size < 0) { /* - Exit when -1.0 is received */        
        set_task_slots_at_worker(wid, - MAX_TASKS_PER_NODE - 1); //to make sure it is a negative number even when all tasks end at the same time
        XBT_INFO("I'm done receiving tasks. See you!");
        break;
      }
      if(get_task_slots_at_worker(wid) > 0){        
        dec_task_slots_at_worker(wid);
        simgrid::s4u::ActorPtr task_exec = simgrid::s4u::Actor::createActor("task", simgrid::s4u::Host::current(), Task(task_data));
      }
      simgrid::s4u::this_actor::yield();
    }
}



//print the remaining percentage of a task execution every 0.5 seconds
static void printRemainingPercentage(long worker_id, simgrid::s4u::ExecPtr execution){
  while(1){
    XBT_INFO("Remaining percentage: %.2f%% on worker-%ld", execution->getRemainingRatio()*100, worker_id);
    simgrid::s4u::this_actor::sleep_for(0.5);
  }
  return;
}


#endif