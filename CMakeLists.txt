cmake_minimum_required(VERSION 3.17)
project(new_curve_fever)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-pthread -O0 ")

add_executable(screen-worms-client client_files/client_main.cpp common_files/timer.cpp common_files/common.cpp client_files/arrow_state.cpp client_files/arrow_state.h client_files/parsed_event.cpp client_files/parsed_event.h client_files/tcp_input_handler.cpp client_files/tcp_input_handler.h client_files/client.cpp client_files/client.h)
add_executable(screen-worms-server server_files/server_main.cpp common_files/timer.cpp common_files/common.cpp server_files/rand_generator.cpp server_files/worm_info.cpp server_files/event_record.cpp server_files/bool_map.cpp server_files/management_order_queue.cpp server_files/client_info.cpp server_files/Server.cpp server_files/Server.h server_files/client_message.cpp client_files/parsed_event.cpp)