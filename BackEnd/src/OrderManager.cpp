//
// Created by Rainy Memory on 2021/4/14.
//

#include "OrderManager.h"

void OrderManager::outputSuccess() {
    defaultOut << "0" << endl;
}

void OrderManager::outputFailure() {
    defaultOut << "-1" << endl;
}

void OrderManager::printOrder(const OrderManager::order_t &o) {
    defaultOut << status[o.status] << " " << o.trainID << " " << o.from << " " << o.departureTime << " -> " << o.to << " " << o.arrivalTime << " " << o.price << " " << o.num << endl;
}

void OrderManager::buyTicket(const Parser &p) {

}

void OrderManager::queryOrder(const Parser &p) {

}

void OrderManager::refundTicket(const Parser &p) {

}

void OrderManager::clear() {
    indexPool.clear();
    storagePool.clear();
    pendingList.clear();
}
