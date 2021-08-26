#include "socket_address.hpp"

SocketAddress::SocketAddress(std::string ip, std::string port) {
  initialize_SocketAddress(ip, port);
}

SocketAddress::SocketAddress(int descriptor) {
  initialize_SocketAddress(descriptor);
}

//represent_address should come from as_string
SocketAddress::SocketAddress(std::string represent_address) {
  char first_char = represent_address[0];
  
  if(first_char == DESCRIPTOR_HEADER) {
    int descriptor_start = 1;
    std::string descriptor_str = represent_address.substr(descriptor_start);

    int descriptor = std::stoi(descriptor_str);
    initialize_SocketAddress(descriptor);
    
  } else {
    std::string ip;
    std::string port;
  
    int ip_end = represent_address.find(IP_PORT_SEP);
    int port_start = ip_end + 1;
  
    ip = represent_address.substr(0, ip_end);
    port = represent_address.substr(port_start);
    initialize_SocketAddress(ip, port);
  }
}

void SocketAddress::initialize_SocketAddress(std::string ip,
					     std::string port) {
  this->is_descriptor = false;
  set_ip(ip);
  set_port(port);
}

void SocketAddress::initialize_SocketAddress(int descriptor) {
  this->is_descriptor = true;
  set_descriptor(descriptor);
}


std::string SocketAddress::get_ip() const {
  if(get_is_descriptor()) {
    access_error("ip", "descriptor");
  }
  
  return ip_and_port.ip;
}

std::string SocketAddress::get_port() const {
  if(get_is_descriptor()) {
    access_error("port", "descriptor");
  }
  
  return ip_and_port.port;
}

int SocketAddress::get_descriptor() const {
  if(!get_is_descriptor()) {
    access_error("descriptor", "ip_and_port");
  }

  return descriptor;
}

bool SocketAddress::get_is_descriptor() const {
  return is_descriptor;
}

void SocketAddress::set_ip(std::string ip) {
  if(get_is_descriptor()) {
    access_error("ip", "descriptor");
  }
  this->ip_and_port.ip = ip;
}

void SocketAddress::set_port(std::string port) {
  if(get_is_descriptor()) {
    access_error("port", "descriptor");
  }
  this->ip_and_port.port = port;
}

void SocketAddress::set_descriptor(int descriptor) {
  if(!get_is_descriptor()) {
    access_error("descriptor", "ip_and_port");
  }
  this->descriptor = descriptor;
}

void SocketAddress::access_error(const char* accessing, const char* socket_address_type) const {
  std::cout << "Error: accessing " << accessing << " in SocketAddress of type " << socket_address_type << std::endl;
  exit(1);
}

bool SocketAddress::operator<(const SocketAddress &other) const {
  if(get_is_descriptor() != other.get_is_descriptor()) {
    return get_is_descriptor();
  }

  int compare_val;
  
  if(get_is_descriptor()) {
    compare_val = get_descriptor() - other.get_descriptor();

  } else {
    compare_val = get_ip().compare(other.get_ip());
    if(compare_val == 0) {
      compare_val = get_port().compare(other.get_port());
    }
  }
  

  return compare_val < 0;
}

std::string SocketAddress::as_string() const {
  std::string represent_address;
  if(get_is_descriptor()) {
    represent_address = DESCRIPTOR_HEADER + std::to_string(get_descriptor());

  } else {
    represent_address = get_ip() + IP_PORT_SEP + get_port();
  }
  
  return represent_address;
}


#ifdef TEST_SOCKET_ADDRESS

bool check_equality(SocketAddress addr1, SocketAddress addr2);

bool check_equality(SocketAddress addr1, SocketAddress addr2) {
  return !(addr1 < addr2) && !(addr1 < addr2);
}

int main(int argc, char* argv[]) {
  SocketAddress empty_port("hello", "");
  SocketAddress empty_ip("", "goodbye");
  SocketAddress empty_empty("", "");
  SocketAddress basic("hello", "goodbye");
  SocketAddress descriptor1(1);
  SocketAddress descriptor0(0);
  SocketAddress descriptorn1(-1);
  
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
    std::cout << "empty_port " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(empty_ip, empty_ip_2)) {
    std::cout << "empty_ip " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(empty_empty, empty_empty_2)) {
    std::cout << "empty_empty " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(basic, basic_2)) {
    std::cout << "basic " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(descriptor1, descriptor1_2)) {
    std::cout << "basic " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(descriptor0, descriptor0_2)) {
    std::cout << "basic " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(descriptorn1, descriptorn1_2)) {
    std::cout << "basic " << conversion_error << "\n";
    exit(1);
  }
    
  std::cout << "Tests passed!\n";
  return 0;
}

#endif
