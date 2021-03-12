//
// Created by Rainy Memory on 2021/3/6.
//

#ifndef BPLUSTREE_HASHTYPE_H
#define BPLUSTREE_HASHTYPE_H

#include <iostream>

using std::string;

namespace RainyMemory{
    class HashType {
        enum prime {
            PRIME = 1000000007
        };
        
        static int getHash1(const string &o) {
            int h = o.length();
            for (char i : o)h = (h << 4) ^ (h >> 28) ^ i;
            return h % PRIME;
        }
        
        static int getHash2(const string &o) {
            int h = o.length();
            for (char i : o)h = (h << 7) ^ (h >> 25) ^ i;
            return h % PRIME;
        }
    
    public:
        int hash1 = 0;
        int hash2 = 0;
        
        explicit HashType(const string &o) : hash1(getHash1(o)), hash2(getHash2(o)) {}
        
        HashType() = default;
        
        HashType &operator=(const HashType &o) = default;
        
        void print() const {
            std::cout << "hash1: " << hash1 << " hash2: " << hash2 << std::endl;
        }
        
        bool operator==(const HashType &o) const {
            return hash1 == o.hash1 && hash2 == o.hash2;
        }
        
        bool operator!=(const HashType &o) const {
            return !(*this == o);
        }
        
        bool operator<(const HashType &o) const {
            if (hash1 < o.hash1)return true;
            else if (hash1 > o.hash1)return false;
            else return hash2 < o.hash2;
        }
        
        bool operator>(const HashType &o) const {
            return !(*this == o) && !(*this < o);
        }
        
        bool operator<=(const HashType &o) const {
            return *this == o || *this < o;
        }
        
        bool operator>=(const HashType &o) const {
            return *this == o || *this > o;
        }
    };
    
}

#endif //BPLUSTREE_HASHTYPE_H
