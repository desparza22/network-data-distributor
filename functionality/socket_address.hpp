#ifndef SOCKET_ADDRESS_HPP
#define SOCKET_ADDRESS_HPP


#include "stringable.hpp"
#include "address.hpp"

#include <string>
#include <iostream>
#include <iterator>

#define IP_PORT_SEP ('$')
#define DESCRIPTOR_HEADER ('#')


struct IPPort;
class SocketAddress;

struct IPPort {
  std::string ip;
  std::string port;
};

class SocketAddress: public Stringable, public Address {
private:

  bool is_descriptor;
  
  struct IPPort ip_and_port;
  int descriptor;

  void get_error(const std::string getting, const std::string socket_address_type);
  
public:
  SocketAddress(std::string ip, std::string port);
  SocketAddress(int descriptor);
  SocketAddress(std::string represent_address);
  
  void initialize_SocketAddress(std::string ip,
				std::string port);
  void initialize_SocketAddress(int descriptor);

  std::string get_ip() const;
  std::string get_port() const;
  int get_descriptor() const;
  bool get_is_descriptor() const;
  void set_ip(std::string ip);
  void set_port(std::string port);
  void set_descriptor(int descriptor);
  void access_error(const char* accessing, const char* socket_address_type) const;
  bool operator<(const SocketAddress &other) const;

  virtual std::string as_string() const override;
};


#endif
