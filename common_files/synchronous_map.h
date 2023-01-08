#ifndef CURVE_FEVER_SYNCHRONOUS_MAP_H
#define CURVE_FEVER_SYNCHRONOUS_MAP_H


#include <map>
#include <mutex>
#include <set>

template<typename key_t, typename val_t>
class synchronous_map {
    std::map<key_t, val_t> map_attr;
    std::mutex mut;

public:
    val_t at(const key_t &key);
    int count(key_t key);
    void insert(key_t k, val_t v);
    void remove(key_t k);
    void clear();
    std::set<key_t> get_key_set();

    void lock_map();
    void unlock_map();

    explicit synchronous_map():
            map_attr(),
            mut(){}
};

template<typename key_t, typename val_t>
val_t synchronous_map<key_t, val_t>::at(const key_t& key){
    return map_attr.at(key);
}

template<typename key_t, typename val_t>
int synchronous_map<key_t, val_t>::count(key_t key){
    return map_attr.count(key);
}

template<typename key_t, typename val_t>
void synchronous_map<key_t, val_t>::lock_map(){
    mut.lock();
}

template<typename key_t, typename val_t>
void synchronous_map<key_t, val_t>::unlock_map(){
    mut.unlock();
}

template<typename key_t, typename val_t>
void synchronous_map<key_t, val_t>::clear(){
    map_attr = std::map<key_t, val_t>();
}

template<typename key_t, typename val_t>
void synchronous_map<key_t, val_t>::insert(key_t k, val_t v) {
    map_attr.insert({k, v});
}

template<typename key_t, typename val_t>
void synchronous_map<key_t, val_t>::remove(key_t k) {
    map_attr.erase(k);
}



#endif //CURVE_FEVER_SYNCHRONOUS_MAP_H
