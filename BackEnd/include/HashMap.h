//
// Created by Rainy Memory on 2021/3/17.
//

#ifndef TICKETSYSTEM_AUTOMATA_HASHMAP_H
#define TICKETSYSTEM_AUTOMATA_HASHMAP_H

#include <functional>
#include "RTL/algorithm.h"

namespace RainyMemory {
    template<class Key, class Value, class Hash = std::hash<Key>>
    class HashMap {
        /*
         * class HashMap
         * --------------------------------------------------------
         * A class implements o(1) find, insert and delete map like
         * std::unordered_map.
         * This class support dynamic capacity.
         *
         */
    private:
        static const int PrimeNum = 25;
        
        const int PrimeList[PrimeNum] = {
                53, 97, 193, 389, 769,
                1543, 3079, 6151, 12289, 24593,
                49157, 98317, 196613, 393241, 786433,
                1572869, 3145739, 6291469, 12582917, 25165843,
                50331653, 100663319, 201326611, 402653189, 805306457
        };
        
        inline int nextPrime(int n) {
            const int *first = PrimeList;
            const int *last = PrimeList + PrimeNum;
            const int *pos = RainyMemory::lower_bound(first, last, n);
            return pos == last ? *(last - 1) : *pos;
        }
        
        class LinkedList {
        public:
            class Node {
            public:
                Key *key;
                Value *value;
                Node *next;
                
                Node(const Key &k, const Value &v, Node *n) : key(new Key(k)), value(new Value(v)), next(n) {}
                
                ~Node() {
                    delete key;
                    delete value;
                }
            };
            
            Node *head = nullptr;
            int listSize = 0;
            
            LinkedList() = default;
            
            ~LinkedList() {
                Node *temp = head;
                while (head != nullptr) {
                    head = head->next;
                    delete temp;
                    temp = head;
                }
            }
            
            Node *find(const Key &o) {
                Node *now = head;
                while (now != nullptr) {
                    if (*now->key == o)return now;
                    now = now->next;
                }
                return nullptr;
            }
            
            void insert(const Key &k, const Value &v) {
                head = new Node(k, v, head);
                listSize++;
            }
            
            void erase(const Key &k) {
                if (*head->key == k) {
                    Node *temp = head;
                    head = head->next;
                    delete temp;
                }
                else {
                    Node *now = head;
                    while (now->next != nullptr) {
                        if (*now->next->key == k) {
                            Node *temp = now->next;
                            now->next = now->next->next;
                            delete temp;
                            break;
                        }
                        now = now->next;
                    }
                }
                listSize--;
            }
            
            bool empty() const {
                return listSize == 0;
            }
            
            void addNode(Node *n) {
                n->next = head;
                head = n;
            }
        };
        
        using node_t = typename LinkedList::Node;
        
        int capacity = 0;
        int number = 0;
        LinkedList *buckets;
        Hash hash;
        
        inline int calculateIndex(const Key &k) const {
            int index = hash(k) % capacity;
            if (index < 0)index += capacity;
            return index;
        }
        
        void resize() {
            int n = nextPrime(capacity);
            if (n <= capacity)return;
            LinkedList *temp = new LinkedList[n];
            for (int i = 0; i < capacity; i++) {
                node_t *p = buckets[i].head;
                while (p != nullptr) {
                    int index = (hash(*p->key) % n + n) % n;
                    buckets[i].head = p->next;
                    temp[index].addNode(p);
                    p = buckets[i].head;
                }
            }
            capacity = n;
            delete[] buckets;
            buckets = temp;
        }
    
    public:
        HashMap() : capacity(PrimeList[0]) {
            buckets = new LinkedList[capacity];
        }
        
        ~HashMap() {
            delete[] buckets;
        }
        
        void clear() {
            delete[] buckets;
            buckets = new LinkedList[capacity];
            number = 0;
        }
        
        bool containsKey(const Key &k) const {
            int index = calculateIndex(k);
            return !(buckets[index].empty() || buckets[index].find(k) == nullptr);
        }
        
        Value &operator[](const Key &k) {
            int index = calculateIndex(k);
            if (containsKey(k))return *buckets[index].find(k)->value;
            else {
                if (number + 1 > capacity)resize();
                buckets[index].insert(k, Value());
                number++;
                return *buckets[index].head->value;
            }
        }
        
        void erase(const Key &k) {
            int index = calculateIndex(k);
            buckets[index].erase(k);
            number--;
        }
    };
    
}

#endif //TICKETSYSTEM_AUTOMATA_HASHMAP_H
