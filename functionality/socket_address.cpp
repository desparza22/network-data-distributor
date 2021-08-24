#include "socket_address.hpp"

SocketAddress::SocketAddress(std::string ip, std::string port) {
  initialize_SocketAddress(ip, port);
}

//represent_address[read_index] should come from as_string
SocketAddress::SocketAddress(std::string represent_address) {
  std::string ip;
  std::string port;

  int ip_end = represent_address.find(IP_PORT_SEP);
  int port_start = ip_end + 1;

  ip = represent_address.substr(0, ip_end);
  port = represent_address.substr(port_start);

  initialize_SocketAddress(ip, port);
}


void SocketAddress::initialize_SocketAddress(std::string ip,
				 std::string port) {
  this->ip = ip;
  this->port = port;
}


std::string SocketAddress::get_ip() const {
  return ip;
}

std::string SocketAddress::get_port() const {
  return port;
}

void SocketAddress::set_ip(std::string ip) {
  this->ip = ip;
}

void SocketAddress::set_port(std::string port) {
  this->port = port;
}

bool SocketAddress::operator<(const SocketAddress &other) const {
  int compare_val = ip.compare(other.get_ip());
  if(compare_val == 0) {
    compare_val = port.compare(other.get_port());
  }

  return compare_val;
}

std::string SocketAddress::as_string() const {
  std::string represent_address = ip + IP_PORT_SEP + port;

  return represent_address;
}


#ifdef TEST_ADDRESS

bool check_equality(SocketAddress addr1, SocketAddress addr2);

bool check_equality(SocketAddress addr1, SocketAddress addr2) {
  return !(addr1 < addr2) && !(addr1 < addr2);
}

int main(int argc, char* argv[]) {
  SocketAddress empty_port("hello", "");
  SocketAddress empty_ip("", "goodbye");
  SocketAddress empty_empty("", "");
  SocketAddress basic("hello", "goodbye");
  
  std::string empty_port_str = empty_port.as_string();
  std::string empty_ip_str = empty_ip.as_string();
  std::string empty_empty_str = empty_empty.as_string();
  std::string basic_str = basic.as_string();

  
  SocketAddress empty_port2 = SocketAddress(empty_port_str);
  SocketAddress empty_ip2 = SocketAddress(empty_ip_str);
  SocketAddress empty_empty2 = SocketAddress(empty_empty_str);
  SocketAddress basic2 = SocketAddress(basic_str);
  
  std::string conversion_error = "failed conversion to/from string";
  if(!check_equality(empty_port, empty_port2)) {
    std::cout << "empty_port " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(empty_ip, empty_ip2)) {
    std::cout << "empty_ip " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(empty_empty, empty_empty2)) {
    std::cout << "empty_empty " << conversion_error << "\n";
    exit(1);
  }
  if(!check_equality(basic, basic2)) {
    std::cout << "basic " << conversion_error << "\n";
    exit(1);
  }
    
  std::cout << "Tests passed!\n";
  return 0;
}

#endif
