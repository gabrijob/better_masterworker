#ifndef MASTER_CODE
#define MASTER_CODE


#include "master.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);

using namespace dfs;
using namespace job_info::available_task_slots;
using namespace job_info::chunk_execution;
using namespace job_info::processing;



void fill_task_queue(data_queue_ptr_type task_data_queue, long number_of_tasks, double comp_size) {

    DATA *data = nullptr;


    for (int i = 0; i < number_of_tasks; i++) { /* For each task to be executed: */
      if (number_of_tasks < 10000 || i % 10000 == 0)
        XBT_INFO("Sending \"%s\" (of %ld) to queue", (std::string("Task_") + std::to_string(i+1)).c_str(),
                 number_of_tasks);

      data = (DATA *) malloc(sizeof(DATA));
      data->comp_size = comp_size;
      data->chunk_id = -1;
      if(task_data_queue){ 
        task_data_queue->push(data);
      }
      else task_data_queue.reset(new std::queue<DATA*>);
    }
}


//returns the number of functioning workers
long listen_heartbeats(data_queue_ptr_type task_data_queue, double comm_size, long workers_count) {

    simgrid::s4u::MailboxPtr mailbox = nullptr;
    DATA *data = nullptr;
    W_INFO* worker_info = nullptr;


    while(!task_data_queue->empty()){
        mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
        worker_info = (W_INFO*) mailbox->get();
        XBT_INFO("Receiveing heartbeat data");
        xbt_assert(worker_info != nullptr, "mailbox->get() failed");

        //regular heartbeat
        if(worker_info->msg.compare("HEARTBEAT") == 0){
            if(get_task_slots_at_worker(worker_info->wid) > 0){
            XBT_INFO("Sending task to worker-%ld", worker_info->wid);
            mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(worker_info->wid));
            data = task_data_queue->front();
            data->chunk_id = pop_chunk_from_worker(worker_info->wid); //get unprocessed chunk
            task_data_queue->pop();
            mailbox->put(data, comm_size);
            }
        }
        //failing heartbeat
        else if(worker_info->msg.compare("FAILING") == 0){ 
            long_vector_ptr chunks = get_chunks_executing_at_worker(worker_info->wid);

            long_vector_ptr workers_with_chunk;
/*
            for(unsigned int i = 0; i < chunks->size(); i++){
                workers_with_chunk = find_workers_with_chunk(chunks[i]);
                for(unsigned int j = 0; j < workers_with_chunk->size(); j++){
                    if(get_task_slots_at_worker(workers_with_chunks[j] > 0)){
                        //mandar pro dito worker
                        //ou colocar na queue o tipo DIRECT ou alguma coisa do tipo
                        //MUDAR DE QUEUE PRA LIST E COLOCAR COMO PRIMEIRO DA QUEUE
                        //break; ou mudar pra um while sei la
                    }
                }
                if(workers_with_chunk->empty()){
                    data = (DATA*) malloc(sizeof(DATA));
                    data->chunk_id = chunks[i];
                    //colocar na queue com o tipo REMOTE
                }
                    
                
            }
*/
            workers_count--;            
        } 
    } 

    return workers_count;
}


void send_finish_task_to_all_workers(long workers_count) {

    simgrid::s4u::MailboxPtr mailbox = nullptr;   
    DATA *data = nullptr;

    for (int i = 0; i < workers_count; i++) {
        /* - Eventually tell all the workers to stop by sending a "finalize" task */
        mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(i % workers_count));
        data = (DATA *) malloc(sizeof(DATA)); //ici
        data->comp_size = -1.0;
        data->wid = i % workers_count;
        mailbox->put(data, 0);
    }

}


void wait_finish_msg(long workers_count) {

    simgrid::s4u::MailboxPtr mailbox = nullptr;
    W_INFO* worker_info = nullptr;

    while(workers_count){
      mailbox = simgrid::s4u::Mailbox::byName(std::string("MASTER_MAILBOX"));
      worker_info = (W_INFO*) mailbox->get();
      xbt_assert(worker_info != nullptr, "mailbox->get() failed");
      if(worker_info->msg.compare("TERMINATED") == 0){
        workers_count--;
      }
    }   
    
}




#endif