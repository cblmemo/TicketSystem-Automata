//
// Created by Rainy Memory on 2021/4/14.
//

#include "OrderManager.h"

std::string OrderManager::outputSuccess(long long message) {
    return std::to_string(message) + "\n";
}

std::string OrderManager::outputFailure() {
    return "-1\n";
}

std::string OrderManager::outputQueue() {
    return "queue\n";
}

std::string OrderManager::printOrder(const OrderManager::order_t &o) {
    std::string ret;
    ret += status[o.status] + " " + std::string(o.trainID) + " " + std::string(o.fromStation) + " " + std::string(o.departureTime) + " -> " + std::string(o.toStation) + " " + std::string(o.arrivalTime) + " " + std::to_string(o.price) + " " + std::to_string(o.num) + "\n";
    return ret;
}

std::string OrderManager::buyTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    std::pair<int, bool> temp {trainManager->indexPool.find(trainManager->hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t targetTrain {trainManager->storagePool.read(temp.first)};
    if (!targetTrain.released)return outputFailure();
    int n = p("-n");
    if (targetTrain.seatNum < n)return outputFailure();
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
    int price = targetTrain.prices[to] - targetTrain.prices[from];
    bool queue = num < n, candidate = p.haveThisArgument("-q") && p["-q"] == "true";
    if (queue) {
        if (!candidate)return outputFailure();
        order_t order {p["-u"], PENDING, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                       targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
        indexPool.insert(userManager->hashUsername(p["-u"]), order);
        pendingPool.insert(trainManager->hashTrainID(targetTrain.trainID), order);
        return outputQueue();
    }
    for (int i = from; i < to; i++)targetTrain.remainSeats[dist][i] -= n;
    trainManager->storagePool.update(targetTrain, temp.first);
    order_t order {p["-u"], SUCCESS, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                   targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
    indexPool.insert(userManager->hashUsername(p["-u"]), order);
    return outputSuccess((long long) price * (long long) n);
}

std::string OrderManager::queryOrder(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    static vector<order_t> result;
    result.clear();
    indexPool.find(userManager->hashUsername(p["-u"]), result);
    std::string ret;
    ret += std::to_string(result.size()) + "\n";
    for (const order_t &i : result)ret += printOrder(i);
    return ret;
}

std::string OrderManager::refundTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    int n = p.haveThisArgument("-n") ? p("-n") : 1;
    std::pair<order_t, bool> o = indexPool.findNth(userManager->hashUsername(p["-u"]), n);
    if (!o.second)return outputFailure();
    order_t rOrder {o.first};
    if (rOrder.status == REFUNDED)return outputFailure();
    bool newTicket = rOrder.status == SUCCESS;
    rOrder.status = REFUNDED;
    indexPool.update(userManager->hashUsername(p["-u"]), o.first, rOrder);
    if (!newTicket) {
        pendingPool.erase(trainManager->hashTrainID(o.first.trainID), o.first);
        return outputSuccess();
    }
    std::pair<int, bool> temp {trainManager->indexPool.find(trainManager->hashTrainID(rOrder.trainID))};
    train_t rTrain {trainManager->storagePool.read(temp.first)};
    for (int i = rOrder.from; i < rOrder.to; i++)rTrain.remainSeats[rOrder.dist][i] += rOrder.num;
    static vector<order_t> pOrder;
    pOrder.clear();
    pendingPool.find(trainManager->hashTrainID(rOrder.trainID), pOrder);
    int num;
    for (int i = pOrder.size() - 1; i >= 0; i--) {
        const order_t &k = pOrder[i];
        if (k.dist != rOrder.dist)continue;
        if (k.to < rOrder.from || rOrder.to < k.from)continue;
        num = SEAT_NUM_INFINITY;
        for (int j = k.from; j < k.to; j++)num = min(num, rTrain.remainSeats[k.dist][j]);
        if (num < k.num)continue;
        for (int j = k.from; j < k.to; j++)rTrain.remainSeats[k.dist][j] -= k.num;
        order_t mOrder {k};
        mOrder.status = SUCCESS;
        indexPool.update(userManager->hashUsername(k.username), k, mOrder);
        pendingPool.erase(trainManager->hashTrainID(k.trainID), k);
    }
    trainManager->storagePool.update(rTrain, temp.first);
    return outputSuccess();
}

void OrderManager::clear() {
    indexPool.clear();
    pendingPool.clear();
}
