
#ifndef CURVE_FEVER_CLIENT_INFO_H
#define CURVE_FEVER_CLIENT_INFO_H


#include <map>
#include "../common_files/timer.h"
#include "management_order_queue.h"

class client_info {
    timestamp_t last_contact_timestamp;
    uint32_t smallest_not_updated_event; // co tam juz probowalismy doslac w ramach aktualizacji od serwera
    uint64_t session_id;
    bool active;
    management_order_queue q;
    const struct sockaddr_in6 address;
public:

    explicit client_info(uint64_t session_id_arg, sockaddr_in6 address_arg, timestamp_t timestamp_arg);

    client_info(const client_info &ci);
    void order_deletion(timestamp_t timestamp);
    void order_sending(unsigned turn_direction, unsigned next_requested, timestamp_t timestamp);
    void set_inactive();
    void set_active();
    bool queue_empty();
    management_order get_next_order();
    uint32_t get_smallest_not_updated_event() const;
    void set_smallest_not_updated_event(uint32_t new_event_no);
    uint64_t get_session_id() const;
    timestamp_t get_timestamp() const;
    void set_timestamp(timestamp_t timestamp);
    bool is_active() const;
    void touch_timer();
    [[nodiscard]] sockaddr_in6 get_address() const;
};

using client_info_map_t = std::map<std::string, client_info>;

#endif //CURVE_FEVER_CLIENT_INFO_H
