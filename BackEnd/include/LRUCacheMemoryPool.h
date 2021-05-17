//
// Created by Rainy Memory on 2021/3/18.
//

#ifndef TICKETSYSTEM_AUTOMATA_LRUCACHEMEMORYPOOL_H
#define TICKETSYSTEM_AUTOMATA_LRUCACHEMEMORYPOOL_H

#include <iostream>
#include <fstream>
#include "HashMap.h"

using std::string;
using std::fstream;
using std::ios;

namespace RainyMemory {
    template<class T, class extraMessage = int>
    class LRUCacheMemoryPool {
        /*
         * class LRUCacheMemoryPool
         * --------------------------------------------------------
         * A class implements memory pool which has built-in cache
         * strategy to storage and quick access data.
         * This class offer single type data's storage, update and
         * delete in file, and LRU Cache to accelerate accession.
         * ALso, this class support deleted data's space reclamation,
         * and an extra (not same type of stored data) message storage.
         *
         */
    private:
        class DoublyLinkedList {
        public:
            class Node {
            public:
                int key = -1;
                T *value = nullptr;
                Node *pre = nullptr;
                Node *next = nullptr;
                bool dirtyBit = false;
                
                Node() = default;
                
                Node(int k, const T &v, Node *p = nullptr, Node *n = nullptr) : key(k), value(new T(v)), pre(p), next(n) {}
                
                ~Node() {
                    delete value;
                }
            };
            
            Node *head = nullptr;
            Node *tail = nullptr;
            int listSize = 0;
            int capacity = 0;
            
            explicit DoublyLinkedList(int _capacity) : capacity(_capacity) {
                head = new Node(), tail = new Node();
                head->next = tail, tail->pre = head;
            }
            
            ~DoublyLinkedList() {
                Node *temp = head;
                while (head != nullptr) {
                    head = head->next;
                    delete temp;
                    temp = head;
                }
            }
            
            void clear() {
                listSize = 0;
                Node *temp = head;
                while (head != nullptr) {
                    head = head->next;
                    delete temp;
                    temp = head;
                }
                head = new Node(), tail = new Node();
                head->next = tail, tail->pre = head;
            }
            
            void push_front(Node *n) {
                head->next->pre = n;
                n->next = head->next;
                head->next = n;
                n->pre = head;
                listSize++;
            }
            
            void to_front(Node *n) {
                n->pre->next = n->next;
                n->next->pre = n->pre;
                listSize--;
                push_front(n);
            }
            
            Node *pop_back() {
                Node *target = tail->pre;
                target->pre->next = tail;
                tail->pre = tail->pre->pre;
                listSize--;
                return target;
            }
            
            void erase(Node *n) {
                n->pre->next = n->next;
                n->next->pre = n->pre;
                listSize--;
                delete n;
            }
            
            bool full() const {
                return listSize == capacity;
            }
        };
        
        using node_t = typename DoublyLinkedList::Node;
    
    private:
        int writePoint;
        const string filename;
        FILE *file;
        
        HashMap<int, node_t *> hashmap;
        DoublyLinkedList cache;
        
        bool existInCache(int key) {
            return hashmap.containsKey(key);
        }
        
        void discardLRU() {
            node_t *target = cache.pop_back();
            hashmap.erase(target->key);
            if (target->dirtyBit)updateInFile(target->key, *target->value);
            delete target;
        }
        
        void eraseInCache(int key) {
            cache.erase(hashmap[key]);
            hashmap.erase(key);
        }
        
        void putInCache(int key, const T &o) {
            if (existInCache(key)) {
                cache.to_front(hashmap[key]);
                *hashmap[key]->value = o;
                return;
            }
            auto newNode = new node_t(key, o);
            if (cache.full())discardLRU();
            cache.push_front(newNode);
            hashmap[key] = newNode;
        }
        
        int writeInFile(const T &o) {
            int offset;
            file = fopen(filename.c_str(), "rb+");
            if (writePoint < 0) {
                fseek(file, 0, SEEK_END);
                offset = ftell(file);
            }
            else {
                offset = writePoint;
                fseek(file, writePoint, SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fseek(file, sizeof(extraMessage), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
                fseek(file, offset, SEEK_SET);
            }
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);
            return offset;
        }
        
        T readInFile(int offset) {
            file = fopen(filename.c_str(), "rb");
            T temp;
            fseek(file, offset, SEEK_SET);
            fread(reinterpret_cast<char *>(&temp), sizeof(T), 1, file);
            fclose(file);
            return temp;
        }
        
        void updateInFile(int offset, const T &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);
        }
        
        void eraseInFile(int offset) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            writePoint = offset;
            fseek(file, sizeof(extraMessage), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);
        }
    
    public:
        explicit LRUCacheMemoryPool(const string &_filename, const extraMessage &ex = extraMessage {}, int _capacity = 100) : filename(_filename), cache(_capacity), hashmap() {
            file = fopen(filename.c_str(), "rb");
            if (file == NULL) {
                file = fopen(filename.c_str(), "wb+");
                fclose(file);
                writePoint = -1;
                extraMessage temp(ex);
                file = fopen(filename.c_str(), "rb+");
                fseek(file, 0, SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&temp), sizeof(extraMessage), 1, file);
                fseek(file, sizeof(extraMessage), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
                fclose(file);
            }
            else {
                fseek(file, sizeof(extraMessage), SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fclose(file);
            }
        }
        
        ~LRUCacheMemoryPool() {
            node_t *now = cache.head->next;
            while (now != cache.tail) {
                if (now->dirtyBit)updateInFile(now->key, *now->value);
                now = now->next;
            }
        }
        
        T read(int offset) {
            T temp = existInCache(offset) ? *hashmap[offset]->value : readInFile(offset);
            putInCache(offset, temp);
            return temp;
        }
        
        int write(const T &o) {
            int offset = writeInFile(o);
            putInCache(offset, o);
            return offset;
        }
        
        void update(const T &o, int offset) {
            putInCache(offset, o);
            hashmap[offset]->dirtyBit = true;
        }
        
        void erase(int offset) {
            if (existInCache(offset))eraseInCache(offset);
            eraseInFile(offset);
        }
        
        void clear(extraMessage ex = extraMessage {}) {
            hashmap.clear();
            cache.clear();
            file = fopen(filename.c_str(), "wb+");
            fclose(file);
            writePoint = -1;
            extraMessage temp(ex);
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&temp), sizeof(extraMessage), 1, file);
            fseek(file, sizeof(extraMessage), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);
        }
        
        extraMessage readExtraMessage() {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            extraMessage temp;
            fread(reinterpret_cast<char *>(&temp), sizeof(extraMessage), 1, file);
            fclose(file);
            return temp;
        }
        
        void updateExtraMessage(const extraMessage &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(extraMessage), 1, file);
            fclose(file);
        }
        
        int tellWritePoint() {
            if (writePoint >= 0)return writePoint;
            else {
                file = fopen(filename.c_str(), "rb+");
                fseek(file, 0, SEEK_END);
                int tempWritePoint = ftell(file);
                fclose(file);
                return tempWritePoint;
            }
        }
    };
}

#endif //TICKETSYSTEM_AUTOMATA_LRUCACHEMEMORYPOOL_H
