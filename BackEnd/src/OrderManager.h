//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
#define TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H

#include "UserManager.h"
#include "TrainManager.h"

class OrderManager {
    /*
     * class OrderManager
     * --------------------------------------------------------
     * A class implements all functions of orders, including three
     * commands: [buy_ticket], [query_order], [refund_ticket].
     * This class used BPlusTree to directly store order data
     * indexing by the identifier [username], and for each pending
     * order, store a copy of it indexing by [trainID]. These
     * BPlusTrees allow duplicate key value.
     *
     */
private:
    enum status_t {
        SUCCESS, PENDING, REFUNDED
    };
    
    using username_t = UserManager::username_t;
    using train_t = TrainManager::train_t;
    using trainID_t = TrainManager::trainID_t;
    using station_t = TrainManager::station_t;
    using train_time_t = TrainManager::train_time_t;
    using date_ticket_t = TrainManager::date_ticket_t;
    
    struct order_t {
        /*
         * order_t
         * Store order's relevant information
         * --------------------------------------------------------
         * [timeStamp]: Order's creation time, to judge whether two
         * order is same, therefore could avoid exactly same orders
         * interfere BPLusTree's delete.
         * [from] && [to]: From station and to station's station index.
         * [dist]: Order train's departure date distance between start
         * time.
         *
         */
        username_t username {};
        long long timeStamp {};
        status_t status = SUCCESS;
        trainID_t trainID {};
        station_t fromStation {};
        station_t toStation {};
        train_time_t departureTime {};
        train_time_t arrivalTime {};
        int price = 0;
        int num = 0;
        int from = 0;
        int to = 0;
        int dist = 0;
        
        order_t() = default;
        
        order_t(const username_t &u, status_t s, const trainID_t &i, const station_t &f, const station_t &t, const train_time_t &d, const train_time_t &a, int p, int n, int fk, int tk, int di) :
                username(u), status(s), trainID(i), fromStation(f), toStation(t), departureTime(d), arrivalTime(a), price(p), num(n), from(fk), to(tk), dist(di) {
            std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
            timeStamp = (long long) (ns.count());
        }
        
        order_t &operator=(const order_t &o) = default;
        
        bool operator==(const order_t &o) const {
            return timeStamp == o.timeStamp && status == o.status && trainID == o.trainID && fromStation == o.fromStation && toStation == o.toStation
                   && departureTime == o.departureTime && arrivalTime == o.arrivalTime && price == o.price && num == o.num && from == o.from && to == o.to && dist == o.dist;
        }
    };
    
    /*
     * Data Members
     * --------------------------------------------------------
     * [userManager]: Manage functions that is relevant to users.
     * [trainManager]: Manage functions that is relevant to trains.
     * [indexPool]: Store every order, mapping username to order.
     * [pendingPool]: Store every pending order, mapping trainID
     * to order.
     *
     */
    UserManager *userManager;
    TrainManager *trainManager;
    MultiBPlusTree<hash_t, order_t, MULTI_BPLUSTREE_L, MULTI_BPLUSTREE_M> indexPool;//[username] -> [order]
    MultiBPlusTree<hash_t, order_t, MULTI_BPLUSTREE_L, MULTI_BPLUSTREE_M> pendingPool;//[trainID] -> [order]
    const string status[3] = {"[success]", "[pending]", "[refunded]"};
    rmstream &defaultOut;
    
    inline void outputSuccess(long long message = 0);
    
    inline void outputFailure();
    
    inline void outputQueue();
    
    inline void printOrder(const order_t &o);
    
    static inline int min(int a, int b) { return a < b ? a : b; }

public:
    OrderManager(UserManager *um, TrainManager *tm, const string &indexPath, const string &pendingPath, rmstream &dft) :
            userManager(um), trainManager(tm), indexPool(indexPath), pendingPool(pendingPath), defaultOut(dft) {}
    
    void buyTicket(const Parser &p);
    
    void queryOrder(const Parser &p);
    
    void refundTicket(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
