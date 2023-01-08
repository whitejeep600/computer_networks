#include "rand_generator.h"

uint64_t MULTIPLIER = 279410273;
uint64_t MODULUS = 4294967291;

uint32_t rand_generator::get_next() {
    uint32_t res = value;
    uint64_t temp = ((uint64_t) value * MULTIPLIER) % MODULUS;
    value = (uint32_t) temp;
    return res;
}