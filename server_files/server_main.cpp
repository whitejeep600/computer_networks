#include <string>
#include <regex>
#include <set>
#include <cmath>
#include <unistd.h>
#include "Server.h"
#include "../common_files/common.h"


bool is_number(const std::string& s){
    for(const auto a: s){
        if(!isdigit(a)){
            return false;
        }
    }
    return true;
}


int main(int argc, char** argv) {
    int port_number;
    std::string port_number_str = "2021";
    uint32_t rand_seed = time(nullptr);
    auto rand_seed_string = std::string();
    int32_t turning_speed = 6;
    auto turning_speed_string = std::string();
    int32_t rounds_per_sec = 50;
    auto rounds_per_sec_string = std::string();
    int32_t width_x = 640;
    auto width_x_string = std::string();
    int32_t height_y = 480;
    auto height_y_string = std::string();
    int32_t option_char;
    std::string option_argument;
    initial_argument_validation(argc, argv, 1);
    const char *args = "p:s:t:v:w:h:";
    while ((option_char = getopt(argc, argv, args)) != -1) {
        switch (option_char) {
            case ('p'):
                port_number_str = optarg;
                break;
            case ('s'):
                rand_seed_string = optarg;
                break;
            case ('t'):
                turning_speed_string = optarg;
                break;
            case ('v'):
                rounds_per_sec_string = optarg;
                break;
            case ('w'):
                width_x_string = optarg;
                break;
            case ('h'):
                height_y_string = optarg;
                break;
            default:
                exit(1);
        }
    }
    try {
        port_number = stoi(port_number_str);
        if(!port_number_str.empty()){
            exit_if(!is_number(port_number_str), "invalid port number\n");
        }
        if(!rand_seed_string.empty()){
            rand_seed = stoi(rand_seed_string);
            exit_if(!is_number(rand_seed_string), "invalid random seed\n");
        }
        if(!turning_speed_string.empty()){
            turning_speed = stoi(turning_speed_string);
            exit_if(!is_number(turning_speed_string), "invalid turning speed\n");
        }
        if(!rounds_per_sec_string.empty()){
            rounds_per_sec = stoi(rounds_per_sec_string);
            exit_if(!is_number(rounds_per_sec_string), "invalid rounds per second\n");
        }
        if(!width_x_string.empty()){
            width_x = stoi(width_x_string);
            exit_if(!is_number(width_x_string), "invalid width\n");
        }
        if(!height_y_string.empty()){
            height_y = stoi(height_y_string);
            exit_if(!is_number(height_y_string), "invalid height\n");
        }
    }
    catch (std::invalid_argument &e) {
        exit(1);
    }
    exit_if(!within_range(port_number, 1, 65535), "invalid port number");
    exit_if(!within_range(rand_seed, 1, UINT32_MAX), "invalid random seed");
    exit_if(!within_range(width_x, 16, 5000), "invalid board width");
    exit_if(!within_range(height_y, 16, 5000), "invalid board height");
    exit_if(!within_range(rounds_per_sec, 1, 1000), "invalid number of rounds per second");
    exit_if(!within_range(turning_speed, 1, 90), "invalid turning speed");  // hope that qualifies as reasonable
    auto server = Server(port_number, rand_seed, turning_speed, rounds_per_sec, (uint32_t) width_x, (uint32_t)  height_y);
    server.initialize_socket();
    server.listen_for_clients();
    return 0;
}


