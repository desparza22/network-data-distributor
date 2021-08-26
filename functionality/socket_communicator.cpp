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
void SocketCommunicator::go_online(std::shared_ptr<Address> address) {
  if(!get_is_online()) {
    set_is_online(true);
    do_go_online(address);
  }
}

//The is_online variable is checked by the loop that accepts connections.
void SocketCommunicator::go_offline() {
  if(get_is_online()) {
    set_is_online(false);
  }
}

void SocketCommunicator::send_to(std::shared_ptr<Address> address,
				 std::string message) {
  if(!get_is_online()) {
    std::cout <<
      "SocketCommunicator is not online. Could not send \"" <<
      message << "\"\n";
    
  } else if(message.size() > MAX_MESSAGE_SIZE) {
    std::cout << "Message \"" << message << "\" is too long.\n";

  } else {
    do_send_to(address, message);
  }
}

//The inbox "message_pool" is filled by other functions and simply
//retrieved from in this function.
struct Message SocketCommunicator::poll_inbox() {
  struct Message result;
  if(get_is_online()) {
    result = do_poll_inbox();

  } else {
    std::cout << "SocketCommunicator is not online. Could not receive from messages\n";
  }

  return result;
}

//For optimization reasons, this function may be called externally
//instructing that any connection descriptor associated with address
//be closed.
void SocketCommunicator::close_connection(std::shared_ptr<Address> address) {
  int descriptor = get_descriptor(address);

  if(descriptor != -1) {
    socket_agent.agent_close_connection(descriptor, EXIT_ON_ERROR);

    std::shared_ptr<SocketAddress> socket_address =
	std::dynamic_pointer_cast<SocketAddress>(address);
    if(!socket_address->get_is_descriptor()) {
      std::string address_str = socket_address->as_string();
      address_to_descriptor.erase(address_str);
    }
  }
}

//Continuous accepting of connections and assignment of a thread
//to receive messages from those connections.
void SocketCommunicator::enter_accept_loop() {
  while(get_is_online()) {
    int set_to_descriptor;
    socket_agent.agent_accept(local_descriptor,
			      set_to_descriptor,
			      EXIT_ON_ERROR);

    std::shared_ptr<SocketAddress> socket_address(new SocketAddress(set_to_descriptor));
    std::shared_ptr<Address> address =
      std::dynamic_pointer_cast<Address>(socket_address);
    std::thread receiving_thread(call_enter_receive_loop,
				 *this,
				 address);
  }
}

//Continuously receives messages from the connection at descriptor
//while it is open. Adds those messages to the inbox "message_pool".
void SocketCommunicator::enter_receive_loop(std::shared_ptr<Address> address) {
  bool connection_active = true;
  std::shared_ptr<SocketAddress> socket_address =
    std::dynamic_pointer_cast<SocketAddress>(address);
  int descriptor = socket_address->get_descriptor();
  
  while(true) {
    std::string receive_buffer;
    struct SocketReturn socket_return =
      socket_agent.agent_receive(receive_buffer,
				 MAX_MESSAGE_SIZE,
				 descriptor,
				 EXIT_ON_ERROR);

    connection_active = socket_return.return_val != 0;

    if(connection_active) {
      struct Message message = construct_message(address,
						 receive_buffer);
      message_pool.add_object(message);

    } else {
      break;
    }
  }

  close_connection(address);
}

//It is assumed that is_online was false and set to true immediately before
//calling this.
void SocketCommunicator::do_go_online(std::shared_ptr<Address> address) {
  std::shared_ptr<SocketAddress> socket_address =
    std::dynamic_pointer_cast<SocketAddress>(address);
  std::string ip = socket_address->get_ip();
  std::string port = socket_address->get_port();
  
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
void SocketCommunicator::do_send_to(std::shared_ptr<Address> address,
				    std::string message) {
  int descriptor = get_or_create_descriptor(address);
  
  socket_agent.agent_send(message, descriptor, EXIT_ON_ERROR);
}

//It is assumed that is_online is true.
struct Message SocketCommunicator::do_poll_inbox() {
  struct Message received;
  message_pool.wait_for_object(received);

  return received;
}

int SocketCommunicator::get_or_create_descriptor(std::shared_ptr<Address> address) {
  int descriptor = get_descriptor(address);
  
  if(descriptor == -1) {
    std::shared_ptr<SocketAddress> socket_address =
      std::dynamic_pointer_cast<SocketAddress>(address);
    std::string ip = socket_address->get_ip();
    std::string port = socket_address->get_port();

    socket_agent.agent_connect(ip, port, descriptor, EXIT_ON_ERROR);
    address_to_descriptor[address_str] = descriptor;
  }

  return descriptor;
}

int SocketCommunicator::get_descriptor(std::shared_ptr<Address> address) {
  int descriptor = -1;
  std::shared_ptr<SocketAddress> socket_address =
    std::dynamic_pointer_cast<SocketAddress>(address);
  
  if(socket_address->get_is_descriptor()) {
    descriptor = socket_address->get_descriptor();
    
  } else {
    std::string address_str = socket_address->as_string();
    if(address_to_descriptor.contains(address_str)) {
      descriptor = address_to_descriptor[address_str];
    }
  }

  return descriptor;
}

void SocketCommunicator::set_is_online(bool is_online) {
  is_online_access.acquire(1);
  this->is_online = is_online;
  is_online_access.give(1);
}

bool SocketCommunicator::get_is_online() {
  is_online_access.acquire(1);
  bool result = is_online;
  is_online_access.give(1);

  return result;
}
/*
std::string SocketCommunicator::generate_dummy_address() {
  return "no_address_" + dummy_addresses_generated++;
}*/

//Faciliates creating a thread on enter_receive_loop, which is otherwise
//not possible because it is a member function
void call_enter_receive_loop(SocketCommunicator socket_communicator,
			     std::shared_ptr<Address> address) {
  socket_communicator.enter_receive_loop(address);
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

struct MessageHasher {

  std::size_t operator()(const Message& message) {
    std::size_t address_hash = hash_address(message.address);
    std::size_t message_hash = hash_message(message.message);

    return address_hash ^ (message_hash << 1);
  }

  std::size_t hash_message(std::string message) {
    return std::hash<std::string>()(message);
  }

  std::size_t hash_address(std::shared_ptr<Address> address) {
    std::shared_ptr<SocketAddress> socket_address =
      std::dynamic_pointer_cast<SocketAddress>(address);
    std::size_t address_hash;
    if(socket_address->get_is_descriptor()) {
      address_hash = std::hash<int>()(socket_address->get_descriptor());

    } else {
      std::size_t ip_hash =
	std::hash<std::string>()(socket_address->get_ip());
      std::size_t port_hash =
	std::hash<std::string>()(socket_address->get_port());
      address_hash = (ip_hash ^ (port_hash << 1)) >> 1;
    }

    return address_hash;
  }

};


int main(int argc, char* argv[]) {
  std::unordered_map<struct Message, struct TestingMessageInfo, MessageHasher> delivered;
  std::unordered_map<struct Message, struct TestingMessageInfo, MessageHasher> received;

  

  
  return 0;
}

#endif
