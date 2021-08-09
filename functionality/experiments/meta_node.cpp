

class MetaNode;
struct Address;

class MetaNode {
  
private:
  const char server_IP[] = "127.0.0.1";
  const char server_port[] = "2000";
  
  struct Address** application_node_addrs;

  SocketServer* server;
  SocketClient* client;

  void handle_server();
  void handle_join();

  
public:
  void run();
};

struct Address {
  char* IP;
  char* port;
};


void MetaNode::MetaNode() {

}

void MetaNode::run() {

  //start process for handling
  //server

  //start process for maintenance
  //(periodically checking for system
  //switch, etc)
}

void MetaNode::handle_server() {
  //probably a call to accept, then a
  //switch statement depending on the message
  //received


  //one of the switch statements is a call to handle join
}


void MetaNode::handle_join() {
  //add node's address to the list and
  //respond with the list of addresses (maybe
  //cast list of structs to whatever message
  //format can be sent, precede with some flag
  //indicating it should be cast to an address
  //list)
}
