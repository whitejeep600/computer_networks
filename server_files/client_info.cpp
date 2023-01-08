
#include <iostream>
#include "client_info.h"

void client_info::set_inactive() {
    //std::cerr << "setting inactive\n";
    active = false;
}

void client_info::set_active() {
    //std::cerr << "setting active\n";
    active = true;
}


client_info::client_info(uint64_t session_id_arg, struct sockaddr_in6 address_arg, timestamp_t timestamp_arg) :
        last_contact_timestamp(timestamp_arg),
        smallest_not_updated_event(0),
        session_id(session_id_arg),
        active(false),
        address(address_arg){}

client_info::client_info(const client_info &ci) :
        last_contact_timestamp(ci.last_contact_timestamp),
        smallest_not_updated_event(0),
        session_id(ci.session_id),
        active(false),
        address(ci.address)
        {}

void client_info::order_deletion(timestamp_t timestamp) {
    q.order_deletion(timestamp);
}

void client_info::order_sending(unsigned int turn_direction, unsigned int next_requested, timestamp_t timestamp) {
    q.order_sending(turn_direction, next_requested, timestamp);
}

bool client_info::queue_empty() {
    return q.empty();
}

management_order client_info::get_next_order() {
    return q.get_first();
}

timestamp_t client_info::get_timestamp() const {
    return last_contact_timestamp;
}

uint32_t client_info::get_smallest_not_updated_event() const {
    return smallest_not_updated_event;
}

void client_info::set_smallest_not_updated_event(uint32_t new_event_no) {
    smallest_not_updated_event = new_event_no;
}

uint64_t client_info::get_session_id() const {
    return session_id;
}

bool client_info::is_active() const {
    return active;
}

sockaddr_in6 client_info::get_address() const {
    return address;
}

void client_info::set_timestamp(timestamp_t timestamp_arg) {
    last_contact_timestamp = timestamp_arg;
}

