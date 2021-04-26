//
// Created by Rainy Memory on 2021/4/14.
//

#include "OrderManager.h"

void OrderManager::outputSuccess(int message) {
    if (message > 0)defaultOut << message << endl;
    else defaultOut << "queue" << endl;
}

void OrderManager::outputFailure() {
    defaultOut << "-1" << endl;
}

void OrderManager::printOrder(const OrderManager::order_t &o) {
    defaultOut << status[o.status] << " " << o.trainID << " " << o.from << " " << o.departureTime << " -> " << o.to << " " << o.arrivalTime << " " << o.price << " " << o.num << endl;
}

void OrderManager::buyTicket(const Parser &p) {
    if (!userManager->isLogin(p["-u"]))return outputFailure();
    vector<int> temp;
    trainManager->indexPool.find(p["-i"], temp);
    if (temp.size() != 1)return outputFailure();
    train_t targetTrain {trainManager->storagePool.read(temp[0])};
    station_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    if (!targetTrain.startTime.lessOrEqualDate(departureDate) || !departureDate.lessOrEqualDate(targetTrain.endTime))return outputFailure();
    int from = -1, to = -1, num = 2000000, dist = departureDate.dateDistance(targetTrain.startTime);
    for (int i = 0; i < targetTrain.stationNum; i++) {
        if (targetTrain.stations[i] == p["-f"])from = i;
        if (targetTrain.stations[i] == p["-t"])to = i;
    }
    if (from == -1 || to == -1 || from >= to)return outputFailure();
    for (int i = from; i <= to; i++)num = min(num, targetTrain.remainSeats[dist][i]);
    int n = p("-n"), price = targetTrain.prices[from] - targetTrain.prices[to];
    bool queue = num < n, candidate = p["q"] == "true";
    if (queue) {
        if (!candidate)return outputFailure();
        order_t order {PENDING, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                       targetTrain.departureTimes[from], targetTrain.arrivalTimes[to], price * n, n};
        int index = storagePool.write(order);
        indexPool.insert(targetTrain.trainID, index);
        pendingList.insert(targetTrain.trainID, index);
        return outputSuccess(-1);
    }
    for (int i = from; i <= to; i++)targetTrain.remainSeats[dist][i] -= n;
    trainManager->storagePool.update(targetTrain, temp[0]);
    order_t order {SUCCESS, targetTrain.trainID, targetTrain.stations[from], targetTrain.stations[to],
                   targetTrain.departureTimes[from], targetTrain.arrivalTimes[to], price * n, n};
    int offset = storagePool.write(order);
    indexPool.insert(targetTrain.trainID, offset);
    outputSuccess(price * n);
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
