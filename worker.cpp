#ifndef WORKER_CODE
#define WORKER_CODE

#include "worker.hpp"



XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);

static void printRemainingPercentage(long worker_id, simgrid::s4u::ExecPtr execution);


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
    exec_info->currently_executing->push_back(chunk_id);
    XBT_INFO("Starting execution");
    execution->wait();
    XBT_INFO("Execution ended");
    exec_info->available_task_slots = exec_info->available_task_slots + 1;
    exec_info->currently_executing->pop_back();
    printer_percentage->kill();
  }
};



void heartbeat(long wid, aid_t pid, TASK_EXEC_INFO *exec_info){
  W_INFO* w_info = (W_INFO*) malloc(sizeof(W_INFO)); //pas ici
  w_info->wid = wid;
  w_info->pid = pid;
  w_info->msg = "HEARTBEAT";

  simgrid::s4u::MailboxPtr mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
  while(exec_info->available_task_slots >= 0){
    XBT_INFO("TUM TUM");
    w_info->available = exec_info->available_task_slots;
    w_info->currently_executing->assign(exec_info->currently_executing->begin(),exec_info->currently_executing->end());
    /*
    std::vector<long> printv(w_info->currently_executing->begin(), w_info->currently_executing->end());
    if(printv.empty()) XBT_INFO("EMPTY");
    else XBT_INFO("IN VECTOR: %ld %ld" , printv[0], printv[1]);
    */
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



//print the remaining percentage of a task execution every 0.5 seconds
static void printRemainingPercentage(long worker_id, simgrid::s4u::ExecPtr execution){
  while(1){
    XBT_INFO("Remaining percentage: %.2f%% on worker-%ld", execution->getRemainingRatio()*100, worker_id);
    simgrid::s4u::this_actor::sleep_for(0.5);
  }
  return;
}


#endif