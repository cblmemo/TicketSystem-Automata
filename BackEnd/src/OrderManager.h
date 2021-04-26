//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
#define TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H

#include "UserManager.h"
#include "TrainManager.h"

using RainyMemory::reverseVector;

class OrderManager {
private:
    enum sizeInfo {
        MEMORYPOOL_CAPACITY = 100
    };
    
    enum status_t {
        SUCCESS, PENDING, REFUNDED
    };
    
    using username_t = UserManager::username_t;
    using train_t = TrainManager::train_t;
    using trainID_t = TrainManager::trainID_t;
    using station_t = TrainManager::station_t;
    using station_time_t = TrainManager::station_time_t;
    
    struct order_t {
        status_t status = SUCCESS;
        trainID_t trainID {};
        station_t from {};
        station_t to {};
        station_time_t departureTime {};
        station_time_t arrivalTime {};
        int price = 0;
        int num = 0;
        
        order_t() = default;
        
        order_t(status_t s, const trainID_t &i, const station_t &f, const station_t &t, const station_time_t &d, const station_time_t &a, int p, int n) :
                status(s), trainID(i), from(f), to(t), departureTime(d), arrivalTime(a), price(p), num(n) {}
    };
    
    UserManager *userManager;
    TrainManager *trainManager;
    BPlusTree<username_t, int> indexPool;
    LRUCacheMemoryPool<order_t> storagePool;
    BPlusTree<trainID_t, int> pendingList;//int is the pending order's offset
    const string status[3] = {"[success]", "[pending]", "[refunded]"};
    std::ostream &defaultOut;
    
    inline void outputSuccess(int message = 0);
    
    inline void outputFailure();
    
    inline void printOrder(const order_t &o);
    
    static inline int min(int a, int b) { return a < b ? a : b; }

public:
    OrderManager(UserManager *um, TrainManager *tm, const string &indexPath, const string &storagePath, const string &pendingPath, std::ostream &dft) :
            userManager(um), trainManager(tm), indexPool(indexPath), storagePool(storagePath, 0, MEMORYPOOL_CAPACITY), pendingList(pendingPath), defaultOut(dft) {}
    
    void buyTicket(const Parser &p);
    
    void queryOrder(const Parser &p);
    
    void refundTicket(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
