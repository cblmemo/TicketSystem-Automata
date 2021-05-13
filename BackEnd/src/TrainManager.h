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
        MEMORYPOOL_CAPACITY = 300
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
    
    struct hash_trainID_t {
        int operator()(const trainID_t &o) const {
            int len = o.length();
            int h = len;
            for (int i = 0; i < len; i++)h = (h << 7) ^ (h >> 25) ^ o[i];
            return h;
        }
    };
    
    struct train_time_t {
        /*
         * Time Class (Only support date from [00:00,June1] ~ [23:59,Sept30])
         * Modified By PaperL at 2021.5.13
         * Modification:
         *      Store data by 4-byte-long unsigned integer
         *      Use bit-operation
         *      Change most variable name in functions
         */
#ifndef _GLIBCXX_IOSTREAM
#error "<iostream> is not included. Overloading operator<< is inavailable."
#endif
#ifndef _GLIBCXX_CSTDINT
#error "<cstdint> is not included. uint_fast32_t is inavailable."
#endif
    private:
    public:
        typedef uint_fast32_t value_type;
        
        value_type value = 0x00970000;
        constexpr const static value_type dayNumOfMonth[13] = {0, 31, 59, 90, 120, 151,
                                                               181, 212, 243, 273, 304, 334,
                                                               365};
        constexpr const static value_type MinuteNum = 24 * 60;
        
        [[nodiscard]] inline value_type Front4Byte() const { return ((value) & 0xffff0000); }
        
        [[nodiscard]] inline value_type Back4Byte() const { return ((value) & 0x0000ffff); }
    
    public:
        explicit train_time_t(int mon, int day, int hou = 0, int min = 0)
                : value((value_type(dayNumOfMonth[mon - 1] + (day - 1)) << 16) | (value_type(hou * 60 + min))) {}
        
        explicit train_time_t(const value_type val) : value(val) {}
        
        train_time_t() = default;
        
        train_time_t(const train_time_t &other) = default;
        
        train_time_t &operator=(const train_time_t &other) = default;
        
        friend std::ostream &operator<<(std::ostream &os, const train_time_t &arg) {
            auto date = arg.Front4Byte() >> 16, time = arg.Back4Byte();
            value_type mon = 1, day = date, hou, min = time;
            
            while (day >= dayNumOfMonth[mon]) mon++;
            day = day - dayNumOfMonth[mon - 1] + 1;
            if (mon < 10) os << '0' << mon; else os << mon;
            os << '-';
            if (day >= 10) os << day; else os << '0' << day;
            
            os << ' ';
            
            hou = min / 60, min %= 60;
            if (hou < 10) os << '0' << hou; else os << hou;
            os << ':';
            if (min >= 10)os << min; else os << '0' << min;
            
            return os;
        }
        
        bool operator==(const train_time_t &right) const { return value == right.value; }
        
        bool operator!=(const train_time_t &right) const { return value != right.value; }
        
        bool operator<(const train_time_t &right) const { return value < right.value; }
        
        bool operator>(const train_time_t &right) const { return value > right.value; }
        
        bool operator<=(const train_time_t &right) const { return value <= right.value; }
        
        bool operator>=(const train_time_t &right) const { return value >= right.value; }
        
        
        train_time_t operator+(int minute) const {
            value_type time = Back4Byte() + minute;
            return train_time_t((time >= MinuteNum)
                                ? ((Front4Byte() + ((time / MinuteNum) << 16)) | (time % MinuteNum))
                                : (Front4Byte() | time));
        }
        
        train_time_t &operator+=(int minute) {
            value_type time = Back4Byte() + minute;
            value = (time >= MinuteNum)
                    ? ((Front4Byte() + ((time / MinuteNum) << 16)) | (time % MinuteNum))
                    : (Front4Byte() | time);
            return *this;
        }
        
        int operator-(const train_time_t &right) const
        // Return time difference in unit of minute
        {
            if (value > right.value)
                return int(((Front4Byte() - right.Front4Byte()) >> 16) * MinuteNum
                           + (Back4Byte() - right.Back4Byte()));
            else if (value < right.value)
                return int(((right.Front4Byte() - Front4Byte()) >> 16) * MinuteNum
                           + (right.Back4Byte() - Back4Byte()));
            else return int(0);
        }
        
        [[nodiscard]] int dateDistance(const train_time_t &right) const
        // Return num of day from this date to a {FORMER} date, otherwise return 0
        {
            return ((value > right.value) ? int((Front4Byte() - right.Front4Byte()) >> 16) : 0);
        }
        
        train_time_t &updateDate(const int dd) {
            value += value_type(dd) << 16;
            return *this;
        }
        
        [[nodiscard]] bool lessOrEqualDate(const train_time_t &right) const {
            return (value <= right.value) || (Front4Byte() == right.Front4Byte());
        }
        
        void setDate(const int mon, const int day) {
            value = Back4Byte() | (value_type(dayNumOfMonth[mon - 1] + (day - 1)) << 16);
        }
        
        void setTime(const int hou, const int min) {
            value = Front4Byte() | value_type(hou * 60 + min);
        }
    };
    
    struct ticket_t {
        trainID_t trainID {};
        station_t from {};
        station_t to {};
        train_time_t departureTime {};
        train_time_t arrivalTime {};
        int time = 0;
        int price = 0;
        int seat = 0;
        
        ticket_t() = default;
        
        ticket_t(const trainID_t &i, const station_t &f, const station_t &t, const train_time_t &d, const train_time_t &a, int p, int s = 0) :
                trainID(i), from(f), to(t), departureTime(d), arrivalTime(a), price(p), seat(s) {
            time = arrivalTime - departureTime;
        }
        
        friend std::ostream &operator<<(std::ostream &os, const ticket_t &t) {
            os << t.trainID << " " << t.from << " " << t.departureTime << " -> " << t.to << " " << t.arrivalTime << " " << t.price << " " << t.seat;
            return os;
        }
    };
    
    struct train_t {
        /*
         * train_t
         * Store train's relevant information
         * --------------------------------------------------------
         * [prices]: A prefix sum of ticket prices.
         * [arrivalTimes] && [departureTimes]: Records arrival time
         * and departure time of the first train (departure at the
         * first day of sales date).
         * [startTime] && [endTime]: Records departure time (date
         * and time) of the first train and the last train.
         * [remainSeats]: Records each train's remain ticket,
         * the first index represent date distance between
         * start time, the second represent station index. For
         * example, remainSeats[0][0] represent the first train's
         * ticket number, from the first station to the second station.
         * [dateGap]: Date gap between end time and start time.
         *
         */
        trainID_t trainID {};
        int stationNum = 0;
        station_t stations[100] {};
        int seatNum = 0;
        int prices[100] = {0};
        train_time_t arrivalTimes[100] {};
        train_time_t departureTimes[100] {};
        train_time_t startTime {};
        train_time_t endTime {};
        char type = '0';
        bool released = false;
        int remainSeats[100][100] = {0};
        int dateGap = 0;
        
        train_t() = default;
        
        train_t(const string &_i, int _n, int _m, char _y) : trainID(_i), stationNum(_n), seatNum(_m), type(_y) {}
        
        train_t(const train_t &o) = default;
    };
    
    /*
     * Data Members
     * --------------------------------------------------------
     * [stationPool]: Store every train pass through a specific
     * station, and the station's index in the train's route.
     * [splitTool]: split string divided by '|'.
     *
     */
    BPlusTree<trainID_t, int> indexPool;
    LRUCacheMemoryPool<train_t> storagePool;
    BPlusTree<station_t, std::pair<trainID_t, int>> stationPool;
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
