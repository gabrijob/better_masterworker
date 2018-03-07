#ifndef DFS_HEADER
#define DFS_HEADER


#include "common.hpp"


typedef std::list<long> chunk_list_type;
typedef std::vector<chunk_list_type*> chunk_to_worker_vector_type;


namespace dfs
{
    /** Creates the vector of chunks for each worker. */
    void init_dfs(long number_of_workers);

    /** Allocates the chunks to the workers based on the number of chunks, workers and chunk copies. */
    void allocate_chunks(long number_of_chunks);

    /** Allocates a new chunk to a worker. */
    void add_chunk_to_worker(long chunk_id, long worker_id);

    /** Returns the list of chunks owned by a worker. */
    chunk_list_type* get_chunk_list_from_worker(long worker_id);

    /** Pops the front of the chunks list of a worker. */
    long pop_chunk_from_worker(long worker_id);

    /** Returns a pointer to a vector with all workers owning a chunk. */
    long_vector_ptr find_workers_with_chunk(long chunk_id);

    /** Prints the distribution of chunks to worker.*/
    void print_distribution();

}


#endif