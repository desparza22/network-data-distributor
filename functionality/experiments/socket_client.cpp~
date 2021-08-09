

class SocketClient {

private:
  bool is_connected = false;
  int connection_descriptor = -1;

public:
  int connect(char* IP, char* port);
  int send(char* message);
  int receive(char** buffer, int* set_bytes_received);
  int close_connection();

};
