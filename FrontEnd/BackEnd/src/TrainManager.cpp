//
// Created by Rainy Memory on 2021/4/14.
//

#include "TrainManager.h"

std::string TrainManager::outputSuccess() {
    return std::string("0\n");
}

std::string TrainManager::outputFailure() {
    return std::string("-1\n");
}

std::string TrainManager::printTrain(const TrainManager::train_t &t, int date) {
    //int date: the distance between query train argument -d and startTime sate
    std::string ret;
    std::string ty = "a";
    ty[0] = t.type;
    ret += std::string(t.trainID) + " " + ty + "\n";
    train_time_t temp {};
    for (int i = 0; i < t.stationNum; i++) {
        ret += std::string(t.stations[i]) + " ";
        if (i == 0)ret += "xx-xx xx:xx";
        else {
            temp = t.arrivalTimes[i];
            temp.updateDate(date);
            ret += std::string(temp);
        }
        ret += " -> ";
        if (i == t.stationNum - 1)ret += "xx-xx xx:xx";
        else {
            temp = t.departureTimes[i];
            temp.updateDate(date);
            ret += std::string(temp);
        }
        ret += " " + std::to_string(t.prices[i]) + " ";
        if (i == t.stationNum - 1)ret += "x";
        else ret += std::to_string(t.remainSeats[date][i]);
        ret += "\n";
    }
    return ret;
}

std::string TrainManager::addTrain(const Parser &p) {
    if (indexPool.containsKey(hashTrainID(p["-i"])))return outputFailure();
    int travelTimes[100] = {0};
    int stopoverTimes[100] = {0};
    train_t newTrain {p["-i"], p("-n"), p("-m"), p["-y"][0]};
    splitTool.resetBuffer(p["-s"]);
    for (int i = 0; i < newTrain.stationNum; i++)newTrain.stations[i] = splitTool.nextToken();
    splitTool.resetBuffer(p["-p"]);
    newTrain.prices[0] = 0;
    for (int i = 1; i < newTrain.stationNum; i++)newTrain.prices[i] = splitTool.nextIntToken() + newTrain.prices[i - 1];
    string st {p["-x"]};
    newTrain.startTime.setTime((st[0] - '0') * 10 + st[1] - '0', (st[3] - '0') * 10 + st[4] - '0');
    newTrain.endTime = newTrain.startTime;
    splitTool.resetBuffer(p["-t"]);
    for (int i = 0; i < newTrain.stationNum - 1; i++)travelTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-o"]);
    for (int i = 0; i < newTrain.stationNum - 2; i++)stopoverTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-d"]);
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
    for (auto &remainSeat : newTrain.remainSeats)
        for (int &j : remainSeat)
            j = newTrain.seatNum;
    int offset = storagePool.write(newTrain);
    indexPool.insert(hashTrainID(newTrain.trainID), offset);
    return outputSuccess();
}

std::string TrainManager::releaseTrain(const Parser &p) {
    std::pair<int, bool> temp {indexPool.find(hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t rTrain {storagePool.read(temp.first)};
    if (rTrain.released)return outputFailure();
    rTrain.released = true;
    for (int i = 0; i < rTrain.stationNum; i++)stationPool.insert(hashStation(rTrain.stations[i]), std::pair<long long, int> {hashTrainID(rTrain.trainID), i});
    storagePool.update(rTrain, temp.first);
    return outputSuccess();
}

std::string TrainManager::queryTrain(const Parser &p) {
    std::pair<int, bool> temp {indexPool.find(hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t qTrain {storagePool.read(temp.first)};
    train_time_t ti {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    if (!(qTrain.startTime.lessOrEqualDate(ti) && ti.lessOrEqualDate(qTrain.endTime)))return outputFailure();
    return printTrain(qTrain, ti.dateDistance(qTrain.startTime));
}

std::string TrainManager::deleteTrain(const Parser &p) {
    std::pair<int, bool> temp {indexPool.find(hashTrainID(p["-i"]))};
    if (!temp.second)return outputFailure();
    train_t dTrain {storagePool.read(temp.first)};
    if (dTrain.released)return outputFailure();
    indexPool.erase(hashTrainID(p["-i"]));
    storagePool.erase(temp.first);
    return outputSuccess();
}

std::string TrainManager::queryTicket(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time";
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    static vector<std::pair<long long, int>> sTrains, eTrains;
    sTrains.clear(), eTrains.clear();
    static vector<ticket_t> result;
    result.clear();
    stationPool.find(hashStation(p["-s"]), sTrains);
    stationPool.find(hashStation(p["-t"]), eTrains);
    if (sTrains.empty() || eTrains.empty())return outputSuccess();
    static HashMap<long long, int> hashmap;
    hashmap.clear();
    for (const std::pair<long long, int> &i : sTrains)hashmap[i.first] = i.second;
    for (const std::pair<long long, int> &j : eTrains) {
        int i;
        if (hashmap.containsKey(j.first) && (i = hashmap[j.first]) < j.second) {
            std::pair<int, bool> temp {indexPool.find(j.first)};
            train_t targetTrain {storagePool.read(temp.first)};
            train_time_t dDate {targetTrain.departureTimes[i]};
            if (dDate.lessOrEqualDate(departureDate) && departureDate.lessOrEqualDate(dDate.updateDate(targetTrain.dateGap))) {
                int dist = departureDate.dateDistance(targetTrain.departureTimes[i]);
                train_time_t tempTime1 {targetTrain.departureTimes[i]}, tempTime2 {targetTrain.arrivalTimes[j.second]};
                ticket_t ticket {targetTrain.trainID, targetTrain.stations[i], targetTrain.stations[j.second], tempTime1.updateDate(dist),
                                 tempTime2.updateDate(dist), targetTrain.prices[j.second] - targetTrain.prices[i], SEAT_NUM_INFINITY};
                for (int k = i; k < j.second; k++)ticket.seat = min(ticket.seat, targetTrain.remainSeats[dist][k]);
                result.push_back(ticket);
            }
        }
    }
    if (sortByTime)sortVector<ticket_t>(result, [](const ticket_t &o1, const ticket_t &o2) -> bool { return o1.time != o2.time ? o1.time < o2.time : o1.trainID < o2.trainID; });
    else sortVector<ticket_t>(result, [](const ticket_t &o1, const ticket_t &o2) -> bool { return o1.price != o2.price ? o1.price < o2.price : o1.trainID < o2.trainID; });
    std::string ret;
    ret += std::to_string(result.size()) + "\n";
    for (const ticket_t &i : result)ret += std::string(i) + "\n";
    return ret;
}

std::string TrainManager::queryTransfer(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time", hasResult = false;
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    static vector<std::pair<long long, int>> sTrains, eTrains;
    sTrains.clear(), eTrains.clear();
    ticket_t st {}, en {};
    int nowTime, nowPrice;
    stationPool.find(hashStation(p["-s"]), sTrains);
    stationPool.find(hashStation(p["-t"]), eTrains);
    for (const std::pair<long long, int> &i : sTrains) {
        for (const std::pair<long long, int> &j : eTrains) {
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
                            int sDist = dist;
                            //tempTime(i): avoid updateDate(int) deals changes in original train_t
                            train_time_t tempTime1 {sTrain.departureTimes[i.second]}, tempTime2 {sTrain.arrivalTimes[k]},
                                    tempTime3 {eTrain.departureTimes[l]}, tempTime4 {eTrain.arrivalTimes[j.second]};
                            ticket_t tempSt {sTrain.trainID, sTrain.stations[i.second], sTrain.stations[k], tempTime1.updateDate(sDist),
                                             tempTime2.updateDate(sDist), sTrain.prices[k] - sTrain.prices[i.second]};
                            train_time_t tempTime0 {eTrain.departureTimes[l]};
                            int eDist = aTime.dateDistance(tempTime0);
                            if (aTime > tempTime0.updateDate(eDist))eDist++;
                            ticket_t tempEn {eTrain.trainID, eTrain.stations[l], eTrain.stations[j.second], tempTime3.updateDate(eDist),
                                             tempTime4.updateDate(eDist), eTrain.prices[j.second] - eTrain.prices[l]};
                            tempSt.seat = tempEn.seat = SEAT_NUM_INFINITY;
                            int tempPrice = tempSt.price + tempEn.price, tempTime = tempTime4 - tempTime1;
                            for (int si = i.second; si < k; si++)tempSt.seat = min(tempSt.seat, sTrain.remainSeats[sDist][si]);
                            for (int si = l; si < j.second; si++)tempEn.seat = min(tempEn.seat, eTrain.remainSeats[eDist][si]);
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
    std::string ret;
    if (hasResult)ret += std::string(st) + "\n" + std::string(en) + "\n";
    else ret += "0\n";
    return ret;
}

void TrainManager::clear() {
    indexPool.clear();
    storagePool.clear();
    stationPool.clear();
}
