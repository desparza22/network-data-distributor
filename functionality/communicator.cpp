#include "communicator.hpp"

void Communicator::close_connection(std::string address) {
  //do nothing if not overridden. not all communication
  //systems will need a connection to be closed.
}


struct Message construct_message(int descriptor,
				 std::string message_str,
				 bool has_message) {
  struct Message message;
  message.descriptor = descriptor;
  message.message = message_str;
  message.has_message = has_message;

  return message;
}
