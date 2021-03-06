#include <iostream>
#include <set>
#include <iterator>

class Shoe {
public:
  int x;
  Shoe(int starting_x);
  Shoe();
};

Shoe::Shoe(int starting_x) {
  x = starting_x;
}

Shoe::Shoe() {
  x = 100;
}

Shoe modify(Shoe& s, int new_x) {
  s.x = new_x;
  return s;
}

std::set<Shoe, bool(*)(const Shoe&,const Shoe&)>& modify_set(std::set<Shoe, bool(*)(const Shoe&,const Shoe&)>& original);

bool compare_shoes(const Shoe& a, const Shoe& b);

bool compare_shoes(const Shoe& a, const Shoe& b) {
  return a.x - b.x > 0;
}

std::set<Shoe, bool(*)(const Shoe&,const Shoe&)>& modify_set(std::set<Shoe, bool(*)(const Shoe&,const Shoe&)>& original) {
  Shoe another_s = Shoe(15);
  original.insert(another_s);
  std::set<Shoe, bool(*)(const Shoe&,const Shoe&)>::iterator it;

  for(it = original.begin(); it != original.end(); it++) {
    Shoe member = *it;
    member.x += 5;

    original.erase(it);
    original.insert(member);
  }

  std::cout << &original << "\n";
  return original;
}

  

int main(int argc, char* argv[]) {
  Shoe s = Shoe(5);
  std::set<Shoe, decltype(compare_shoes)*> set(compare_shoes);
  set.insert(s);
  std::set<Shoe, decltype(compare_shoes)*>& modified = modify_set(set);
  std::set<Shoe, decltype(compare_shoes)*>::iterator it;

  /*
  std::cout << "printing modified\n";
  for(it = modified.begin(); it != modified.end(); it++) {
    Shoe visit = *it;
    std::cout << visit.x << "\n";
  }

  std::cout << "\nprinting original\n";
  for(it = set.begin(); it != set.end(); it++) {
    Shoe visit = *it;
    std::cout << visit.x << "\n";
  }*/

  std::cout << &set << " " << &modified;
}
