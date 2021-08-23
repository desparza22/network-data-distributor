#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include "semaphore.hpp"

#include <deque>

template <class T>
class ObjectPool {
private:
  std::shared_ptr<std::deque<T>> object_list = std::shared_ptr<std::deque<T>>(new std::deque<T>());
  Semaphore object_availability;
  Semaphore object_list_access = Semaphore(1);

  void enqueue_object(T object);
  T get_next_object();
  
public:
  void add_object(T object);
  bool check_for_object(T& set_to_object);
  void wait_for_object(T& set_to_object);
};



template <class T>
void ObjectPool<T>::add_object(T object) {
  object_list_access.acquire(1);

  enqueue_object(object);
  object_availability.give(1);

  object_list_access.give(1);
}

template <class T>
bool ObjectPool<T>::check_for_object(T& set_to_object) {
  bool object_set = false;
  
  object_list_access.acquire(1);

  if((*object_list).size() != 0) {
    object_availability.acquire(1);
    set_to_object = get_next_object();
    object_set = true;
  }
  
  object_list_access.give(1);

  return object_set;
}

template <class T>
void ObjectPool<T>::wait_for_object(T& set_to_object) {
  object_availability.acquire(1);
  object_list_access.acquire(1);

  set_to_object = get_next_object();

  object_list_access.give(1);
}

template <class T>
void ObjectPool<T>::enqueue_object(T object) {
  (*object_list).push_back(object);
}

template <class T>
T ObjectPool<T>::get_next_object() {
  T retrieved_object = (*object_list).front();
  (*object_list).pop_front();

  return retrieved_object;
}


#endif
