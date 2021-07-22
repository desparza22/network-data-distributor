#ifndef STRINGABLE_HPP
#define STRINGABLE_HPP

#include <string>

class Stringable {
public:
  virtual std::basic_string<char> as_string() const = 0;
};

#endif
