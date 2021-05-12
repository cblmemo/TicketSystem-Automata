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
    vector<int> temp;
    trainManager->indexPool.find(p["-i"], temp);
    if (temp.size() != 1)return outputFailure();
    train_t targetTrain {trainManager->storagePool.read(temp[0])};
    if (!targetTrain.released)return outputFailure();
    if (targetTrain.seatNum < p("-n"))return outputFailure();
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    int from = -1, to = -1, num = SEAT_NUM_INFINITY;
    for (int i = 0; i < targetTrain.stationNum; i++) {
        if (targetTrain.stations[i] == p["-f"])from = i;
        if (targetTrain.stations[i] == p["-t"])to = i;
    }
    if (from == -1 || to == -1 || from >= to)return outputFailure();
    train_time_t dTimes {targetTrain.departureTimes[from]};
    if (!(dTimes.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dTimes.updateDate(targetTrain.dateGap))))return outputFailure();
    int dist = departureDate.dateDistance(targetTrain.departureTimes[from]);
    for (int i = from; i < to; i++)num = min(num, targetTrain.remainSeats[dist][i]);
    int n = p("-n"), price = targetTrain.prices[to] - targetTrain.prices[from];
    bool queue = num < n, candidate = p.haveThisArgument("-q") && p["-q"] == "true";
    if (queue) {
        if (!candidate)return outputFailure();
        order_t order {p["-u"], PENDING, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                       targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
        indexPool.insert(p["-u"], order);
        pendingPool.insert(targetTrain.trainID, order);
        return outputQueue();
    }
    for (int i = from; i < to; i++)targetTrain.remainSeats[dist][i] -= n;
    trainManager->storagePool.update(targetTrain, temp[0]);
    order_t order {p["-u"], SUCCESS, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                   targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
    indexPool.insert(p["-u"], order);
    outputSuccess((long long) price * (long long) n);
}

void OrderManager::queryOrder(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    vector<order_t> result;
    indexPool.find(p["-u"], result);
    defaultOut << result.size() << endl;
    for (const order_t &i : result)printOrder(i);
}

void OrderManager::refundTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    int n = p.haveThisArgument("-n") ? p("-n") : 1;
    std::pair<order_t, bool> o = indexPool.findNth(p["-u"], n);
    if (!o.second)return outputFailure();
    order_t rOrder {o.first};
    if (rOrder.status == REFUNDED)return outputFailure();
    bool newTicket = rOrder.status == SUCCESS;
    rOrder.status = REFUNDED;
    indexPool.update(p["-u"], o.first, rOrder);
    if (!newTicket) {
        pendingPool.erase(o.first.trainID, o.first);
        return outputSuccess();
    }
    vector<int> temp;
    trainManager->indexPool.find(rOrder.trainID, temp);
    train_t rTrain {trainManager->storagePool.read(temp[0])};
    for (int i = rOrder.from; i < rOrder.to; i++)rTrain.remainSeats[rOrder.dist][i] += rOrder.num;
    vector<order_t> pOrder;
    pendingPool.find(rOrder.trainID, pOrder);
    int num;
    for (int i = pOrder.size() - 1; i >= 0; i--) {
        const order_t &k = pOrder[i];
        if (k.dist != rOrder.dist)continue;
        num = SEAT_NUM_INFINITY;
        for (int j = k.from; j < k.to; j++)num = min(num, rTrain.remainSeats[k.dist][j]);
        if (num < k.num)continue;
        for (int j = k.from; j < k.to; j++)rTrain.remainSeats[k.dist][j] -= k.num;
        order_t mOrder {k};
        mOrder.status = SUCCESS;
        indexPool.update(k.username, k, mOrder);
        pendingPool.erase(k.trainID, k);
    }
    trainManager->storagePool.update(rTrain, temp[0]);
    outputSuccess();
}

void OrderManager::clear() {
    indexPool.clear();
    pendingPool.clear();
}
