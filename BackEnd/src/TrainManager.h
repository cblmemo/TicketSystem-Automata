//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
#define TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H

#include "GLobal.h"
#include "../include/BPlusTree.h"
#include "../include/RTL/string_t.h"
#include "../include/Parser.h"
#include "../include/RTL/Clock.h"

using RainyMemory::BPlusTree;
using RainyMemory::TokenScanner;
using RainyMemory::Parser;

class TrainManager {
private:
    friend class OrderManager;
    
    enum sizeInfo {
        MEMORYPOOL_CAPACITY = 100
    };
    
    using trainID_t = string_t<20>;
    using station_t = string_t<lengthOfChineseCharacters(10)>;
    using startTime_t = std::pair<int, int>;
    using saleDate_t = std::pair<int, int>;
    
    struct station_time_t {
        int month = -1;
        int day = -1;
        int hour = -1;
        int minute = -1;
        
        friend std::ostream &operator<<(std::ostream &os, const station_time_t &t) {
            os << t.month << "-" << t.day << " " << t.hour << ":" << t.minute;
            return os;
        }
    };
    
    struct train_t {
        trainID_t trainID {};//
        int stationNum = 0;//
        station_t stations[100] {};//
        int seatNum = 0;//
        int prices[100] = {0};//
        startTime_t startTime {};//
        station_time_t departureTimes[100] {};
        station_time_t arrivalTimes[100] {};
        int travelTimes[100] = {0};//
        int stopoverTimes[100] = {0};//
        saleDate_t start {};//
        saleDate_t end {};//
        char type = '0';//
        bool released = false;
        int remainSeats[70][100] = {0};
        
        train_t(const string &_i, int _n, int _m, char _y) : trainID(_i), stationNum(_n), seatNum(_m), type(_y) {}
    };
    
    BPlusTree<trainID_t, int> indexPool;
    LRUCacheMemoryPool<train_t, int> storagePool;
    BPlusTree<station_t, trainID_t> stationPool;//store every train pass through a station
    TokenScanner splitTool;
    std::ostream &defaultOut;
    
    inline void outputSuccess(int value = 0);
    
    inline void outputFailure();
    
    inline void printTrain(const train_t &t, int date);

public:
    TrainManager(const string &indexPath, const string &storagePath, const string &stationPath, std::ostream &dft = std::cout) :
            indexPool(indexPath), storagePool(storagePath, 0, MEMORYPOOL_CAPACITY), stationPool(stationPath), defaultOut(dft) { splitTool.resetDelim('|'); }
    
    void addTrain(const Parser &p);
    
    void releaseTrain(const Parser &p);
    
    void queryTrain(const Parser &p);
    
    void deleteTrain(const Parser &p);
};

#endif //TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
