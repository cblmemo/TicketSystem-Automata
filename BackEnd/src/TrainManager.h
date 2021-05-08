//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
#define TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H

#include "Global.h"

class TrainManager {
private:
    friend class OrderManager;
    
    enum sizeInfo {
        MEMORYPOOL_CAPACITY = 100
    };
    
    using trainID_t = string_t<20>;
    using station_t = string_t<lengthOfChineseCharacters(10)>;
    
    struct hash_station_t {
        int operator()(const station_t &o) const {
            int len = o.length();
            int h = len;
            for (int i = 0; i < len; i++)h = (h << 7) ^ (h >> 25) ^ o[i];
            return h;
        }
    };
    
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
            if (month == 7 && day > 31)day -= 31, month++;
            if (month == 8 && day > 31)day -= 31, month++;
            return *this;
        }
        
        station_time_t &operator+=(int mi) {
            *this = *this + mi;
            return *this;
        }
        
        int operator-(const station_time_t &o) const {
            //return distance in minute
            if (*this > o)return dateDistance(o) * 24 * 60 + (hour - o.hour) * 60 + minute - o.minute;
            else return o.dateDistance(*this) * 24 * 60 + (o.hour - hour) * 60 + o.minute - minute;
        }
        
        int dateDistance(const station_time_t &o) const {
            if ((*this) <= o)return 0;
            //assume this is later than o
            int ret = day - o.day;
            switch (month - o.month) {
                case 3:
                    ret += 92;
                    break;
                case 2:
                    ret += 61;
                    break;
                case 1:
                    if (month == 7)ret += 30;
                    else ret += 31;
            }
            return ret;
        }
        
        station_time_t &updateDate(int da) {
            day += da;
            if (month == 6 && day > 30)day -= 30, month++;
            if (month == 7 && day > 31)day -= 31, month++;
            if (month == 8 && day > 31)day -= 31, month++;
            return *this;
        }
        
        bool operator<(const station_time_t &o) const {
            if (month < o.month)return true;
            else if (month > o.month)return false;
            if (day < o.day)return true;
            else if (day > o.day)return false;
            if (hour < o.hour)return true;
            else if (hour > o.hour)return false;
            if (minute < o.minute)return true;
            else return false;
        }
        
        bool operator==(const station_time_t &o) const {
            return month == o.month && day == o.day && hour == o.hour && minute == o.minute;
        }
        
        bool operator!=(const station_time_t &o) const {
            return !(*this == o);
        }
        
        bool operator>(const station_time_t &o) const {
            if (*this == o)return false;
            return !(*this < o);
        }
        
        bool operator<=(const station_time_t &o) const {
            if (*this == o)return true;
            return *this < o;
        }
        
        bool operator>=(const station_time_t &o) const {
            if (*this == o)return true;
            return !(*this < o);
        }
        
        bool lessOrEqualDate(const station_time_t &o) const {
            if (month < o.month)return true;
            else if (month > o.month)return false;
            if (day <= o.day)return true;
            else return false;
        }
    };
    
    struct ticket_t {
        trainID_t trainID {};
        station_t from {};
        station_t to {};
        station_time_t departureTime {};
        station_time_t arrivalTime {};
        int time = 0;
        int price = 0;
        int seat = 0;
        
        ticket_t() = default;
        
        ticket_t(const trainID_t &i, const station_t &f, const station_t &t, const station_time_t &d, const station_time_t &a, int p, int s = 0) :
                trainID(i), from(f), to(t), departureTime(d), arrivalTime(a), price(p), seat(s) {
            time = arrivalTime - departureTime;
        }
        
        friend std::ostream &operator<<(std::ostream &os, const ticket_t &t) {
            os << t.trainID << " " << t.from << " " << t.departureTime << " -> " << t.to << " " << t.arrivalTime << " " << t.price << " " << t.seat;
            return os;
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
        station_time_t startTime {};//salesDate.first + startTime(in README.md)
        station_time_t endTime {};//salesDate.second + startTime(in README.md)
        char type = '0';
        bool released = false;
        int remainSeats[100][100] = {0};//[date distance with startTime][k'th station]
        int dateGap = 0;//endTime - startTime
        
        train_t() = default;
        
        train_t(const string &_i, int _n, int _m, char _y) : trainID(_i), stationNum(_n), seatNum(_m), type(_y) {}
        
        train_t(const train_t &o) = default;
    };
    
    BPlusTree<trainID_t, int> indexPool;
    LRUCacheMemoryPool<train_t> storagePool;
    BPlusTree<station_t, std::pair<trainID_t, int>> stationPool;//store every train pass through a station, and the serial number (station index)
    TokenScanner splitTool;
    std::ostream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printTrain(const train_t &t, int date);
    
    static inline int min(int a, int b) { return a < b ? a : b; }

public:
    TrainManager(const string &indexPath, const string &storagePath, const string &stationPath, std::ostream &dft) :
            indexPool(indexPath), storagePool(storagePath, 0, MEMORYPOOL_CAPACITY), stationPool(stationPath), defaultOut(dft) { splitTool.resetDelim('|'); }
    
    void addTrain(const Parser &p);
    
    void releaseTrain(const Parser &p);
    
    void queryTrain(const Parser &p);
    
    void deleteTrain(const Parser &p);
    
    void queryTicket(const Parser &p);
    
    void queryTransfer(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
