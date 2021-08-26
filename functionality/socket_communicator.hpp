#ifndef SOCKET_COMMUNICATOR_HPP
#define SOCKET_COMMUNICATOR_HPP

#include "communicator.hpp"
#include "socket_agent.hpp"
#include "object_pool.hpp"
#include "semaphore.hpp"
#include "address.hpp"
#include "socket_address.hpp"

#include <memory>
#include <thread>
#include <string>
#include <unordered_map>

#define MAX_MESSAGE_SIZE (1000000000)
#define QUEUE_SIZE (4096)
#define EXIT_ON_ERROR (true)

class SocketCommunicator;

class SocketCommunicator : public Communicator {
private:
  ObjectPool<struct Message> message_pool;
  SocketAgent socket_agent;

  bool is_online = false;
  Semaphore is_online_access = Semaphore(1);
  int local_descriptor = -1;

  void do_go_online(std::shared_ptr<Address> address);
  void do_send_to(std::shared_ptr<Address> address, std::string message);
  struct Message do_poll_inbox();

  int get_or_create_descriptor(std::shared_ptr<Address> address);
  int get_descriptor(std::shared_ptr<Address> address);

  void set_is_online(bool is_online);
  bool get_is_online();
  
public:
  SocketCommunicator();
  
  void go_online(std::shared_ptr<Address> address) override;
  void go_offline() override;
  void send_to(std::shared_ptr<Address> address,
	       std::string message) override;
  struct Message poll_inbox() override;
  void close_connection(std::shared_ptr<Address> address) override;

  //enter_accept_loop and enter_receive_loop are  public so that they
  //can be called by the helper function. they should not be called by
  //other classes
  void enter_accept_loop();
  void enter_receive_loop(std::shared_ptr<Address> address);
};

void call_enter_accept_loop(SocketCommunicator socket_communicator);

void call_enter_receive_loop(SocketCommunicator socket_communicator,
			     int descriptor);

#endif
