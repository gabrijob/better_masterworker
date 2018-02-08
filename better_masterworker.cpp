/* Copyright (c) 2010-2017. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "xbt/sysdep.h"
#include <simgrid/s4u.hpp>
#include <string>

XBT_LOG_NEW_DEFAULT_CATEGORY(better_masterworker, "Messages specific for this s4u example");

//------------------------------------------EXTRA STUFF-----------------------------------
typedef struct data_struct {
  double comp_size;
  long wid;
}DATA;

//print the remaining percentage of a task execution every 0.5 seconds
static void printRemainingPercentage(long worker_id, simgrid::s4u::ExecPtr execution, bool* is_executing){
  while(*is_executing){
    XBT_INFO("Remaining percentage: %.2f%% on worker-%ld", execution->getRemainingRatio()*100, worker_id);
    simgrid::s4u::this_actor::sleep_for(0.5);
  }
  return;
}

class Task {
  double comp_size;
  long worker_id;
  bool* is_executing;
  simgrid::s4u::ExecPtr execution = nullptr;
  simgrid::s4u::ActorPtr printer_percentage = nullptr;

public:
  explicit Task(DATA* task_data, bool* is_exec_ptr)  //any this_actor method called on the contructor considers the creating actor "this_actor" not the created
  {
    comp_size = task_data->comp_size;
    worker_id = task_data->wid;
    free(task_data);
    is_executing = is_exec_ptr;
  }

  void operator()()
  {
    execution = simgrid::s4u::this_actor::exec_init(comp_size);
    execution->start(); 
    *is_executing = true; //so that the percentage printer can start printing  

    //create a percentage printer as another actor to tell the remanining percentage of the execution while it's running
    printer_percentage = simgrid::s4u::Actor::createActor("percentage printer", simgrid::s4u::Host::current(), printRemainingPercentage, worker_id, execution, is_executing);
  
    //start execution
    XBT_INFO("Starting execution");
    execution->wait();
    XBT_INFO("Execution ended");
    *is_executing = false;
  }
};
//----------------------------------------------------------------------------------------------
//------------------------------MASTER-&-WORKER-------------------------------------------------

class Master {
  long number_of_tasks             = 0; /* - Number of tasks      */
  double comp_size                 = 0; /* - Task compute cost    */
  double comm_size                 = 0; /* - Task communication size */
  long workers_count               = 0; /* - Number of workers    */
  simgrid::s4u::MailboxPtr mailbox = nullptr;

public:
  explicit Master(std::vector<std::string> args)
  {
    xbt_assert(args.size() == 5, "The master function expects 4 arguments from the XML deployment file");

    number_of_tasks = std::stol(args[1]);
    comp_size       = std::stod(args[2]);
    comm_size       = std::stod(args[3]);
    workers_count   = std::stol(args[4]);

    XBT_INFO("Got %ld workers and %ld tasks to process", workers_count, number_of_tasks);
  }

  void operator()()
  {
    DATA *data = nullptr;
    for (int i = 0; i < number_of_tasks; i++) { /* For each task to be executed: */
      /* - Select a @ref worker in a round-robin way */
      mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(i % workers_count));

      if (number_of_tasks < 10000 || i % 10000 == 0)
        XBT_INFO("Sending \"%s\" (of %ld) to mailbox \"%s\"", (std::string("Task_") + std::to_string(i+1)).c_str(),
                 number_of_tasks, mailbox->getCname());

      /* - Send the computation amount to the @ref worker */
      data = (DATA *) malloc(sizeof(DATA));
      data->comp_size = comp_size;
      data->wid = i % workers_count;
      mailbox->put(data, comm_size);
    }

    XBT_INFO("All tasks have been dispatched. Let's tell everybody the computation is over.");
    for (int i = 0; i < workers_count; i++) {
      /* - Eventually tell all the workers to stop by sending a "finalize" task */
      mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(i % workers_count));
      data = (DATA *) malloc(sizeof(DATA));
      data->comp_size = -1.0;
      data->wid = i % workers_count;
      mailbox->put(data, 0);
    }
  }
};

class Worker {
  long id = -1;
  bool* is_executing;
  simgrid::s4u::MailboxPtr mailbox = nullptr;
  simgrid::s4u::ActorPtr task_exec = nullptr;

public:
  explicit Worker(std::vector<std::string> args)
  {
    xbt_assert(args.size() == 2, "The worker expects a single argument from the XML deployment file: "
                                 "its worker ID (its numerical rank)");
    id      = std::stol(args[1]);
    mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(id));
  }

  void operator()()
  {
    XBT_INFO("Hello!");
    while (1) { /* The worker waits in an infinite loop for tasks sent by the \ref master */
      DATA* task_data = (DATA*) mailbox->get();
      xbt_assert(task_data != nullptr, "mailbox->get() failed");
      if (task_data->comp_size < 0) { /* - Exit when -1.0 is received */
        XBT_INFO("I'm done. See you!");
        break;
      }
      /*  - Otherwise, process the task */
      //flag to inform the percentage printer when to end
      is_executing = (bool*) malloc(sizeof(bool));
      *is_executing = false;

      //create a separate actor for the task execution  so that we can kill it if needed
      task_exec = simgrid::s4u::Actor::createActor("task", simgrid::s4u::Host::current(), Task(task_data,is_executing));
                
      //wait the end of the execution until the timeout of 1 second
      task_exec->join(1);
      //kill the task if timeout (if the task already ended kill() does nothing)
      *is_executing = false;  //inform percentage printer to stop printing
      task_exec->kill();     
    }
  }
};
//-------------------------------------------------------------------------------------------------
//---------------------------------------------------MAIN------------------------------------------

int main(int argc, char* argv[])
{
  simgrid::s4u::Engine e(&argc, argv);
  /*xbt_assert(argc > 2, "Usage: %s platform_file deployment_file\n"
                       "\tExample: %s msg_platform.xml msg_deployment.xml\n",
             argv[0], argv[0]);
  */
  //e.loadPlatform(argv[1]); /** - Load the platform description */
  e.loadPlatform("../platforms/small_platform.xml");
  e.registerFunction<Master>("master");
  e.registerFunction<Worker>("worker"); /** - Register the function to be executed by the processes */
  //e.loadDeployment(argv[2]);            /** - Deploy the application */
  e.loadDeployment("better_masterworker_d.xml"); 

  e.run(); /** - Run the simulation */

  XBT_INFO("Simulation time %g", e.getClock());

  return 0;
}
