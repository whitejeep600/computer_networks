#include <cassert>
#include <iostream>
#include "client_message.h"

client_message client_message::parse_from_message(uint8_t *buffer, unsigned len, struct sockaddr_in6 sender,
                                                  bool *sensible, timestamp_t timestamp_arg) {
    if(len < 13) {
        *sensible = false;
        return client_message::get_empty();
    }
    uint64_t session_id = be64toh(((uint64_t *) buffer)[0]);
    uint8_t turn_direction = (buffer)[8];
    if(turn_direction != LEFT && turn_direction != RIGHT && turn_direction != FORWARD) {
        //std::cerr << "invalid turn direction\n";
        *sensible = false;
        return client_message::get_empty();
    }
    uint32_t next_expected_event_no = get_uint32_with_net_endianness(buffer, 9);
    auto player_name = std::string();;
    uint32_t i = 13;
    while(i < CLIENT_MESSAGE_SIZE and i < len){
        player_name += buffer[i];
        ++i;
    }
    if(player_name.length() != len - 13){
        //std::cerr << "something is no yes\n";
        exit(1);
    }
    if(!is_valid_player_name(player_name)) {
        *sensible = false;
        //std::cerr << "invalid player name\n";
        return client_message::get_empty();
    }
    else{
        //std::cerr << "correct player name: " << player_name << "\n";
    }
    return client_message(session_id, turn_direction, next_expected_event_no, player_name, sender, timestamp_arg);
}

