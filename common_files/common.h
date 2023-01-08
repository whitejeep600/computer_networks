#ifndef CURVE_FEVER_COMMON_H
#define CURVE_FEVER_COMMON_H

#include <string>
#include <netdb.h>

#define FORWARD 0
#define RIGHT 1
#define LEFT 2

#define PLAYERS_TO_START 2 // todo

using direction_t = uint8_t;

using timestamp_t = uint64_t;

// x in [a, b]
bool within_range(uint64_t x, uint64_t a, uint64_t b);
bool is_valid_player_name(std::string& s);
uint32_t control_sum(const uint8_t *data, unsigned data_length);
void rewrite(const uint8_t *source, uint8_t *destination, unsigned length, unsigned dest_start);
void send_datagram(uint8_t *buffer, unsigned length, sockaddr_in6 &target, int socket_no);
void exit_if(bool condition, std::string&& message);
void initial_argument_validation(int argc, char** argv, unsigned start);
void write_uint32_with_net_endianness(uint8_t* target, uint32_t offset, uint32_t arg);
uint32_t get_uint32_with_net_endianness(uint8_t* target, uint32_t offset);
#endif //CURVE_FEVER_COMMON_H
