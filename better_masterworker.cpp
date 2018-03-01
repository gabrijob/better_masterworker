/* Copyright (c) 2010-2017. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */


#include "common.hpp"
#include "dfs.hpp"
#include "master.hpp"
#include "worker.hpp"


XBT_LOG_NEW_DEFAULT_CATEGORY(better_masterworker, "Messages specific for this s4u example");

//----------------------------------------------------------------------------------------------
//------------------------------MASTER-&-WORKER-------------------------------------------------

class Master {
  long number_of_tasks             = 0; /* - Number of tasks      */
  double comp_size                 = 0; /* - Task compute cost    */
  double comm_size                 = 0; /* - Task communication size */
  long workers_count               = 0; /* - Number of workers    */
  simgrid::s4u::MailboxPtr mailbox = nullptr;
  data_queue_ptr_type task_data_queue;

public:
  explicit Master(std::vector<std::string> args)
  {
    xbt_assert(args.size() == 5, "The master function expects 4 arguments from the XML deployment file");

    number_of_tasks = std::stol(args[1]);
    comp_size       = std::stod(args[2]);
    comm_size       = std::stod(args[3]);
    workers_count   = std::stol(args[4]);

    task_data_queue.reset(new std::queue<DATA*>);
    
    XBT_INFO("Got %ld workers and %ld tasks to process", workers_count, number_of_tasks);
  }


  void operator()()
  {
    DATA *data = nullptr;
    W_INFO* worker_info = nullptr;

    Dfs my_dfs(workers_count);
    my_dfs.allocate_chunks(number_of_tasks);
    my_dfs.print_distribution();

    fill_task_queue(task_data_queue, number_of_tasks, comp_size);

    XBT_INFO("Awaiting heartbeats");
    workers_count = listen_heartbeats(task_data_queue, comm_size, workers_count);

    XBT_INFO("All tasks have been dispatched. Let's tell everybody the computation is over.");
    send_finish_task_to_all_workers(workers_count);

    wait_finish_msg(workers_count);    

    XBT_INFO("Master's work here is done");
  }
};

class Worker {
  long id = -1;
  TASK_EXEC_INFO *exec_info;
  simgrid::s4u::MailboxPtr mailbox = nullptr;
  simgrid::s4u::ActorPtr task_receiver = nullptr;

public:
  explicit Worker(std::vector<std::string> args)
  {
    xbt_assert(args.size() == 3, "The worker expects two arguments from the XML deployment file: "
                                 "its worker ID (its numerical rank) and if it will fail or not (boolean)");
    id      = std::stol(args[1]);
    mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(id));

    exec_info = new TASK_EXEC_INFO; //ici
    exec_info->will_fail = std::stol(args[2]);
    exec_info->available_task_slots = MAX_TASKS_PER_NODE;
  }

  void operator()()
  {
    XBT_INFO("Hello!");
    
    task_receiver = simgrid::s4u::Actor::createActor("receiver", simgrid::s4u::Host::current(), receive_task,id,exec_info);

    heartbeat(id, simgrid::s4u::this_actor::getPid(), exec_info);

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
