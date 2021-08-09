#include "semaphore.hpp"


void alert_that_function_isnt_thread_safe(const char* func_name) {
  std::cerr << "Warning: " << func_name << " is not thread safe yet\n";
}

Semaphore::Semaphore() {
  do_constructor();
}

Semaphore::Semaphore(int initial_resources) {
  do_constructor();
  
  give(initial_resources);
}

void Semaphore::do_constructor() {
  resources = std::shared_ptr<int>(new int(0));
  mutex_ptr = std::shared_ptr<std::mutex>(new std::mutex());
  waiting_threads = std::shared_ptr<std::deque<struct WaitingThreadEntry>>(new std::deque<struct WaitingThreadEntry>());
}

void Semaphore::acquire(unsigned int resources_acquired) {
  mutex_ptr->lock();
  
  if(has_queued_threads() || *resources < resources_acquired) {
    struct WaitingThreadEntry entry_for_this = create_entry(resources_acquired);
    add_back(entry_for_this);
    
    mutex_ptr->unlock();
    entry_for_this.thread_blocker.block_this();
    
  } else {
    *resources -= resources_acquired;
    mutex_ptr->unlock();
  }
}

void Semaphore::give(unsigned int resources_given) {
  std::lock_guard<std::mutex> lg(*mutex_ptr);
  
  *resources += resources_given;
  
  bool thread_was_unblocked;
  do {
    thread_was_unblocked = attempt_unblock();
      
  } while(thread_was_unblocked);
}

bool Semaphore::has_queued_threads() {
  return waiting_threads->size() > 0;
}

bool Semaphore::attempt_unblock() {
  bool thread_was_unblocked = false;

  if(!has_queued_threads()) {
    return thread_was_unblocked;
  }

  struct WaitingThreadEntry front_thread_entry = peek_front();
  
  if(front_thread_entry.resources_needed <= *resources) {
    remove_front();
    *resources -= front_thread_entry.resources_needed;
    front_thread_entry.thread_blocker.unblock_other();
    
    thread_was_unblocked = true;
  } else {
    thread_was_unblocked = false;
  }
  
  return thread_was_unblocked;
}

void Semaphore::add_back(struct WaitingThreadEntry thread_entry) {
  waiting_threads->push_back(thread_entry);
}

struct WaitingThreadEntry Semaphore::peek_front() {
  return waiting_threads->front();
}

void Semaphore::remove_front() {
  waiting_threads->pop_front();
}



struct WaitingThreadEntry create_entry(int resources_needed) {
  struct WaitingThreadEntry thread_entry;
  thread_entry.resources_needed = resources_needed;

  return thread_entry;
}

#ifdef TEST_SEMAPHORE

#include <vector>
#include <thread>
#include <mutex>
#include <functional>

void test(Semaphore acquire_from, Semaphore give_to, std::shared_ptr<bool> flag) {  
  acquire_from.acquire(1);

  *flag = true;
  give_to.give(1);
}

bool n_are_true(std::vector<std::shared_ptr<bool>> flags, int n_should_be_true) {
  int num_true = 0;
  for(std::shared_ptr<bool> flag: flags) {
    if(*flag) {
      num_true++;
    }
  }

  return num_true == n_should_be_true;
}

int main(int argc, char* argv[]) {
  int give_segments_arr[] = {0, 3, 2, 0, 5};
  std::vector<int> give_segments(give_segments_arr, give_segments_arr + sizeof(give_segments_arr) / sizeof(*give_segments_arr));
  int num_threads = 0;
  for(int i = 0; i < give_segments.size(); i++) {
    num_threads += give_segments[i];
  }
  std::vector<std::shared_ptr<bool>> flags;
  for(int i = 0; i < num_threads; i++) {
    flags.push_back(std::shared_ptr<bool>(new bool(false)));
  }
  
  Semaphore threads_acquire_from;
  Semaphore threads_give_to;
  std::vector<std::thread> threads;
  for(int i = 0; i < num_threads; i++) {
    std::thread thread(test, threads_acquire_from, threads_give_to, flags[i]);
    threads.push_back(std::move(thread));
  }

  bool test_passed = true;
  int num_true = 0;
  for(int i = 0; i < give_segments.size() && test_passed; i++) {
    int giving = give_segments[i];
    threads_acquire_from.give(giving);
    threads_give_to.acquire(giving);

    num_true += giving;
    test_passed = n_are_true(flags, num_true);
  }

  for(int i = 0; i < threads.size(); i++) {
    std::thread waiting_for = std::move(threads[i]);
    waiting_for.join();
  }

  if(test_passed) {
    std::cout << "Tests passed!\n";
  } else {
    std::cout << "Test failed. Some thread might have unblocked too soon or too late?\n";
  }
  
  return 0;
}

#endif

