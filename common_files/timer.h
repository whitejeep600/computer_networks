
#ifndef CURVE_FEVER_TIMER_H
#define CURVE_FEVER_TIMER_H

#include <cstdint>
#include "../common_files/common.h"

#define CLIENT_TIMEOUT 2000


class timer {
    timestamp_t last_timestamp;

public:
    explicit timer();

    [[nodiscard]] timestamp_t get_elapsed() const;
    void set_timestamp_to_now();
    void add_to_timestamp(timestamp_t to_add);
    [[nodiscard]] bool communication_outdated() const;
    [[nodiscard]] bool time_for_client_message() const;
    timestamp_t get_now() const;
    void set_timestamp(timestamp_t new_timestamp);
};


#endif //CURVE_FEVER_TIMER_H
