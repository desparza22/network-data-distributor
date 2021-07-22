#ifndef OBJ_STR_ITERATOR
#define OBJ_STR_ITERATOR

#include "stringable.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <memory>

#define OBJECT_SEP ('#')

class ObjStrIterator {
private:
  std::basic_string<char> remaining_to_iterate;
  
public:
  ObjStrIterator(std::basic_string<char> iterate_over);

  std::basic_string<char> get_next();
  bool has_next();

  static std::basic_string<char> vector_to_string(std::vector<std::shared_ptr<Stringable>> stringables);
};

#endif
