#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include <string>

class Communicator;
struct Message;

struct Message {
  int descriptor;
  std::string message;
};

class Communicator {

public:
  virtual void go_online(std::string address) = 0;
  virtual void go_offline() = 0;
  virtual void send_to(std::string address, std::string message) = 0;
  virtual Message poll_inbox() = 0;

  virtual void close_connection(std::string address);
};

struct Message construct_message(int descriptor,
				 std::string message_str,
				 bool has_message);

#endif
