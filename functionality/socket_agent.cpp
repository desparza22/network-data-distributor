#include "socket_agent.hpp"

SocketAgent::SocketAgent() {
  //clear and populate hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
}

void SocketAgent::print_error_info_helper(const char* func_name, int error) {
  std::cerr << func_name << " error. errno: " << error << '\n';
}

void SocketAgent::print_error_info(struct SocketReturn return_info) {
  switch(return_info.flags) {
  case NO_ERROR:
    std::cout << "No error was returned\n";
    break;

  case OPEN_ERROR:
    print_error_info_helper("Open", return_info.error);
    break;
  case ACCEPT_ERROR:
    print_error_info_helper("Accept", return_info.error);
    break;

  case CONNECT_ERROR:
    print_error_info_helper("Connect", return_info.error);
    break;

  case SEND_ERROR:
    print_error_info_helper("Send", return_info.error);
    break;

  case RECV_ERROR:
    print_error_info_helper("Recv", return_info.error);
    break;

  case CLOSE_ERROR:
    print_error_info_helper("Close", return_info.error);
    break;

  case GETADDRINFO_ERROR:
    std::cerr << "getaddrinfo error: " << gai_strerror(return_info.return_val) << '\n';
    break;

  case SOCKET_ERROR:
    print_error_info_helper("Socket", return_info.error);
    break;

  case BIND_ERROR:
    print_error_info_helper("Bind", return_info.error);
    break;

  case LISTEN_ERROR:
    print_error_info_helper("Listen", return_info.error);
    break;

  default:
    std::cout << __FUNCTION__ << " called with unrecognized return_info.flags\n";
  }
}

struct SocketReturn SocketAgent::agent_open_server(std::basic_string<char> IP,
						   std::basic_string<char> port,
						   int queue_size,
						   bool exit_on_error) {

  struct addrinfo results;
  struct SocketReturn return_info = call_getaddrinfo(IP, port, results, exit_on_error);
  if((return_info.flags & GETADDRINFO_ERROR) == GETADDRINFO_ERROR) {
    return return_info;
  }

  int socket_descriptor;
  return_info = call_socket(results, exit_on_error);
  if((return_info.flags & SOCKET_ERROR) == SOCKET_ERROR) {
    return return_info;

  } else {
    socket_descriptor = return_info.return_val;
  }

  return_info = call_bind(socket_descriptor, results, exit_on_error);
  if((return_info.flags & BIND_ERROR) == BIND_ERROR) {
    return return_info;
  }

  return_info = call_listen(socket_descriptor, queue_size, exit_on_error);
  return return_info;
}

struct SocketReturn SocketAgent::agent_accept(int socket_descriptor,
					      int& set_to_descriptor,
					      struct sockaddr& incoming,
					      int& incoming_size,
					      bool exit_on_error) {
  struct SocketReturn return_info = call_accept(socket_descriptor,
						incoming,
						incoming_size,
						exit_on_error);
  
  return return_info;
}

struct SocketReturn SocketAgent::agent_connect(std::basic_string<char> IP,
			   std::basic_string<char> port,
			   int& socket_descriptor, bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  struct addrinfo results;
  return_info = call_getaddrinfo(IP, port, results, exit_on_error);
  if((return_info.flags & GETADDRINFO_ERROR) == GETADDRINFO_ERROR) {
    return return_info;
  }


  return_info = call_socket(results, exit_on_error);
  if((return_info.flags & SOCKET_ERROR) == SOCKET_ERROR) {
    return return_info;

  } else {
    socket_descriptor = return_info.return_val;
  }
  
  
  return_info = call_connect(socket_descriptor, results, exit_on_error);
  return return_info;
}

struct SocketReturn SocketAgent::agent_send(std::basic_string<char> message,
				      int socket_descriptor, bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));

  char sendable_message[message.size() + 1];
  strcpy(sendable_message, message.c_str());
  int message_index = 0;

  int message_bytes = message.size();
  int bytes_left = message_bytes;

  while(bytes_left != 0) {
    return_info = call_send(socket_descriptor, &sendable_message[message_index],
			    bytes_left, exit_on_error);
    if((return_info.flags & SEND_ERROR) == SEND_ERROR) {
      return return_info;

    } else {
      int send_ret = return_info.return_val;
      message_index += send_ret;
      bytes_left -= send_ret;
    }
  }

  return return_info;
}

struct SocketReturn SocketAgent::agent_receive(std::basic_string<char>& buffer, int max_bytes,
					 int socket_descriptor, bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  char receivable_buffer[max_bytes + 1];
  return_info = call_recv(socket_descriptor, receivable_buffer, max_bytes,
			  exit_on_error);
  if((return_info.flags & RECV_ERROR) == RECV_ERROR) {
    return return_info;

  } else {
    int recv_ret = return_info.return_val;
    receivable_buffer[recv_ret] = '\0';
    buffer = std::basic_string<char>(&receivable_buffer[0]);
  }
  
  return return_info;
}

struct SocketReturn SocketAgent::agent_close_connection(int socket_descriptor, bool exit_on_error) {
  struct SocketReturn return_info = call_close(socket_descriptor, exit_on_error);  

  return return_info;
}

struct SocketReturn SocketAgent::call_getaddrinfo(std::basic_string<char> IP,
						  std::basic_string<char> port,
						  struct addrinfo& results,
						  bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));

  struct addrinfo* results_ptr;
  int gai_return_val = getaddrinfo(IP.c_str(), port.c_str(), &hints, &results_ptr);
  return_info.return_val = gai_return_val;
  
  if(gai_return_val != 0) {
    return_info.flags = GETADDRINFO_ERROR;
    return_info.error = errno;  
    
    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
    
  } else {
    results = *results_ptr;
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_socket(const struct addrinfo& results,
					     bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  int socket_descriptor = socket(results.ai_family, results.ai_socktype,
				 results.ai_protocol);
  return_info.return_val = socket_descriptor;
  
  if(socket_descriptor == -1) {
    return_info.flags = SOCKET_ERROR;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_bind(int socket_descriptor,
					   const struct addrinfo& results,
					   bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));

  int bind_ret = bind(socket_descriptor, results.ai_addr, results.ai_addrlen);
  return_info.return_val = bind_ret;
  
  if(bind_ret == -1) {
    return_info.flags = BIND_ERROR;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_listen(int socket_descriptor,
					     int queue_size,
					     bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));

  int listen_ret = listen(socket_descriptor, queue_size);
  return_info.return_val = listen_ret;
  
  if(listen_ret == -1) {
    return_info.flags = LISTEN_ERROR;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_accept(int socket_descriptor, struct sockaddr& incoming,
					     int& incoming_size, bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));

  
  int accept_ret = accept(socket_descriptor, &incoming, &incoming_size);
  return_info.return_val = accept_ret;
  
  if(accept_ret == -1) {
    return_info.flags = ACCEPT_ERROR;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_connect(int socket_descriptor,
				 const struct addrinfo results,
				 bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  int connect_ret = connect(socket_descriptor,
			    results.ai_addr,
			    results.ai_addrlen);
  if(connect_ret == -1) {
    return_info.flags = CONNECT_ERROR;
    return_info.return_val = connect_ret;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_send(int socket_descriptor,
					   char* message,
					   int bytes,
					   bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  int send_ret = send(socket_descriptor,
		      message,
		      bytes,
		      0);
  
  if(send_ret == -1) {
    return_info.flags = SEND_ERROR;
    return_info.return_val = send_ret;
    return_info.error = errno;
    
    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }
  
  return return_info;
}

struct SocketReturn SocketAgent::call_recv(int socket_descriptor,
					   char buffer[],
					   int max_bytes,
					   bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  int recv_ret = recv(socket_descriptor, buffer, max_bytes, 0);
  if(recv_ret == -1) {
    return_info.flags = RECV_ERROR;
    return_info.return_val = recv_ret;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
    
  } else {
    return_info.return_val = recv_ret;
  }

  return return_info;
}

struct SocketReturn SocketAgent::call_close(int socket_descriptor,
					    bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));
  
  int close_ret = close(socket_descriptor);
  if(close_ret == -1) {
    return_info.flags = CLOSE_ERROR;
    return_info.return_val = close_ret;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  }
  
  return return_info;
}

#ifdef TEST_SOCKET_AGENT


int main(int argc, char* argv[]) {

  //Two processes opening as servers and connecting to each other
  std::basic_string<char> server1_ip("127.0.0.1");
  std::basic_string<char> server2_ip("127.0.0.1");

  std::basic_string<char> server1_port("2000");
  std::basic_string<char> server2_port("2001");

  int shared_var1 = 1;
  int shared_var2 = 2;
  
  int fork_ret = fork();
  if(fork_ret == -1) {
    std::cout << "Error with fork. errno: " << errno << '\n';
    exit(1);
  }

  if(fork_ret) { //parent
    struct SocketReturn return_info = agent_open_server(server1_ip, server1_port, true);
    
    
    
  } else { //child
    struct SocketReturn return_info = agent_open_server(server2_ip, server2_port, true);
    
    
  }
  

  return 0;
}

#endif
