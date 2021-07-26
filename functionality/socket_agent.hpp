#ifndef SOCKET_AGENT_HPP
#define SOCKET_AGENT_HPP

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

class SocketAgent {

private:
  struct addrinfo hints;
  
public:
  SocketAgent();

  void print_error_info_helper(const char* func_name, int errno);
  
  void print_error_info(struct SocketReturn return_info);

  
  struct SocketReturn open_server(std::basic_string<char> IP,
				  std::basic_string<char> port, bool exit_on_error);
  struct SocketReturn accept(int& set_to_descriptor, bool exit_on_error);
  
  struct SocketReturn connect(std::basic_string<char> IP,
			      std::basic_string<char> port,
			      int& socket_descriptor, bool exit_on_error);
  struct SocketReturn send(std::basic_string<char> message, int socket_descriptor, bool exit_on_error);
  struct SocketReturn receive(std::basic_string<char>& buffer, int max_size,
			      int socket_descriptor, bool exit_on_error);
  struct SocketReturn close_connection(int socket_descriptor, bool exit_on_error);

  struct SocketReturn call_getaddrinfo(std::basic_string<char> IP,
				       std::basic_string<char> port,
				       struct addrinfo& results,
				       bool exit_on_error);
  struct SocketReturn call_socket(const struct addrinfo results,
				  bool exit_on_error);
  struct SocketReturn call_connect(int socket_descriptor,
				   const struct addrinfo results,
				   bool exit_on_error);
  struct SocketReturn call_send(int socket_descriptor, char* message,
				int bytes);

  struct SocketReturn call_recv(int socket_descriptor, char buffer[],
				int max_bytes, bool exit_on_error);
  
  struct SocketReturn call_close(int socket_descriptor,
				 bool exit_on_error);

};

struct SocketReturn {
  char flags;
  int return_val;
  int errno;
};

#endif
