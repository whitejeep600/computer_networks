
#ifndef CURVE_FEVER_PARSED_EVENT_H
#define CURVE_FEVER_PARSED_EVENT_H


#include <cstdint>
#include <string>
#include <vector>

#define NEW_GAME 0
#define PIXEL 1
#define PLAYER_ELIMINATED 2
#define GAME_OVER 3

using event_type = uint8_t;

class parsed_event {
    uint32_t game_id;
    event_type type;
    std::vector<std::string> player_names;
    uint32_t first_parameter;
    uint32_t second_parameter;
    uint32_t event_no;
    uint8_t player_number;
    // this format is capable of storing every kind of event.

public:
    std::string get_message_to_gui();

    parsed_event(uint32_t game_id_arg, event_type type_arg, std::vector<std::string> &player_names_arg,
                 uint32_t first_parameter_arg, uint32_t second_parameter_arg, uint32_t event_no_arg,
                 uint8_t player_number_arg) :
            game_id(game_id_arg),
            type(type_arg),
            player_names(player_names_arg),
            first_parameter(first_parameter_arg),
            second_parameter(second_parameter_arg),
            event_no(event_no_arg),
            player_number(player_number_arg){}

    static std::vector<parsed_event> get_from_datagram(const uint8_t *datagram, unsigned datagram_length);

    friend class client;
    friend class Server; // todo
};

#endif //CURVE_FEVER_PARSED_EVENT_H
