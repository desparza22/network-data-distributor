#include "two_way_map.hpp"

#ifdef TEST_TWO_WAY_MAP

#include <string>

int main(int argc, char* argv[]) {
  bool tests_passed = true;
  std::string result_str;
  std::string expected_str;
  int result_int;
  int expected_int;
  
  TwoWayMap<std::string, int> test_map;
  int num1 = 1;
  std::string str1 = "1";
  test_map.put(num1, str1);

  result_str = test_map.get_or_default(num1, "not present");
  expected_str = str1;
  tests_passed = tests_passed && (result_str.compare(expected_str) == 0);

  if(!tests_passed) {
    std::cout << "not passed 1\n";
  }
  
  result_int = test_map.get_or_default(str1, -1);
  expected_int = num1;
  tests_passed = tests_passed && (result_int == expected_int);

  if(!tests_passed) {
    std::cout << "not passed 2\n";
  }
  
  int num2 = 2;
  test_map.put(str1, num2);

  result_str = test_map.get_or_default(num2, "not present");
  expected_str = str1;
  tests_passed = tests_passed && (result_str.compare(expected_str) == 0);

  if(!tests_passed) {
    std::cout << "not passed 3\n";
  }
  
  result_int = test_map.get_or_default(str1, -1);
  expected_int = num2;
  tests_passed = tests_passed && (result_int == expected_int);

  if(!tests_passed) {
    std::cout << "not passed 4\n";
  }
  
  result_str = test_map.get_or_default(num1, "not present");
  expected_str = "not present";
  tests_passed = tests_passed && (result_str.compare(expected_str) == 0);

  if(!tests_passed) {
    std::cout << "not passed 5 " << result_str << std::endl;
  }
  
  result_str = test_map.get_or_default(-1, "not present");
  expected_str = "not present";
  tests_passed = tests_passed && (result_str.compare(expected_str) == 0);

  if(!tests_passed) {
    std::cout << "not passed 6\n";
  }
  
  result_int = test_map.get_or_default("not present", -1);
  expected_int = -1;
  tests_passed = tests_passed && (result_int == expected_int);

  if(!tests_passed) {
    std::cout << "not passed 7\n";
  }
  
  if(tests_passed) {
    std::cout << "Tests passed!\n";

  } else {
    std::cout << "Tests failed\n";
  }
  
  return 0;
}


#endif
