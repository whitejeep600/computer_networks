#include <netdb.h>
#include <cstring>
#include <iostream>
#include <cassert>

#include "event_record.h"

unsigned player_name_list_length(client_info_map_t& client_map) {
    unsigned res = 0;
    for (auto &a: client_map) {
        res += a.first.size() + 1;
    }
    return res;
}


event_record event_record::new_pixel(unsigned event_no, uint8_t player_no, unsigned x, unsigned y) {
    unsigned event_len = 14;
    unsigned total_len = 8 + event_len;
    auto record_data = (uint8_t *) malloc(total_len * sizeof(uint8_t));
    write_uint32_with_net_endianness(record_data, 0, event_len);
    write_uint32_with_net_endianness(record_data, 4, event_no);
    (record_data)[8] = PIXEL;
    (record_data)[9] = player_no;
    write_uint32_with_net_endianness(record_data, 10, x);
    write_uint32_with_net_endianness(record_data, 14, y);
    uint32_t cs = control_sum(record_data, total_len - 4);
    //std::cerr << cs << "\n";
    write_uint32_with_net_endianness(record_data, 18, cs);
    return event_record(total_len, record_data);
}

event_record event_record::new_player_eliminated(unsigned event_no, uint8_t player_no) {
    unsigned event_len = 6;
    unsigned total_len = 8 + event_len;
    auto record_data = (uint8_t *) malloc(total_len * sizeof(uint8_t));
    write_uint32_with_net_endianness(record_data, 0, event_len);
    write_uint32_with_net_endianness(record_data, 4, event_no);
    record_data[8] = PLAYER_ELIMINATED;
    record_data[9] = player_no;
    write_uint32_with_net_endianness(record_data, 10, control_sum(record_data, total_len - 4));
    return event_record(total_len, record_data);
}


event_record event_record::new_game_over(unsigned event_no) {
    unsigned event_len = 5;
    unsigned total_len = event_len + 8;
    auto record_data = (uint8_t *) malloc(total_len * sizeof(uint8_t));
    write_uint32_with_net_endianness(record_data, 0, event_len);
    write_uint32_with_net_endianness(record_data, 4, event_no);
    record_data[8] = GAME_OVER;
    write_uint32_with_net_endianness(record_data, 9, control_sum(record_data, total_len - 4));
    return event_record(total_len, record_data);
}


event_record event_record::new_new_game(unsigned width, unsigned height, client_info_map_t &client_map) {
    unsigned event_len = 13 + player_name_list_length(client_map);
    unsigned total_len = event_len + 8;
    auto record_data = (uint8_t *) malloc(total_len * sizeof(uint8_t));
    write_uint32_with_net_endianness(record_data, 0, event_len);
    write_uint32_with_net_endianness(record_data, 4, 0);
    record_data[8] = NEW_GAME;
    write_uint32_with_net_endianness(record_data, 9, width);
    write_uint32_with_net_endianness(record_data, 13, height);
    unsigned next_ind = 17;
    for (auto &client: client_map) {
        for (unsigned int i = 0; i < client.first.size(); ++i) {
            record_data[next_ind++] = client.first[i];
        }
        record_data[next_ind++] = '\0';
    }
    write_uint32_with_net_endianness(record_data, next_ind, control_sum(record_data, total_len - 4));
    return event_record(total_len, record_data);
}

unsigned event_record::get_length() const {
    return udp_message_length;
}

void *event_record::get_record() {
    return record;
}

event_record event_record_list::get_nth(unsigned int n) {
    auto res = event_vector[n];
    return res;
}

unsigned event_record_list::get_size() const {
    return event_vector.size();
}

void event_record_list::push_back(event_record &&new_event) {
    event_vector.push_back(new_event);
    // todo no na przyklad tutaj moze sie spokojnie wywalic - debiloodpornosc
}

void event_record_list::reset() {
    event_vector = std::vector<event_record>();
}