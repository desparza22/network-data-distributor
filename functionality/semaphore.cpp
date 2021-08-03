#include "semaphore.hpp"

void alert_that_function_isnt_thread_safe(const char* func_name) {
  std::cout << "Warning: " << func_name << " is not thread safe yet\n";
}

Semaphore::Semaphore() {
  do_constructor();
}

Semaphore::Semaphore(int initial_resources) {
  do_constructor();
  
  give(initial_resources);
}

void Semaphore::do_constructor() {
  resources = 0;
}

void Semaphore::acquire(unsigned int resources_acquired) {
  alert_that_function_isnt_thread_safe(__FUNCTION__);
  
  if(has_queued_threads() || resources < resources_acquired) {
    struct WaitingThreadEntry entry_for_this = create_entry(resources_acquired);
    add_back(entry_for_this);
    
  } else {
    resources -= resources_acquired;
  }
}

void Semaphore::give(unsigned int resources_given) {
  alert_that_function_isnt_thread_safe(__FUNCTION__);
  
  resources += resources_given;

  struct WaitingThreadEntry front_thread_entry = peek_front();
  if(front_thread_entry.resources_needed <= resources) {
    remove_front();
    resources -= front_thread_entry.resources_needed;
    front_thread_entry.thread_blocker.unblock_other();
  }
}

bool Semaphore::has_queued_threads() {
  return waiting_threads.size() > 0;
}

void Semaphore::add_back(struct WaitingThreadEntry thread_entry) {
  alert_that_function_isnt_thread_safe(__FUNCTION__);
  
  waiting_threads.push_back(thread_entry);
  thread_entry.thread_blocker.block_this();
}





struct WaitingThreadEntry create_entry(int resources_needed) {
  struct WaitingThreadEntry thread_entry;
  thread_entry.resources_needed = resources_needed;

  return thread_entry;
}

#ifdef TEST_SEMAPHORE

#include <thread>
#include <mutex>

void test_func() {

}

int main(int argc, char* argv[]) {
  return 0;
}

#endif

