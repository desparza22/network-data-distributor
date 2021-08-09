#ifndef THREAD_BLOCKER_HPP
#define THREAD_BLOCKER_HPP

#include <mutex>
#include <condition_variable>
#include <iostream>

class ThreadBlocker {
private:
  std::shared_ptr<bool> thread_should_continue;
  
  std::shared_ptr<std::mutex> mutex_ptr;
  std::shared_ptr<std::condition_variable> cond_var_ptr;

public:
  ThreadBlocker();
  
  void block_this();
  void unblock_other();

};

#endif
