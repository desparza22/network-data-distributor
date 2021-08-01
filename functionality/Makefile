test_address = -D TEST_ADDRESS=1
test_obj_str_iterator = -D TEST_OBJ_STR_ITERATOR=1
test_socket_agent = -D TEST_SOCKET_AGENT=1
test_thread_blocker = -D TEST_THREAD_BLOCKER=1

SOURCE = ./
FLAGS = -std=c++17 -g

address_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_address) -o address address.cpp

obj_str_iterator_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_obj_str_iterator) -o obj_str_iterator obj_str_iterator.cpp

socket_agent_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_socket_agent) -o socket_agent socket_agent.cpp

thread_blocker_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_thread_blocker) -o thread_blocker thread_blocker.cpp

clean:
	rm address obj_str_iterator *~
