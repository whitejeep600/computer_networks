#include <iostream>
#include "arrow_state.h"

arrow_state::arrow_state() :
        mut(),
        state(FORWARD),
        left_down(false),
        right_down(false){

}

direction_t arrow_state::get() {
    mut.lock();
    auto res = state;
    mut.unlock();
    return res;
}

void arrow_state::update(std::string& message) {
    std::cerr << message << "\n";
    if(message == "LEFT_KEY_UP"){
        left_down = false;
    }
    else if(message == "RIGHT_KEY_UP"){
        right_down = false;
    }
    else if(message == "LEFT_KEY_DOWN"){
        left_down = true;
    }
    else if(message == "RIGHT_KEY_DOWN"){
        right_down = true;
    }
    determine_state();
    std::cerr << "set arrow state: " << (int) state << "\n";
}

void arrow_state::determine_state() {
    if(left_down ^ right_down){
        if(left_down){
            state = LEFT;
        }
        else{
            state = RIGHT;
        }
    }
    else{
        state = FORWARD;
    }
}
