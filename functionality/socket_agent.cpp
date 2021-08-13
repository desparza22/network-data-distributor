#include "socket_agent.hpp"

SocketAgent::SocketAgent() {
  
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
						   int& set_to_descriptor,
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

  set_to_descriptor = socket_descriptor;
  return return_info;
}

struct SocketReturn SocketAgent::agent_accept(int socket_descriptor,
					      int& set_to_descriptor,
					      bool exit_on_error) {

  struct sockaddr incoming;
  socklen_t incoming_size = sizeof(incoming);
  struct SocketReturn return_info = call_accept(socket_descriptor,
						set_to_descriptor,
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
					    int socket_descriptor,
					    bool exit_on_error) {
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

  //clear and populate hints
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
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

struct SocketReturn SocketAgent::call_accept(int socket_descriptor,
					     int& set_to_descriptor,
					     struct sockaddr& incoming,
					     socklen_t& incoming_size,
					     bool exit_on_error) {
  struct SocketReturn return_info;
  memset(&return_info, 0, sizeof(return_info));

  int accept_ret = accept(socket_descriptor, &incoming,
			  &incoming_size);
  return_info.return_val = accept_ret;
  
  if(accept_ret == -1) {
    return_info.flags = ACCEPT_ERROR;
    return_info.error = errno;

    if(exit_on_error) {
      print_error_info(return_info);
      exit(1);
    }
  } else {
    set_to_descriptor = accept_ret;
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
  return_info.return_val = send_ret;
  
  if(send_ret == -1) {
    return_info.flags = SEND_ERROR;
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

#include <thread>
#include <vector>

void send_receive(int thread_number,
		  std::vector<std::basic_string<char>> ips,
		  std::vector<std::basic_string<char>> ports,
		  Semaphore coordinate_this,
		  Semaphore coordinate_main,
		  std::shared_ptr<bool> success);

void connect_send(std::basic_string<char> other_ip,
		  std::basic_string<char> other_port,
		  std::basic_string<char> message);

void listen_accept_receive(int socket_descriptor,
			   std::basic_string<char>& buffer);

void send_receive(int thread_number,
		  std::vector<std::basic_string<char>> ips,
		  std::vector<std::basic_string<char>> ports,
		  Semaphore coordinate_this,
		  Semaphore coordinate_main,
		  std::shared_ptr<bool> success) {

  SocketAgent socket_agent;
  struct SocketReturn socket_return;

  int num_threads = ips.size();
  int num_other_threads = num_threads - 1;
  int queue_size = num_other_threads;
  bool exit_on_error = true;
  int socket_descriptor;
  std::basic_string<char> ip = ips[thread_number];
  std::basic_string<char> port = ports[thread_number];
  socket_return = socket_agent.agent_open_server(ip,
						 port,
						 socket_descriptor,
						 queue_size,
						 exit_on_error);
  
  coordinate_main.give(1);
  coordinate_this.acquire(1);

  std::vector<std::thread> senders;
  for(int i = 0; i < num_threads; i++) {
    if(i == thread_number) {
      continue;
    }
    
    std::basic_string<char> other_ip = ips[i];
    std::basic_string<char> other_port = ports[i];
    std::basic_string<char> message_to_send = std::to_string(i);
    std::thread sender(connect_send, other_ip, other_port, message_to_send);
    senders.push_back(std::move(sender));
  }

  std::basic_string<char> received;
  std::basic_string<char> expecting = std::to_string(thread_number);

  *success = true;
  for(int i = 0; i < num_other_threads; i++) {
    listen_accept_receive(socket_descriptor, received);
    *success = *success && (received.compare(expecting) == 0);
  }

  for(int i = 0; i < senders.size(); i++) {
    std::thread sender = std::move(senders[i]);
    sender.join();
  }

  coordinate_main.give(1);
  coordinate_this.acquire(1);
  
  socket_return = socket_agent.agent_close_connection(socket_descriptor,
						      exit_on_error);
}

void connect_send(std::basic_string<char> other_ip,
		  std::basic_string<char> other_port,
		  std::basic_string<char> message) {
  SocketAgent socket_agent;
  struct SocketReturn socket_return;
  int socket_descriptor;
  bool exit_on_error = true;
  socket_return = socket_agent.agent_connect(other_ip,
					     other_port,
					     socket_descriptor,
					     exit_on_error);

  socket_return = socket_agent.agent_send(message,
					  socket_descriptor,
					  exit_on_error);

  socket_return = socket_agent.agent_close_connection(socket_descriptor,
						      exit_on_error);
}

void listen_accept_receive(int socket_descriptor,
			   std::basic_string<char>& buffer) {
  SocketAgent socket_agent;
  struct SocketReturn socket_return;
  
  int other_descriptor;
  bool exit_on_error = true;
  socket_return = socket_agent.agent_accept(socket_descriptor,
					    other_descriptor,
					    exit_on_error);

  int max_size = 1000;
  socket_return = socket_agent.agent_receive(buffer,
					     max_size,
					     other_descriptor,
					     exit_on_error);
}


int main(int argc, char* argv[]) {

  //n threads opening servers and sending messages
  int num_threads = 30;

  std::vector<std::basic_string<char>> ips(num_threads,
					   std::basic_string<char>("127.0.0.1"));
  std::vector<std::basic_string<char>> ports;
  int start_port = 2000;
  for(int port = start_port; port < start_port + num_threads; port++) {
    std::basic_string<char> port_string = std::to_string(port);
    ports.push_back(port_string);
  }
  
  Semaphore coordinate_threads;
  Semaphore coordinate_main;  

  std::vector<std::shared_ptr<bool>> successes;
  for(int i = 0; i < num_threads; i++) {
    successes.push_back(std::shared_ptr<bool>(new bool(false)));
  }

  std::vector<std::thread> threads;
  for(int i = 0; i < num_threads; i++) {
    int thread_number = i;
    std::thread server(send_receive,
		       thread_number,
		       ips,
		       ports,
		       coordinate_threads,
		       coordinate_main,
		       successes[i]);
    threads.push_back(std::move(server));
  }

  coordinate_main.acquire(num_threads);
  coordinate_threads.give(num_threads);

  coordinate_main.acquire(num_threads);
  coordinate_threads.give(num_threads);

  for(int i = 0; i < num_threads; i++) {
    std::thread server = std::move(threads[i]);
    server.join();
  }

  bool all_threads_successful = true;
  for(int i = 0; i < num_threads; i++) {
    all_threads_successful = all_threads_successful && *(successes[i]);
  }

  if(all_threads_successful) {
    std::cout << "Tests passed!\n";
  } else {
    std::cout << "Error\n";
  }

  return 0;
}

#endif
