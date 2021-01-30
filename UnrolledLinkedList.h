//
// Created by Rainy Memory on 2021/1/10.
//

//finish

#ifndef BOOKSTORE_UNROLLEDLINKEDLIST_H
#define BOOKSTORE_UNROLLEDLINKEDLIST_H

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <algorithm>

#define BLOCK_SIZE 320
#define SPLIT_REMAIN 160
#define MAX_KEY_LENGTH 65
#define MERGE_THRESHOLD 288

using std::string;
using std::fstream;
using std::ios;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::lower_bound;

//#define debug

class Element {
public:
    int offset;
    char key[MAX_KEY_LENGTH];
    
    bool operator<(const Element &o) const;
    
    explicit Element(int _offset = -1, const string &_key = "");
    
    Element(const Element &o);
    
    Element &operator=(const Element &o);
};

class Block {
public:
    int next;
    int pre;
    int length;
    Element array[BLOCK_SIZE];
    
    Block();
};

class UnrolledLinkedList {
    //-1 <-Block_1-> <-Block_2-> ... <-Block_n-> -1
private:
    string filename;
    fstream fin, fout, fin_nextBlock;
    
    int nextBlock(int offset);
    
    void mergeBlock(int offset1, int offset2);//presume offset1 < offset2, which means block 1 is before block 2
    
    void splitBlock(int offset);

public:
    explicit UnrolledLinkedList(const string &_filename) : filename(_filename) {}
    
    void findElement(const string &key, vector<int> &result);
    
    void addElement(const Element &o);
    
    void deleteElement(const Element &o);

#ifdef debug
    void printList();
#endif
};


#endif //BOOKSTORE_UNROLLEDLINKEDLIST_H
