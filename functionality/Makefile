test_address = -D TEST_ADDRESS=1
test_obj_str_iterator = -D TEST_OBJ_STR_ITERATOR=1
test_socket_agent = -D TEST_SOCKET_AGENT=1
test_thread_blocker = -D TEST_THREAD_BLOCKER=1
test_semaphore = -D TEST_SEMAPHORE=1
test_object_pool = -D TEST_OBJECT_POOL=1
test_socket_communicator = -D TEST_SOCKET_COMMUNICATOR=1

SOURCE = ./
FLAGS = -std=c++20 -g

address_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_address) -o address address.cpp

obj_str_iterator_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_obj_str_iterator) -o obj_str_iterator obj_str_iterator.cpp

socket_agent_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_socket_agent) -o socket_agent socket_agent.cpp semaphore.cpp thread_blocker.cpp

thread_blocker_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_thread_blocker) -o thread_blocker thread_blocker.cpp

semaphore_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_semaphore) -o semaphore semaphore.cpp thread_blocker.cpp

object_pool_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_object_pool) -o object_pool object_pool.cpp semaphore.cpp thread_blocker.cpp

socket_communicator_test:
	g++ -I$(SOURCE) $(FLAGS) $(test_socket_communicator) -o socket_communicator socket_communicator.cpp communicator.cpp socket_agent.cpp object_pool.cpp semaphore.cpp thread_blocker.cpp address.cpp

clean:
	rm address obj_str_iterator socket_agent thread_blocker semaphore object_pool socket_communicator *~ *.o
