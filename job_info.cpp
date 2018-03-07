#ifndef JOBINFO_CODE
#define JOBINFO_CODE

#include "job_info.hpp"


XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);


namespace job_info
{
    namespace
    {
        /** Vector with the number of available task slot at iterator(worker). */
        std::vector<int> available_task_slots_at;

        /** Vector with the location a chunk is executing, or if it was processed already or not. */
        std::vector<long> chunk_executing_at;
    }

    namespace available_task_slots
    {

        void init_available_task_slots_at_vec(long number_of_workers) {
            available_task_slots_at.assign(number_of_workers, MAX_TASKS_PER_NODE);
        }

        void add_worker_to_available_task_slots_vec() {
            available_task_slots_at.push_back(MAX_TASKS_PER_NODE);
        }

        void remove_worker_from_available_task_slots_vec(long worker_id) {
            available_task_slots_at.erase( available_task_slots_at.begin() + worker_id );
        }

        void inc_task_slots_at_worker(long worker_id) {
            available_task_slots_at[worker_id] = available_task_slots_at[worker_id] + 1;
        }

        void dec_task_slots_at_worker(long worker_id) {
            available_task_slots_at[worker_id] = available_task_slots_at[worker_id] - 1;
        }

        void set_task_slots_at_worker(long worker_id, int value) {
            available_task_slots_at[worker_id] = value;
        }

        int get_task_slots_at_worker(long worker_id) {
            return available_task_slots_at[worker_id];
        }

        void print_available_task_slots_vec() {
            for(unsigned int i = 0; i < available_task_slots_at.size(); i++){
                XBT_INFO("worker %i has %i slots available", i, available_task_slots_at[i]);
            }
        }
    }



    namespace chunk_execution
    {
        
        void init_chunk_executing_at_vec(long number_of_chunks) {
            chunk_executing_at.assign(number_of_chunks, AVAILABLE);
        }


        void add_chunk_to_executing_at_vec() {
            chunk_executing_at.push_back(-1);
        }


        void remove_chunk_from_executing_at_vec(long chunk_id) {
            chunk_executing_at.erase( chunk_executing_at.begin() + chunk_id );
        }


        void set_worker_executing_chunk(long chunk_id, long worker_id) {
            chunk_executing_at[chunk_id] = worker_id;
        }


        long get_worker_executing_chunk(long chunk_id) {
            return chunk_executing_at[chunk_id];
        }


        void print_chunk_executing_at_vec() {
            for(unsigned int i = 0; i < chunk_executing_at.size(); i++){
                XBT_INFO("worker %i has %ld slots available", i, chunk_executing_at[i]);
            }
        }


        void mark_chunk_available(long chunk_id) {
            chunk_executing_at[chunk_id] = AVAILABLE;
        }


        bool is_chunk_available(long chunk_id) {
            if(chunk_executing_at[chunk_id] == AVAILABLE)
                return true;
            
            return false;
        }


        void mark_chunk_as_processed(long chunk_id) {
            chunk_executing_at[chunk_id] = FINISHED;
        }


        bool was_chunk_processed(long chunk_id) {
            return chunk_executing_at[chunk_id] == FINISHED;
        }


        long_vector_ptr get_chunks_executing_at_worker(long worker_id) {
            long_vector_ptr chunks;
            chunks.reset(new std::vector<long>);
            
            for(unsigned int chunk = 0; chunk < chunk_executing_at.size(); chunk++){
                if(chunk_executing_at[chunk] == worker_id)
                    chunks->push_back(chunk);
            }
            return chunks;
        }
    }
}



#endif