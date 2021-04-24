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
    station_time_t temp {};
    for (int i = 0; i < t.stationNum; i++) {
        defaultOut << t.stations[i] << " ";
        if (i == 0)defaultOut << "xx-xx xx:xx";
        else {
            temp = t.arrivalTimes[i];
            temp.updateDate(date);
            defaultOut << temp;
        }
        defaultOut << " -> ";
        if (i == t.stationNum - 1)defaultOut << "xx-xx xx:xx";
        else {
            temp = t.departureTimes[i];
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
    if (indexPool.containsKey(p["-i"]))return outputFailure();
    train_t newTrain {p["-i"], p("-n"), p("-m"), p["-y"][0]};
    splitTool.resetBuffer(p["-s"]);
    for (int i = 0; i < newTrain.stationNum; i++)newTrain.stations[i] = splitTool.nextToken();
    splitTool.resetBuffer(p["-p"]);
    newTrain.prices[0] = 0;
    for (int i = 1; i < newTrain.stationNum; i++)newTrain.prices[i] = splitTool.nextIntToken() + newTrain.prices[i - 1];
    string st {p["-x"]};
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
    newTrain.dateGap = newTrain.endTime.dateDistance(newTrain.startTime);
    station_time_t nowTime {newTrain.startTime};
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
    if (temp.size() != 1)return outputFailure();
    train_t rTrain {storagePool.read(temp[0])};
    if (rTrain.released)return outputFailure();
    rTrain.released = true;
    for (int i = 0; i < rTrain.stationNum; i++)stationPool.insert(rTrain.stations[i], std::pair<trainID_t, int> {rTrain.trainID, i});
    storagePool.update(rTrain, temp[0]), outputSuccess();
}

void TrainManager::queryTrain(const Parser &p) {
    vector<int> temp;
    indexPool.find(p["-i"], temp);
    if (temp.size() != 1)return outputFailure();
    train_t qTrain(storagePool.read(temp[0]));
    string st = p["-d"];
    station_time_t ti {(st[0] - '0') * 10 + st[1] - '0', (st[3] - '0') * 10 + st[4] - '0'};
    printTrain(qTrain, ti.dateDistance(qTrain.startTime));
}

void TrainManager::deleteTrain(const Parser &p) {
    vector<int> temp;
    indexPool.find(p["-i"], temp);
    if (temp.size() != 1)return outputFailure();
    indexPool.erase(p["-i"], temp[0]);
    storagePool.erase(temp[0]);
    outputSuccess();
}

void TrainManager::queryTicket(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time";
    station_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    vector<std::pair<trainID_t, int>> sTrains, eTrains;
    vector<ticket_t> result;
    stationPool.find(p["-s"], sTrains);
    stationPool.find(p["-t"], eTrains);
    for (const auto &i:sTrains) {
        for (const auto &j:eTrains) {
            if (i.first == j.first && i.second < j.second) {
                vector<int> temp;
                indexPool.find(i.first, temp);
                train_t targetTrain {storagePool.read(temp[0])};
                station_time_t dDate {targetTrain.departureTimes[i.second]};
                if (dDate.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dDate.updateDate(targetTrain.dateGap))) {
                    int dist = departureDate.dateDistance(targetTrain.departureTimes[i.second]);
                    ticket_t ticket {targetTrain.trainID, targetTrain.stations[i.second], targetTrain.stations[j.second],
                                     targetTrain.departureTimes[i.second].updateDate(dist), targetTrain.arrivalTimes[j.second].updateDate(dist),
                                     targetTrain.prices[j.second] - targetTrain.prices[i.second], targetTrain.remainSeats[dist][i.second]};
                    for (int k = i.second; k <= j.second; k++)
                        if (targetTrain.remainSeats[dist][k] < ticket.seat)ticket.seat = targetTrain.remainSeats[dist][k];
                    result.push_back(ticket);
                }
            }
        }
    }
    if (result.empty())return outputFailure();
    if (sortByTime)RainyMemory::sortVector<ticket_t>(result, [](const ticket_t &o1, const ticket_t &o2) -> bool { return o1.time < o2.time; });
    else RainyMemory::sortVector<ticket_t>(result, [](const ticket_t &o1, const ticket_t &o2) -> bool { return o1.price < o2.price; });
    defaultOut << result.size() << endl;
    for (const auto &i:result)defaultOut << i << endl;
}

void TrainManager::queryTransfer(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time";
    station_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    vector<std::pair<trainID_t, int>> sTrains, eTrains;
    vector<ticket_t> result;
    stationPool.find(p["-s"], sTrains);
    stationPool.find(p["-t"], eTrains);
    for (const auto &i:sTrains) {
        for (const auto &j:eTrains) {
            if (i.first != j.first) {
                vector<int> temp1, temp2;
                indexPool.find(i.first, temp1), indexPool.find(j.first, temp2);
                train_t sTrain {storagePool.read(temp1[0])}, eTrain {storagePool.read(temp2[0])};
                
            }
        }
    }
}

void TrainManager::clear() {
    indexPool.clear();
    storagePool.clear();
    stationPool.clear();
}
