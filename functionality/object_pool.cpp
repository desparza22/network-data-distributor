#include "object_pool.hpp"

#ifdef TEST_OBJECT_POOL

#include <thread>
#include <vector>

void wait_for_object_expected(ObjectPool<int> object_pool,
			      int expected,
			      std::shared_ptr<bool> success) {
  int retrieved = expected - 1;
  object_pool.wait_for_object(retrieved);

  *success = expected == retrieved;
}

void wait_for_object_unknown(ObjectPool<int> object_pool,
			     std::shared_ptr<int> value_found,
			     Semaphore mark_when_completed) {
  int retrieved;
  object_pool.wait_for_object(retrieved);
  *value_found = retrieved;
  
  mark_when_completed.give(1);
}

void check_for_object_expected(ObjectPool<int> object_pool,
			       int value_expected,
			       bool object_found_expected,
			       std::shared_ptr<bool> success) {
  int retrieved = value_expected - 1;
  bool object_found = object_pool.check_for_object(retrieved);

  *success =
    (object_found == object_found_expected) &&
    (!object_found || (retrieved == value_expected));
}

void join_threads_in_vector(std::vector<std::thread> thread_vector) {
  for(int i = 0; i < thread_vector.size(); i++) {
    std::thread thread = std::move(thread_vector[i]);
    thread.join();
  }
}

bool wait_and_check(int num_vals,
		    int num_waits,
		    int expected_val,
		    bool expected_object_found,
		    ObjectPool<int> object_pool) {

  
  std::vector<std::thread> wait_for_one_threads;
  std::vector<std::shared_ptr<bool>> wait_for_one_results;
  for(int i = 0; i < num_waits; i++) {
    std::shared_ptr<bool> result(new bool(false));
    std::thread wait_for_one(wait_for_object_expected,
			     object_pool,
			     expected_val,
			     result);

    wait_for_one_results.push_back(result);
    wait_for_one_threads.push_back(std::move(wait_for_one));
  }

  std::vector<std::thread> check_for_one_threads;
  std::vector<std::shared_ptr<bool>> check_for_one_results;
  for(int i = 0; i < num_vals - num_waits; i++) {
    std::shared_ptr<bool> result(new bool(false));
    std::thread check_for_one(check_for_object_expected,
			      object_pool,
			      expected_val,
			      expected_object_found,
			      result);

    check_for_one_results.push_back(result);
    check_for_one_threads.push_back(std::move(check_for_one));
  }

  join_threads_in_vector(std::move(wait_for_one_threads));
  join_threads_in_vector(std::move(check_for_one_threads));

  bool tests_passed = true;
  for(int i = 0; i < wait_for_one_results.size(); i++) {
    tests_passed = tests_passed && wait_for_one_results[i];
  }
  for(int i = 0; i < check_for_one_results.size(); i++) {
    tests_passed = tests_passed && check_for_one_results[i];
  }

  return tests_passed;
}

int num_vals_equal_to(std::vector<std::shared_ptr<int>> values,
		      int target_value) {

  int num_equal = 0;
  for(int i = 0; i < values.size(); i++) {
    if(*(values[i]) == target_value) {
      num_equal++;
    }
  }

  return num_equal;
}
  

int main(int argc, char* argv[]) {
  //1. add num_ones 1s and num_twos 2s
  //2. have n threads wait_for_object_expected
  // and (num_ones - n) threads check_for_object. All threads should
  // get a 1
  //3. repeat step 2 with new value for n and threads
  // expecting a 2 instead of a 1
  //4. have p threads check_for_object, expecting not
  // to find one
  int num_ones = 11;
  int num_twos = 7;
  int n = 5;
  int m = 3;
  int p = 2;
  bool tests_passed = true;
  
  ObjectPool<int> object_pool1;
  for(int i = 0; i < num_ones; i++) {
    object_pool1.add_object(1);
  }
  for(int i = 0; i < num_twos; i++) {
    object_pool1.add_object(2);
  }

  tests_passed = tests_passed && wait_and_check(num_ones,
						n,
						1,
						true,
						object_pool1);
  tests_passed = tests_passed && wait_and_check(num_twos,
						m,
						2,
						true,
						object_pool1);
  tests_passed = tests_passed && wait_and_check(p,
						0,
						56789,
						false,
						object_pool1);
  
  
  //1. have (num_ones + num_twos) threads wait_for_object_unknown
  //2. add num_ones 1s. num_ones threads should complete with
  // value 1
  //3. repeat step 2, replacing 1 with 2
  num_ones = 29;
  num_twos = 23;
  
  ObjectPool<int> object_pool2;
  Semaphore threads_completed;
  
  std::vector<std::thread> wait_for_unknown_threads;
  std::vector<std::shared_ptr<int>> wait_for_unknown_results;
  for(int i = 0; i < num_ones + num_twos; i++) {
    std::shared_ptr<int> set_to_object(new int(-1));
    std::thread wait_for_unknown(wait_for_object_unknown,
				 object_pool2,
				 set_to_object,
				 threads_completed);

    wait_for_unknown_threads.push_back(std::move(wait_for_unknown));
    wait_for_unknown_results.push_back(set_to_object);
  }

  for(int i = 0; i < num_ones; i++) {
    object_pool2.add_object(1);
  }
  threads_completed.acquire(num_ones);
  int num_ones_found = num_vals_equal_to(wait_for_unknown_results, 1);
  tests_passed = tests_passed && (num_ones_found == num_ones);

  for(int i = 0; i < num_twos; i++) {
    object_pool2.add_object(2);
  }
  threads_completed.acquire(num_twos);
  int num_twos_found = num_vals_equal_to(wait_for_unknown_results, 2);
  tests_passed = tests_passed && (num_twos_found == num_twos);

  for(int i = 0; i < wait_for_unknown_threads.size(); i++) {
    std::thread wait_for_unknown = std::move(wait_for_unknown_threads[i]);
    wait_for_unknown.join();
  }
  
  if(tests_passed) {
    std::cout << "Tests passed!\n";
  } else {
    std::cout << "A test failed\n";
  }
}

#endif
