
#ifndef CURVE_FEVER_MANAGEMENT_ORDER_QUEUE_H
#define CURVE_FEVER_MANAGEMENT_ORDER_QUEUE_H


#include <deque>
#include <mutex>
#include "../common_files/timer.h"
#include "../common_files/common.h"

#define DELETE true
#define SEND false

class management_order {
    bool order_type;
    direction_t turn_parameter;
    uint32_t event_parameter;
    timestamp_t timestamp;

public:
    explicit management_order(bool order_type_arg, uint8_t turn_parameter_arg, uint32_t event_parameter_arg, timestamp_t timestamp_arg) :
            order_type(order_type_arg),
            turn_parameter(turn_parameter_arg),
            event_parameter(event_parameter_arg),
            timestamp(timestamp_arg) {}

    friend class Server;
};

class management_order_queue {
    std::deque<management_order> dq;

public:
    management_order_queue() :
            dq(){}


    void order_deletion(timestamp_t timestamp);
    void order_sending(uint8_t turn_direction, uint32_t next_requested, timestamp_t timestamp);
    management_order get_first();
    bool empty();
};


#endif //CURVE_FEVER_MANAGEMENT_ORDER_QUEUE_H
