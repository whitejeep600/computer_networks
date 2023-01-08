#ifndef CURVE_FEVER_EVENT_RECORD_H
#define CURVE_FEVER_EVENT_RECORD_H

#include <string>
#include <map>
#include <vector>

#include "../common_files/common.h"
#include "client_info.h"

#define NEW_GAME 0
#define PIXEL 1
#define PLAYER_ELIMINATED 2
#define GAME_OVER 3



class event_record {
    unsigned udp_message_length;
    void *record;

    event_record(unsigned usp_message_length_arg, void *udp_message_arg) :
            udp_message_length(usp_message_length_arg),
            record(udp_message_arg) {}

public:

    static event_record new_new_game(unsigned width, unsigned height, client_info_map_t &client_map);
    static event_record new_pixel(unsigned event_no, uint8_t player_no, unsigned x, unsigned y);
    static event_record new_player_eliminated(unsigned event_no, uint8_t player_no);
    static event_record new_game_over(unsigned event_no);

    unsigned get_length() const;
    void *get_record();
};

class event_record_list {
    std::vector<event_record> event_vector; // it can only increase in size during the game or be deleted altogether

public:

    event_record_list() :
            event_vector() {}

    event_record get_nth(unsigned int n);
    [[nodiscard]] unsigned get_size() const;
    void push_back(event_record &&new_event);
    void reset();
};



#endif //CURVE_FEVER_EVENT_RECORD_H
