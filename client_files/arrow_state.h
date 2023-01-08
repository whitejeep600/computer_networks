
#ifndef CURVE_FEVER_ARROW_STATE_H
#define CURVE_FEVER_ARROW_STATE_H

#include <cstdint>
#include <mutex>

#define FORWARD 0
#define RIGHT 1
#define LEFT 2

using direction_t = uint8_t;

class arrow_state {
    std::mutex mut;
    direction_t state;
    bool left_down;
    bool right_down;

    void determine_state();

public:
    explicit arrow_state();
    direction_t get();
    void update(std::string& message);

};

#endif //CURVE_FEVER_ARROW_STATE_H
