//
// Created by Rainy Memory on 2021/1/10.
//

#include "UnrolledLinkedList.h"

bool Element::operator<(const Element &o) const {
    return strcmp(key, o.key) < 0;
}

Element::Element(int _offset, const string &_key) : offset(_offset) {
    memset(key, 0, sizeof(key));
    strcpy(key, _key.c_str());
}

Element::Element(const Element &o) : offset(o.offset) {
    memset(key, 0, sizeof(key));
    strcpy(key, o.key);
}

Element &Element::operator=(const Element &o) {
    if (this == &o)return *this;
    offset = o.offset;
    memset(key, 0, sizeof(key));
    strcpy(key, o.key);
    return *this;
}

Block::Block() : next(-1), pre(-1), length(0) {}

int UnrolledLinkedList::nextBlock(int offset) {
    //the reason using fin.nextBlock: same fstream object cannot open if it was opened.
    fin_nextBlock.open(filename, ios::in | ios::binary);
    if (!fin_nextBlock)cerr << "[Error] File open failed in \"UnrolledLinkedList::nextBlock\"." << endl;
    fin_nextBlock.seekg(offset);
    int next;
    fin_nextBlock.read(reinterpret_cast<char *>(&next), sizeof(int));//"int next" is at the begin of class Block.
    fin_nextBlock.close();
    return next;
}

void UnrolledLinkedList::mergeBlock(int offset1, int offset2) {
    //presume offset1 < offset2, which means block 1 is before block 2
    //delete block 2
    fin.open(filename, ios::in | ios::binary);
    fout.open(filename, ios::in | ios::out | ios::binary);
    if ((!fin) | (!fout))cerr << "[Error] File open failed in \"UnrolledLinkedList::mergeBlock\"." << endl;
    
    //read Block1 & Block2
    Block tempBlock1, tempBlock2;
    fin.seekg(offset1);
    fin.read(reinterpret_cast<char *>(&tempBlock1), sizeof(Block));
    fin.seekg(offset2);
    fin.read(reinterpret_cast<char *>(&tempBlock2), sizeof(Block));
    //o1->next=o2->next;
    tempBlock1.next = tempBlock2.next;
    //o2->next->pre=o1;
    if (tempBlock2.next >= 0) {
        fout.seekp(tempBlock2.next + sizeof(int));
        fout.write(reinterpret_cast<const char *>(&offset1), sizeof(int));
    }
    for (int i = 0; i < tempBlock2.length; i++)tempBlock1.array[i + tempBlock1.length] = tempBlock2.array[i];
    tempBlock1.length += tempBlock2.length;
    fout.seekp(offset1);
    fout.write(reinterpret_cast<const char *>(&tempBlock1), sizeof(Block));
    
    fin.close();
    fout.close();
}

void UnrolledLinkedList::splitBlock(int offset) {
    fin.open(filename, ios::in | ios::binary);
    fout.open(filename, ios::in | ios::out | ios::binary);
    if ((!fin) | (!fout))cerr << "[Error] File open failed in \"UnrolledLinkedList::splitBlock\"." << endl;
    
    fin.seekg(0, ios::end);
    int temp = fin.tellg();
    Block thisBlock, tempBlock;
    fin.seekg(offset);
    fin.read(reinterpret_cast<char *>(&thisBlock), sizeof(Block));
    
    //temp->pre=this
    tempBlock.pre = offset;
    //temp->next=this->next
    tempBlock.next = thisBlock.next;
    //this->next->pre=temp
    if (thisBlock.next >= 0) {
        fout.seekp(thisBlock.next + sizeof(int));
        fout.write(reinterpret_cast<const char *>(&temp), sizeof(int));
    }
    //this->next=temp
    thisBlock.next = temp;
    
    for (int i = SPLIT_REMAIN; i < BLOCK_SIZE; i++) tempBlock.array[i - SPLIT_REMAIN] = thisBlock.array[i];
    thisBlock.length = SPLIT_REMAIN;
    tempBlock.length = BLOCK_SIZE - SPLIT_REMAIN;
    
    fout.seekp(temp);
    fout.write(reinterpret_cast<const char *>(&tempBlock), sizeof(Block));
    fout.seekp(offset);
    fout.write(reinterpret_cast<const char *>(&thisBlock), sizeof(Block));
    
    fin.close();
    fout.close();
}

void UnrolledLinkedList::findElement(const string &key, vector<int> &result) {
    fin.open(filename, ios::in | ios::binary);
    if (!fin)cerr << "[Error] File open failed in \"UnrolledLinkedList::findElement\"." << endl;
    
    fin.seekg(0, ios::end);
    if (fin.tellg() == 0) {
        Block temp;
        fout.open(filename, ios::in | ios::out | ios::binary);
        fout.write(reinterpret_cast<const char *>(&temp), sizeof(Block));
        fout.close();
    }
    
    char _key[MAX_KEY_LENGTH] = {0};
    for (int i = 0; i < key.length(); i++)_key[i] = key[i];
    
    fin.seekg(0);
    int cur = fin.tellg();
    int next = nextBlock(cur);
    Element nextElement;
    while (next != -1) {
        fin.seekg(next + 3 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));
        if (strcmp(_key, nextElement.key) <= 0) break;
        cur = next;
        next = nextBlock(next);
    }
    
    bool flag = true;
    while (flag && (cur >= 0)) {
        flag = false;
        Block tempBlock;
        fin.seekg(cur);
        fin.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
        Element tempElement(-1, key);
        int len = tempBlock.length;
        int pos = lower_bound(tempBlock.array, tempBlock.array + len, tempElement) - tempBlock.array;
        for (int i = pos; i < len; i++) {
            if (strcmp(tempBlock.array[i].key, _key) < 0) break;
            if (strcmp(tempBlock.array[i].key, _key) == 0) {
                result.push_back(tempBlock.array[i].offset);
                if (i == len - 1) {
                    flag = true;
                    cur = nextBlock(cur);
                }
            }
        }
        if (pos == len) {
            flag = true;
            cur = nextBlock(cur);
        }
    }
    
    fin.close();
}

void UnrolledLinkedList::addElement(const Element &o) {
    fin.open(filename, ios::in | ios::binary);
    fout.open(filename, ios::in | ios::out | ios::binary);
    if ((!fin) | (!fout))cerr << "[Error] File open failed in \"UnrolledLinkedList::addElement\"." << endl;
    
    fin.seekg(0, ios::end);
    if (fin.tellg() == 0) {
        Block temp;
        fout.write(reinterpret_cast<const char *>(&temp), sizeof(Block));
    }
    
    fin.seekg(0);
    int cur = fin.tellg();
    int next = nextBlock(cur);
    Element nextElement;
    while (next != -1) {
        fin.seekg(next + 3 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));
        if (o < nextElement)break;
        cur = next;
        next = nextBlock(next);
    }
    
    Block tempBlock;
    fin.seekg(cur);
    fin.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
    int len = tempBlock.length;
    int pos;
    bool flag = true;
    for (int i = 0; i < len; i++) {
        if (o < tempBlock.array[i]) {
            pos = i;
            flag = false;
            break;
        }
    }
    if (flag)pos = len;
    for (int i = len - 1; i >= pos; i--)tempBlock.array[i + 1] = tempBlock.array[i];
    tempBlock.array[pos] = o;
    tempBlock.length = ++len;
    fout.seekp(cur);
    fout.write(reinterpret_cast<const char *>(&tempBlock), sizeof(Block));
    
    fin.close();
    fout.close();
    
    if (len == BLOCK_SIZE)splitBlock(cur);
}

void UnrolledLinkedList::deleteElement(const Element &o) {
    fin.open(filename, ios::in | ios::binary);
    fout.open(filename, ios::in | ios::out | ios::binary);
    if ((!fin) | (!fout))cerr << "[Error] File open failed in \"UnrolledLinkedList::deleteElement\"." << endl;
    
    fin.seekg(0, ios::end);
    if (fin.tellg() == 0) {
        Block temp;
        fout.write(reinterpret_cast<const char *>(&temp), sizeof(Block));
    }
    
    fin.seekg(0);
    int cur = fin.tellg();
    int next = nextBlock(cur);
    Element nextElement;
    while (next != -1) {
        fin.seekg(next + 3 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));
        if (strcmp(o.key, nextElement.key) <= 0)break;
        cur = next;
        next = nextBlock(next);
    }
    
    int pos = -1;
    int len;
    Block tempBlock;
    while (pos < 0 && (cur >= 0)) {
        fin.seekg(cur);
        fin.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
        Element tempElement(o);
        
        len = tempBlock.length;
        int equal_key_pos = lower_bound(tempBlock.array, tempBlock.array + len, tempElement) - tempBlock.array;
        
        for (int i = equal_key_pos; i < len; i++) {
            if (strcmp(tempBlock.array[i].key, o.key) == 0) {
                if (tempBlock.array[i].offset == o.offset) {
                    pos = i;
                    break;
                }
            }
        }
        if (pos < 0)cur = nextBlock(cur);
    }
    
    for (int i = pos; i < len; i++) tempBlock.array[i] = tempBlock.array[i + 1];
    tempBlock.length = --len;
    fout.seekp(cur);
    fout.write(reinterpret_cast<const char *>(&tempBlock), sizeof(Block));
    
    next = nextBlock(cur);
    int nextLen;
    if (next >= 0) {
        fin.seekg(next + 2 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextLen), sizeof(int));
    }
    
    fin.close();
    fout.close();
    
    if (next >= 0 && ((len + nextLen) <= MERGE_THRESHOLD))mergeBlock(cur, next);
}

#ifdef debug

void UnrolledLinkedList::printList() {
    fin.open(filename, ios::in | ios::binary);
    fout.open(filename, ios::in | ios::out | ios::binary);
    if ((!fin) | (!fout))cerr << "[Error] File open failed in \"UnrolledLinkedList::printList\"." << endl;
    
    cout << "[debug] UnrolledLinkedList----------\\/" << endl;
    fin.seekg(0);
    int cur = fin.tellg();
    int next = nextBlock(cur);
    Block tempBlock;
    int cnt = 0, tot_len = 0;
    while (cur >= 0) {
        fin.seekg(cur);
        fin.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
        cout << "[debug] Block   " << cnt++ << ":" << endl;
        cout << "[debug] next:   " << tempBlock.next << endl;
        cout << "[debug] cur:    " << cur << endl;
        cout << "[debug] pre:    " << tempBlock.pre << endl;
        cout << "[debug] length: " << tempBlock.length << endl;
        tot_len += tempBlock.length;
        for (int i = 0; i < tempBlock.length; i++) {
            cout << "[debug] Element " << i << ":   offset = " << tempBlock.array[i].offset << "\t\t\tkey = " << tempBlock.array[i].key << endl;
        }
        cout << endl;
        cur = next;
        next = nextBlock(cur);
    }
    cout << "[debug] totalLength: " << tot_len << endl;
    cout << "[debug] UnrolledLinkedList----------/\\" << endl;
    cout << endl;
    
    fin.close();
    fout.close();
}

#endif













