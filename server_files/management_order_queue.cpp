#include "management_order_queue.h"


void management_order_queue::order_deletion(timestamp_t timestamp) {
    dq.push_front(management_order(DELETE, 0, 0, timestamp));
}

void management_order_queue::order_sending(uint8_t turn_direction, uint32_t next_requested, timestamp_t timestamp) {
    dq.emplace_back(SEND, turn_direction, next_requested, timestamp);
}

management_order management_order_queue::get_first() {
    management_order res = dq.front();
    dq.pop_front();
    return res;
}

bool management_order_queue::empty() {
    return dq.empty();
}