#ifndef SOCKET_ADDRESS_HPP
#define SOCKET_ADDRESS_HPP


#include "stringable.hpp"

#include <string>
#include <iostream>
#include <iterator>

#define IP_PORT_SEP ('$')

class SocketAddress: public Stringable {
private:
  std::string ip;
  std::string port;
  
public:
  SocketAddress(std::string ip, std::string port);
  SocketAddress(std::string represent_address);
  void initialize_SocketAddress(std::string ip,
				std::string port);


  std::string get_ip() const;
  std::string get_port() const;
  void set_ip(std::string ip);
  void set_port(std::string port);

  bool operator<(const SocketAddress &other) const;

  virtual std::string as_string() const override;
};


#endif
