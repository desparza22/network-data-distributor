#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

class CommunicationLayer {

private:
  int local_socket;
  int incoming_socket;
  
  char* message_buffer;
  
  struct sockaddr_in local_addr;
  struct sockaddr_in external_connection;
  

  void create_socket();
  void fill_and_bind_socket_addr(char* IP, uint16_t port_num);
  

public:
  CommunicationLayer(int max_message_size);

  void listen_and_accept(int num_connections,
			      struct sockaddr_in** fill_with_connections);
  void connect_to(char* IP, uint16_t port_num);
};

CommunicationLayer::CommunicationLayer(int max_message_size) {
  try {
    this->create_socket();

  } catch(const char* msg) {
    throw msg;
  }

  try {
    this->fill_and_bind_socket_addr();

  } catch(const char* msg) {
    close(this->local_socket);
    throw msg;
  }
  
  this->message_buffer = malloc((max_message_size + 1) * sizeof(char));
}

void CommunicationLayer::create_socket() {
  //Create the local socket
  this->local_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(this->local_socket < 0) {
    char create_socket_error_message[100];
    sprintf(create_socket_error_message,
	    "Error while creating socket. Returned %d\n",
	    this->local_socket);
    throw create_socket_error_message;
  }
}

void CommunicationLayer::fill_and_bind_socket_addr(char* IP, uint16_t port_num) { 
  //Fill the fields of the socket address
  this->local_addr.sin_family = AF_INET;
  this->local_addr.sin_port = htons(port_num);
  this->local_addr.sin_addr.s_addr = inet_addr(IP);

  //Bind the socket address to the local socket
  int bind_res = bind(local_socket, (struct sockaddr*)&(this->local_addr),
		      sizeof(this->local_addr));
  if(bind_res) {
    char bind_socket_error_message[100];
    sprintf(bind_socket_error_message,
	    "Error while binding socket. Returned %d\n",
	    bind_res);
    throw bind_socket_error_message;
  }
}

void CommunicationLayer::listen_and_accept(int num_connections,
					   struct sockaddr_in** fill_with_connections,
					   int* fill_with_sockets) {

  int listen_res = listen(this->local_socket, num_connections);
  if(listen_res) {
    char listen_error_message[100];
    sprintf(listen_error_message,
	    "Error while listening. Returned %d\n",
	    listen_res);
    throw listen_error_message;
  }

  for(int i = 0; i < num_connections; i++) {
    connection_size = sizeof(*(fill_with_connections[i]));
    fill_with_sockets[i] = accept(this->local_socket,
				  (struct sockaddr*)(fill_with_connections[i]),
				  &connection_size);

    if(fill_with_sockets[i] < 0) {
      char accept_error_message[100];
      sprintf(accept_error_message,
	      "Error while accepting connection. Returned %d\n",
	      fill_with_sockets[i]);
      throw accept_error_message;
    }
  }
}

void CommunicationLayer::connect_to(char* IP, uint16_t port_num) {
  this->external_connection.sin_family = AF_INET;
  this->external_connection.sin_port = htons(port_num);
  this->external_connection.sin_addr.s_addr = inet_addr(IP);

  int connection_res = connect(this->local_socket,
			       (struct sockaddr*)&(this->external_connection),
			       sizeof(this->external_connection));
  if(connection_res) {
    char connect_error_message[100];
    sprintf(connect_error_message,
	    "Error while connecting. Returned %d\n",
	    connection_res);
    throw connect_error_message;
  }
}
