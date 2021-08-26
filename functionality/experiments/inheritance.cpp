
#include <iostream>

class Base {

public:
  virtual void printer();

};

void Base::printer() {
  std::cout << "Implemented in Base\n";
}

class DerivedOne : public Base {

public:
  void printer();

};

void DerivedOne::printer() {
  std::cout << "Implemented in DerivedOne\n";
}

int main(int argc, char* argv[]) {
  DerivedOne derived_one;
  derived_one.printer();
}
