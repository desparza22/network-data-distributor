#ifndef THREAD_BLOCKER_HPP
#define THREAD_BLOCKER_HPP

#include <mutex>
#include <condition_variable>
#include <iostream>

class ThreadBlocker {
private:
  bool thread_should_continue;
  
  std::mutex mutex_;
  std::condition_variable cond_var;

public:
  ThreadBlocker();
  ThreadBlocker(const ThreadBlocker &older);
  ~ThreadBlocker();
  
  void block_this();
  void unblock_other();

};

#endif
