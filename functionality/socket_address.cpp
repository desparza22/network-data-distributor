#include "socket_address.hpp"

SocketAddress::SocketAddress(std::string ip, std::string port) {
  initialize_SocketAddress(ip, port, NO_DESCRIPTOR);
}

SocketAddress::SocketAddress(int descriptor) {
  initialize_SocketAddress(NO_IP_PORT, NO_IP_PORT, descriptor);
}

SocketAddress::SocketAddress(std::string ip, std::string port, int descriptor) {
  initialize_SocketAddress(ip, port, descriptor);
}

//represent_address should come from as_string
SocketAddress::SocketAddress(std::string represent_address) {
  std::string ip;
  std::string port;
  int descriptor;
  
  int seperator1_index = represent_address.find(FIELD_SEP);
  int seperator2_index = represent_address.substr(seperator1_index + 1).find(FIELD_SEP) + seperator1_index + 1; 
  
  int ip_start = 0;
  int ip_end = seperator1_index;
  int port_start = seperator1_index + 1;
  int port_end = seperator2_index;
  int descriptor_start = seperator2_index + 1;
  
  ip = represent_address.substr(ip_start, ip_end - ip_start);
  port = represent_address.substr(port_start, port_end - port_start);
  descriptor = std::stoi(represent_address.substr(descriptor_start));
  
  initialize_SocketAddress(ip, port, descriptor);
}

void SocketAddress::initialize_SocketAddress(std::string ip, std::string port, int descriptor) {
  if(ip == NO_IP_PORT && port == NO_IP_PORT && descriptor == NO_DESCRIPTOR) {
    std::cerr << "Error: SocketAddress must at least have valid ip, port or descriptor\n";
    exit(1);
  }
  set_ip(ip);
  set_port(port);
  set_descriptor(descriptor);
}

std::string SocketAddress::get_ip() const {
  return ip;
}

std::string SocketAddress::get_port() const {
  return port;
}

int SocketAddress::get_descriptor() const {
  return descriptor;
}

bool SocketAddress::has_descriptor() const {
  return descriptor != NO_DESCRIPTOR;
}

bool SocketAddress::has_ip_port() const {
  return ip.compare(NO_IP_PORT) != 0 || port.compare(NO_IP_PORT) != 0;
}

void SocketAddress::set_ip(std::string ip) {
  this->ip = ip;
}

void SocketAddress::set_port(std::string port) {
  this->port = port;
}

void SocketAddress::set_descriptor(int descriptor) {
  this->descriptor = descriptor;
}

/*void SocketAddress::access_error(const std::string accessing) const {
  std::cerr << "Error: accessing " << accessing << " but this SocketAddress does not have " << accessing << std::endl;
  exit(1);
}*/

bool SocketAddress::operator<(const SocketAddress &other) const {
  int comparison = 0;

  comparison = compare_indicators(other);

  if(comparison == 0) {
    comparison = compare_ip_port(other);
  }

  if(comparison == 0) {
    comparison = compare_descriptor(other);
  }

  return comparison < 0;
}

int SocketAddress::compare_indicators(const SocketAddress &other) const {
  int comparison = 0;
  if(has_ip_port() != other.has_ip_port()) {
    comparison = has_ip_port()? -1: 1;

  } else if(has_descriptor() != other.has_descriptor()) {
    comparison = has_descriptor()? -1: 1;
  }

  return comparison;
}

//assumed that has_ip_port() == other.has_ip_port()
int SocketAddress::compare_ip_port(const SocketAddress &other) const {
  int comparison = 0;
  if(has_ip_port()) {
    comparison = get_ip().compare(other.get_ip());

    if(comparison == 0) {
      comparison = get_port().compare(other.get_port());
    }
  }

  return comparison;
}

//assumed that has_descriptor() == other.has_descriptor()
int SocketAddress::compare_descriptor(const SocketAddress &other) const {
  int comparison = 0;
  if(has_descriptor()) {
    comparison = get_descriptor() - other.get_descriptor();
  }

  return comparison;
}

std::string SocketAddress::as_string() const {
  std::string ip_str;
  std::string port_str;
  std::string descriptor_str;

  ip_str = get_ip();
  port_str = get_port();
  descriptor_str = std::to_string(get_descriptor());
  
  std::string represent_address = ip_str + FIELD_SEP + port_str + FIELD_SEP + descriptor_str;
  
  return represent_address;
}


#ifdef TEST_SOCKET_ADDRESS

bool check_equality(SocketAddress addr1, SocketAddress addr2);

bool check_equality(SocketAddress addr1, SocketAddress addr2) {
  return !(addr1 < addr2) && !(addr1 < addr2);
}

int main(int argc, char* argv[]) {
  SocketAddress empty_port("hello", NO_IP_PORT);
  SocketAddress empty_ip(NO_IP_PORT, "goodbye");
  SocketAddress empty_empty(NO_IP_PORT, NO_IP_PORT, 2);
  SocketAddress basic("hello", "goodbye");
  SocketAddress descriptor1("hello", "goodbye", 1);
  SocketAddress descriptor0(0);
  SocketAddress descriptorn1(-2);
  
  std::string empty_port_str = empty_port.as_string();
  std::string empty_ip_str = empty_ip.as_string();
  std::string empty_empty_str = empty_empty.as_string();
  std::string basic_str = basic.as_string();
  std::string descriptor1_str = descriptor1.as_string();
  std::string descriptor0_str = descriptor0.as_string();
  std::string descriptorn1_str = descriptorn1.as_string();

  SocketAddress empty_port_2 = SocketAddress(empty_port_str);
  SocketAddress empty_ip_2 = SocketAddress(empty_ip_str);
  SocketAddress empty_empty_2 = SocketAddress(empty_empty_str);
  SocketAddress basic_2 = SocketAddress(basic_str);
  SocketAddress descriptor1_2 = SocketAddress(descriptor1_str);
  SocketAddress descriptor0_2 = SocketAddress(descriptor0_str);
  SocketAddress descriptorn1_2 = SocketAddress(descriptorn1_str);
  
  std::string conversion_error = "failed conversion to/from string";
  if(!check_equality(empty_port, empty_port_2)) {
    std::cerr << "empty_port " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(empty_ip, empty_ip_2)) {
    std::cerr << "empty_ip " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(empty_empty, empty_empty_2)) {
    std::cerr << "empty_empty " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(basic, basic_2)) {
    std::cerr << "basic " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(descriptor1, descriptor1_2)) {
    std::cerr << "basic " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(descriptor0, descriptor0_2)) {
    std::cerr << "basic " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(descriptorn1, descriptorn1_2)) {
    std::cerr << "basic " << conversion_error << "\n";
    exit(1);
  }
    
  std::cout << "Tests passed!\n";
  return 0;
}

#endif
