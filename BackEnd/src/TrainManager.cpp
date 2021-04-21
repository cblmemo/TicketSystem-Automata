//
// Created by Rainy Memory on 2021/4/14.
//

#include "TrainManager.h"

void TrainManager::outputSuccess() {
    defaultOut << "0" << endl;
}

void TrainManager::outputFailure() {
    defaultOut << "-1" << endl;
}

void TrainManager::printTrain(const TrainManager::train_t &t, int date) {
    //int date: the distance between query train argument -d and startTime sate
    defaultOut << t.trainID << " " << t.type << endl;
    for (int i = 0; i < t.stationNum; i++) {
        defaultOut << t.stations[i] << " ";
        if (i == 0)defaultOut << "xx-xx xx:xx";
        else {
            station_time_t temp = t.arrivalTimes[i];
            temp.updateDate(date);
            defaultOut << temp;
        }
        defaultOut << " -> ";
        if (i == t.stationNum - 1)defaultOut << "xx-xx xx:xx";
        else {
            station_time_t temp = t.departureTimes[i];
            temp.updateDate(date);
            defaultOut << temp;
        }
        defaultOut << " " << t.prices[i] << " ";
        if (i == t.stationNum - 1)defaultOut << 'x';
        else defaultOut << t.remainSeats[date][i];
        defaultOut << endl;
    }
}

void TrainManager::addTrain(const Parser &p) {
    if (indexPool.containsKey(p["-i"])) {
        outputFailure();
        return;
    }
    train_t newTrain(p["-i"], p("-n"), p("-m"), p["-y"][0]);
    splitTool.resetBuffer(p["-s"]);
    for (int i = 0; i < newTrain.stationNum; i++)newTrain.stations[i] = splitTool.nextToken();
    splitTool.resetBuffer(p["-p"]);
    newTrain.prices[0] = 0;
    for (int i = 1; i < newTrain.stationNum; i++)newTrain.prices[i] = splitTool.nextIntToken() + newTrain.prices[i - 1];
    string st = p["-x"];
    newTrain.startTime.hour = (st[0] - '0') * 10 + st[1] - '0', newTrain.startTime.minute = (st[3] - '0') * 10 + st[4] - '0';
    newTrain.endTime.hour = newTrain.startTime.hour, newTrain.endTime.minute = newTrain.startTime.minute;
    splitTool.resetBuffer(p["-t"]);
    for (int i = 0; i < newTrain.stationNum - 1; i++)newTrain.travelTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-o"]);
    for (int i = 0; i < newTrain.stationNum - 2; i++)newTrain.stopoverTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-d"]);
    st = splitTool.nextToken();
    newTrain.startTime.month = (st[0] - '0') * 10 + st[1] - '0', newTrain.startTime.day = (st[3] - '0') * 10 + st[4] - '0';
    st = splitTool.nextToken();
    newTrain.endTime.month = (st[0] - '0') * 10 + st[1] - '0', newTrain.endTime.day = (st[3] - '0') * 10 + st[4] - '0';
    station_time_t nowTime(newTrain.startTime);
    newTrain.departureTimes[0] = nowTime;
    for (int i = 1; i < newTrain.stationNum - 1; i++) {
        nowTime += newTrain.travelTimes[i - 1];
        newTrain.arrivalTimes[i] = nowTime;
        nowTime += newTrain.stopoverTimes[i - 1];
        newTrain.departureTimes[i] = nowTime;
    }
    nowTime += newTrain.travelTimes[newTrain.stationNum - 2];
    newTrain.arrivalTimes[newTrain.stationNum - 1] = nowTime;
    for (auto &remainSeat : newTrain.remainSeats)
        for (int &j : remainSeat)
            j = newTrain.seatNum;
    int index = storagePool.write(newTrain);
    indexPool.insert(newTrain.trainID, index);
    outputSuccess();
}

void TrainManager::releaseTrain(const Parser &p) {
    vector<int> temp;
    indexPool.find(p["-i"], temp);
    if (temp.size() != 1) {
        outputFailure();
        return;
    }
    train_t rTrain(storagePool.read(temp[0]));
    if (rTrain.released)outputFailure();
    else rTrain.released = true, storagePool.update(rTrain, temp[0]), outputSuccess();
}

void TrainManager::queryTrain(const Parser &p) {
    vector<int> temp;
    indexPool.find(p["-i"], temp);
    if (temp.size() != 1) {
        outputFailure();
        return;
    }
    train_t qTrain(storagePool.read(temp[0]));
    string st = p["-d"];
    station_time_t ti((st[0] - '0') * 10 + st[1] - '0', (st[3] - '0') * 10 + st[4] - '0');
    printTrain(qTrain, ti.dateDistance(qTrain.startTime));
}

void TrainManager::deleteTrain(const Parser &p) {
    vector<int> temp;
    indexPool.find(p["-i"], temp);
    if (temp.size() != 1) {
        outputFailure();
        return;
    }
    indexPool.erase(p["-i"], temp[0]);
    outputSuccess();
}

void TrainManager::clear() {
    indexPool.clear();
    storagePool.clear();
    stationPool.clear();
}
