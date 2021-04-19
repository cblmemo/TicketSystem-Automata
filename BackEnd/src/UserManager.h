//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_USERMANAGER_H
#define TICKETSYSTEM_AUTOMATA_USERMANAGER_H

#include "GLobal.h"
#include "../include/BPlusTree.h"
#include "../include/RTL/string_t.h"
#include "../include/Parser.h"

using RainyMemory::HashMap;
using RainyMemory::BPlusTree;
using RainyMemory::Parser;

class UserManager {
private:
    enum sizeInfo {
        HASHMAP_CAPACITY = 178,
        MEMORYPOOL_CAPACITY = 100
    };
    
    using username_t = string_t<20>;
    using password_t = string_t<30>;
    using name_t = string_t<lengthOfChineseCharacters(5)>;
    using mailAddr_t = string_t<30>;
    
    struct hash_username_t {
        int operator()(const username_t &o) const {
            int len = o.length();
            int h = len;
            for (int i = 0; i < len; i++)h = (h << 7) ^ (h >> 25) ^ o[i];
            return h;
        }
    };
    
    struct user_t {
        username_t username {};
        password_t password {};
        name_t name {};
        mailAddr_t mailAddr {};
        int privilege = 0;
        
        user_t() = default;
        
        user_t(const string &_u, const string &_p, const string &_n, const string &_m, int _g) :
                username(_u), password(_p), name(_n), mailAddr(_m), privilege(_g) {}
    };
    
    HashMap<username_t, bool, hash_username_t> loginPool;
    BPlusTree<username_t, int> indexPool;
    LRUCacheMemoryPool<user_t, bool> storagePool;
    std::ostream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printUser(const user_t &u);
    
    int getPrivilege(const username_t &u);
    
    bool checkPassword(const username_t &u, const password_t &p);

public:
    UserManager(const string &indexPath, const string &storagePath, std::ostream &dft = std::cout) :
            loginPool(HASHMAP_CAPACITY), indexPool(indexPath), storagePool(storagePath, true, MEMORYPOOL_CAPACITY), defaultOut(dft) {}
    
    bool isLogin(const username_t &u);
    
    void addUser(const Parser &p);
    
    void login(const Parser &p);
    
    void logout(const Parser &p);
    
    void queryProfile(const Parser &p);
    
    void modifyProfile(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_USERMANAGER_H
