#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include "semaphore.hpp"

#include <deque>

template <typename T>
class ObjectPool {
private:
  std::shared_ptr<std::deque<T>> object_list = 0;
  Semaphore object_availability = 0;
  Semaphore object_list_access = 0;

  void enqueue_object(T object);
  T get_next_object();
  
public:
  ObjectPool();
  
  void add_object(T object);
  bool check_for_object(T& set_to_object);
  void wait_for_object(T& set_to_object);
};


#endif
