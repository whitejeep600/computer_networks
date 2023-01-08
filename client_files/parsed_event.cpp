#include <netdb.h>
#include <iostream>
#include "parsed_event.h"
#include "../common_files/common.h"

std::string number_to_string(uint32_t x) {
    uint32_t converted = htonl(x);
    uint32_t buf[1];
    buf[0] = converted;
    return std::string((const char *) buf);
}

std::string parsed_event::get_message_to_gui() {
    if(type == NEW_GAME) {
        std::string res = "NEW GAME " + number_to_string(first_parameter) + " " + number_to_string(second_parameter);
        for (auto &s: player_names) {
            res += " " + s;
        }
        return res;
    }
    else if(type == PIXEL) {
        return "PIXEL " + number_to_string(first_parameter) + " " + number_to_string(second_parameter) +
               player_names[0];
    }
    else {
        return "PLAYER_ELIMINATED " + player_names[0];
    }
}

uint32_t get_next_uint32(const uint8_t *data, unsigned *offset) {
    auto res = ntohl(((uint32_t *) (data + *offset))[0]);
    *offset += 4;
    return res;
}

std::vector<std::string> get_player_names_from_datagram(const uint8_t *datagram, uint32_t offset, uint32_t max_length) {
    std::vector<std::string> res;
    for (unsigned i = 0; i < max_length; ++i) {
        std::string next_name;
        while (i < max_length and datagram[offset + i] != '\0') {
            next_name += datagram[offset + i];
            ++i;
        }
        if(next_name.empty() || i == max_length) {
            exit(1);
        }
        res.push_back(next_name);
    }
    return res;
}


std::vector<parsed_event> parsed_event::get_from_datagram(const uint8_t *datagram, unsigned int left_in_datagram) {
    auto res = std::vector<parsed_event>();
    if(left_in_datagram < 4) {
        return res;
    }
    auto player_names_var = std::vector<std::string>();
    uint32_t first_parameter_var = 0;
    uint32_t second_parameter_var = 0;
    uint32_t next_byte_to_read = 0;
    uint32_t game_id_var = get_next_uint32(datagram, &next_byte_to_read);
    left_in_datagram -= 4;
    uint32_t length, event_no, calculated_control_sum;
    uint8_t type_var, player_number_var = 0;
    while (left_in_datagram != 0) {
        if(left_in_datagram < 13) {
            exit(1);
        }
        length = get_next_uint32(datagram, &next_byte_to_read);
        if(length + 8 > left_in_datagram) {
            exit(1);
        }
        calculated_control_sum = control_sum(datagram + next_byte_to_read - 4, 4 + length);
        //std::cerr << "calculated control sum: " << calculated_control_sum;
        event_no = get_next_uint32(datagram, &next_byte_to_read);
        type_var = datagram[next_byte_to_read++]; // now next_byte_to_read points to event_data
        switch (type_var) {
            case NEW_GAME:
                if(left_in_datagram < 25) { // len + event_no + event_type + crc32 + at least 4 bytes for player names
                    exit(1);
                }
                //std::cerr << " for newgame. ";
                first_parameter_var = get_next_uint32(datagram, &next_byte_to_read);
                second_parameter_var = get_next_uint32(datagram, &next_byte_to_read);
                player_names_var = get_player_names_from_datagram(datagram, next_byte_to_read, length - 13);
                next_byte_to_read += length - 13; // len - event_no - event_type - maxx - maxy
                break;
            case PIXEL:
                //std::cerr << " for pixel. ";
                player_number_var = (uint8_t) datagram[next_byte_to_read++];
                first_parameter_var = get_next_uint32(datagram, &next_byte_to_read);
                second_parameter_var = get_next_uint32(datagram, &next_byte_to_read);
                break;
            case PLAYER_ELIMINATED:
                player_number_var = (uint32_t) datagram[next_byte_to_read++];
                break;
            case GAME_OVER:
                break;
            default:
                next_byte_to_read += length - 5;
                break;
        }
        // now next_byte_to_read points to event_data crc32
        auto received_cs = get_next_uint32(datagram, &next_byte_to_read);
        //std::cerr << "received control sum: " << received_cs << "\n";
        if(received_cs != calculated_control_sum) {
            return res;
        }
        else if(type_var == NEW_GAME || type_var == PIXEL || type_var == PLAYER_ELIMINATED || type_var == GAME_OVER) {
            res.emplace_back(parsed_event(game_id_var, type_var, player_names_var, first_parameter_var, second_parameter_var, event_no,
                                          player_number_var));
        }
        left_in_datagram -= length + 8;
        player_names_var.clear();
    }
    return res;
}