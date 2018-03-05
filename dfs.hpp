#ifndef DFS_HEADER
#define DFS_HEADER


#include "common.hpp"


typedef std::list<long> chunk_list_type;
typedef std::vector<chunk_list_type*> chunk_to_worker_vector_type;


namespace dfs
{

    void init_dfs(long number_of_workers);

    void allocate_chunks(long);

    void add_chunk_to_worker(long, long);

    long pop_chunk_from_worker(long);

    long_vector_ptr find_workers_with_chunk(long);

    void print_distribution();

}


#endif