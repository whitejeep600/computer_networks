#include <unistd.h>
#include <csignal>
#include <netdb.h>
#include <cstring>
#include <cassert>
#include <netinet/tcp.h>
#include <thread>
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>

#include "client.h"
#include "../common_files/common.h"


int game_socket_no;
int gui_socket_no;

void signal_handler(int) {
    close(game_socket_no);
    close(gui_socket_no);
    exit(1);
}

void set_sockets_closing_at_exit() {
    struct sigaction killing_signal_action;
    sigset_t empty_block_mask;
    sigemptyset(&empty_block_mask);
    killing_signal_action.sa_handler = signal_handler;
    killing_signal_action.sa_mask = empty_block_mask;
    killing_signal_action.sa_flags = 0;
    sigaction(SIGINT, &killing_signal_action, nullptr);
    sigaction(SIGKILL, &killing_signal_action, nullptr);
    sigaction(SIGTERM, &killing_signal_action, nullptr);
}

void gui_error() {
    std::cerr << "connection to gui failed\n";
    exit(1);
}

void game_error() {
    std::cerr << "connection to game server failed\n";
    exit(1);
}

void client::initialize_game_communication() {
    struct addrinfo addr_hints;
    (void) memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_UNSPEC;
    addr_hints.ai_socktype = SOCK_DGRAM;
    addr_hints.ai_protocol = IPPROTO_UDP;
    addr_hints.ai_flags = 0;
    addr_hints.ai_addrlen = 0;
    addr_hints.ai_addr = nullptr;
    addr_hints.ai_canonname = nullptr;
    addr_hints.ai_next = nullptr;
    exit_if(getaddrinfo(game_server.c_str(), game_port.c_str(), &addr_hints, &server_address) != 0,
            "game connection failure") ;
    if((game_socket_no = socket(PF_INET6, SOCK_DGRAM, 0)) < 0) {
        game_error();
    }
    exit_if(connect(game_socket_no, server_address->ai_addr, server_address->ai_addrlen) != 0, "failed to connect to server");
    fcntl(game_socket_no, F_SETFL, O_NONBLOCK);
}

int32_t client::send_message_to_game_server() {
    return send(game_socket_no, game_output_buffer, 13 + player_name.length(), 0);
}

void client::initialize_gui_communication() {
    sockaddr_in6 gui_address;
    memset(&gui_address, 0, sizeof(gui_address));
    gui_address.sin6_family = AF_INET6;
    inet_pton(AF_INET6, gui_server.c_str(), &gui_address.sin6_addr);
    gui_address.sin6_port = htons(atoi(gui_port.c_str()));
    if((gui_socket_no = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        gui_error();
    }
    int flag = 1;
    setsockopt(gui_socket_no, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
    if(connect(gui_socket_no, (struct sockaddr *) &gui_address, sizeof(gui_address)) != 0) {
        gui_error();
    }
    fcntl(gui_socket_no, F_SETFL, O_NONBLOCK);
    tcp_handler.set_socket_no(gui_socket_no);
}

void client::receive_from_gui() {
    std::string message;
    while (!(message = tcp_handler.next_message()).empty()) {
        arrow.update(message);
        std::cerr << "received from gui";
    }
}

void client::send_to_gui() {
    parsed_events_map.lock_map();
    if(parsed_events_map.count(next_event_to_send_to_gui) != 0) {
        std::cerr << "sending to gui\n";
        auto event = parsed_events_map.at(next_event_to_send_to_gui);
        parsed_events_map.remove(next_event_to_send_to_gui++);
        parsed_events_map.unlock_map();
        std::string to_send = event.get_message_to_gui();
        exit_if(write(gui_socket_no, to_send.c_str(), to_send.size()) == -1, "error on sending to server");
    }
    else {
        parsed_events_map.unlock_map();
    }
}

bool validate_and_add_player_names(std::vector<std::string> &player_names) {
    if(player_names.size() < PLAYERS_TO_START) {
        return false;
    }
    for (auto &name:player_names) {
        if(!is_valid_player_name(name)) {
            return false;
        }
    }
    return true;
}


// returns false if the current event is not to be added to the parsed events map
bool client::valid_event_and_should_add(parsed_event &event) {
    switch (event.type) {
        case NEW_GAME:
            current_game_id = event.game_id;
            next_event_to_send_to_gui = 0;
            next_expected_event_no = 0;
            maxx = event.first_parameter;
            maxy = event.second_parameter;
            parsed_events_map.clear();
            parsed_events_bool_set.clear();
            current_players_no = event.player_names.size();
            game_active = true;
            exit_if(maxx == 0 || maxy == 0 || !validate_and_add_player_names(event.player_names),
                    "game arguments make no sense");
            std::cerr << "got new game\n";
            return true;
        case PIXEL:
            if(event.game_id != current_game_id || !game_active) {
                return false;
            }
            exit_if(event.first_parameter > maxx || event.second_parameter > maxy ||
                    event.player_number >= current_players_no,
                    "invalid PIXEL event");
            std::cerr << "got pixel\n";
            return true;
        case PLAYER_ELIMINATED:
            if(event.game_id != current_game_id || game_active) {
                return false;
            }
            exit_if(event.player_number >= current_players_no || player_names.count(event.player_names[0]) == 0,
                    "event for an invalid player");
            std::cerr << "got pl eliminated\n";
            return true;
        case GAME_OVER:
            std::cerr << "got game over\n";
            if(event.game_id != current_game_id) {
                return false;
            }
            game_active = false;
            return true;
        default:
            assert(false); // for debugging
    }
}

void client::manage_gui() {
    std::cerr << "managing gui\n";
    while (true) {
        // Theoretically this control flow means that the gui server can flood us with information so that
        // we are never able to actually send something back. That said, the speed at which it sends messages
        // is limited by how fast the player can press arrows, so it can't reasonably be more than one message
        // per 50 ms, which gives us ample time to process the message and send our own.
        receive_from_gui();
        send_to_gui();
    }
}

void client::receive_from_game_server() {
    static struct sockaddr_in6 client_address;
    socklen_t rcva_len;
    rcva_len = (socklen_t) sizeof(client_address);
    int32_t len;
    //std::cerr << "receiving from the game server\n";
    if((len = recvfrom(game_socket_no, game_input_buffer, MAX_UDP_MESSAGE_SIZE, 0,
                       (struct sockaddr *) &client_address, &rcva_len)) != -1) {
        std::cerr << "received " << len << " bytes\n";
        auto new_events = parsed_event::get_from_datagram(game_input_buffer, len);
        parsed_events_map.lock_map();
        for (parsed_event &event: new_events) {
            if(valid_event_and_should_add(event)) {
                if(parsed_events_bool_set.count(event.event_no) != 0) {
                    parsed_events_map.insert(event.event_no, event);
                }
                else {
                    parsed_events_bool_set.insert(event.event_no);
                    while (parsed_events_bool_set.count(next_expected_event_no++)); // can't believe this works
                    // the idea here is to skip the events that we've managed to process before
                }
            }
        }
        parsed_events_map.unlock_map();
    }
    else {
        //std::cerr << "nothing to receive from the game server\n";
    }
}


void client::cyclic_message_to_game_server() {
    game_output_buffer[8] = arrow.get();
    for (unsigned i = 0; i < player_name.length(); ++i) {
        game_output_buffer[13 + i] = player_name[i]; // todo to nie powinno byc potrzebne
    }
    write_uint32_with_net_endianness(game_output_buffer, 9, next_expected_event_no);
    ((uint64_t *) (game_output_buffer))[0] = htobe64(session_id);
    std::cerr << "sending to game server. session id: " << be64toh(((uint64_t *) (game_output_buffer))[0]) << "\n"
              << "arrow direction: " << (uint32_t) game_output_buffer[8] << ", next expected event: "
              << ntohl(((uint32_t *) (game_output_buffer + 9))[0]) << "\n\n";

    int sent_bytes;
    exit_if((sent_bytes = send_message_to_game_server()) == -1,"error on sending to server");

    //std::cerr << "sent " << sent_bytes << " bytes\n";
}


void client::manage_game() {
    std::cerr << "managing the game server\n";
    while (true) {
        if(game_timer.time_for_client_message()) {
            game_timer.add_to_timestamp(30);
            cyclic_message_to_game_server();
        }
        else {
            receive_from_game_server();
        }
    }
}

void client::play() {
    for (unsigned i = 0; i < player_name.length(); ++i) {
        game_output_buffer[13 + i] = player_name[i];
    }
    ((uint64_t *) (game_output_buffer))[0] = htobe64(session_id);
    set_sockets_closing_at_exit();
    game_timer.set_timestamp_to_now();
    std::cerr << "starting to play\n";
    std::thread gui_thread{&client::manage_gui, this};
    std::thread game_thread{&client::manage_game, this};
    gui_thread.detach();
    game_thread.join(); // effectively an infinite loop, the thread will only end due to an error situation or
    // after being terminated with a signal
}

client::client(std::string &game_server_arg, std::string &game_port_arg, std::string &gui_server_arg,
               std::string &gui_port_arg, uint64_t session_id_arg, std::string &player_name_arg) :
        game_server(game_server_arg),
        game_port(game_port_arg),
        gui_server(gui_server_arg),
        gui_port(gui_port_arg),
        session_id(session_id_arg),
        next_event_to_send_to_gui(0),
        next_expected_event_no(0),
        arrow(),
        game_timer(),
        tcp_handler(),
        parsed_events_map(),
        current_game_id(0),
        current_players_no(),
        maxx(0),
        maxy(0),
        game_active(false),
        player_name(player_name_arg),
        player_names(),
        server_address(),
        parsed_events_bool_set() {
    game_input_buffer = (uint8_t *) malloc(MAX_GUI_TCP_MESSAGE_SIZE * sizeof(uint8_t));
    game_output_buffer = (uint8_t *) malloc(player_name.length() + 13);
    if(game_input_buffer == nullptr || game_output_buffer == nullptr) {
        std::cout << "Unable to allocate the required memory.\n";
        exit(1);
    }
}