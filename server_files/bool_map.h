
#ifndef CURVE_FEVER_BOOL_MAP_H
#define CURVE_FEVER_BOOL_MAP_H


#include <cstdint>
#include <cstdlib>



class bool_map {
    uint32_t width;
    uint32_t height;
    bool **map;

public:

    bool_map(uint32_t width_arg, uint32_t height_arg) :
            width(width_arg),
            height(height_arg) {
        map = (bool **) malloc(width * sizeof(bool *));
        for (unsigned int i = 0; i < width; i++) {
            map[i] = (bool *) malloc(height * sizeof(bool));
            for (unsigned int j = 0; j < height; j++) {
                set_false(i, j);
            }
        }
    }

    [[nodiscard]] bool get(uint32_t x, uint32_t y) const;
    void set_true(uint32_t x, uint32_t y);
    void set_false(uint32_t x, uint32_t y);
    void zero_out();
};


#endif //CURVE_FEVER_BOOL_MAP_H
