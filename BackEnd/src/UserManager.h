//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_USERMANAGER_H
#define TICKETSYSTEM_AUTOMATA_USERMANAGER_H

#include "Global.h"

#define storageUserData

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
        password_t password {};
        name_t name {};
        mailAddr_t mailAddr {};
        int privilege = 0;
        
        user_t() = default;
        
        user_t(const string &_p, const string &_n, const string &_m, int _g) :
                password(_p), name(_n), mailAddr(_m), privilege(_g) {}
    };
    
    /*
     * Data Members
     * --------------------------------------------------------
     * [loginPool]: Store user's login status. In the same time,
     * implement a simple cache strategy, which is to store every
     * login user's privilege.
     *
     */
    HashMap<username_t, int, hash_username_t> loginPool;//[username] -> [privilege]
#ifdef storageUserData
    BPlusTree<hash_t, user_t, BPLUSTREE_L, BPLUSTREE_M, USER_CACHE_SIZE> indexPool;//[username] -> [user]
#else
    BPlusTree<hash_t, int, BPLUSTREE_L, BPLUSTREE_M> indexPool;//[username] -> [offset]
#endif
#ifndef storageUserData
    LRUCacheMemoryPool<user_t, bool> storagePool;
#endif
    hash_username_t hashUsername;
    rmstream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printUser(const username_t &un, const user_t &u);
    
    bool isLogin(const username_t &u);

public:
    UserManager(const string &indexPath, const string &storagePath, rmstream &dft) :
#ifdef storageUserData
            loginPool(), indexPool(indexPath), defaultOut(dft) {}
#else
            loginPool(), storagePool(storagePath, false, USER_CACHE_SIZE), indexPool(indexPath), defaultOut(dft) {}
#endif
    
    void addUser(const Parser &p);
    
    void login(const Parser &p);
    
    void logout(const Parser &p);
    
    void queryProfile(const Parser &p);
    
    void modifyProfile(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_USERMANAGER_H
