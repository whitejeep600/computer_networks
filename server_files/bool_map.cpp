
#include "bool_map.h"

bool bool_map::get(unsigned int x, unsigned int y) const {
    return map[x][y];
}

void bool_map::set_true(unsigned int x, unsigned int y) {
    map[x][y] = true;
}

void bool_map::set_false(unsigned int x, unsigned int y) {
    map[x][y] = false;
}

void bool_map::zero_out() {
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            map[i][j] = false;
        }
    }
}