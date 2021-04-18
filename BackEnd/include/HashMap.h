//
// Created by Rainy Memory on 2021/3/17.
//

#ifndef TICKETSYSTEM_AUTOMATA_HASHMAP_H
#define TICKETSYSTEM_AUTOMATA_HASHMAP_H

#include <functional>

namespace RainyMemory {
    template<class Key, class Value, class Hash = std::hash<Key>>
    class HashMap {
    private:
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
        };
    
    private:
        enum Info {
            DEFAULT_CAPACITY = 127
        };
        
        int capacity = 0;
        LinkedList *buckets;
        Hash hash;
        
        inline int calculateIndex(const Key &k) const {
            int index = hash(k) % capacity;
            if (index < 0)index += capacity;
            return index;
        }
    
    public:
        explicit HashMap(int _capacity) : capacity(_capacity) {
            buckets = new LinkedList[capacity];
        }
        
        HashMap() : capacity(DEFAULT_CAPACITY) {
            buckets = new LinkedList[capacity];
        }
        
        ~HashMap() {
            delete[]buckets;
        }
        
        void clear() {
            delete[]buckets;
            buckets = new LinkedList[capacity];
        }
        
        bool containsKey(const Key &k) const {
            int index = calculateIndex(k);
            return !(buckets[index].empty() || buckets[index].find(k) == nullptr);
        }
        
        Value &operator[](const Key &k) {
            int index = calculateIndex(k);
            if (containsKey(k))return *buckets[index].find(k)->value;
            else {
                buckets[index].insert(k, Value());
                return *buckets[index].head->value;
            }
        }
        
        void erase(const Key &k) {
            int index = calculateIndex(k);
            buckets[index].erase(k);
        }
    };
    
}

#endif //TICKETSYSTEM_AUTOMATA_HASHMAP_H
