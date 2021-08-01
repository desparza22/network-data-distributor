#include "obj_str_iterator.hpp"

ObjStrIterator::ObjStrIterator(std::basic_string<char> iterate_over) {
  remaining_to_iterate = iterate_over;
}

std::basic_string<char> ObjStrIterator::get_next() {
  std::basic_string<char> next_obj_str;

  int next_separator = remaining_to_iterate.find(OBJECT_SEP);
  if(next_separator != std::string::npos) {
    next_obj_str = remaining_to_iterate.substr(0, next_separator);
    remaining_to_iterate = remaining_to_iterate.substr(next_separator + 1);
    
  } else {
    std::cout << "Error: remaining string left to iterate is '" << remaining_to_iterate <<
      "'\n" << "Did you check 'has_next()' and did the string used to instantiate ObjStrIterator " <<
      "come from 'vector_to_string'?\n";
    exit(1);
  }

  return next_obj_str;
}

bool ObjStrIterator::has_next() {
  return remaining_to_iterate.size() != 0;
}

std::basic_string<char> ObjStrIterator::vector_to_string(std::vector<std::shared_ptr<Stringable>> stringables) {
  std::basic_string<char> represent_vector;
  
  std::vector<std::shared_ptr<Stringable>>::iterator it;
  for(const auto stringable : stringables) {
    std::basic_string<char> represent_object = stringable->as_string();
    
    represent_vector.append(represent_object + OBJECT_SEP);
  }

  return represent_vector;
}

#ifdef TEST_OBJ_STR_ITERATOR

class StringWrapper: public Stringable {
private:
  std::basic_string<char> original_string;
  
public:
  StringWrapper(std::basic_string<char> string);

  std::basic_string<char> as_string() const override;

  bool operator<(const StringWrapper &other) const;
};

StringWrapper::StringWrapper(std::basic_string<char> string) {
  original_string = string;
}

std::basic_string<char> StringWrapper::as_string() const {
  return original_string;
}

bool StringWrapper::operator<(const StringWrapper &other) const {
  return original_string.compare(other.original_string) < 0;
}


bool check_string_wrapper_equality(std::shared_ptr<Stringable> a,
				   std::shared_ptr<Stringable> b);
bool check_string_wrapper_vector_equality(std::vector<std::shared_ptr<Stringable>> a, std::vector<std::shared_ptr<Stringable>> b);

bool check_string_wrapper_vector_equality(std::vector<std::shared_ptr<Stringable>> a, std::vector<std::shared_ptr<Stringable>> b) {
  int size_a = a.size();
  int size_b = b.size();

  if(size_a != size_b) {
    return false;
  }

  for(int i = 0; i < size_a; i++) {
    std::shared_ptr<Stringable> element_a = a[i];
    std::shared_ptr<Stringable> element_b = b[i];

    if(!check_string_wrapper_equality(element_a, element_b)) {
      return false;
    }
  }

  return true;
}

bool check_string_wrapper_equality(std::shared_ptr<Stringable> a,
				   std::shared_ptr<Stringable> b) {
  std::shared_ptr<StringWrapper> string_wrapper_a = std::dynamic_pointer_cast<StringWrapper>(a);
  std::shared_ptr<StringWrapper> string_wrapper_b = std::dynamic_pointer_cast<StringWrapper>(b);
  return !(*string_wrapper_a < *string_wrapper_b) && !(*string_wrapper_b < *string_wrapper_a);
}

int main(int argc, char* argv[]) {
  //Make StringWrappers
  std::basic_string<char> hello_str("hello\0");
  std::basic_string<char> world_str("world\0");
  std::basic_string<char> empty_str("\0");
  
  std::shared_ptr<StringWrapper> hello_wrapper(new StringWrapper(hello_str));
  std::shared_ptr<StringWrapper> world_wrapper(new StringWrapper(world_str));
  std::shared_ptr<StringWrapper> empty_wrapper(new StringWrapper(empty_str));
  //


  //Test < operator on StringWrapper
  if(!(*hello_wrapper < *world_wrapper)) {
    std::cout << "hello_wrapper should be < world_wrapper\n";
    exit(1);
  }
  if(!(*empty_wrapper < *hello_wrapper)) {
    std::cout << "empty_wrapper should be < hello_wrapper\n";
    exit(1);
  }
  //

  //Make a vector of the StringWrapper objects
  std::vector<std::shared_ptr<Stringable>> vector_a;
  vector_a.push_back(std::shared_ptr<Stringable>(hello_wrapper));
  vector_a.push_back(std::shared_ptr<Stringable>(world_wrapper));
  vector_a.push_back(std::shared_ptr<Stringable>(empty_wrapper));
  //


  //Convert the vector of StringWrapper objects to and from
  //a basic_string
  std::vector<std::shared_ptr<Stringable>> vector_b;
  
  std::basic_string<char> represent_vector = ObjStrIterator::vector_to_string(vector_a);
  ObjStrIterator iterator(represent_vector);
  while(iterator.has_next()) {
    std::basic_string<char> string_wrapper_representation = iterator.get_next();
    vector_b.push_back(std::shared_ptr<Stringable>(new StringWrapper(string_wrapper_representation)));
  }
  //


  //Check that the vector is unchanged after conversion to and
  //from a basic_string
  if(!(check_string_wrapper_vector_equality(vector_a, vector_b))) {
    std::cout << "vectors are not equal\n";
    std::cout << "vector_a prints as " << ObjStrIterator::vector_to_string(vector_a) << "\n";
    std::cout << "vector_b prints as " << ObjStrIterator::vector_to_string(vector_b) << "\n";
    exit(1);
  }
  //
  

  std::cout << "Tests passed!\n";
  return 0;
}

#endif
