//
// Created by Rainy Memory on 2021/2/2.
//

#ifndef BPLUSTREE_BPLUSTREESTRING_H
#define BPLUSTREE_BPLUSTREESTRING_H

#include <iostream>
#include <string>

using std::ostream;

#define MAX_KEY_LENGTH 65

class BPlusTreeString {
public:
    char key[MAX_KEY_LENGTH] = {0};
    
    BPlusTreeString() {
        memset(key, 0, sizeof(key));
        key[0] = '\0';
    }
    
    BPlusTreeString(const BPlusTreeString &o) {
        strcpy(key, o.key);
    }
    
    explicit BPlusTreeString(const string &o) {
        strcpy(key, o.c_str());
    }
    
    bool operator<(const BPlusTreeString &o) const {
        return strcmp(key, o.key) < 0;
    }
    
    bool operator==(const BPlusTreeString &o) const {
        return strcmp(key, o.key) == 0;
    }
    
    bool operator!=(const BPlusTreeString &o) const {
        return !((*this) == o);
    }
    
    bool operator>(const BPlusTreeString &o) const {
        if ((*this) == o)return false;
        return !((*this) < o);
    }
    
    bool operator<=(const BPlusTreeString &o) const {
        if ((*this) == o)return true;
        return (*this) < o;
    }
    
    bool operator>=(const BPlusTreeString &o) const {
        if ((*this) == o)return true;
        return (*this) > o;
    }
    
    BPlusTreeString &operator=(const BPlusTreeString &o) {
        if (this == &o)return *this;
        strcpy(key, o.key);
        return *this;
    }
    
    BPlusTreeString &operator=(const string &o) {
        strcpy(key, o.c_str());
        return *this;
    }
    
    explicit operator string() {
        string temp = key;
        return temp;
    }
    
    friend ostream &operator<<(ostream &os, const BPlusTreeString &o) {
        os << o.key;
        return os;
    }
};

#endif //BPLUSTREE_BPLUSTREESTRING_H
