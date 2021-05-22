//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_STRING_T_H
#define TICKETSYSTEM_AUTOMATA_STRING_T_H

#include "rmstream.h"
#include <iostream>
#include <cstring>

namespace RainyMemory {
    template<int len>
    class string_t {
        /*
         * class string_t
         * --------------------------------------------------------
         * A class implements fixed length string to support file operations.
         *
         */
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
            if (this == &o)return *this;
            strcpy(store, o.store);
            store[len] = '\0';
            return *this;
        }
        
        string_t &operator=(const std::string &o) {
            strcpy(store, o.c_str());
            store[o.length()] = '\0';
            return *this;
        }
        
        operator std::string() {
            return std::string(store);
        }
        
        int length() const {
            return strlen(store);
        }
        
        char &operator[](int index) {
            return store[index];
        }
        
        const char &operator[](int index) const {
            return store[index];
        }
        
        bool operator==(const string_t<len> &o) const {
            return strcmp(store, o.store) == 0;
        }
        
        bool operator!=(const string_t<len> &o) const {
            return strcmp(store, o.store) != 0;
        }
        
        bool operator<(const string_t<len> &o) const {
            return strcmp(store, o.store) < 0;
        }
        
        bool operator>(const string_t<len> &o) const {
            return strcmp(store, o.store) > 0;
        }
        
        bool operator<=(const string_t<len> &o) const {
            return strcmp(store, o.store) <= 0;
        }
        
        bool operator>=(const string_t<len> &o) const {
            return strcmp(store, o.store) >= 0;
        }
        
        friend rmstream &operator<<(rmstream &os, const string_t &o) {
            os << o.store;
            return os;
        }
    };
}

#endif //TICKETSYSTEM_AUTOMATA_STRING_T_H
