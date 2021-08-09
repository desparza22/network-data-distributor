#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>

std::mutex m;

void func1();
void func2();
void func3();

void func1() {
  m.lock();

  for(int i = 0; i < 5; i++) {
    std::cout << "hello from func1\n";
    sleep(1);
  }
  
  m.unlock();
}

void func2() {
  std::lock_guard<std::mutex> lg(m);

  for(int i = 0; i < 5; i++) {
    std::cout << "hello from func2\n";
    sleep(1);
    func3();
  }
}

void func3() {
  sleep(2);
}

int main(int argc, char* argv[]) {
  std::thread t2(func2);
  std::thread t1(func1);
  

  t1.join();
  t2.join();

  return 0;
}
