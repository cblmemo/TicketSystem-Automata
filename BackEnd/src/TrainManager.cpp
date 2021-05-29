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
    train_time_t temp {};
    if (t.released) {
        std::pair<hash_t, int> key {hashTrainID(t.trainID), date};
#ifndef storageData
        date_ticket_t seats {ticketStoragePool.read(ticketPool.find(key).first)};
#else
        date_ticket_t seats {ticketPool.find(key).first};
#endif
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
            else defaultOut << seats[i];
            defaultOut << endl;
        }
    }
    else {
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
            else defaultOut << t.seatNum;
            defaultOut << endl;
        }
    }
}

void TrainManager::addTrain(const Parser &p) {
    if (indexPool.containsKey(hashTrainID(p["-i"])))return outputFailure();
    int travelTimes[100] = {0};
    int stopoverTimes[100] = {0};
    train_t newTrain {p["-i"], p("-n"), p("-m"), p["-y"][0]};
    splitTool.resetBuffer(p["-s"].c_str());
    for (int i = 0; i < newTrain.stationNum; i++)newTrain.stations[i] = splitTool.nextToken();
    splitTool.resetBuffer(p["-p"].c_str());
    newTrain.prices[0] = 0;
    for (int i = 1; i < newTrain.stationNum; i++)newTrain.prices[i] = splitTool.nextIntToken() + newTrain.prices[i - 1];
    string st {p["-x"]};
    newTrain.startTime.setTime((st[0] - '0') * 10 + st[1] - '0', (st[3] - '0') * 10 + st[4] - '0');
    newTrain.endTime = newTrain.startTime;
    splitTool.resetBuffer(p["-t"].c_str());
    for (int i = 0; i < newTrain.stationNum - 1; i++)travelTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-o"].c_str());
    for (int i = 0; i < newTrain.stationNum - 2; i++)stopoverTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-d"].c_str());
    st = splitTool.nextToken();
    newTrain.startTime.setDate((st[0] - '0') * 10 + st[1] - '0', (st[3] - '0') * 10 + st[4] - '0');
    st = splitTool.nextToken();
    newTrain.endTime.setDate((st[0] - '0') * 10 + st[1] - '0', (st[3] - '0') * 10 + st[4] - '0');
    newTrain.dateGap = newTrain.endTime.dateDistance(newTrain.startTime);
    train_time_t nowTime {newTrain.startTime};
    newTrain.departureTimes[0] = nowTime;
    for (int i = 1; i < newTrain.stationNum - 1; i++) {
        nowTime += travelTimes[i - 1];
        newTrain.arrivalTimes[i] = nowTime;
        nowTime += stopoverTimes[i - 1];
        newTrain.departureTimes[i] = nowTime;
    }
    nowTime += travelTimes[newTrain.stationNum - 2];
    newTrain.arrivalTimes[newTrain.stationNum - 1] = nowTime;
    int offset = storagePool.write(newTrain);
    indexPool.insert(hashTrainID(newTrain.trainID), offset);
    outputSuccess();
}

void TrainManager::releaseTrain(const Parser &p) {
    hash_t hash = hashTrainID(p["-i"]);
    std::pair<int, bool> temp {indexPool.find(hash)};
    if (!temp.second)return outputFailure();
    train_t rTrain {storagePool.read(temp.first)};
    if (rTrain.released)return outputFailure();
    rTrain.released = true;
#ifndef storageData
    for (int i = 0; i <= rTrain.dateGap; i++)ticketPool.insert(std::pair<hash_t, int> {hash, i}, ticketStoragePool.write(date_ticket_t(rTrain.seatNum, rTrain.stationNum)));
#else
    for (int i = 0; i <= rTrain.dateGap; i++)ticketPool.insert(std::pair<hash_t, int> {hash, i}, date_ticket_t(rTrain.seatNum, rTrain.stationNum));
#endif
    for (int i = 0; i < rTrain.stationNum; i++)stationPool.insert(hashStation(rTrain.stations[i]), stationPool.size(), std::pair<hash_t, int> {hashTrainID(rTrain.trainID), i});
    storagePool.update(rTrain, temp.first), outputSuccess();
}

void TrainManager::queryTrain(const Parser &p) {
    std::pair<int, bool> temp {indexPool.find(hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t qTrain {storagePool.read(temp.first)};
    train_time_t ti {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    if (!(qTrain.startTime.lessOrEqualDate(ti) && ti.lessOrEqualDate(qTrain.endTime)))return outputFailure();
    printTrain(qTrain, ti.dateDistance(qTrain.startTime));
}

void TrainManager::deleteTrain(const Parser &p) {
    std::pair<int, bool> temp {indexPool.find(hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t dTrain {storagePool.read(temp.first)};
    if (dTrain.released)return outputFailure();
    indexPool.erase(hashTrainID(p["-i"]));
    storagePool.erase(temp.first);
    outputSuccess();
}

void TrainManager::queryTicket(const Parser &p) {
    if (p["-s"] == p["-t"])return outputSuccess();
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time";
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    static vector<std::pair<hash_t, int>> sTrains, eTrains;
    sTrains.clear(), eTrains.clear();
    static vector<ticket_t> result;
    result.clear();
    stationPool.find(hashStation(p["-s"]), sTrains);
    stationPool.find(hashStation(p["-t"]), eTrains);
    if (sTrains.empty() || eTrains.empty())return outputSuccess();
    static HashMap<hash_t, int> hashmap;
    hashmap.clear();
    for (const std::pair<hash_t, int> &i : sTrains)hashmap[i.first] = i.second;
    for (const std::pair<hash_t, int> &j : eTrains) {
        int i;
        if (hashmap.containsKey(j.first) && (i = hashmap[j.first]) < j.second) {
            std::pair<int, bool> temp {indexPool.find(j.first)};
            train_t targetTrain {storagePool.read(temp.first)};
            train_time_t dDate {targetTrain.departureTimes[i]};
            if (dDate.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dDate.updateDate(targetTrain.dateGap))) {
                int dist = departureDate.dateDistance(targetTrain.departureTimes[i]);
                std::pair<hash_t, int> key {hashTrainID(targetTrain.trainID), dist};
#ifndef storageData
                date_ticket_t seats {ticketStoragePool.read(ticketPool.find(key).first)};
#else
                date_ticket_t seats {ticketPool.find(key).first};
#endif
                train_time_t tempTime1 {targetTrain.departureTimes[i]}, tempTime2 {targetTrain.arrivalTimes[j.second]};
                ticket_t ticket {targetTrain.trainID, targetTrain.stations[i], targetTrain.stations[j.second], tempTime1.updateDate(dist),
                                 tempTime2.updateDate(dist), targetTrain.prices[j.second] - targetTrain.prices[i], seats.ticketNum(i, j.second)};
                result.push_back(ticket);
            }
        }
    }
    if (sortByTime)sortVector<ticket_t>(result, [](const ticket_t &o1, const ticket_t &o2) -> bool { return o1.time != o2.time ? o1.time < o2.time : o1.trainID < o2.trainID; });
    else sortVector<ticket_t>(result, [](const ticket_t &o1, const ticket_t &o2) -> bool { return o1.price != o2.price ? o1.price < o2.price : o1.trainID < o2.trainID; });
    defaultOut << result.size() << endl;
    for (const ticket_t &i : result)defaultOut << i << endl;
}

void TrainManager::queryTransfer(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time", hasResult = false;
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    static vector<std::pair<hash_t, int>> sTrains, eTrains;
    sTrains.clear(), eTrains.clear();
    ticket_t st {}, en {};
    int nowTime, nowPrice;
    stationPool.find(hashStation(p["-s"]), sTrains);
    stationPool.find(hashStation(p["-t"]), eTrains);
    if (sTrains.empty() || eTrains.empty())return outputSuccess();
    for (const std::pair<hash_t, int> &i : sTrains) {
        for (const std::pair<hash_t, int> &j : eTrains) {
            if (i.first != j.first) {
                std::pair<int, bool> temp1 {indexPool.find(i.first)}, temp2 {indexPool.find(j.first)};
                train_t sTrain {storagePool.read(temp1.first)}, eTrain {storagePool.read(temp2.first)};
                static HashMap<station_t, int, hash_station_t> hashmap;
                hashmap.clear();
                for (int k = i.second + 1; k < sTrain.stationNum; k++)hashmap[sTrain.stations[k]] = k;
                for (int l = 0; l < j.second; l++) {
                    if (hashmap.containsKey(eTrain.stations[l])) {
                        int k = hashmap[eTrain.stations[l]];
                        train_time_t dDate {sTrain.departureTimes[i.second]};
                        bool judge1 = dDate.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dDate.updateDate(sTrain.dateGap));//can boarding on sTrain
                        int dist = departureDate.dateDistance(sTrain.departureTimes[i.second]);
                        train_time_t aTime {sTrain.arrivalTimes[k]}, lastTime {eTrain.departureTimes[l]};
                        aTime.updateDate(dist), lastTime.updateDate(eTrain.dateGap);//can boarding on eTrain
                        bool judge2 = aTime <= lastTime;
                        if (judge1 && judge2) {
                            //tempTime(i): avoid updateDate(int) deals changes in original train_t
                            train_time_t tempTime0 {eTrain.departureTimes[l]},
                                    tempTime1 {sTrain.departureTimes[i.second]}, tempTime2 {sTrain.arrivalTimes[k]},
                                    tempTime3 {eTrain.departureTimes[l]}, tempTime4 {eTrain.arrivalTimes[j.second]};
                            int sDist = dist, eDist = aTime.dateDistance(tempTime0);
                            if (aTime > tempTime0.updateDate(eDist))eDist++;
                            std::pair<hash_t, int> keySt {hashTrainID(sTrain.trainID), sDist}, keyEn {hashTrainID(eTrain.trainID), eDist};
#ifndef storageData
                            date_ticket_t seatsSt {ticketStoragePool.read(ticketPool.find(keySt).first)}, seatsEn {ticketStoragePool.read(ticketPool.find(keyEn).first)};
#else
                            date_ticket_t seatsSt {ticketPool.find(keySt).first}, seatsEn {ticketPool.find(keyEn).first};
#endif
                            ticket_t tempSt {sTrain.trainID, sTrain.stations[i.second], sTrain.stations[k], tempTime1.updateDate(sDist),
                                             tempTime2.updateDate(sDist), sTrain.prices[k] - sTrain.prices[i.second], seatsSt.ticketNum(i.second, k)};
                            ticket_t tempEn {eTrain.trainID, eTrain.stations[l], eTrain.stations[j.second], tempTime3.updateDate(eDist),
                                             tempTime4.updateDate(eDist), eTrain.prices[j.second] - eTrain.prices[l], seatsEn.ticketNum(l, j.second)};
                            int tempPrice = tempSt.price + tempEn.price, tempTime = tempTime4 - tempTime1;
                            if (hasResult) {
                                if (sortByTime && (tempTime < nowTime || tempTime == nowTime && tempSt.time < st.time))nowTime = tempTime, st = tempSt, en = tempEn;
                                if (!sortByTime && (tempPrice < nowPrice || tempPrice == nowPrice && tempSt.time < st.time))nowPrice = tempPrice, st = tempSt, en = tempEn;
                            }
                            else hasResult = true, nowTime = tempTime, nowPrice = tempPrice, st = tempSt, en = tempEn;
                        }
                    }
                }
            }
        }
    }
    if (!hasResult)return outputSuccess();
    defaultOut << st << endl << en << endl;
}

void TrainManager::clear() {
    indexPool.clear();
    storagePool.clear();
    ticketPool.clear();
#ifndef storageData
    ticketStoragePool.clear();
#endif
    stationPool.clear();
}
