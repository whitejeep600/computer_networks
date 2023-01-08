
#ifndef CURVE_FEVER_CLIENT_H
#define CURVE_FEVER_CLIENT_H


#include <string>
#include <atomic>
#include "arrow_state.h"
#include "../common_files/timer.h"
#include "parsed_event.h"
#include "../common_files/synchronous_map.h"
#include "tcp_input_handler.h"

using parsed_events_map_t = synchronous_map<unsigned, parsed_event>;

class client {
    std::string game_server;
    std::string game_port;
    std::string gui_server;
    std::string gui_port;
    std::atomic_uint64_t session_id;
    std::atomic_uint32_t next_event_to_send_to_gui;
    uint32_t next_expected_event_no;
    arrow_state arrow;
    timer game_timer;
    tcp_input_handler tcp_handler;
    uint8_t *game_input_buffer;
    uint8_t *game_output_buffer;
    parsed_events_map_t parsed_events_map;
    uint32_t current_game_id;
    uint32_t current_players_no;
    uint32_t maxx;
    uint32_t maxy;
    bool game_active;
    std::string player_name;
    std::set<std::string> player_names;
    struct addrinfo* server_address;
    std::set<uint32_t> parsed_events_bool_set;

    void manage_gui();
    void manage_game();

    bool valid_event_and_should_add(parsed_event &event);

    void receive_from_gui();
    void send_to_gui();

    void receive_from_game_server();
    void cyclic_message_to_game_server();
    int32_t send_message_to_game_server();

public:

    explicit client(std::string &game_server_arg, std::string &game_port_arg, std::string &gui_server_arg,
                    std::string &gui_port_arg, uint64_t session_id_arg, std::string &player_name_arg);

    void initialize_game_communication();
    void initialize_gui_communication();
    void play();
};

#endif //CURVE_FEVER_CLIENT_H
