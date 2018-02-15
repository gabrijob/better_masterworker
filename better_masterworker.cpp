/* Copyright (c) 2010-2017. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */


#include "common.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(better_masterworker, "Messages specific for this s4u example");

//----------------------------------------------------------------------------------------------
//------------------------------MASTER-&-WORKER-------------------------------------------------

class Master {
  long number_of_tasks             = 0; /* - Number of tasks      */
  double comp_size                 = 0; /* - Task compute cost    */
  double comm_size                 = 0; /* - Task communication size */
  long workers_count               = 0; /* - Number of workers    */
  simgrid::s4u::MailboxPtr mailbox = nullptr;
  std::queue<DATA*> task_data_list;

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
    HEARTBEAT_DATA* hb_data = nullptr;
    
    for (int i = 0; i < number_of_tasks; i++) { /* For each task to be executed: */
      if (number_of_tasks < 10000 || i % 10000 == 0)
        XBT_INFO("Sending \"%s\" (of %ld) to queue", (std::string("Task_") + std::to_string(i+1)).c_str(),
                 number_of_tasks);

      data = (DATA *) malloc(sizeof(DATA));
      data->comp_size = comp_size;
      task_data_list.push(data);
    }

    XBT_INFO("Waiting heartbeats");
    while(!task_data_list.empty()){
      mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
      hb_data = (HEARTBEAT_DATA*) mailbox->get();
      XBT_INFO("Receiveing heartbeat data");
      xbt_assert(hb_data != nullptr, "mailbox->get() failed");
      if(hb_data->p_worker_info->available > 0){
        XBT_INFO("Sending task to worker-%ld", hb_data->p_worker_info->wid);
        mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(hb_data->p_worker_info->wid));
        data = task_data_list.front();
        task_data_list.pop();
        mailbox->put(data, comm_size);
      }
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

    XBT_INFO("Master out");
  }
};

class Worker {
  long id = -1;
  bool* is_executing;
  int* available_task_slots;
  simgrid::s4u::MailboxPtr mailbox = nullptr;
  simgrid::s4u::ActorPtr task_receiver = nullptr;

public:
  explicit Worker(std::vector<std::string> args)
  {
    xbt_assert(args.size() == 2, "The worker expects a single argument from the XML deployment file: "
                                 "its worker ID (its numerical rank)");
    id      = std::stol(args[1]);
    mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(id));
    available_task_slots = (int*) malloc(sizeof(int));
    *available_task_slots = 2;
  }

  void operator()()
  {
    XBT_INFO("Hello!");
    
    task_receiver = simgrid::s4u::Actor::createActor("receiver", simgrid::s4u::Host::current(), receive_task,id,available_task_slots);

    heartbeat(id, simgrid::s4u::this_actor::getPid(), available_task_slots);

    XBT_INFO("Worker-%ld out", id);
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
