//
// Created by Rainy Memory on 2021/4/14.
//

#include "TrainManager.h"

void TrainManager::outputSuccess(int value) {
    defaultOut << value << endl;
}

void TrainManager::outputFailure() {
    defaultOut << "-1" << endl;
}

void TrainManager::printTrain(const TrainManager::train_t &t, int date) {
    defaultOut << t.trainID << " " << t.type << endl;
    for (int i = 0; i < t.stationNum; i++) {
        defaultOut << t.stations[i] << " ";
        if (i == 0)defaultOut << "xx-xx xx:xx";
        else defaultOut << t.departureTimes[i];
        defaultOut << " -> ";
        if (i == t.stationNum - 1)defaultOut << "xx-xx xx:xx";
        else defaultOut << t.arrivalTimes[i];
        defaultOut << t.prices[i] << " " << (i == t.stationNum - 1 ? 'x' : t.remainSeats[date][i]) << endl;
    }
}

void TrainManager::addTrain(const Parser &p) {
    if (!indexPool.containsKey(p["-i"])) {
        train_t newTrain(p["-i"], p("-n"), p("-m"), p["-y"][0]);
        splitTool.resetBuffer(p["-s"]);
        for (int i = 0; i < newTrain.stationNum; i++)newTrain.stations[i] = splitTool.nextToken();
        splitTool.resetBuffer(p["-p"]);
        for (int i = 0; i < newTrain.stationNum - 1; i++)newTrain.prices[i] = splitTool.nextIntToken();
        std::string st = p["-x"];
        newTrain.startTime.first = (st[0] - '0') * 10 + st[1] - '0', newTrain.startTime.second = (st[3] - '0') * 10 + st[4] - '0';
        splitTool.resetBuffer(p["-t"]);
        for (int i = 0; i < newTrain.stationNum - 1; i++)newTrain.travelTimes[i] = splitTool.nextIntToken();
        splitTool.resetBuffer(p["-o"]);
        for (int i = 0; i < newTrain.stationNum - 2; i++)newTrain.stopoverTimes[i] = splitTool.nextIntToken();
        //todo departureTimes and arrivalTimes
        splitTool.resetBuffer(p["-d"]);
        st = splitTool.nextToken();
        newTrain.start.first = (st[0] - '0') * 10 + st[1] - '0', newTrain.start.second = (st[3] - '0') * 10 + st[4] - '0';
        st = splitTool.nextToken();
        newTrain.end.first = (st[0] - '0') * 10 + st[1] - '0', newTrain.end.second = (st[3] - '0') * 10 + st[4] - '0';
        int index = storagePool.write(newTrain);
        indexPool.insert(newTrain.trainID, index);
        outputSuccess();
    }
    else outputFailure();
}

void TrainManager::releaseTrain(const Parser &p) {

}

void TrainManager::queryTrain(const Parser &p) {

}

void TrainManager::deleteTrain(const Parser &p) {

}
