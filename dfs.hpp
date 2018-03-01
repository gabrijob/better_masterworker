#ifndef DFS_HEADER
#define DFS_HEADER


#include "common.hpp"


typedef std::list<long> chunk_list_type;
typedef std::vector<chunk_list_type*> chunk_to_worker_vector_type;


class Dfs
{


  public:

    explicit Dfs(long number_of_workers);

    void allocate_chunks(long);

    void add_chunk_to_worker(long, long);

    std::vector<long> find_workers_with_chunk(long);

    void print_distribution();

};


#endif