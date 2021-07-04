
#define QUERY_ALL 0
#define UPDATE_ALL 1
#define TRANSITIONING 2

class CenterNode() {

 private:
  int state;
  
  CenterNodeAddress** connections;
  int num_connections;
  
 public:
  CenterNode();

};

CenterNode::CenterNode(CenterNodeAddress** initial_connections, int num_initial_connections) {
  this->connections = initial_connections;
  this->num_initial_connections = num_initial_connections;
}
