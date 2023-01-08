all: screen-worms-server screen-worms-client

screen-worms-server: server_main.o timer.o common.o rand_generator.o event_record.o worm_info.o bool_map.o management_order_queue.o \
client_info.o server.o client_message.o
	g++ -g -std=c++17 -Wall -Wextra -pthread -o screen-worms-server server_main.o timer.o common.o rand_generator.o \
		event_record.o worm_info.o bool_map.o management_order_queue.o client_info.o server.o client_message.o

rand_generator.o: server_files/rand_generator.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o rand_generator.o server_files/rand_generator.cpp

server_main.o: server_files/server_main.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o server_main.o server_files/server_main.cpp

server.o: server_files/Server.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o server.o server_files/Server.cpp

event_record.o: server_files/event_record.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o event_record.o server_files/event_record.cpp

worm_info.o: server_files/worm_info.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o worm_info.o server_files/worm_info.cpp

bool_map.o: server_files/bool_map.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o bool_map.o server_files/bool_map.cpp

client_info.o: server_files/client_info.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o client_info.o server_files/client_info.cpp

client_message.o: server_files/client_message.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o client_message.o server_files/client_message.cpp

management_order_queue.o: server_files/management_order_queue.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o management_order_queue.o server_files/management_order_queue.cpp

common.o: common_files/common.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o common.o common_files/common.cpp

timer.o: common_files/timer.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -o timer.o common_files/timer.cpp

screen-worms-client: client_main.o timer.o common.o arrow_state.o parsed_event.o client.o tcp_input_handler.o
	g++ -g -std=c++17 -Wall -Wextra -pthread -o screen-worms-client client_main.o timer.o common.o arrow_state.o parsed_event.o \
	client.o tcp_input_handler.o

client_main.o: client_files/client_main.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -pthread -o client_main.o client_files/client_main.cpp

arrow_state.o: client_files/arrow_state.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -o arrow_state.o client_files/arrow_state.cpp

parsed_event.o: client_files/parsed_event.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -o parsed_event.o client_files/parsed_event.cpp

tcp_input_handler.o: client_files/tcp_input_handler.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -o tcp_input_handler.o client_files/tcp_input_handler.cpp

client.o: client_files/client.cpp
	g++ -g -std=c++17 -Wall -Wextra -c -o client.o client_files/client.cpp

clean:
	rm *.o screen-worms-server screen-worms-client