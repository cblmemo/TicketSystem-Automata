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
     * login user's information and offset.
     *
     */
    HashMap<username_t, std::pair<user_t, int>, hash_username_t> loginPool;
    BPlusTree<username_t, int> indexPool;
    LRUCacheMemoryPool<user_t, bool> storagePool;
    std::ostream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printUser(const user_t &u);
    
    bool isLogin(const username_t &u);

public:
    UserManager(const string &indexPath, const string &storagePath, std::ostream &dft) :
            loginPool(), indexPool(indexPath), storagePool(storagePath, true, USER_MANAGER_MEMORYPOOL_CAPACITY), defaultOut(dft) {}
    
    void addUser(const Parser &p);
    
    void login(const Parser &p);
    
    void logout(const Parser &p);
    
    void queryProfile(const Parser &p);
    
    void modifyProfile(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_USERMANAGER_H
