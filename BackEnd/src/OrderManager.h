//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
#define TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H

#include "UserManager.h"
#include "TrainManager.h"

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
        username_t username {};
        long long timeStamp {};
        status_t status = SUCCESS;
        trainID_t trainID {};
        station_t fromStation {};
        station_t toStation {};
        station_time_t departureTime {};
        station_time_t arrivalTime {};
        int price = 0;
        int num = 0;
        int from = 0;
        int to = 0;
        int dist = 0;
        
        order_t() = default;
        
        order_t(const username_t &u, status_t s, const trainID_t &i, const station_t &f, const station_t &t, const station_time_t &d, const station_time_t &a, int p, int n, int fk, int tk, int di) :
                username(u), status(s), trainID(i), fromStation(f), toStation(t), departureTime(d), arrivalTime(a), price(p), num(n), from(fk), to(tk), dist(di) {
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            timeStamp = (long long)(ms.count());
        }
        
        order_t &operator=(const order_t &o) = default;
        
        bool operator==(const order_t &o) const {
            return timeStamp == o.timeStamp && status == o.status && trainID == o.trainID && fromStation == o.fromStation && toStation == o.toStation
                   && departureTime == o.departureTime && arrivalTime == o.arrivalTime && price == o.price && num == o.num && from == o.from && to == o.to && dist == o.dist;
        }
    };
    
    UserManager *userManager;
    TrainManager *trainManager;
    BPlusTree<username_t, order_t> indexPool;
    BPlusTree<trainID_t, order_t> pendingPool;
    const string status[3] = {"[success]", "[pending]", "[refunded]"};
    std::ostream &defaultOut;
    
    inline void outputSuccess(int message = 0);
    
    inline void outputFailure();
    
    inline void outputQueue();
    
    inline void printOrder(const order_t &o);
    
    static inline int min(int a, int b) { return a < b ? a : b; }

public:
    OrderManager(UserManager *um, TrainManager *tm, const string &indexPath, const string &pendingPath, std::ostream &dft) :
            userManager(um), trainManager(tm), indexPool(indexPath), pendingPool(pendingPath), defaultOut(dft) {}
    
    void buyTicket(const Parser &p);
    
    void queryOrder(const Parser &p);
    
    void refundTicket(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
