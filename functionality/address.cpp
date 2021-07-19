#include "address.hpp"

Address::Address(std::basic_string<char> ip, std::basic_string<char> port) {
  initialize_Address(ip, port);
}

//represent_address[read_index] should come from to_string
Address::Address(std::basic_string<char> represent_address) {
  std::basic_string<char> ip;
  std::basic_string<char> port;

  int ip_end = represent_address.find(IP_PORT_SEP);
  int port_start = ip_end + 1;

  ip = represent_address.substr(0, ip_end);
  port = represent_address.substr(port_start);

  initialize_Address(ip, port);
}


void Address::initialize_Address(std::basic_string<char> ip,
				 std::basic_string<char> port) {
  this->ip = ip;
  this->port = port;
}


std::basic_string<char> Address::get_ip() const {
  return ip;
}

std::basic_string<char> Address::get_port() const {
  return port;
}

void Address::set_ip(std::basic_string<char> ip) {
  this->ip = ip;
}

void Address::set_port(std::basic_string<char> port) {
  this->port = port;
}

bool Address::operator<(const Address &other) const {
  int compare_val = ip.compare(other.get_ip());
  if(compare_val == 0) {
    compare_val = port.compare(other.get_port());
  }

  return compare_val;
}

std::basic_string<char> Address::to_string() {
  std::basic_string<char> represent_address = ip + IP_PORT_SEP + port;

  return represent_address;
}

std::set<Address> Address::addresses_from_string(std::basic_string<char> represent_addresses) {

  std::set<Address> addresses;
  bool addresses_left = true;
  do {
    std::basic_string<char> next_address_str;
    int next_sep = represent_addresses.find(ADDRESS_SEP);

    if(next_sep != std::basic_string::npos) {
      next_address_str = represent_addresses.substr(0, next_sep);
      represent_addresses = represent_addresses.substr(next_sep+1);

    } else {
      next_address_str = represet_addresses;
      addresses_left = false;
    }

    Address address(next_address_str);
    addresses.insert(address);

  } while(addresses_left);
  
  return addresses;
}




#ifdef TEST_ADDRESS

bool check_equality(Address addr1, Address addr2);

bool check_equality(Address addr1, Address addr2) {
  return !(addr1 < addr2) && !(addr1 < addr2);
}

int main(int argc, char* argv[]) {
  Address empty_port("hello", "");
  Address empty_ip("", "goodbye");
  Address empty_empty("", "");
  Address basic("hello", "goodbye");
  
  std::basic_string<char> empty_port_str = empty_port.to_string();
  std::basic_string<char> empty_ip_str = empty_port.to_string();
  std::basic_string<char> empty_empty_str = empty_empty.to_string();
  std::basic_string<char> basic_str = basic.to_string();

  
  Address empty_port2 = Address(empty_port_str);
  Address empty_ip2 = Address(empty_ip_str);
  Address empty_empty2 = Address(empty_empty_str);
  Address basic2 = Address(basic_str);
  
  std::basic_string<char> conversion_error = "failed conversion to/from string";
  if(!check_equality(empty_port, empty_port2)) {
    std::cout << "empty_port " << conversion_error << "\n";
  }
  if(!check_equality(empty_ip, empty_ip2)) {
    std::cout << "empty_ip " << conversion_error << "\n";
  }
  if(!check_equality(empty_empty, empty_empty2)) {
    std::cout << "empty_empty " << conversion_error << "\n";
  }
  if(!check_equality(basic, basic2)) {
    std::cout << "basic " << conversion_error << "\n";
  }
    
  
  return 0;
}

#endif
