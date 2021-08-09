#include "thread_blocker.hpp"


ThreadBlocker::ThreadBlocker() {
  thread_should_continue = std::shared_ptr<bool>(new bool(false));
  mutex_ptr = std::shared_ptr<std::mutex>(new std::mutex());
  cond_var_ptr = std::shared_ptr<std::condition_variable>(new std::condition_variable());
}

void ThreadBlocker::block_this() {
  std::unique_lock<std::mutex> ul(*mutex_ptr);

  cond_var_ptr->wait(ul, [=]{return *thread_should_continue;});
  *thread_should_continue = false;
  
  ul.unlock();
}

void ThreadBlocker::unblock_other() {
  {
    std::lock_guard<std::mutex> lg(*mutex_ptr);
    *thread_should_continue = true;
  }

  cond_var_ptr->notify_one();
}


#ifdef TEST_THREAD_BLOCKER

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>

std::mutex mutex_;
std::condition_variable cond_var;

void function_a(std::shared_ptr<ThreadBlocker> blocker, std::shared_ptr<int> var1, int target) {
  blocker->block_this();

  *var1 = target;
}


int main(int argc, char* argv[]) {
  int sequence[] = {9, 8, 0, 2, 1, 5, 10, 12, 6, 3, 7, 4, 11};
  int sequence_len = sizeof(sequence) / sizeof(*sequence);

  std::vector<std::shared_ptr<int>> vars;
  std::vector<std::shared_ptr<ThreadBlocker>> blockers;
  std::vector<std::thread> threads;
  
  int original_var = -1;
  for(int i = 0; i < sequence_len; i++) {
    std::shared_ptr<ThreadBlocker> blocker(new ThreadBlocker());
    blockers.push_back(blocker);

    std::shared_ptr<int> var_ref(new int(original_var));
    vars.push_back(var_ref);

    int target = i;
    std::thread thread(function_a, blocker, var_ref, target);
    threads.push_back(std::move(thread));
  }

  for(int i = 0; i < sequence_len; i++) {
    int index = sequence[i];
    
    std::shared_ptr<int> var_ref = vars[index];
    std::shared_ptr<ThreadBlocker> blocker_ref = blockers[index];
    std::thread& thread = threads[index];

    if(*var_ref != original_var) {
      std::cout << "Error on thread at index " << index << ". var_ref did not equal " <<
	"original_var but thread was not yet unblocked\n";
      exit(1);
    }
    
    blocker_ref->unblock_other();
    thread.join();

    if(*var_ref == original_var) {
      std::cout << "Error on thread at index " << index << ". var_ref equaled " <<
	"original_var but thread was unblocked and joined\n";
      exit(1);
    }
  }

  std::cout << "Tests passed!\n";
  return 0;
}










/*
#include <condition_variable>
#include <iostream>
#include <thread>

std::mutex* mutex_var;
std::condition_variable condVar; 

bool dataReady{false};

void waitingForWork(){
    std::cout << "Waiting " << std::endl;
    std::unique_lock<std::mutex> lck(*mutex_var);
    condVar.wait(lck, []{ return dataReady; });   // (4)
    std::cout << "Running " << std::endl;
}

void setDataReady(){
    {
        std::lock_guard<std::mutex> lck(*mutex_var);
        dataReady = true;
    }
    std::cout << "Data prepared" << std::endl;
    condVar.notify_one();                        // (3)
}

int main(){
  mutex_var = new std::mutex();
    
  std::cout << std::endl;

  std::thread t1(waitingForWork);               // (1)
  std::thread t2(setDataReady);                 // (2)

  t1.join();
  t2.join();
  
  std::cout << std::endl;
  
}
*/
#endif
