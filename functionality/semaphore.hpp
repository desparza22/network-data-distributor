#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <deque>
#include <mutex>

#include "thread_blocker.hpp"

//class Semaphore;
struct WaitingThreadEntry;


struct WaitingThreadEntry {
  int resources_needed;
  ThreadBlocker thread_blocker;
};

struct WaitingThreadEntry create_entry(int resources_needed);



class Semaphore {
private:
  std::shared_ptr<std::mutex> mutex_ptr;
  
  std::shared_ptr<std::deque<struct WaitingThreadEntry>> waiting_threads;
  std::shared_ptr<int> resources;
  
  bool attempt_unblock();
  void add_back(struct WaitingThreadEntry thread_entry);
  struct WaitingThreadEntry peek_front();
  void remove_front();
  
public:
  Semaphore();
  Semaphore(int initial_resources);
  void do_constructor();
  
  void acquire(unsigned int resources_acquired);
  void give(unsigned int resources_given);
  bool has_queued_threads();
};

#endif
