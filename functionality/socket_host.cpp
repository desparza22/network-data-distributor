
class SocketHost {

private:
  bool is_open = false;
  int host_descriptor = -1;

public:
  int open_server(char* IP, char* port);
  int accept(int* set_connection_descriptor);
  int send(int connection_descriptor, char* message);
  int receive(int connection_descriptor, char** buffer,
	      int* set_bytes_received);
  int close_server();

};
