/* Copyright (c) 2010-2017. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */


#include "common.hpp"
#include "dfs.hpp"
#include "master.hpp"
#include "worker.hpp"



XBT_LOG_NEW_DEFAULT_CATEGORY(better_masterworker, "Messages specific for this s4u example");


using namespace dfs;
using namespace job_info::available_task_slots;
using namespace job_info::chunk_execution;




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

    task_data_queue.reset(new std::list<DATA*>);
    
    XBT_INFO("Got %ld workers and %ld tasks to process", workers_count, number_of_tasks);
  }


  void operator()()
  {
    //initialize dfs structure
    init_dfs(workers_count);
    allocate_chunks(number_of_tasks);
    print_distribution();

    //initialize general job informations structures
    init_available_task_slots_at_vec(workers_count);
    init_chunk_executing_at_vec(number_of_tasks);


    fill_task_queue(task_data_queue, number_of_tasks, comp_size);

    XBT_INFO("Awaiting heartbeats");
    workers_count = listen_heartbeats(task_data_queue, comm_size, comp_size, workers_count);

    XBT_INFO("All tasks have been dispatched. Let's tell everybody the computation is over.");
    send_finish_task_to_all_workers(workers_count);

    wait_finish_msg(workers_count);    

    XBT_INFO("Master's work here is done");
  }
};



class Worker {
  long id = -1;
  exec_info_ptr_type e_info_ptr;
  simgrid::s4u::MailboxPtr mailbox = nullptr;
  simgrid::s4u::ActorPtr task_receiver = nullptr;

public:
  explicit Worker(std::vector<std::string> args)
  {
    xbt_assert(args.size() == 3, "The worker expects two arguments from the XML deployment file: "
                                 "its worker ID (its numerical rank) and if it will fail or not (boolean)");
    id      = std::stol(args[1]);
    mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(id));

    e_info_ptr.reset(new EXEC_INFO);
    e_info_ptr->will_fail = std::stol(args[2]);
  }

  void operator()()
  {
    XBT_INFO("Hello!");
    
    task_receiver = simgrid::s4u::Actor::createActor("receiver", simgrid::s4u::Host::current(), receive_task,id/*,exec_info*/);

    heartbeat(id, simgrid::s4u::this_actor::getPid(), e_info_ptr);

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
