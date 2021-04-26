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
    
    struct order_t {
        status_t status;
        TrainManager::trainID_t trainID;
        TrainManager::station_t from;
        TrainManager::station_t to;
        TrainManager::station_time_t departureTime;
        TrainManager::station_time_t arrivalTime;
        int price;
        int num;
    };
    
    UserManager *userManager;
    TrainManager *trainManager;
    BPlusTree<UserManager::username_t, int> indexPool;
    LRUCacheMemoryPool<order_t> storagePool;
    BPlusTree<TrainManager::trainID_t, int> pendingList;//int is the pending order's offset
    const string status[3] = {"[success]", "[pending]", "[refunded]"};
    std::ostream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printOrder(const order_t &o);

public:
    OrderManager(UserManager *um, TrainManager *tm, const string &indexPath, const string &storagePath, const string &pendingPath, std::ostream &dft = std::cout) :
            userManager(um), trainManager(tm), indexPool(indexPath), storagePool(storagePath, 0, MEMORYPOOL_CAPACITY), pendingList(pendingPath), defaultOut(dft) {}
    
    void buyTicket(const Parser &p);
    
    void queryOrder(const Parser &p);
    
    void refundTicket(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
