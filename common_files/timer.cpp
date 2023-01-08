#include "timer.h"

#include <chrono>
#include <iostream>

uint64_t get_current_miliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

timer::timer() {
    last_timestamp = get_current_miliseconds();
}

uint64_t timer::get_elapsed() const {
    return get_current_miliseconds() - last_timestamp;
}

void timer::set_timestamp_to_now() {
    last_timestamp = get_current_miliseconds();
}

void timer::add_to_timestamp(uint64_t to_add) {
    last_timestamp += to_add;
}

bool timer::communication_outdated() const {
    return get_current_miliseconds() - last_timestamp >= CLIENT_TIMEOUT;
}

bool timer::time_for_client_message() const {
    auto elapsed = get_elapsed();
    return elapsed >= 30;
}

uint64_t timer::get_now() const {
    return get_current_miliseconds();
}

void timer::set_timestamp(timestamp_t new_timestamp) {
    last_timestamp = new_timestamp;
}
