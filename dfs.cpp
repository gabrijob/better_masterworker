#ifndef DFS_CODE
#define DFS_CODE

#include "dfs.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(better_masterworker);//remover depois junto com os XBT_INFO

Dfs::Dfs(long number_of_workers)
{
  vector_size = number_of_workers;

  for(long i = 0; i < vector_size; i++){
    chunk_list_type* chunk_list = new chunk_list_type;
    chunk_worker_v.push_back(chunk_list);
  }
}



void Dfs::allocate_chunks(long number_of_chunks){
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



void Dfs::add_chunk_to_worker(long chunk_id, long worker_id)
{
  chunk_list_type* chunk_list = chunk_worker_v[worker_id];
  chunk_list->push_back(chunk_id);
}

std::vector<long> Dfs::find_workers_with_chunk(long chunk_id)
{
  std::vector<long> workers_id_v;
  chunk_list_type* chunk_list;

  for(long worker = 0; worker < vector_size; worker++){
    chunk_list = chunk_worker_v[worker];
    for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
      if(*chunk_p == chunk_id)
        workers_id_v.push_back(worker);
    }
  }

  return workers_id_v; //must test if empty after
}



void Dfs::print_distribution(){
  chunk_list_type* chunk_list;

  for(long worker = 0; worker < vector_size; worker++){
    XBT_INFO("Worker-%ld has chunks:", worker);
    chunk_list = chunk_worker_v[worker];
    for(chunk_list_type::iterator chunk_p = chunk_list->begin(); chunk_p != chunk_list->end(); chunk_p++){
      XBT_INFO(" %ld ", *chunk_p);
    }
  }
}



#endif