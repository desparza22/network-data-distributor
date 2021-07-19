#ifndef ADDRESS_HPP
#define ADDRESS_HPP

#include "string_help.hpp"
#include "stringable.hpp"

#include <set>
#include <string>
#include <iostream>
#include <iterator>

#define IP_PORT_SEP ('$')
#define ADDRESS_SEP ('#')

class Address {
private:
  std::basic_string<char> ip;
  std::basic_string<char> port;
  
public:
  Address(std::basic_string<char> ip, std::basic_string<char> port);
  Address(std::basic_string<char> represent_address);
  void initialize_Address(std::basic_string<char> ip,
			  std::basic_string<char> port);


  std::basic_string<char> get_ip() const;
  std::basic_string<char> get_port() const;
  void set_ip(std::basic_string<char> ip);
  void set_port(std::basic_string<char> port);

  bool operator<(const Address &other) const;

  std::basic_string<char> to_string();
  static std::set<Address> addresses_from_string(std::basic_string<char> represent_addresses);
  static std::basic_string<char> read_address_string(std::basic_string<char> represent_addresses, int& read_index);
};


#endif
