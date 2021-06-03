//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_USERMANAGER_H
#define TICKETSYSTEM_AUTOMATA_USERMANAGER_H

#include "Global.h"

class UserManager {
    /*
     * class UserManager
     * --------------------------------------------------------
     * A class implements all functions of users, including five
     * commands: [add_user], [login], [logout], [query_profile],
     * [modify_profile].
     * This class used BPlusTree to indexing user data by the
     * unique identifier [username], and a memory pool which has
     * built-in LRU Cache to storage data.
     *
     */
private:
    friend class OrderManager;
    
    using username_t = string_t<20>;
    using password_t = string_t<30>;
    using name_t = string_t<lengthOfChineseCharacters(5)>;
    using mailAddr_t = string_t<30>;
    
    using hash_username_t = hash_string_t<20>;
    
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
    
    /*
     * Data Members
     * --------------------------------------------------------
     * [loginPool]: Store user's login status. In the same time,
     * implement a simple cache strategy, which is to store every
     * login user's privilege(first) and offset(second).
     *
     */
    HashMap<username_t, std::pair<int, int>, hash_username_t> loginPool;
    BPlusTree<long long, int, BPLUSTREE_L, BPLUSTREE_M> indexPool;
    MemoryPool<user_t, bool> storagePool;
    hash_username_t hashUsername;
    
    static inline std::string outputSuccess();
    
    static inline std::string outputFailure();
    
    static inline std::string printUser(const user_t &u);
    
    bool isLogin(const username_t &u);

public:
    UserManager(const string &indexPath, const string &storagePath) :
            loginPool(), indexPool(indexPath), storagePool(storagePath, true) {}
    
    std::string addUser(const Parser &p);
    
    std::string login(const Parser &p);
    
    std::string logout(const Parser &p);
    
    std::string queryProfile(const Parser &p);
    
    std::string modifyProfile(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_USERMANAGER_H
