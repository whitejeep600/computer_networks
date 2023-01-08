#include <csignal>
#include <netdb.h>
#include <cassert>
#include <unistd.h>
#include <thread>
#include <fcntl.h>

#include "Server.h"

// better for it to be global for the sake of the signal handler
int socket_no;

void signal_handler(int) {
    close(socket_no);
    exit(1);
}

void set_socket_closing_at_exit() {
    struct sigaction killing_signal_action;
    sigset_t empty_block_mask;
    sigemptyset(&empty_block_mask);
    killing_signal_action.sa_handler = signal_handler;
    killing_signal_action.sa_mask = empty_block_mask;
    killing_signal_action.sa_flags = 0;
    sigaction(SIGINT, &killing_signal_action, nullptr);
    sigaction(SIGKILL, &killing_signal_action, nullptr);
    sigaction(SIGTERM, &killing_signal_action, nullptr);
}

/* much of the code below and in other places which handle network communication is based on
   the examples to lab scenario no. 4 (I didn't want to reinvent the wheel). */
void Server::initialize_socket() {
    struct sockaddr_in6 server_address;
    assert(socket_no >= 0);
    server_address.sin6_family = AF_INET6;
    server_address.sin6_addr = in6addr_any;
    server_address.sin6_port = htons(port_number);
    socket_no = socket(server_address.sin6_family, SOCK_DGRAM, 0);
    assert((bind(socket_no, (struct sockaddr *) &server_address,
                 (socklen_t) sizeof(server_address)) == 0));
    fcntl(socket_no, F_SETFL, O_NONBLOCK);
    set_socket_closing_at_exit();
    //std::cerr << "initialized socket\n";
}

bool Server::time_for_next_round() const {
    return timer_attribute.get_elapsed() >= time_per_round;
}

bool Server::should_generate_events() const {
    if(!game_active) return false;
    return time_for_next_round();
}

bool Server::should_start_new_game() const {
    //std::cerr << "active: " << active_players << ", nonempty: " << nonempty_players << "\n";
    return !game_active and active_players == nonempty_players and nonempty_players >= PLAYERS_TO_START;
}

bool Server::should_end_game() const {
    //std::cerr << "game active: " << game_active << ", active: " << active_players << "\n";
    return game_active and active_players == 1; // todo
}

bool Server::pixel_eaten(uint32_t x, uint32_t y) const {
    if(pixel_map.get(x, y)){
        //std::cerr << "pixel" << x << ", " << y << " eaten\n";
        return true;
    }
    else{
        //std::cerr << "pixel" << x << ", " << y << " not eaten\n";
        return false;
    }
}

bool Server::pixel_outside(uint32_t x, uint32_t y) const {
    if(x >= width_x || y >= height_y){
        //std::cerr << "pixel" << x << ", " << y << " outside\n";
        return true;
    }
    else{
        return false;
    }
}

bool Server::pixel_eaten_or_outside(uint32_t x, uint32_t y) {
    return pixel_outside(x, y) || pixel_eaten(x, y);
}

worm_position_info Server::get_random_worm_position() {
    double x = 0.5 + randomizer.get_next() % width_x;
    double y = 0.5 + randomizer.get_next() % height_y;
    //std::cerr << "random " << x << " " << y << "\n";
    unsigned int direction = randomizer.get_next() % 360;
    return worm_position_info(direction, x, y);
}

std::string get_string_address(sockaddr_in6 addr) {
    return std::string((char *) addr.sin6_addr.s6_addr);
}

void Server::eliminate_or_eat(worm_position_info &worm, uint32_t player_no, const std::string &name) {
    if(pixel_eaten_or_outside((uint32_t) worm.width_x, (uint32_t) worm.height_y)) {
        client_info_map.at(name).set_inactive();
        //std::cerr << "eliminating player " << name << "\n";
        --active_players;
        event_list.push_back(event_record::new_player_eliminated(event_list.get_size(), player_no));
    }
    else {
        pixel_map.set_true((uint32_t) worm.width_x, (uint32_t) worm.height_y);
        event_list.push_back(event_record::new_pixel(
                event_list.get_size(), player_no, (uint32_t) worm.width_x, (uint32_t) worm.height_y));
    }
}

void Server::generate_events() {
    unsigned player_no = 0;
    for (auto &worm: worm_map) {
        if(!worm.first.empty()) {
            worm.second.update_absolute_direction(turning_speed);
            auto prev_x = (uint32_t) worm.second.position.width_x;
            auto prev_y = (uint32_t) worm.second.position.height_y;
            worm.second.move();
            if(worm.second.position.width_x != prev_x || worm.second.position.height_y != prev_y) {
                //std::cerr << "worm moved\n";
                eliminate_or_eat(worm.second.position, player_no, worm.first);
            }
            else{
                //std::cerr << "worm didn't move\n";
            }
        }
        ++player_no;
    }
}

void Server::end_game() {
    //std::cerr << "ending game\n";
    game_active = false;
    worm_map.clear();
    active_players = 0;
    pixel_map.zero_out();
    for (auto &a: client_info_map) {
        a.second.set_inactive();
    }
    event_list.push_back(event_record::new_game_over(event_list.get_size()));
    serve_everyone();
    event_list.reset();
}

void Server::start_new_game() {
    //std::cerr << "starting game\n";
    current_game_id = randomizer.get_next();
    game_active = true;
    int player_no = 0;
    event_list.push_back(event_record::new_new_game(width_x, height_y, client_info_map)); // nie bo wm pusta
    for (auto const &a: client_info_map) {
        worm_position_info new_worm_position = get_random_worm_position();
        eliminate_or_eat(new_worm_position, player_no, a.first);
        worm_map.emplace(a.first, worm_info(new_worm_position));
        ++player_no;
    }
    timer_attribute.set_timestamp_to_now();
}

void Server::add_active_player_if_necessary(uint8_t turn_direction, const std::string &player_name) {
    if(turn_direction != FORWARD and !game_active and !client_info_map.at(player_name).is_active()) {
        //std::cerr << "adding new active player " << player_name << "\n";
        ++active_players;
        client_info_map.at(player_name).set_active();
    }
}

void Server::send_events_update(uint32_t current, uint32_t last, sockaddr_in6 &&address, uint8_t *buffer) {
    unsigned space_left;
    unsigned next_ind_to_write;
    unsigned next_length;
    while (current <= last) {
        write_uint32_with_net_endianness(buffer, 0, current_game_id);
        space_left = MAX_UDP_MESSAGE_SIZE - 4;
        next_ind_to_write = 4;
        auto next_record = event_list.get_nth(current);
        while (current <= last and (next_length = next_record.get_length()) <= space_left) {
            rewrite((uint8_t *) next_record.get_record(), buffer, next_length, next_ind_to_write);
            next_ind_to_write += next_length;
            space_left -= next_length;
            ++current;
            next_record = event_list.get_nth(current);
        }
        //auto eee = parsed_event::get_from_datagram(buffer, MAX_UDP_MESSAGE_SIZE-space_left);
        send_datagram(buffer, MAX_UDP_MESSAGE_SIZE - space_left, address, socket_no);
    }
}

void Server::detach_client(client_info &info, bool empty) {
    --connected_clients;
    if(empty) {
        --nonempty_players;
        empty_players_map.erase(get_string_address(info.get_address()));
    }
    else {
        std::string name = address_to_name_map.at(get_string_address(info.get_address()));
        if(client_info_map.at(name).is_active()) {
            --active_players;
        }
        client_info_map.erase(name);
    }
    address_to_name_map.erase(get_string_address(info.get_address()));
}


void Server::reconnect_with_new_session(client_message &message) {
    client_info_map.at(message.player_name).order_deletion(message.timestamp);
    welcome_new_client(message);
}

void Server::welcome_new_client(client_message &message) {
    //std::cerr << "welcoming new client " << message.player_name << "\n";
    ++connected_clients;
    if(message.player_name.empty()) {
        empty_players_map.emplace(get_string_address(message.client_address),
                                  client_info(message.session_id, message.client_address, message.timestamp));
    }
    else {
        if(!is_valid_player_name(message.player_name)) {
            return;
        }
        address_to_name_map.emplace(get_string_address(message.client_address), message.player_name);
        client_info_map.emplace(message.player_name,
                                client_info(message.session_id, message.client_address, message.timestamp));
        ++nonempty_players;
    }
}

void Server::receive_from_known_host(client_message &message) {
    //std::cerr << "receiving from known host " << message.player_name << "\n";
    if(message.player_name.empty()) {
        //std::cerr << "ordering to send\n";
        empty_players_map.at(get_string_address(message.client_address)).order_sending(message.turn_direction,
                                                                                       message.next_expected_event_no,
                                                                                       message.timestamp);
    }
    else {
        add_active_player_if_necessary(message.turn_direction, message.player_name);
        client_info_map.at(message.player_name).order_sending(message.turn_direction, message.next_expected_event_no,
                                                              message.timestamp);
    }
}

void Server::manage_message(client_message &message) {
    //std::cerr << "managing message from " << message.player_name << "\n";
    if(address_to_name_map.count(get_string_address(message.client_address)) == 0) {
        if((client_info_map.count(message.player_name) == 0 or message.player_name.empty())
           and connected_clients < MAX_PLAYERS) {
            welcome_new_client(message);
            receive_from_known_host(message);
        }
    }
    else {
        std::string previous_player_name = address_to_name_map.at(get_string_address(message.client_address));
        auto previous_id = client_info_map.at(previous_player_name).get_session_id();
        if(message.session_id > previous_id) {
            reconnect_with_new_session(message);
        }
        else if(message.session_id == previous_id) {
            if(previous_player_name == message.player_name) {
                receive_from_known_host(message);
            }
        }
    }
}

void Server::receive_communications() {
    static struct sockaddr_in6 client_address;
    socklen_t rcva_len;
    ssize_t len;
    rcva_len = (socklen_t) sizeof(client_address);
    bool message_sensible = true;
    len = recvfrom(socket_no, udp_in_buffer, MAX_UDP_MESSAGE_SIZE, 0,
                   (struct sockaddr *) &client_address, &rcva_len);
    while (len > 0) {
        client_message received = client_message::parse_from_message(udp_in_buffer, len, client_address,
                                                                     &message_sensible, timer_attribute.get_now());
        std::cerr << "received " << len << " bytes\n";
        if(message_sensible) {
            manage_message(received);
        }
        else{
            //std::cerr << "message not sensible\n";
        }
        len = recvfrom(socket_no, udp_in_buffer, MAX_UDP_MESSAGE_SIZE, 0,
                       (struct sockaddr *) &client_address, &rcva_len);
    }
}

void Server::serve_client(const std::string &name, client_info &client) {
    uint32_t no_of_events = event_list.get_size();
    while (!client.queue_empty()) {
        auto mo = client.get_next_order();
        if(mo.order_type == DELETE) {
            detach_client(client, true);
            break;
        }
        else {
            if(mo.timestamp - client.get_timestamp() >= CLIENT_TIMEOUT) {
                detach_client(client, true);
                break;
            }
            //std::cerr << "no of events: " << no_of_events << "game active: " << game_active <<"\n";
            if(no_of_events > 0) {
                send_events_update(mo.event_parameter, no_of_events - 1, client.get_address(), udp_out_buffer);
            }
            client.set_timestamp(mo.timestamp);
            if(!name.empty()) {
                add_active_player_if_necessary(mo.turn_parameter, name);
                if(game_active) {
                    worm_map.at(name).set_turn_direction(mo.turn_parameter); // niekoniecznie, o ile jeszcze nie gramy
                }
            }
        }
    }
    if(timer_attribute.get_now() - client.get_timestamp() >= CLIENT_TIMEOUT) {
        detach_client(client, true);
    }
    if(no_of_events > 0) {
        send_events_update(client.get_smallest_not_updated_event(), no_of_events - 1, client.get_address(),
                           udp_out_buffer);
    }
    client.set_smallest_not_updated_event(no_of_events);
    // the clients are supposed to receive information about an event once after its generation and once per every
    // time that the client sends a request with next_expected_event_no smaller than that of the event. the latter
    // case is handled in the loop, and the former afterwards. this is done in bulk, so to say - that is, we send
    // the events generated so far that have not been sent to the client yet (those send in the loop do not count).
}

void Server::serve_all_empty_clients() {
    for (auto &client: empty_players_map) {
        serve_client("", client.second);
    }
}

void Server::serve_all_players() {
    for (auto &client: client_info_map) {
        serve_client(client.first, client.second);
    }
}


void Server::serve_everyone() {
    serve_all_players();
    serve_all_empty_clients();
}

void Server::listen_for_clients() {
    //std::cerr << "beginning to listen for clients\n";
    while (true) {
        if(should_generate_events()) {
            timer_attribute.add_to_timestamp(time_per_round);
            generate_events();
        }
        else if(should_start_new_game()) {
            start_new_game();
        }
        else {
            if(should_end_game()) {
                end_game();
            }
            // here I'm making the bold assumption that the server will be fast enough to receive new communications
            // between the periods of generating new events. But for a reasonable number of rounds per turn, and
            // given the specified limit for the number of players, this should not be a problem.
            receive_communications();
            serve_everyone();
        }
    }
}




/* duze todo:
           serwer:
                kontrola czy wszystkie zmienne sie odpowiednio zmieniaja
                moze jakas eliminacja redundantnych (poza wydajnoscia) zmiennych zeby to jakos hulało
                Jeśli na przykład odbierzemy komunikat LEFT_KEY_DOWN bezpośrednio po komunikacie RIGHT_KEY_DOWN,
                 to skręcamy w lewo. Generalnie uwzględniamy zawsze najnowszy komunikat.


                 testy na students
                poprawne adresowanie przy laczeniu z gui
                sieciowa kolejnosc bajtow
                testowanie
                debug
                zwalnianie zasobow
                bledo - i debiloodpornosc
                komentarze, to_do revision
                valgrind
                opcja -O2
                usunac komunikaty na cerr
                co tam robić jak użytkownik przyciska szczauki jak debil
                unordered mapy?
        */