#include <iostream>
#include <regex>
#include <map>
#include <set>
#include <cmath>
#include <unistd.h>
#include <cassert>
#include <sys/time.h>

#include "../common_files/timer.h"
#include "../common_files/common.h"
#include "../common_files/synchronous_map.h"

#include "tcp_input_handler.h"
#include "client.h"

#define MAX_PLAYERS 25



uint64_t get_micro_since_epoch() {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return 1000 * t.tv_sec + t.tv_usec;
}

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Too few command line arguments";
        exit(1);
    }
    auto game_server = std::string(argv[1]);
    std::string player_name_string;
    std::string game_port_string = "2021";
    std::string gui_server = "localhost";
    std::string gui_port_string = "20210";
    initial_argument_validation(argc, argv, 2);
    int option_char;
    const char* args = "n:p:i:r:";
    while((option_char=getopt(argc, argv, args)) != -1){
        switch(option_char){
            case('n'):
                player_name_string = optarg;
                break;
            case('p'):
                game_port_string = optarg;
                break;
            case('i'):
                gui_server = optarg;
                break;
            case('r'):
                gui_port_string = optarg;
                break;
            default:
                break;
        }
    }
    assert(is_valid_player_name(player_name_string));
    try{
        stoi(game_port_string);
        stoi(gui_port_string);
    }
    catch(std::invalid_argument& e){
        std::cout << "Invalid command line arguments";
        exit(1);
    }
    auto c = client(game_server, game_port_string, gui_server, gui_port_string, get_micro_since_epoch(), player_name_string);
    c.initialize_game_communication();
    c.initialize_gui_communication();
    c.play();
    return 0;
}
