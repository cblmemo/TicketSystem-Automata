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
        int orderID = -1;
        username_t username {};
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
        
        order_t(int id, const username_t &u, status_t s, const trainID_t &i, const station_t &f, const station_t &t, const train_time_t &d, const train_time_t &a, int p, int n, int fk, int tk, int di) :
                orderID(id), username(u), status(s), trainID(i), fromStation(f), toStation(t), departureTime(d), arrivalTime(a), price(p), num(n), from(fk), to(tk), dist(di) {}
        
        order_t &operator=(const order_t &o) = default;
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
    AlternativeMultiBPlusTree<hash_t, order_t, MULTI_BPLUSTREE_L, MULTI_BPLUSTREE_M> indexPool;//[username] -> [order]
    AlternativeMultiBPlusTree<std::pair<hash_t, int>, order_t, MULTI_BPLUSTREE_L, MULTI_BPLUSTREE_M> pendingPool;//[[trainID], [dist]] -> [order]
    const string status[3] = {"[success]", "[pending]", "[refunded]"};
    rmstream &defaultOut;
    
    inline void outputSuccess(long long message = 0);
    
    inline void outputFailure();
    
    inline void outputQueue();
    
    inline void printOrder(const order_t &o);

public:
    OrderManager(UserManager *um, TrainManager *tm, const string &indexPath, const string &pendingPath, rmstream &dft) :
            userManager(um), trainManager(tm), indexPool(indexPath), pendingPool(pendingPath), defaultOut(dft) {}
    
    void buyTicket(const Parser &p);
    
    void queryOrder(const Parser &p);
    
    void refundTicket(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_ORDERMANAGER_H
