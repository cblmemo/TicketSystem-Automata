//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
#define TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H

#include "Global.h"

class TrainManager {
    /*
     * class TrainManager
     * --------------------------------------------------------
     * A class implements all functions of trains, including six
     * commands: [add_train], [release_train], [query_train],
     * [delete_train], [query_ticket], [query_transfer].
     * This class used BPlusTree to indexing train data by the
     * unique identifier [trainID], and a memory pool which has
     * built-in LRU Cache to storage data.
     * For [query_train] and [query_transfer], this class also
     * implement a BPlusTree to indexing [trainID] by the station
     * that train passed. This BPlusTree allows duplicate key value.
     *
     */
private:
    friend class OrderManager;
    
    using trainID_t = string_t<20>;
    using station_t = string_t<lengthOfChineseCharacters(10)>;
    using hash_station_t = hash_string_t<lengthOfChineseCharacters(10)>;
    using hash_trainID_t = hash_string_t<20>;
    
    struct train_time_t {
        /*
         * Time Class
         * Modified By PaperL at 2021.5.13
         * Modification:
         *      Store data by 4-byte-long unsigned integer
         *      Use bit-operation
         *      Change most variable name in functions
         *      Support date of full year
         */
#ifndef _GLIBCXX_IOSTREAM
#error "<iostream> is not included. Overloading operator<< is inavailable."
#endif
#ifndef _GLIBCXX_CSTDINT
#error "<cstdint> is not included. uint_fast32_t is inavailable."
#endif
    private:
        typedef uint_fast32_t value_type;
        
        value_type value = 0x00970000;
        constexpr const static value_type accumulateDay[13] = {0, 31, 59, 90, 120, 151,
                                                               181, 212, 243, 273, 304, 334,
                                                               365}; // Accumulate day num of each month
        constexpr const static char numString[60][3] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                                        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                                                        "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
                                                        "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
                                                        "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
                                                        "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"};
        constexpr const static value_type MinuteNum = 24 * 60;
        
        [[nodiscard]] inline value_type Front4Byte() const { return ((value) & 0xffff0000); }
        
        [[nodiscard]] inline value_type Back4Byte() const { return ((value) & 0x0000ffff); }
    
    public:
        explicit train_time_t(int mon, int day, int hou = 0, int min = 0)
                : value((value_type(accumulateDay[mon - 1] + (day - 1)) << 16) | (value_type(hou * 60 + min))) {}
        
        explicit train_time_t(const value_type val) : value(val) {} // 0x00970000 for 00:00,June 1
        
        train_time_t() = default;
        
        train_time_t(const train_time_t &other) = default;
        
        train_time_t &operator=(const train_time_t &other) = default;
        
        
        friend rmstream &operator<<(rmstream &os, const train_time_t &arg) {
            auto date = arg.Front4Byte() >> 16, time = arg.Back4Byte();
            value_type mon = 1, day = date, hou, min = time;
            while (day >= accumulateDay[mon]) mon++;
            day = day - accumulateDay[mon - 1] + 1;
            os << numString[mon] << '-' << numString[day] << ' ';
            hou = min / 60, min %= 60;
            os << numString[hou] << ':' << numString[min];
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
        // Return num of day from a {FORMER} date to this date, otherwise return 0
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
            value = Back4Byte() | (value_type(accumulateDay[mon - 1] + (day - 1)) << 16);
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
        
        ticket_t(const trainID_t &i, const station_t &f, const station_t &t, const train_time_t &d, const train_time_t &a, int p, int s) :
                trainID(i), from(f), to(t), departureTime(d), arrivalTime(a), price(p), seat(s) {
            time = arrivalTime - departureTime;
        }
        
        friend rmstream &operator<<(rmstream &os, const ticket_t &t) {
            os << t.trainID << " " << t.from << " " << t.departureTime << " -> " << t.to << " " << t.arrivalTime << " " << t.price << " " << t.seat;
            return os;
        }
    };
    
    struct date_ticket_t {
        /*
         * date_ticket_t
         * Store train's remain seats, indexing by date distance between
         * start time.
         * --------------------------------------------------------
         * [remainSeats]: store a specific day's remain seats.
         *
         */
        int remainSeats[100] {};
        
        date_ticket_t() = default;
        
        date_ticket_t(int seatNum, int stationNum) {
            for (int i = 0; i < stationNum; i++)remainSeats[i] = seatNum;
        }
        
        int ticketNum(int l, int r) {
            int ret = SEAT_NUM_INFINITY;
            for (int i = l; i < r; i++)ret = min(ret, remainSeats[i]);
            return ret;
        }
        
        void modifyRemain(int l, int r, int delta) {
            for (int i = l; i < r; i++)remainSeats[i] += delta;
        }
        
        int operator[](int index) const {
            return remainSeats[index];
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
        int dateGap = 0;
        
        train_t() = default;
        
        train_t(const string &_i, int _n, int _m, char _y) : trainID(_i), stationNum(_n), seatNum(_m), type(_y) {}
        
        train_t(const train_t &o) = default;
    };
    
    /*
     * Data Members
     * --------------------------------------------------------
     * [ticketPool]: Store every train's ticket for every available
     * day, indexing by trainID and date between
     * [stationPool]: Store every train pass through a specific
     * station, and the station's index in the train's route.
     * [splitTool]: split string divided by '|'.
     *
     */
    BPlusTree<hash_t, int, BPLUSTREE_L, BPLUSTREE_M> indexPool;
    LRUCacheMemoryPool<train_t> storagePool;
    BPlusTree<std::pair<hash_t, int>, date_ticket_t> ticketPool;
    MultiBPlusTree<hash_t, std::pair<hash_t, int>, MULTI_BPLUSTREE_L, MULTI_BPLUSTREE_M> stationPool;
    hash_trainID_t hashTrainID;
    hash_station_t hashStation;
    TokenScanner splitTool;
    rmstream &defaultOut;
    
    inline void outputSuccess();
    
    inline void outputFailure();
    
    inline void printTrain(const train_t &t, int date);
    
    static inline int min(int a, int b) { return a < b ? a : b; }

public:
    TrainManager(const string &indexPath, const string &storagePath, const string &ticketPath, const string &stationPath, rmstream &dft) :
            indexPool(indexPath), storagePool(storagePath, 0, TRAIN_CACHE_SIZE), ticketPool(ticketPath), stationPool(stationPath), defaultOut(dft) { splitTool.resetDelim('|'); }
    
    void addTrain(const Parser &p);
    
    void releaseTrain(const Parser &p);
    
    void queryTrain(const Parser &p);
    
    void deleteTrain(const Parser &p);
    
    void queryTicket(const Parser &p);
    
    void queryTransfer(const Parser &p);
    
    void clear();
};

#endif //TICKETSYSTEM_AUTOMATA_TRAINMANAGER_H
