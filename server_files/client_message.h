
#ifndef CURVE_FEVER_CLIENT_MESSAGE_H
#define CURVE_FEVER_CLIENT_MESSAGE_H

#include "../common_files/common.h"
#include "event_record.h"

#define CLIENT_MESSAGE_SIZE 33

class client_message {
    uint64_t session_id;
    direction_t turn_direction;
    uint32_t next_expected_event_no;
    std::string player_name;
    struct sockaddr_in6 client_address;
    timestamp_t timestamp;

public:
    client_message(uint64_t session_id_arg, uint8_t turn_direction_arg, uint32_t next_expected_event_no_arg,
                   std::string &player_name_arg, struct sockaddr_in6 client_address_arg, timestamp_t timestamp_arg) :
            session_id(session_id_arg),
            turn_direction(turn_direction_arg),
            next_expected_event_no(next_expected_event_no_arg),
            player_name(player_name_arg),
            client_address(client_address_arg),
            timestamp(timestamp_arg){}


    static client_message
    parse_from_message(uint8_t *buffer, unsigned len, struct sockaddr_in6 sender, bool *sensible,
                       timestamp_t timestamp_arg);

    static client_message get_empty() {
        struct sockaddr_in6 empty;
        std::string empty_string;
        return client_message(0, 0, 0, empty_string, empty, 0);
    }

    friend class Server;
};


#endif //CURVE_FEVER_CLIENT_MESSAGE_H
