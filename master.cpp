#ifndef MASTER_CODE
#define MASTER_CODE


#include "master.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);

using namespace dfs;
using namespace job_info::available_task_slots;
using namespace job_info::chunk_execution;
using namespace job_info::processing;



long get_available_chunk_to_worker(long worker_id) {
    chunk_list_type* chunk_list = get_chunk_list_from_worker(worker_id);

    if(chunk_list != nullptr){
      for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
        if(is_chunk_available(*chunk_p))
            return *chunk_p;
      }
    }
    return -1;
}


DATA* make_data(double comp_size, long worker_id, long chunk_id, int exec_type) {
    DATA* data = nullptr;
    data = (DATA*) malloc(sizeof(DATA));
    data->comp_size = comp_size;
    data->wid = worker_id;
    data->chunk_id = chunk_id;
    data->execution_type = exec_type;

    return data;
}




void fill_task_queue(data_queue_ptr_type task_data_queue, long number_of_tasks, double comp_size) {

    DATA *data = nullptr;


    for (int i = 0; i < number_of_tasks; i++) { /* For each task to be executed: */
      if (number_of_tasks < 10000 || i % 10000 == 0)
        XBT_INFO("Sending \"%s\" (of %ld) to queue", (std::string("Task_") + std::to_string(i+1)).c_str(),
                 number_of_tasks);


      data = make_data(comp_size, -1, -1, LOCAL);

      if(task_data_queue){ 
        task_data_queue->push_back(data);
      }
      else task_data_queue.reset(new std::list<DATA*>);
    }
}



//returns the number of functioning workers
long listen_heartbeats(data_queue_ptr_type task_data_queue, double comm_size, double comp_size, long workers_count) {

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

            mailbox = simgrid::s4u::Mailbox::byName(std::string("worker-") + std::to_string(worker_info->wid));
            data = task_data_queue->front();

            if(data->execution_type != REMOTE){
                data->chunk_id = get_available_chunk_to_worker(worker_info->wid);
                
                if(data->chunk_id == -1) 
                    data->execution_type = NO_TASK;
                else    
                    task_data_queue->pop_front();
            }
            

            XBT_INFO("Sending task to worker-%ld to execute chunk %ld", worker_info->wid, data->chunk_id);
            mailbox->put(data, comm_size);
            }
        }
        //failing heartbeat
        else if(worker_info->msg.compare("FAILING") == 0){ 
            long_vector_ptr chunks_lost = get_chunks_executing_at_worker(worker_info->wid);

            long_vector_ptr workers_with_chunk;

            for(unsigned int i = 0; i < chunks_lost->size(); i++){
                workers_with_chunk = find_workers_with_chunk(chunks_lost->at(i));

                if(!workers_with_chunk->empty()){                
                    mark_chunk_available(chunks_lost->at(i));

                    data = make_data(comp_size, -1, -1, LOCAL_REC);                        
                    task_data_queue->push_back(data);
                }
                else{
                    data = make_data(comp_size, -1, chunks_lost->at(i), REMOTE);
                    task_data_queue->push_back(data);
                }                 
            }
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

        data = make_data(-1.0, i % workers_count, -1, -1); 
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