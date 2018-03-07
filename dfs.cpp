#ifndef DFS_CODE
#define DFS_CODE

#include "dfs.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);

namespace dfs
{

  namespace
  {
    long vector_size;
  
    chunk_to_worker_vector_type chunk_worker_v;
  }


  void init_dfs(long number_of_workers)
  {
    vector_size = number_of_workers;

    for(long i = 0; i < vector_size; i++){
      chunk_list_type* chunk_list = new chunk_list_type;
      chunk_worker_v.push_back(chunk_list);
    }
  }



  void allocate_chunks(long number_of_chunks){
    long chunk_id;
    long owner_id;

    if(NUMBER_OF_COPIES >= vector_size){
      for(chunk_id = 0; chunk_id < number_of_chunks; chunk_id++){
        for(owner_id = 0; owner_id < vector_size; owner_id++){
          add_chunk_to_worker(chunk_id, owner_id);
        }
      }
    }
    else{
      for(chunk_id = 0; chunk_id < number_of_chunks; chunk_id++){
        for(int copy_n = 0; copy_n < NUMBER_OF_COPIES; copy_n++){
          owner_id = ((chunk_id % vector_size) + ((vector_size / NUMBER_OF_COPIES) * copy_n)
                    ) % vector_size;
          add_chunk_to_worker(chunk_id, owner_id);
        }
      }
    }
  }



  void add_chunk_to_worker(long chunk_id, long worker_id)
  {
    chunk_list_type* chunk_list = chunk_worker_v[worker_id];
    chunk_list->push_back(chunk_id);
  }



  chunk_list_type* get_chunk_list_from_worker(long worker_id)
  {
    chunk_list_type* chunk_list;
    chunk_list = chunk_worker_v[worker_id];
    return chunk_list;
  }



  long pop_chunk_from_worker(long worker_id)
  {
    chunk_list_type* chunk_list;
    chunk_list = chunk_worker_v[worker_id];

    if(chunk_list != nullptr){
      long chunk = chunk_list->front();
      chunk_list->pop_front();
      return chunk;
    }
    else return -1;
  }



  long_vector_ptr find_workers_with_chunk(long chunk_id)
  {
    long_vector_ptr workers_id_v;
    workers_id_v.reset(new std::vector<long>);
    chunk_list_type* chunk_list;

    for(long worker = 0; worker < vector_size; worker++){
      chunk_list = chunk_worker_v[worker];
      for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
        if(*chunk_p == chunk_id)
          workers_id_v->push_back(worker);
      }
    }

    return workers_id_v; //must test if empty after
  }



  void print_distribution()
  {
    chunk_list_type* chunk_list;

    for(long worker = 0; worker < vector_size; worker++){
      XBT_INFO("Worker-%ld has chunks:", worker);
      chunk_list = chunk_worker_v[worker];
      for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
        XBT_INFO(" %ld ", *chunk_p);
      }
    }
  }


}


#endif