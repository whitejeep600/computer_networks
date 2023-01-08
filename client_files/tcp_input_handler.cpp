#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>
#include "tcp_input_handler.h"

unsigned get_bytes(int socket_no, uint8_t *buffer, unsigned max) {
    int read_bytes = read(socket_no, buffer, max);
    if(read_bytes == -1) {
        return 0;
    }
    else {
        return read_bytes;
    }
}

std::string tcp_input_handler::next_message() {
    char new_char;
    do {
        while (left_in_buffer != 0) {
            new_char = (char) buffer[position_in_buffer++];
            if(new_char == '\n') {
                if(!overflow) {
                    --left_in_buffer;
                    std::string result = res;
                    res = "";
                    return result;
                }
                else {
                    overflow = false;
                }
            }
            else {
                res += new_char;
            }
            --left_in_buffer;
            if(res.length() == MAX_GUI_TCP_MESSAGE_SIZE) {
                overflow = true;
                res = "";
            }
        }
        position_in_buffer = 0;
    } while ((left_in_buffer = get_bytes(socket_no, buffer, MAX_GUI_TCP_MESSAGE_SIZE)) != 0);
    return "";
}

tcp_input_handler::tcp_input_handler() :
        overflow(false),
        socket_no(0),
        position_in_buffer(0),
        left_in_buffer(0),
        res() {
    buffer = (uint8_t *) malloc(MAX_GUI_TCP_MESSAGE_SIZE * sizeof(uint8_t));
}
