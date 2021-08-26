#ifndef SOCKET_AGENT_HPP
#define SOCKET_AGENT_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>


#include "semaphore.hpp"

class SocketAgent;
struct SocketReturn;

#define NO_ERROR (0)
#define OPEN_ERROR (1<<1)
#define ACCEPT_ERROR (1<<2)
#define CONNECT_ERROR (1<<3)
#define SEND_ERROR (1<<4)
#define RECV_ERROR (1<<5)
#define CLOSE_ERROR (1<<6)
#define GETADDRINFO_ERROR (1<<7)
#define SOCKET_ERROR (1<<8)
#define BIND_ERROR (1<<9)
#define LISTEN_ERROR (1<<10)

class SocketAgent {

private:
  void print_error_info_helper(const char* func_name, int error);
  
  
public:
  SocketAgent();

  
  void print_error_info(struct SocketReturn return_info);

  
  struct SocketReturn agent_open_server(std::string IP,
					std::string port,
					int& set_to_descriptor,
					int queue_size,
					bool exit_on_error);
  struct SocketReturn agent_accept(int socket_descriptor,
				   int& set_to_descriptor,
				   bool exit_on_error);  
  struct SocketReturn agent_connect(std::string IP,
				    std::string port,
				    int& socket_descriptor, bool exit_on_error);
  struct SocketReturn agent_send(std::string message,
				 int socket_descriptor,
				 bool exit_on_error);
  struct SocketReturn agent_receive(std::string& buffer,
				    int max_size,
				    int socket_descriptor,
				    bool exit_on_error);
  struct SocketReturn agent_close_connection(int socket_descriptor, bool exit_on_error);

  struct SocketReturn call_getaddrinfo(std::string IP,
				       std::string port,
				       struct addrinfo& results,
				       bool exit_on_error);
  struct SocketReturn call_socket(const struct addrinfo& results,
				  bool exit_on_error);
  struct SocketReturn call_bind(int socket_descriptor,
				const struct addrinfo& results,
				bool exit_on_error);
  struct SocketReturn call_listen(int socket_descriptor, int queue_size,
				  bool exit_on_error);
  struct SocketReturn call_accept(int socket_descriptor,
				  int& set_to_descriptor,
				  struct sockaddr& incoming,
				  socklen_t& incoming_size,
				  bool exit_on_error);
  struct SocketReturn call_connect(int socket_descriptor,
				   const struct addrinfo results,
				   bool exit_on_error);
  struct SocketReturn call_send(int socket_descriptor, char* message,
				int bytes, bool exit_on_error);

  struct SocketReturn call_recv(int socket_descriptor,
				char buffer[],
				int max_bytes,
				bool exit_on_error);
  
  struct SocketReturn call_close(int socket_descriptor,
				 bool exit_on_error);

};

struct SocketReturn {
  int flags;
  int return_val;
  int error;
};

#endif
