#include <cstdint>
#include <cmath>
#include <iostream>

#include "worm_info.h"
#include "../common_files/common.h"

double TO_RADIANS_MULTIPLIER() {
    static const double res = 180.0 / M_PI;
    return res;
}

void worm_info::move() {
    //std::cerr << "previous position: (" << position.width_x << "," << position.height_y << ")\n";
    position.width_x += cos(position.direction * TO_RADIANS_MULTIPLIER());
    position.height_y += sin(position.direction * TO_RADIANS_MULTIPLIER());
    //std::cerr << "new position: (" << position.width_x << "," << position.height_y << ")\n";
}

void worm_info::set_turn_direction(uint32_t new_direction) {
    turn_direction = new_direction;
}

void worm_info::update_absolute_direction(uint32_t turning_speed) {
    if(turn_direction == LEFT){
        position.direction = position.direction - turning_speed % 360;
    }
    else if(turn_direction == RIGHT){
        position.direction = position.direction + turning_speed % 360;
    }
}
