#include "thread_blocker.hpp"


ThreadBlocker::ThreadBlocker() {
  thread_should_continue = false;

  mutex_ = new std::mutex();
  cond_var = new std::condition_variable();
}

ThreadBlocker::ThreadBlocker(const ThreadBlocker &older) {
  thread_should_continue = older.thread_should_continue;

  mutex_ = older.mutex_;
  cond_var = older.cond_var;
}

ThreadBlocker::~ThreadBlocker() {
  free(mutex_);
  free(cond_var);
}

void ThreadBlocker::block_this() {
  std::unique_lock<std::mutex> ul(*mutex_);

  (*cond_var).wait(ul, [=]{return thread_should_continue;});
  thread_should_continue = false;
  
  ul.unlock();
}

void ThreadBlocker::unblock_other() {
  {
    std::lock_guard<std::mutex> lg(*mutex_);
    thread_should_continue = true;
  }

  (*cond_var).notify_one();
}


#ifdef TEST_THREAD_BLOCKER

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>

std::mutex mutex_;
std::condition_variable cond_var;

void function_a(ThreadBlocker& blocker, int& var1, int target) {
  std::cerr << "gets here 1\n";
  blocker.block_this();
  std::cerr << "gets here 2\n";

  var1 = target;
}

int main(int argc, char* argv[]) {
  int sequence[] = {9, 8, 0, 2, 1, 5, 10, 12, 6, 3, 7, 4, 11};
  int sequence_len = sizeof(sequence) / sizeof(*sequence);

  std::vector<std::unique_ptr<int>> vars;
  std::vector<std::unique_ptr<ThreadBlocker>> blockers;
  std::vector<std::thread> threads;
  
  int original_var = -1;
  for(int i = 0; i < sequence_len; i++) {
    std::unique_ptr<ThreadBlocker> blocker(new ThreadBlocker());
    blockers.push_back(std::move(blocker));

    std::unique_ptr<int> var_ref(new int(original_var));
    vars.push_back(std::move(var_ref));

    int target = i;
    std::thread thread(function_a, std::ref(*blocker), std::ref(*var_ref), target);
    threads.push_back(std::move(thread));
  }

  for(int i = 0; i < sequence_len; i++) {
    int index = sequence[i];
    
    int& var_ref = *(vars[index]);
    ThreadBlocker& blocker_ref = *(blockers[index]);
    std::thread& thread = threads[index];

    if(var_ref != original_var) {
      std::cout << "Error on thread at index " << index << ". var_ref did not equal " <<
	"original_var but thread was not yet unblocked\n";
      exit(1);
    } else {
       std::cout << "1: remove this print later. index " << index << std::endl;
    }
    
    blocker_ref.unblock_other();
    thread.join();

    if(var_ref == original_var) {
      std::cout << "Error on thread at index " << index << ". var_ref equaled " <<
	"original_var but thread was unblocked and joined\n";
      exit(1);
    } else {
      std::cout << "2: remove this print later. index " << index << std::endl;
    }
  }
}

#endif
