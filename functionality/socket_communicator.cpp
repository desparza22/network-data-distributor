#include "socket_communicator.hpp"

SocketCommunicator::SocketCommunicator() {
  //EXIT_ON_ERROR is passed to SocketAgent functions, instructing them
  //to terminate on unexpected events such as a bad file descriptor.
  //This makes it easier to build and put off error handling at a higher
  //level, so EXIT_ON_ERROR should be false unless error handling has been
  //implemented.
  if(!EXIT_ON_ERROR) {
    std::cout << "EXIT_ON_ERROR is false. Handle SocketAgent errors and remove this print statement.\n";
  }
}

//Check is_online() because SocketCommunicator cannot handle multiple
//servers, information with online server needs to be handled when it is
//finished with.
void SocketCommunicator::go_online(std::string address_str) {
  if(!is_online()) {
    set_is_online(true);
    do_go_online(address_str);
  }
}

//The is_online variable is checked by the loop that accepts connections.
void SocketCommunicator::go_offline() {
  if(is_online()) {
    set_is_online(false);
  }
}

//It is for now assumed that recipient descriptor is open.
void SocketCommunicator::send_to(int recipient_descriptor,
				 std::string message) {
  if(!is_online()) {
    std::cout << "SocketCommunicator is not online. Could not send \"" << message << "\" to \"" << recipient_descriptor << "\"\n";
    
  } else if(message.size() > MAX_MESSAGE_SIZE) {
    std::cout << "Message \"" << message << "\" being sent to \"" << recipient_descriptor << "\" is too long.\n";

  } else {
    do_send_to(recipient_descriptor, message);
  }
}

//Checks if there is an open connection with the ip and port in
//address_str, creating one if there is not, then sending the message.
void SocketCommunicator::send_to(std::string address_str,
				 std::string message) {
  int recipient_descriptor = get_or_create_descriptor(address_str);
  send_to(recipient_descriptor, message);
}

//The inbox "message_pool" is filled by other functions and simply
//retrieved from in this function.
struct Message SocketCommunicator::poll_inbox() {
  struct Message result;
  if(is_online()) {
    result = do_poll_inbox();

  } else {
    std::cout << "SocketCommunicator is not online. Could not receive from messages\n";
  }

  return result;
}

//For optimization reasons, this function may be called externally
//instructing that any connection descriptor associated with address
//be closed.
void SocketCommunicator::close_connection(std::string address) {
  int descriptor = get_descriptor(address);
  if(descriptor != -1) {
    socket_agent.agent_close_connection(descriptor, EXIT_ON_ERROR);
    address_to_descriptor.erase(address);
  }
}

//Continuous accepting of connections and assignment of a thread
//to receive messages from those connections.
void SocketCommunicator::enter_accept_loop() {
  while(is_online()) { //later this can be a mutable variable
    int set_to_descriptor;
    socket_agent.agent_accept(local_descriptor,
			      set_to_descriptor,
			      EXIT_ON_ERROR);

    std::thread receiving_thread(call_enter_receive_loop,
				 *this,
				 set_to_descriptor);
  }
}

//Continuously receives messages from the connection at descriptor
//while it is open. Adds those messages to the inbox "message_pool".
void SocketCommunicator::enter_receive_loop(int descriptor) {
  bool connection_active = true;
  while(true) {
    std::string receive_buffer;
    struct SocketReturn socket_return =
      socket_agent.agent_receive(receive_buffer,
				 MAX_MESSAGE_SIZE,
				 descriptor,
				 EXIT_ON_ERROR);

    connection_active = socket_return.return_val != 0;

    if(connection_active) {
      struct Message message = construct_message(descriptor, receive_buffer);
      message_pool.add_object(message);

    } else {
      break;
    }
  }
}

//It is assumed that is_online was false and set to true immediately before
//calling this.
void SocketCommunicator::do_go_online(std::string address_str) {  
  std::string ip;
  std::string port;
  
  SocketAddress local_address(address_str);
  ip = local_address.get_ip();
  port = local_address.get_port();
  
  socket_agent.agent_open_server(ip,
				 port,
				 local_descriptor,
				 QUEUE_SIZE,
				 EXIT_ON_ERROR);

  std::thread accepting_thread(call_enter_accept_loop,
			       *this);
}

//It is assumed that is_online is true and, for now, that recipient_descriptor
//is open.
void SocketCommunicator::do_send_to(int recipient_descriptor,
				    std::string message) {
  socket_agent.agent_send(message, recipient_descriptor, EXIT_ON_ERROR);
}

//It is assumed that is_online is true.
struct Message SocketCommunicator::do_poll_inbox() {
  struct Message received;
  message_pool.wait_for_object(received);

  return received;
}

int SocketCommunicator::get_or_create_descriptor(std::string address_str) {
  int descriptor = get_descriptor(address_str);
  
  if(descriptor == -1) {
    SocketAddress address(address_str);
    std::string ip = address.get_ip();
    std::string port = address.get_port();
    socket_agent.agent_connect(ip, port, descriptor, EXIT_ON_ERROR);

    address_to_descriptor[address_str] = descriptor;
  }

  return descriptor;
}

int SocketCommunicator::get_descriptor(std::string address_str) {
  int descriptor = -1;
  if(address_to_descriptor.contains(address_str)) {
    descriptor = address_to_descriptor[address_str];
  }

  return descriptor;
}

//Faciliates creating a thread on enter_receive_loop, which is otherwise
//not possible because it is a member function
void call_enter_receive_loop(SocketCommunicator socket_communicator,
			     int descriptor) {
  socket_communicator.enter_receive_loop(descriptor);
}

void call_enter_accept_loop(SocketCommunicator socket_communicator) {
  socket_communicator.enter_accept_loop();
}

#ifdef TEST_SOCKET_COMMUNICATOR

struct TestingMessageInfo {
  int sender_id;
  int recipient_id;

  int counter_when_communicated;
};

Semaphore counter_access(1);
int counter = 0;
void increment_counter() {
  counter_access.acquire(1);

  counter++;

  counter_access.give(1);
}

bool SocketCommunicator::is_online() {
  is_online_access.acquire(1);
  bool result = is_online;
  is_online_access.give(1);

  return result;
}


int main(int argc, char* argv[]) {
  std::unordered_map<std::string, struct TestingMessageInfo> delivered;
  std::unordered_map<std::string, struct TestingMessageInfo> received;

  

  
  return 0;
}

#endif
