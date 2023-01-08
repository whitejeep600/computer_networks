#ifndef CURVE_FEVER_TCP_INPUT_HANDLER_H
#define CURVE_FEVER_TCP_INPUT_HANDLER_H


#include <cstdint>
#include <string>

#define CLIENT_MESSAGE_SIZE 34 // one extra for the null terminator
#define MAX_UDP_MESSAGE_SIZE 550
#define MAX_GUI_TCP_MESSAGE_SIZE 15

class tcp_input_handler {
    uint8_t* buffer;
    bool overflow;
    int socket_no;
    unsigned position_in_buffer;
    unsigned left_in_buffer;
    std::string res;

public:
    explicit tcp_input_handler();

    void set_socket_no(int socket_no_arg){
        socket_no = socket_no_arg;
    }

    std::string next_message();
};

#endif //CURVE_FEVER_TCP_INPUT_HANDLER_H
