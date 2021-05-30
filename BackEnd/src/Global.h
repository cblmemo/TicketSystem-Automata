//
// Created by Rainy Memory on 2021/4/15.
//

#ifndef TICKETSYSTEM_AUTOMATA_GLOBAL_H
#define TICKETSYSTEM_AUTOMATA_GLOBAL_H

#include "../include/RTL/string_t.h"
#include "../include/RTL/rmstream.h"
#include "../include/BPlusTree.h"
#include "../include/MultiBPlusTree.h"
#include "../include/AlternativeMultiBPlusTree.h"
#include "../include/Parser.h"
#include "../include/MemoryPool.h"

using RainyMemory::string_t;
using RainyMemory::rmstream;
using RainyMemory::MemoryPool;
using RainyMemory::LRUCacheMemoryPool;
using RainyMemory::BPlusTree;
using RainyMemory::MultiBPlusTree;
using RainyMemory::AlternativeMultiBPlusTree;
using RainyMemory::vector;
using RainyMemory::HashMap;
using RainyMemory::TokenScanner;
using RainyMemory::Parser;
using RainyMemory::sortVector;

typedef unsigned long long hash_t;

const char endl = '\n';

enum coefficient {
    SEAT_NUM_INFINITY = 2000000000,
    USER_CACHE_SIZE = 100000,
    TRAIN_CACHE_SIZE = 1000,
    TICKET_CACHE_SIZE = 10000,
    BPLUSTREE_L = 50,
    BPLUSTREE_M = 50,
    MULTI_BPLUSTREE_L = 50,
    MULTI_BPLUSTREE_M = 50
};

constexpr int lengthOfChineseCharacters(int length) {
    return length * 4;
}

template<int len>
struct hash_string_t {
    hash_t operator()(const string_t<len> &o) const {
        int l = o.length();
        hash_t res = 0;
        for (int i = 0; i < l; i++)res = (res << 16) + res + o[i];
        return res;
    }
};

#endif //TICKETSYSTEM_AUTOMATA_GLOBAL_H
