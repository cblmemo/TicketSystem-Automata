//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_STRING_T_H
#define TICKETSYSTEM_AUTOMATA_STRING_T_H

#include <iostream>

template<int len>
class string_t {
private:
    char store[len + 1];

public:
    string_t() {
        store[0] = '\0';
    }
    
    string_t(const std::string &o) {
        strcpy(store, o.c_str());
        store[o.length()] = '\0';
    }
    
    string_t(const char *o) {
        strcpy(store, o);
        store[strlen(o)] = '\0';
    }
    
    string_t(char *o) {
        strcpy(store, o);
        store[strlen(o)] = '\0';
    }
    
    string_t &operator=(const string_t<len> &o) {
        strcpy(store, o.store);
        store[strlen(o)] = '\0';
        return *this;
    }
    
    operator std::string() {
        return std::string(store);
    }
    
    bool operator==(const string_t<len> &o) {
        return strcmp(store, o.store) == 0;
    }
    
    bool operator!=(const string_t<len> &o) {
        return strcmp(store, o.store) != 0;
    }
    
    bool operator<(const string_t<len> &o) {
        return strcmp(store, o.store) < 0;
    }
    
    bool operator>(const string_t<len> &o) {
        return strcmp(store, o.store) > 0;
    }
    
    bool operator<=(const string_t<len> &o) {
        return strcmp(store, o.store) <= 0;
    }
    
    bool operator>=(const string_t<len> &o) {
        return strcmp(store, o.store) >= 0;
    }
    
    friend std::ostream &operator<<(std::ostream &os, const string_t &o) {
        os << o.store;
        return os;
    }
};

#endif //TICKETSYSTEM_AUTOMATA_STRING_T_H
