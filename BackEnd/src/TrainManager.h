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
    
    struct station_time_t {
        int month = 6;
        int day = 1;
        int hour = 0;
        int minute = 0;
        
        explicit station_time_t(int mo, int da, int ho = 0, int mi = 0) : month(mo), day(da), hour(ho), minute(mi) {}
        
        station_time_t() = default;
        
        friend std::ostream &operator<<(std::ostream &os, const station_time_t &t) {
            os << "0" << t.month << "-";
            if (t.day >= 10)os << t.day;
            else os << "0" << t.day;
            os << " ";
            if (t.hour >= 10)os << t.hour;
            else os << "0" << t.hour;
            os << ":";
            if (t.minute >= 10)os << t.minute;
            else os << "0" << t.minute;
            return os;
        }
        
        station_time_t operator+(int mi) {
            minute += mi;
            int ho = minute - minute % 60;
            minute %= 60;
            hour += ho / 60;
            int da = hour - hour % 24;
            hour %= 24;
            day += da / 24;
            if (month == 6 && day > 30)day -= 30, month++;
            if (day > 31)day -= 31, month++;
            return *this;
        }
        
        station_time_t &operator+=(int mi) {
            *this = *this + mi;
            return *this;
        }
        
        int dateDistance(const station_time_t &o) const {
            int ret = day - o.day;
            switch (month - o.month) {
                case 2:
                    ret += 61;
                    break;
                case 1:
                    if (month == 7)ret += 30;
                    else ret += 31;
            }
            return ret;
        }
        
        void updateDate(int da) {
            day += da;
            if (month == 6 && day > 30)day -= 30, month++;
            if (day > 31)day -= 31, month++;
        }
    };
    
    struct train_t {
        trainID_t trainID {};
        int stationNum = 0;
        station_t stations[100] {};
        int seatNum = 0;
        int prices[100] = {0};
        //the following array records arrival and departure time of the first departure. (saleDate_t startTime)
        station_time_t arrivalTimes[100] {};
        station_time_t departureTimes[100] {};
        int travelTimes[100] = {0};
        int stopoverTimes[100] = {0};
        //the following variable include startTime
        station_time_t startTime {};
        station_time_t endTime {};
        char type = '0';
        bool released = false;
        int remainSeats[70][100] = {0};
        
        train_t() = default;
        
        train_t(const string &_i, int _n, int _m, char _y) : trainID(_i), stationNum(_n), seatNum(_m), type(_y) {}
        
        train_t(const train_t &o) = default;
    };
    
    BPlusTree<trainID_t, int> indexPool;
    LRUCacheMemoryPool<train_t> storagePool;
    BPlusTree<station_t, std::pair<trainID_t, int>> stationPool;//store every train pass through a station, and k'th station
    TokenScanner splitTool;
    std::ostream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printTrain(const train_t &t, int date);

public:
    TrainManager(const string &indexPath, const string &storagePath, const string &stationPath, std::ostream &dft = std::cout) :
            indexPool(indexPath), storagePool(storagePath, 0, MEMORYPOOL_CAPACITY), stationPool(stationPath), defaultOut(dft) { splitTool.resetDelim('|'); }
    
    void addTrain(const Parser &p);
    
    void releaseTrain(const Parser &p);
    
    void queryTrain(const Parser &p);
    
    void deleteTrain(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
