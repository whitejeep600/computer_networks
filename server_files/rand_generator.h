
#ifndef CURVE_FEVER_RAND_GENERATOR_H
#define CURVE_FEVER_RAND_GENERATOR_H


#include <cstdint>

class rand_generator {
    uint32_t value;

public:
    explicit rand_generator(uint32_t seed) :
            value(seed) {}

    uint32_t get_next();
};


#endif //CURVE_FEVER_RAND_GENERATOR_H
