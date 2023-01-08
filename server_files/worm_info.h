#ifndef NEW_CURVE_FEVER_WORM_INFO_H
#define NEW_CURVE_FEVER_WORM_INFO_H

#include <memory>
#include <mutex>
#include <map>


class worm_position_info {
    uint32_t direction;
    double width_x;
    double height_y;
public:
    worm_position_info(unsigned direction_arg, double width_x_arg, double height_y_arg) :
            direction(direction_arg),
            width_x(width_x_arg),
            height_y(height_y_arg) {}

    friend class worm_info;

    friend class Server;
};

class worm_info {
    worm_position_info position;
    uint32_t turn_direction;

public:
    worm_info(uint32_t direction_arg, double width_x_arg, double height_y_arg) :
            position(direction_arg, width_x_arg, height_y_arg),
            turn_direction(0) {}

    explicit worm_info(worm_position_info &&position_arg) :
            position(position_arg),
            turn_direction(0) {}

            // todo
    explicit worm_info(worm_position_info &position_arg) :
            position(position_arg),
            turn_direction(0) {}


    void update_absolute_direction(uint32_t turning_speed);
    void set_turn_direction(uint32_t new_direction);
    void move();


    friend class Server;
};

using worm_map_t = std::map<std::string, worm_info>;


#endif //NEW_CURVE_FEVER_WORM_INFO_H
