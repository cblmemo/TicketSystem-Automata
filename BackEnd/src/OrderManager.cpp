//
// Created by Rainy Memory on 2021/4/14.
//

#include "OrderManager.h"

void OrderManager::outputSuccess(int message) {
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
    station_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    int from = -1, to = -1, num = SEAT_NUM_INFINITY;
    for (int i = 0; i < targetTrain.stationNum; i++) {
        if (targetTrain.stations[i] == p["-f"])from = i;
        if (targetTrain.stations[i] == p["-t"])to = i;
    }
    if (from == -1 || to == -1 || from >= to)return outputFailure();
    station_time_t dTimes {targetTrain.departureTimes[from]};
    if (!(dTimes.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dTimes.updateDate(targetTrain.dateGap))))return outputFailure();
    int dist = departureDate.dateDistance(targetTrain.departureTimes[from]);
    for (int i = from; i < to; i++)num = min(num, targetTrain.remainSeats[dist][i]);
    int n = p("-n"), price = targetTrain.prices[to] - targetTrain.prices[from];
    bool queue = num < n, candidate = p["-q"] == "true";
    if (queue) {
        if (!candidate)return outputFailure();
        order_t order {PENDING, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                       targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
        int offset = storagePool.write(order);
        indexPool.insert(p["-u"], offset);
        pendingPool.insert(targetTrain.trainID, offset);
        return outputQueue();
    }
    for (int i = from; i < to; i++)targetTrain.remainSeats[dist][i] -= n;
    trainManager->storagePool.update(targetTrain, temp[0]);
    order_t order {SUCCESS, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                   targetTrain.departureTimes[from].updateDate(dist), targetTrain.arrivalTimes[to].updateDate(dist), price, n, from, to, dist};
    int offset = storagePool.write(order);
    indexPool.insert(p["-u"], offset);
    outputSuccess(price * n);
}

void OrderManager::queryOrder(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    vector<int> offset;
    indexPool.find(p["-u"], offset);
    vector<order_t> result(offset.size());
    for (int i : offset)result.push_back(storagePool.read(i));
    defaultOut << result.size() << endl;
    for (const order_t &i : result)printOrder(i);
}

void OrderManager::refundTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    int n = p.haveThisArgument("-n") ? p("-n") : 1;
    std::pair<int, bool> o = indexPool.findNth(p["-u"], n);
    if (!o.second)return outputFailure();
    order_t rOrder {storagePool.read(o.first)};
    if (rOrder.status == REFUNDED)return outputFailure();
    bool newTicket = rOrder.status == SUCCESS;
    rOrder.status = REFUNDED;
    storagePool.update(rOrder, o.first);
    if (!newTicket)return outputSuccess();
    vector<int> temp;
    trainManager->indexPool.find(rOrder.trainID, temp);
    train_t rTrain {trainManager->storagePool.read(temp[0])};
    for (int i = rOrder.from; i < rOrder.to; i++)rTrain.remainSeats[rOrder.dist][i] += rOrder.num;
    vector<int> offset;
    pendingPool.find(rOrder.trainID, offset);
    reverseVector(offset);
    vector<order_t> pOrder(offset.size());
    for (int i : offset)pOrder.push_back(storagePool.read(i));
    int num;
    for (int k = 0; k < pOrder.size(); k++) {
        num = SEAT_NUM_INFINITY;
        for (int i = pOrder[k].from; i < pOrder[k].to; i++)num = min(num, rTrain.remainSeats[pOrder[k].dist][i]);
        if (num < pOrder[k].num)continue;
        for (int i = pOrder[k].from; i < pOrder[k].to; i++)rTrain.remainSeats[pOrder[k].dist][i] -= pOrder[k].num;
        pOrder[k].status = SUCCESS;
        storagePool.update(pOrder[k], offset[k]);
        pendingPool.erase(pOrder[k].trainID, offset[k]);
    }
    trainManager->storagePool.update(rTrain, temp[0]);
    outputSuccess();
}

void OrderManager::clear() {
    indexPool.clear();
    storagePool.clear();
    pendingPool.clear();
}
