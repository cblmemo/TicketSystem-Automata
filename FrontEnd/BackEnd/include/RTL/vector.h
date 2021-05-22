//
// Created by Rainy Memory on 2021/4/14.
//

#ifndef TICKETSYSTEM_AUTOMATA_VECTOR_H
#define TICKETSYSTEM_AUTOMATA_VECTOR_H

#include "exceptions.h"

#define INIT_LEN 5

namespace RainyMemory {
    template<typename T>
    class vector {
        /*
         * class vector
         * --------------------------------------------------------
         * A class implements infinity space array like std::vector.
         *
         */
    private:
        T **store = nullptr;
        int tail;
        int length;
        
        void double_space() {
            length *= 2;
            T **temp = new T *[length];
            for (int i = 0; i < tail; i++)temp[i] = store[i];
            delete[]store;
            store = temp;
        }
        
        void halve_space() {
            if (length > INIT_LEN) {
                length = (length / 2 < INIT_LEN ? INIT_LEN : length / 2);
                T **temp = new T *[length];
                for (int i = 0; i < tail; i++)temp[i] = store[i];
                delete[]store;
                store = temp;
            }
        }
    
    public:
        class const_iterator;
        
        class iterator {
            friend class vector;
        
        private:
            vector<T> *vec;
            int pos;
        
        public:
            iterator(vector<T> *v, int p) : vec(v), pos(p) {}
            
            iterator(const iterator &other) : vec(other.vec), pos(other.pos) {}
            
            iterator operator+(const int &n) const {
                if (pos + n > vec->size())throw invalid_iterator();
                iterator temp(*this);
                temp.pos += n;
                return temp;
            }
            
            iterator operator-(const int &n) const {
                if (pos < n)throw invalid_iterator();
                iterator temp(*this);
                temp.pos -= n;
                return temp;
            }
            
            int operator-(const iterator &rhs) const {
                if (vec != rhs.vec)throw invalid_iterator();
                return pos - rhs.pos;
            }
            
            iterator &operator+=(const int &n) {
                if (pos + n > vec->size())throw invalid_iterator();
                pos += n;
                return *this;
            }
            
            iterator &operator-=(const int &n) {
                if (pos < n)throw invalid_iterator();
                pos -= n;
                return *this;
            }
            
            iterator operator++(int) {
                if (pos + 1 > vec->size())throw invalid_iterator();
                iterator temp(*this);
                pos++;
                return temp;
            }
            
            iterator &operator++() {
                if (pos + 1 > vec->size())throw invalid_iterator();
                pos++;
                return *this;
            }
            
            iterator operator--(int) {
                if (pos < 1)throw invalid_iterator();
                iterator temp(*this);
                pos--;
                return temp;
            }
            
            iterator &operator--() {
                if (pos < 1)throw invalid_iterator();
                pos--;
                return *this;
            }
            
            T &operator*() const {
                return *(vec->store[pos]);
            }
            
            bool operator==(const iterator &rhs) const {
                if (vec == rhs.vec && pos == rhs.pos)return true;
                else return false;
            }
            
            bool operator==(const const_iterator &rhs) const {
                if (vec == rhs.vec && pos == rhs.pos)return true;
                else return false;
            }
            
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };
        
        class const_iterator {
            friend class vector;
        
        private:
            const vector<T> *vec;
            int pos;
        public:
            const_iterator(const vector<T> *v, int p) : vec(v), pos(p) {}
            
            const_iterator(const const_iterator &other) : vec(other.vec), pos(other.pos) {}
            
            const_iterator operator+(const int &n) const {
                if (pos + n > vec->size())throw invalid_iterator();
                iterator temp(*this);
                temp.pos += n;
                return temp;
            }
            
            const_iterator operator-(const int &n) const {
                if (pos < n)throw invalid_iterator();
                iterator temp(*this);
                temp.pos -= n;
                return temp;
            }
            
            int operator-(const iterator &rhs) const {
                if (vec != rhs.vec)throw invalid_iterator();
                return pos - rhs.pos;
            }
            
            const_iterator &operator+=(const int &n) {
                if (pos + n > vec->size())throw invalid_iterator();
                pos += n;
                return *this;
            }
            
            const_iterator &operator-=(const int &n) {
                if (pos < n)throw invalid_iterator();
                pos -= n;
                return *this;
            }
            
            const_iterator operator++(int) {
                if (pos + 1 > vec->size())throw invalid_iterator();
                iterator temp(*this);
                pos++;
                return temp;
            }
            
            const_iterator &operator++() {
                if (pos + 1 > vec->size())throw invalid_iterator();
                pos++;
                return *this;
            }
            
            const_iterator operator--(int) {
                if (pos < 1)throw invalid_iterator();
                iterator temp(*this);
                pos--;
                return temp;
            }
            
            const_iterator &operator--() {
                if (pos < 1)throw invalid_iterator();
                pos--;
                return *this;
            }
            
            T &operator*() const {
                return *(vec->store[pos]);
            }
            
            bool operator==(const iterator &rhs) const {
                if (vec == rhs.vec && pos == rhs.pos)return true;
                else return false;
            }
            
            bool operator==(const const_iterator &rhs) const {
                if (vec == rhs.vec && pos == rhs.pos)return true;
                else return false;
            }
            
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };
        
        vector() : length(INIT_LEN), tail(0) {
            store = new T *[length];
        }
        
        vector(int capacity) : length(capacity), tail(0) {
            store = new T *[length];
        }
        
        vector(const vector &other) : length(other.length), tail(other.tail) {
            store = new T *[length];
            for (int i = 0; i < tail; i++)store[i] = new T(*(other.store[i]));
        }
        
        ~vector() {
            for (int i = 0; i < tail; i++)delete store[i];
            delete[]store;
        }
        
        vector &operator=(const vector &other) {
            if (this == &other)return *this;
            for (int i = 0; i < tail; i++)delete store[i];
            delete[]store;
            length = other.length;
            tail = other.tail;
            store = new T *[length];
            for (int i = 0; i < tail; i++)store[i] = new T(*(other.store[i]));
            return *this;
        }
        
        T &at(const size_t &pos) {
            if (pos < 0 || pos >= length)throw index_out_of_bound();
            return *(store[pos]);
        }
        
        const T &at(const size_t &pos) const {
            if (pos < 0 || pos >= length)throw index_out_of_bound();
            return *(store[pos]);
        }
        
        T &operator[](const size_t &pos) {
            if (pos < 0 || pos >= tail)throw index_out_of_bound();
            return *(store[pos]);
        }
        
        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= tail)throw index_out_of_bound();
            return *(store[pos]);
        }
        
        const T &front() const {
            if (tail == 0)throw container_is_empty();
            return *(store[0]);
        }
        
        const T &back() const {
            if (tail == 0)throw container_is_empty();
            return *(store[tail - 1]);
        }
        
        iterator begin() {
            return iterator(this, 0);
        }
        
        const_iterator cbegin() const {
            return const_iterator(this, 0);
        }
        
        iterator end() {
            return iterator(this, tail);
        }
        
        const_iterator cend() const {
            return const_iterator(this, tail);
        }
        
        bool empty() const {
            if (tail == 0)return true;
            else return false;
        }
        
        int size() const {
            return tail;
        }
        
        void clear() {
            for (int i = 0; i < tail; i++)delete store[i];
            tail = 0;
        }
        
        iterator insert(iterator pos, const T &value) {
            if (tail == length)double_space();
            for (int i = tail - 1; i >= pos.pos; i--)store[i + 1] = store[i];
            tail++;
            *(store + pos.pos) = new T(value);
            pos++;
            return pos;
        }
        
        iterator insert(const size_t &ind, const T &value) {
            if (ind < 0 || ind > tail)throw index_out_of_bound();
            if (tail == length)double_space();
            for (int i = tail - 1; i >= ind; i--)store[i + 1] = store[i];
            tail++;
            *(store + ind) = new T(value);
            return iterator(this, ind + 1);
        }
        
        iterator erase(iterator pos) {
            delete (*(store + pos.pos));
            for (int i = pos.pos; i < tail - 1; i++)store[i] = store[i + 1];
            tail--;
            if ((tail < (length / 2)))halve_space();
            return pos;
        }
        
        iterator erase(const size_t &ind) {
            delete (*(store + ind));
            for (int i = ind; i < tail - 1; i++)store[i] = store[i + 1];
            tail--;
            if ((tail < (length / 2)))halve_space();
            return iterator(this, ind);
        }
        
        void push_back(const T &value) {
            if (tail == length)double_space();
            *(store + tail) = new T(value);
            tail++;
        }
        
        void pop_back() {
            if (tail == 0)throw container_is_empty();
            tail--;
            delete (*(store + tail));
            if (tail < (length / 2))halve_space();
        }
    };
}

#endif //TICKETSYSTEM_AUTOMATA_VECTOR_H
