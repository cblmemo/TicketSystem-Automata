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
#include <chrono>

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
    USER_CACHE_SIZE = 500,
    TRAIN_CACHE_SIZE = 1000,
    BPLUSTREE_L = 100,
    BPLUSTREE_M = 100,
    MULTI_BPLUSTREE_L = 100,
    MULTI_BPLUSTREE_M = 100
};

constexpr int lengthOfChineseCharacters(int length) {
    return length * 4;
}

template<int len>
struct hash_string_t {
    hash_t operator()(const string_t<len> &o) const {
        int l = o.length();
        int h1 = l, h2 = l;
        for (int i = 0; i < l; i++)h1 = (h1 << 7) ^ (h1 >> 25) ^ o[i], h2 = (h2 << 11) ^ (h2 >> 21) ^ o[i];
        return (hash_t(h1) << 32) | hash_t(h2);
    }
};

#endif //TICKETSYSTEM_AUTOMATA_GLOBAL_H
