//
// Created by Rainy Memory on 2021/3/18.
//

#ifndef BPLUSTREE_LRUCACHEMEMORYPOOL_H
#define BPLUSTREE_LRUCACHEMEMORYPOOL_H

#include <string>
#include <fstream>
#include "HashMap.h"

using std::string;
using std::fstream;
using std::ios;

namespace RainyMemory {
    template<class T, class extraMessage>
    class LRUCacheMemoryPool {
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
    
    private:
        int writePoint;
        const string filename;
        fstream fin, fout;
        
        HashMap<int, typename DoublyLinkedList::Node *> hashmap;
        DoublyLinkedList cache;
        
        bool existInCache(int key) {
            return hashmap.containsKey(key);
        }
        
        void discardLRU() {
            typename DoublyLinkedList::Node *target = cache.pop_back();
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
            auto newNode = new typename DoublyLinkedList::Node(key, o);
            if (cache.full())discardLRU();
            cache.push_front(newNode);
            hashmap[key] = newNode;
        }
        
        int writeInFile(const T &o) {
            int offset;
            fin.open(filename, ios::in | ios::binary);
            fout.open(filename, ios::in | ios::out | ios::binary);
            if (writePoint < 0) {
                fout.seekp(0, ios::end);
                offset = fout.tellp();
            }
            else {
                offset = writePoint;
                fin.seekg(writePoint);
                fin.read(reinterpret_cast<char *>(&writePoint), sizeof(int));
                fout.seekp(sizeof(extraMessage));
                fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
                fout.seekp(offset);
            }
            fout.write(reinterpret_cast<const char *>(&o), sizeof(T));
            fin.close();
            fout.close();
            return offset;
        }
        
        T readInFile(int offset) {
            fin.open(filename, ios::in | ios::binary);
            T temp;
            fin.seekg(offset);
            fin.read(reinterpret_cast<char *>(&temp), sizeof(T));
            fin.close();
            return temp;
        }
        
        void updateInFile(int offset, const T &o) {
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(offset);
            fout.write(reinterpret_cast<const char *>(&o), sizeof(T));
            fout.close();
        }
        
        void eraseInFile(int offset) {
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(offset);
            fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
            writePoint = offset;
            fout.seekp(sizeof(extraMessage));
            fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
            fout.close();
        }
    
    public:
        explicit LRUCacheMemoryPool(const string &_filename, int _capacity = 100) : filename(_filename), cache(_capacity), hashmap(_capacity) {
            fin.open(filename, ios::in);
            if (fin.fail()) {
                fin.clear();
                fin.close();
                fout.open(filename, ios::out | ios::binary);
                fout.close();
                writePoint = -1;
                extraMessage temp;
                fout.open(filename, ios::in | ios::out | ios::binary);
                fout.seekp(0);
                fout.write(reinterpret_cast<const char *>(&temp), sizeof(extraMessage));
                fout.seekp(sizeof(extraMessage));
                fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
                fout.close();
            }
            else {
                fin.seekg(sizeof(extraMessage));
                fin.read(reinterpret_cast<char *>(&writePoint), sizeof(int));
                fin.close();
            }
        }
        
        ~LRUCacheMemoryPool() {
            typename DoublyLinkedList::Node *now = cache.head->next;
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
            hashmap[offset]->dirtyBit = true;
            putInCache(offset, o);
        }
        
        void erase(int offset) {
            if (existInCache(offset))eraseInCache(offset);
            eraseInFile(offset);
        }
        
        extraMessage readExtraMessage() {
            fin.open(filename, ios::in | ios::binary);
            extraMessage temp;
            fin.seekg(0);
            fin.read(reinterpret_cast<char *>(&temp), sizeof(extraMessage));
            fin.close();
            return temp;
        }
        
        void updateExtraMessage(const extraMessage &o) {
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(0);
            fout.write(reinterpret_cast<const char *>(&o), sizeof(extraMessage));
            fout.close();
        }
        
        int tellWritePoint() {
            if (writePoint >= 0)return writePoint;
            else {
                fout.open(filename, ios::in | ios::out | ios::binary);
                fout.seekp(0, ios::end);
                int tempWritePoint = fout.tellp();
                fout.close();
                return tempWritePoint;
            }
        }
    };
}

#endif //BPLUSTREE_LRUCACHEMEMORYPOOL_H
