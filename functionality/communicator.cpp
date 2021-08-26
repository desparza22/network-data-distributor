#include "communicator.hpp"

void Communicator::close_connection(std::shared_ptr<Address> address) {
  //do nothing if not overridden. not all communication
  //systems will need a connection to be closed.
}


struct Message construct_message(std::shared_ptr<Address> address,
				 std::string message_str) {
  struct Message message;
  message.address = address;
  message.message = message_str;

  return message;
}
