#ifndef JOBINFO_CODE
#define JOBINFO_CODE

#include "common.hpp"

namespace job_info
{
    namespace
    {
        std::vector<int> available_task_slots_at;
        std::vector<long> chunk_executing_at;
    }

    namespace available_task_slots
    {

        void init_available_task_slots_at_vec(long number_of_workers) {
            available_task_slots_at.assign(number_of_workers, NUMBER_OF_COPIES);
        }

        void add_worker_to_available_task_slots_vec() {
            available_task_slots_at.push_back(NUMBER_OF_COPIES);
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
    }

    namespace chunk_execution
    {
        
        void init_chunk_executing_at_vec(long number_of_chunks) {
            chunk_executing_at.assign(number_of_chunks, -1);
        }

        void add_chunk_to_executing_at_vec() {
            chunk_executing_at.push_back(-1);
        }

        void remove_chunk_from_executing_at_vec(long chunk_id) {
            chunk_executing_at.erase( chunk_executing_at.begin() + chunk_id);
        }

        void set_worker_executing_chunk(long chunk_id, long worker_id) {
            chunk_executing_at[chunk_id] = worker_id;
        }

        long get_worker_executing_chunk(long chunk_id) {
            return chunk_executing_at[chunk_id];
        }
    }

}



#endif