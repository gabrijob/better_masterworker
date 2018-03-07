#ifndef JOBINFO_HEADER
#define JOBINFO_HEADER


#include "common.hpp"


#define AVAILABLE -1
#define FINISHED -2


namespace job_info
{
    namespace available_task_slots
    {
        /** Initializes the vector of available task slots per worker 
         *  with the maximum number of simnultaneous tasks on a node.*/
        void init_available_task_slots_at_vec(long number_of_workers);

        void add_worker_to_available_task_slots_vec();

        void remove_worker_from_available_task_slots_vec(long worker_id);

        void inc_task_slots_at_worker(long worker_id);

        void dec_task_slots_at_worker(long worker_id);

        void set_task_slots_at_worker(long worker_id, int value);

        int get_task_slots_at_worker(long worker_id);
    }

    namespace chunk_execution
    {
        /** Initializes all chunks as AVAILABLE. */
        void init_chunk_executing_at_vec(long number_of_chunks);

        void add_chunk_to_executing_at_vec();

        void remove_chunk_from_executing_at_vec(long chunk_id);

        void set_worker_executing_chunk(long chunk_id, long worker_id);

        long get_worker_executing_chunk(long chunk_id);

        void mark_chunk_available(long chunk_id);

        bool is_chunk_available(long chunk_id);

        void mark_chunk_as_processed(long chunk_id);

        bool was_chunk_processed(long chunk_id);
        
        /** Returns a pointer to a vector with the chunks executing at a worker at the moment. */
        long_vector_ptr get_chunks_executing_at_worker(long worker_id);
    }
}


#endif
