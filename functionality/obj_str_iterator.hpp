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
  std::string remaining_to_iterate;
  
public:
  ObjStrIterator(std::string iterate_over);

  std::string get_next();
  bool has_next();

  static std::string vector_to_string(std::vector<std::shared_ptr<Stringable>> stringables);
};

#endif
