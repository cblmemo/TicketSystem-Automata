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
    int travelTimes[100] = {0};
    int stopoverTimes[100] = {0};
    train_t newTrain {p["-i"], p("-n"), p("-m"), p["-y"][0]};
    splitTool.resetBuffer(p["-s"]);
    for (int i = 0; i < newTrain.stationNum; i++)newTrain.stations[i] = splitTool.nextToken();
    splitTool.resetBuffer(p["-p"]);
    //prices is store as prefix sum (from starting station)
    newTrain.prices[0] = 0;
    for (int i = 1; i < newTrain.stationNum; i++)newTrain.prices[i] = splitTool.nextIntToken() + newTrain.prices[i - 1];
    string st {p["-x"]};
    newTrain.startTime.hour = (st[0] - '0') * 10 + st[1] - '0', newTrain.startTime.minute = (st[3] - '0') * 10 + st[4] - '0';
    newTrain.endTime.hour = newTrain.startTime.hour, newTrain.endTime.minute = newTrain.startTime.minute;
    splitTool.resetBuffer(p["-t"]);
    for (int i = 0; i < newTrain.stationNum - 1; i++)travelTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-o"]);
    for (int i = 0; i < newTrain.stationNum - 2; i++)stopoverTimes[i] = splitTool.nextIntToken();
    splitTool.resetBuffer(p["-d"]);
    st = splitTool.nextToken();
    newTrain.startTime.month = (st[0] - '0') * 10 + st[1] - '0', newTrain.startTime.day = (st[3] - '0') * 10 + st[4] - '0';
    st = splitTool.nextToken();
    newTrain.endTime.month = (st[0] - '0') * 10 + st[1] - '0', newTrain.endTime.day = (st[3] - '0') * 10 + st[4] - '0';
    newTrain.dateGap = newTrain.endTime.dateDistance(newTrain.startTime);
    //departure time and arrival time store the first train(departure at start time)'s timetable
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
    indexPool.insert(newTrain.trainID, offset);
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
    train_t qTrain {storagePool.read(temp[0])};
    train_time_t ti {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    if (!(qTrain.startTime.lessOrEqualDate(ti) && ti.lessOrEqualDate(qTrain.endTime)))return outputFailure();
    printTrain(qTrain, ti.dateDistance(qTrain.startTime));
}

void TrainManager::deleteTrain(const Parser &p) {
    vector<int> temp;
    indexPool.find(p["-i"], temp);
    if (temp.size() != 1)return outputFailure();
    train_t dTrain {storagePool.read(temp[0])};
    if (dTrain.released)return outputFailure();
    indexPool.erase(p["-i"], temp[0]);
    storagePool.erase(temp[0]);
    outputSuccess();
}

void TrainManager::queryTicket(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time";
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0'};
    vector<std::pair<trainID_t, int>> sTrains, eTrains;
    vector<ticket_t> result;
    stationPool.find(p["-s"], sTrains);
    stationPool.find(p["-t"], eTrains);
    if (sTrains.empty() || eTrains.empty())return outputSuccess();
    HashMap<trainID_t, int, hash_trainID_t> hashmap;
    for (const std::pair<trainID_t, int> &i : sTrains)hashmap[i.first] = i.second;
    for (const std::pair<trainID_t, int> &j : eTrains) {
        int i;
        if (hashmap.containsKey(j.first) && (i = hashmap[j.first]) < j.second) {
            vector<int> temp;
            indexPool.find(j.first, temp);
            train_t targetTrain {storagePool.read(temp[0])};
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
    defaultOut << result.size() << endl;
    for (const ticket_t &i : result)defaultOut << i << endl;
}

void TrainManager::queryTransfer(const Parser &p) {
    bool sortByTime = !p.haveThisArgument("-p") || p["-p"] == "time", hasResult = false;
    train_time_t departureDate {(p["-d"][0] - '0') * 10 + p["-d"][1] - '0', (p["-d"][3] - '0') * 10 + p["-d"][4] - '0', 0, 0};
    vector<std::pair<trainID_t, int>> sTrains, eTrains;
    ticket_t st {}, en {};
    int nowTime, nowPrice;
    stationPool.find(p["-s"], sTrains);
    stationPool.find(p["-t"], eTrains);
    for (const std::pair<trainID_t, int> &i : sTrains) {
        for (const std::pair<trainID_t, int> &j : eTrains) {
            if (i.first != j.first) {
                vector<int> temp1, temp2;
                indexPool.find(i.first, temp1), indexPool.find(j.first, temp2);
                train_t sTrain {storagePool.read(temp1[0])}, eTrain {storagePool.read(temp2[0])};
                HashMap<station_t, int, hash_station_t> hashmap;
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
                            int sDist = departureDate.dateDistance(sTrain.departureTimes[i.second]);
                            //tempTimex: avoid updateDate(int) deals changes in original train_t
                            train_time_t tempTime1 {sTrain.departureTimes[i.second]}, tempTime2 {sTrain.arrivalTimes[k]},
                                    tempTime3 {eTrain.departureTimes[l]}, tempTime4 {eTrain.arrivalTimes[j.second]};
                            ticket_t tempSt {sTrain.trainID, sTrain.stations[i.second], sTrain.stations[k], tempTime1.updateDate(sDist),
                                             tempTime2.updateDate(sDist), sTrain.prices[k] - sTrain.prices[i.second]};
                            train_time_t tempTime0 {eTrain.departureTimes[l]};
                            int eDist = aTime.dateDistance(tempTime0);
                            if (aTime > tempTime0.updateDate(eDist))eDist++;
                            ticket_t tempEn {eTrain.trainID, eTrain.stations[l], eTrain.stations[j.second], tempTime3.updateDate(eDist),
                                             tempTime4.updateDate(eDist), eTrain.prices[j.second] - eTrain.prices[l]};
                            int sTempSeat = SEAT_NUM_INFINITY, eTempSeat = SEAT_NUM_INFINITY;
                            int tempPrice = tempSt.price + tempEn.price, tempTime = tempTime4 - tempTime1;
                            for (int si = i.second; si < k; si++)sTempSeat = min(sTempSeat, sTrain.remainSeats[sDist][si]);
                            for (int si = l; si < j.second; si++)eTempSeat = min(eTempSeat, eTrain.remainSeats[eDist][si]);
                            tempSt.seat = sTempSeat, tempEn.seat = eTempSeat;
                            if (hasResult) {
                                bool timeJudge = tempTime < nowTime || tempTime == nowTime && tempSt.time < st.time;
                                bool priceJudge = tempPrice < nowPrice || tempPrice == nowPrice && tempSt.price < st.price;
                                if (sortByTime && timeJudge || !sortByTime && priceJudge)nowTime = tempTime, nowPrice = tempPrice, st = tempSt, en = tempEn;
                            }
                            else hasResult = true, nowTime = tempTime, nowPrice = tempPrice, st = tempSt, en = tempEn;
                        }
                    }
                }
            }
        }
    }
    if (hasResult)defaultOut << st << endl << en << endl;
    else defaultOut << "0" << endl;
}

void TrainManager::clear() {
    indexPool.clear();
    storagePool.clear();
    stationPool.clear();
}
