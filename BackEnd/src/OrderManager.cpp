//
// Created by Rainy Memory on 2021/4/14.
//

#include "OrderManager.h"

void OrderManager::outputSuccess(long long message) {
    defaultOut << message << endl;
}

void OrderManager::outputFailure() {
    defaultOut << "-1" << endl;
}

void OrderManager::outputQueue() {
    defaultOut << "queue" << endl;
}

void OrderManager::printOrder(const OrderManager::order_t &o) {
    defaultOut << status[o.status] << " " << o.trainID << " " << o.fromStation << " " << o.departureTime << " -> " << o.toStation << " " << o.arrivalTime << " " << o.price << " " << o.num << endl;
}

void OrderManager::buyTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    std::pair<int, bool> temp {trainManager->indexPool.find(trainManager->hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t targetTrain {trainManager->storagePool.read(temp.first)};
    if (!targetTrain.released)return outputFailure();
    int n = p("-n");
    if (targetTrain.seatNum < n)return outputFailure();
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    int from = -1, to = -1;
    for (int i = 0; i < targetTrain.stationNum; i++) {
        if (targetTrain.stations[i] == p["-f"])from = i;
        if (targetTrain.stations[i] == p["-t"])to = i;
    }
    if (from == -1 || to == -1 || from >= to)return outputFailure();
    train_time_t dTimes {targetTrain.departureTimes[from]};
    if (!(dTimes.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dTimes.updateDate(targetTrain.dateGap))))return outputFailure();
    int dist = departureDate.dateDistance(targetTrain.departureTimes[from]);
    std::pair<hash_t, int> key {trainManager->hashTrainID(targetTrain.trainID), dist};
    date_ticket_t seats {trainManager->ticketPool.find(key).first};
    int num = seats.ticketNum(from, to);
    int price = targetTrain.prices[to] - targetTrain.prices[from];
    bool queue = num < n, candidate = p.haveThisArgument("-q") && p["-q"] == "true";
    if (queue) {
        if (!candidate)return outputFailure();
        order_t order {indexPool.size(), p["-u"], PENDING, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                       targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
        indexPool.insert(userManager->hashUsername(p["-u"]), order.orderID, order);
        pendingPool.insert(std::pair<hash_t, int> {trainManager->hashTrainID(targetTrain.trainID), dist}, order.orderID, order);
        return outputQueue();
    }
    seats.modifyRemain(from, to, -n);
    trainManager->storagePool.update(targetTrain, temp.first);
    trainManager->ticketPool.update(key, seats);
    order_t order {indexPool.size(), p["-u"], SUCCESS, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                   targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
    indexPool.insert(userManager->hashUsername(p["-u"]), order.orderID, order);
    outputSuccess((long long) price * (long long) n);
}

void OrderManager::queryOrder(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    static vector<order_t> result;
    result.clear();
    indexPool.find(userManager->hashUsername(p["-u"]), result);
    defaultOut << result.size() << endl;
    for (const order_t &i : result)printOrder(i);
}

void OrderManager::refundTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    int n = p.haveThisArgument("-n") ? p("-n") : 1;
    std::pair<order_t, bool> o = indexPool.findNth(userManager->hashUsername(p["-u"]), n);
    if (!o.second)return outputFailure();
    order_t &rOrder {o.first};
    if (rOrder.status == REFUNDED)return outputFailure();
    bool newTicket = rOrder.status == SUCCESS;
    rOrder.status = REFUNDED;
    indexPool.update(userManager->hashUsername(p["-u"]), rOrder.orderID, rOrder);
    if (!newTicket) {
        pendingPool.erase(std::pair<hash_t, int> {trainManager->hashTrainID(o.first.trainID), o.first.dist}, o.first.orderID);
        return outputSuccess();
    }
    std::pair<hash_t, int> key {trainManager->hashTrainID(rOrder.trainID), rOrder.dist};
    date_ticket_t seats {trainManager->ticketPool.find(key).first};
    seats.modifyRemain(rOrder.from, rOrder.to, rOrder.num);
    static vector<order_t> pOrder;
    pOrder.clear();
    pendingPool.find(std::pair<hash_t, int> {trainManager->hashTrainID(rOrder.trainID), rOrder.dist}, pOrder);
    int num;
    for (int i = pOrder.size() - 1; i >= 0; i--) {
        order_t &k = pOrder[i];
        if (k.to < rOrder.from || rOrder.to < k.from)continue;
        num = seats.ticketNum(k.from, k.to);
        if (num < k.num)continue;
        seats.modifyRemain(k.from, k.to, -k.num);
        k.status = SUCCESS;
        indexPool.update(userManager->hashUsername(k.username), k.orderID, k);
        pendingPool.erase(std::pair<hash_t, int> {trainManager->hashTrainID(k.trainID), rOrder.dist}, k.orderID);
    }
    trainManager->ticketPool.update(key, seats);
    outputSuccess();
}

void OrderManager::clear() {
    indexPool.clear();
    pendingPool.clear();
}
