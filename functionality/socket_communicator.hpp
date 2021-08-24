#ifndef SOCKET_COMMUNICATOR_HPP
#define SOCKET_COMMUNICATOR_HPP

#include "communicator.hpp"
#include "socket_agent.hpp"
#include "object_pool.hpp"
#include "semaphore.hpp"
#include "address.hpp"

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
  TwoWayMap<std::string, int> address_to_descriptor;
  std::unordered_map<std::string, int> server_address_to_descriptor;
  std::unordered_map<int, std::string> 
  SocketAgent socket_agent;

  bool is_online = false;
  Semaphore is_online_access = Semaphore(1);
  int local_descriptor = -1;

  void do_go_online(std::string address_str);
  void do_send_to(int recipient_descriptor, std::string message);
  struct Message do_poll_inbox();

  int get_or_create_descriptor(std::string address_str);
  int get_descriptor(std::string address_str);

  bool is_online();
  
public:
  SocketCommunicator();
  
  void go_online(std::string address_str) override;
  void go_offline() override;
  void send_to(std::string address_str, std::string message) override;
  struct Message poll_inbox() override;
  void close_connection(std::string address) override;

  void send_to(int recipient_descriptor, std::string message);

  //enter_accept_loop and enter_receive_loop are  public so that they
  //can be called by the helper function. they should not be called by
  //other classes
  void enter_accept_loop();
  void enter_receive_loop(int descriptor);
};

void call_enter_accept_loop(SocketCommunicator socket_communicator);

void call_enter_receive_loop(SocketCommunicator socket_communicator,
			     int descriptor);

#endif
