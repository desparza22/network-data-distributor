#ifndef ADDRESS_HPP
#define ADDRESS_HPP


#include "stringable.hpp"

#include <string>
#include <iostream>
#include <iterator>

#define IP_PORT_SEP ('$')

class Address: public Stringable {
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

  virtual std::basic_string<char> as_string() const override;
};


#endif
