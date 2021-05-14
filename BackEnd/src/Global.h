//
// Created by Rainy Memory on 2021/4/15.
//

#ifndef TICKETSYSTEM_AUTOMATA_GLOBAL_H
#define TICKETSYSTEM_AUTOMATA_GLOBAL_H

#include "../include/RTL/string_t.h"
#include "../include/BPlusTree.h"
#include "../include/Parser.h"
#include <chrono>

using RainyMemory::BPlusTree;
using RainyMemory::HashMap;
using RainyMemory::TokenScanner;
using RainyMemory::Parser;
using RainyMemory::sortVector;

#define speedup

#ifdef speedup
const char endl = '\n';
#else
using std::endl;
#endif

enum coefficient {
    SEAT_NUM_INFINITY = 2000000000,
    USER_MANAGER_MEMORYPOOL_CAPACITY = 100000,
    TRAIN_MANAGER_MEMORYPOOL_CAPACITY = 300
};

constexpr int lengthOfChineseCharacters(int length) {
    return length * 4;
}

template<int len>
struct hash_string_t {
    int operator()(const string_t<len> &o) const {
        int l = o.length();
        int h = l;
        for (int i = 0; i < l; i++)h = (h << 7) ^ (h >> 25) ^ o[i];
        return h;
    }
};

#endif //TICKETSYSTEM_AUTOMATA_GLOBAL_H
