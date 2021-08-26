#ifndef STRINGABLE_HPP
#define STRINGABLE_HPP

#include <string>

class Stringable {
public:
  virtual std::string as_string() const = 0;
};

#endif
