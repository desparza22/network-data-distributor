#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <atomic>
#include <mutex>

#include "doubly_linked_list.hpp"
//get first
//get last
//add first
//add last
//peek first
//peek last

class Semaphore {
private:
  
  
  std::atomic<int> pool;
  DoublyLinkedList<ThreadBlocker> waiting_threads;
  
public:
  void acquire(int resources);
  void give(int resources);

};

#endif
