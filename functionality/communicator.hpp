#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include <string>

#include "address.hpp"

class Communicator;
struct Message;

struct Message {
  std::shared_ptr<Address> address;
  std::string message;
};

class Communicator {

public:
  virtual void go_online(std::shared_ptr<Address> address) = 0;
  virtual void go_offline() = 0;
  virtual void send_to(std::shared_ptr<Address> address, std::string message) = 0;
  virtual Message poll_inbox() = 0;

  virtual void close_connection(std::shared_ptr<Address> address);
};

struct Message construct_message(std::shared_ptr<Address> address,
				 std::string message_str);

#endif
