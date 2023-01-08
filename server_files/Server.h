
#ifndef NEW_CURVE_FEVER_SERVER_H
#define NEW_CURVE_FEVER_SERVER_H

#include <iostream>
#include "rand_generator.h"
#include "../common_files/timer.h"
#include "bool_map.h"
#include "event_record.h"
#include "client_message.h"
#include "client_info.h"
#include "../common_files/synchronous_map.h"
#include "worm_info.h"
#include "../client_files/parsed_event.h"


#define MAX_PLAYERS 25
#define MAX_UDP_MESSAGE_SIZE 550
#define MAX_GUI_TCP_MESSAGE_SIZE 15


using empty_players_map_t = std::map<std::string, client_info>;
using address_to_name_map_t = std::map<std::string, std::string>;


class Server {
    uint32_t port_number;
    rand_generator randomizer;
    uint32_t turning_speed;
    double time_per_round;
    uint32_t width_x;
    uint32_t height_y;
    uint32_t connected_clients;
    uint32_t current_game_id;
    bool game_active;
    timer timer_attribute;
    uint8_t *udp_in_buffer;
    uint8_t* udp_out_buffer;
    client_info_map_t client_info_map;
    address_to_name_map_t address_to_name_map;
    empty_players_map_t empty_players_map;
    uint32_t nonempty_players; // name not empty
    uint32_t active_players; // pressed an arrow and not dead yet
    bool_map pixel_map;
    event_record_list event_list;
    worm_map_t worm_map;

    [[nodiscard]] bool should_generate_events() const;
    [[nodiscard]] bool time_for_next_round() const;
    [[nodiscard]] bool should_start_new_game() const;
    [[nodiscard]] bool should_end_game() const;
    [[nodiscard]] bool pixel_eaten(uint32_t x, uint32_t y) const;
    [[nodiscard]] bool pixel_outside(uint32_t x, uint32_t y) const;
    bool pixel_eaten_or_outside(unsigned int x, unsigned int y);
    void generate_events();
    void start_new_game();
    void end_game();
    void receive_communications();
    void manage_message(client_message &message);
    void welcome_new_client(client_message &message);
    void detach_client(client_info& info, bool empty);
    void reconnect_with_new_session(client_message &message);
    void receive_from_known_host(client_message &message);
    worm_position_info get_random_worm_position();
    void eliminate_or_eat(worm_position_info &worm, uint32_t player_no, const std::string& name);
    void add_active_player_if_necessary(uint8_t turn_direction, const std::string &player_name);
    void send_events_update(uint32_t first, uint32_t last, sockaddr_in6 &&address, uint8_t *buffer);
    void serve_all_empty_clients();
    void serve_all_players();
    void serve_everyone();
    void serve_client(const std::string& name, client_info &client);

public:
    explicit Server(int &port_number_arg, time_t rand_seed_arg, unsigned turning_speed_arg, unsigned rounds_per_sec_arg,
                    uint32_t width_x_arg, uint32_t height_y_arg) :
            port_number(port_number_arg),
            randomizer(rand_seed_arg),
            turning_speed(turning_speed_arg),
            time_per_round(1.0 / rounds_per_sec_arg),
            width_x(width_x_arg),
            height_y(height_y_arg),
            connected_clients(0),
            current_game_id(0),
            game_active(false),
            timer_attribute(),
            client_info_map(),
            address_to_name_map(),
            empty_players_map(),
            nonempty_players(0),
            active_players(0),
            pixel_map(width_x_arg, height_y_arg),
            event_list(),
            worm_map() {
                            udp_in_buffer = (uint8_t *) malloc(CLIENT_MESSAGE_SIZE * sizeof(uint8_t));
                            udp_out_buffer = (uint8_t *) malloc(MAX_UDP_MESSAGE_SIZE * sizeof(uint8_t));
                            if(udp_in_buffer == nullptr || udp_out_buffer == nullptr) {
                                std::cerr << "Error allocating memory for the server, terminating";
                                exit(1);
                            }
                        }

    void initialize_socket();
    void listen_for_clients();

};


#endif //NEW_CURVE_FEVER_SERVER_H
