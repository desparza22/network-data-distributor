#ifndef SOCKET_ADDRESS_HPP
#define SOCKET_ADDRESS_HPP


#include "stringable.hpp"
#include "address.hpp"

#include <string>
#include <iostream>
#include <iterator>

#define FIELD_SEP ('$')
#define NO_IP_PORT ("")
#define NO_DESCRIPTOR (-1)

class SocketAddress;

class SocketAddress: public Stringable, public Address {
private:
  bool has_ip_port_indicator;

  std::string ip;
  std::string port;
  int descriptor = -1;

  void get_error(const std::string getting, const std::string socket_address_type);
  
public:
  SocketAddress(std::string ip, std::string port);
  SocketAddress(int descriptor);
  SocketAddress(std::string ip, std::string port, int descriptor);
  SocketAddress(std::string represent_address);
  
  void initialize_SocketAddress(std::string ip,
				std::string port,
				int descriptor);

  std::string get_ip() const;
  std::string get_port() const;
  int get_descriptor() const;
  bool has_ip_port() const;
  bool has_descriptor() const;
  void set_ip(std::string ip);
  void set_port(std::string port);
  void set_descriptor(int descriptor);
  //  void access_error(const std::string accessing) const;

  bool operator<(const SocketAddress &other) const;
  int compare_indicators(const SocketAddress &other) const;
  int compare_ip_port(const SocketAddress &other) const;
  int compare_descriptor(const SocketAddress &other) const;
  
  virtual std::string as_string() const override;
};


#endif
