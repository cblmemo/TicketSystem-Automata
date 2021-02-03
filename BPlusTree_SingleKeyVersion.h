//
// Created by Rainy Memory on 2021/1/28.
//

#ifndef RAINYMEMORY_BPLUSTREE_H
#define RAINYMEMORY_BPLUSTREE_H

//THIS VERSION DOES NOT SUPPORT REPEATED KEY!!!

#include "MemoryPool.h"

#include <iostream>
#include <vector>
#include <algorithm>

using std::cerr;
using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::upper_bound;
using std::lower_bound;

using RainyMemory::MemoryPool;

#define debug

namespace RainyMemory {
    template<class key, class data, int M = 200, int L = 200, data failedSignal = -1>
    class BPlusTree {
    private:
        class leafNode;
        
        class internalNode;
        
        class basicInfo {
        public:
            int root = -1;
            int head = -1;
            int size = 0;
        };
        
        enum sizeInfo {
            MAX_RECORD_NUM = L + 1,
            MIN_RECORD_NUM = (L - 1) / 2,
            MAX_KEY_NUM = M + 1,
            MIN_KEY_NUM = (M - 1) / 2,
            MAX_CHILD_NUM = M + 2
        };
    
    private:
        MemoryPool<leafNode, basicInfo> *leafPool;
        MemoryPool<internalNode, basicInfo> *internalPool;
        basicInfo info;
    
    private:
        class leafNode {
        public:
            int offset = -1;
            int leftBrother = -1;
            int rightBrother = -1;
            int dataNumber = 0;
            key leafKey[MAX_RECORD_NUM];
            data leafData[MAX_RECORD_NUM];
        
        public:
            void addElement(BPlusTree *tree, const key &o1, const data &o2) {
                int pos = upper_bound(leafKey, leafKey + dataNumber, o1) - leafKey;
                for (int i = dataNumber - 1; i >= pos; i--) {
                    leafKey[i + 1] = leafKey[i];
                    leafData[i + 1] = leafData[i];
                }
                leafKey[pos] = o1;
                leafData[pos] = o2;
                dataNumber++;
                tree->leafPool->update(*this, offset);
            }
            
            bool deleteElement(BPlusTree *tree, const key &o) {
                int pos = lower_bound(leafKey, leafKey + dataNumber, o) - leafKey;
                if (pos == dataNumber)return false;
                else {
                    if (leafKey[pos] != o)return false;
                    else {
                        for (int j = pos; j < dataNumber - 1; j++) {
                            leafData[j] = leafData[j + 1];
                            leafKey[j] = leafKey[j + 1];
                        }
                        dataNumber--;
                        tree->leafPool->update(*this, offset);
                        return true;
                    }
                }
            }
            
            pair<int, key> splitNode(BPlusTree *tree) {
                leafNode tempNode;
                tempNode.leftBrother = offset;
                tempNode.rightBrother = rightBrother;
                tempNode.offset = tree->leafPool->tellWritePoint();
                if (rightBrother >= 0) {
                    leafNode tempRightNode = tree->leafPool->read(rightBrother);
                    tempRightNode.leftBrother = tempNode.offset;
                    tree->leafPool->update(tempRightNode, tempRightNode.offset);
                }
                rightBrother = tempNode.offset;
                for (int i = MIN_RECORD_NUM; i < MAX_RECORD_NUM; i++) {
                    tempNode.leafKey[i - MIN_RECORD_NUM] = leafKey[i];
                    tempNode.leafData[i - MIN_RECORD_NUM] = leafData[i];
                }
                tempNode.dataNumber = MAX_RECORD_NUM - MIN_RECORD_NUM;
                dataNumber = MIN_RECORD_NUM;
                tree->leafPool->write(tempNode);
                tree->leafPool->update(*this, offset);
                pair<int, key> temp;
                temp.first = tempNode.offset;
                temp.second = tempNode.leafKey[0];
                return temp;
            }
            
            void borrowLeft(BPlusTree *tree, leafNode &leftNode, internalNode &fatherNode, int index) {
                //transfer data
                for (int i = dataNumber - 1; i >= 0; i--) {
                    leafKey[i + 1] = leafKey[i];
                    leafData[i + 1] = leafData[i];
                }
                leafKey[0] = leftNode.leafKey[leftNode.dataNumber - 1];
                leafData[0] = leftNode.leafData[leftNode.dataNumber - 1];
                dataNumber++;
                leftNode.dataNumber--;
                
                //update fatherNode info
                fatherNode.nodeKey[index - 1] = leafKey[0];
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->leafPool->update(*this, offset);
                tree->leafPool->update(leftNode, leftNode.offset);
            }
            
            void borrowRight(BPlusTree *tree, leafNode &rightNode, internalNode &fatherNode, int index) {
                //transfer data
                leafKey[dataNumber] = rightNode.leafKey[0];
                leafData[dataNumber] = rightNode.leafData[0];
                for (int i = 0; i < rightNode.dataNumber - 1; i++) {
                    rightNode.leafKey[i] = rightNode.leafKey[i + 1];
                    rightNode.leafData[i] = rightNode.leafData[i + 1];
                }
                dataNumber++;
                rightNode.dataNumber--;
                
                //update fatherNode info
                fatherNode.nodeKey[index] = rightNode.leafKey[0];
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->leafPool->update(*this, offset);
                tree->leafPool->update(rightNode, rightNode.offset);
            }
            
            //keep left node and delete right node anyway
            //only right-most point can merge left
            void mergeLeft(BPlusTree *tree, leafNode &leftNode, internalNode &fatherNode) {
                //update left/right brother
                leftNode.rightBrother = rightBrother;
                if (rightBrother >= 0) {
                    leafNode tempRightNode = tree->leafPool->read(rightBrother);
                    tempRightNode.leftBrother = leftBrother;
                    tree->leafPool->update(tempRightNode, tempRightNode.offset);
                }
                
                //update fatherNode info
                fatherNode.keyNumber--;//erase last element
                
                //transfer data
                for (int i = 0; i < dataNumber; i++) {
                    leftNode.leafKey[i + leftNode.dataNumber] = leafKey[i];
                    leftNode.leafData[i + leftNode.dataNumber] = leafData[i];
                }
                leftNode.dataNumber += dataNumber;
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->leafPool->update(leftNode, leftNode.offset);
                tree->leafPool->erase(offset);
            }
            
            void mergeRight(BPlusTree *tree, leafNode &rightNode, internalNode &fatherNode, int index) {
                //update left/right brother
                rightBrother = rightNode.rightBrother;
                if (rightNode.rightBrother >= 0) {
                    leafNode tempRightRightNode = tree->leafPool->read(rightNode.rightBrother);
                    tempRightRightNode.leftBrother = offset;
                    tree->leafPool->update(tempRightRightNode, tempRightRightNode.offset);
                }
                
                //update fatherNode info
                //delete fatherNode.nodeKey[index] & fatherNode.childNode[index + 1]
                for (int i = index; i < fatherNode.keyNumber - 1; i++) {
                    fatherNode.nodeKey[i] = fatherNode.nodeKey[i + 1];
                    fatherNode.childNode[i + 1] = fatherNode.childNode[i + 2];
                }
                fatherNode.keyNumber--;
                
                //transfer data
                for (int i = 0; i < rightNode.dataNumber; i++) {
                    leafKey[i + dataNumber] = rightNode.leafKey[i];
                    leafData[i + dataNumber] = rightNode.leafData[i];
                }
                dataNumber += rightNode.dataNumber;
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->leafPool->update(*this, offset);
                tree->leafPool->erase(rightNode.offset);
            }
            
            //return father node need resize
            bool resize(BPlusTree *tree, internalNode &fatherNode, int index) {
                if (dataNumber < MIN_RECORD_NUM) {
                    if (index == 0 && rightBrother >= 0) {
                        //try borrow/merge right
                        leafNode rightNode = tree->leafPool->read(rightBrother);
                        if (rightNode.dataNumber > MIN_RECORD_NUM) {
                            borrowRight(tree, rightNode, fatherNode, index);
                            return false;
                        }
                        else {
                            mergeRight(tree, rightNode, fatherNode, index);
                            return true;
                        }
                    }
                    else if (index == fatherNode.keyNumber && leftBrother >= 0) {
                        //try borrow/merge left
                        leafNode leftNode = tree->leafPool->read(leftBrother);
                        if (leftNode.dataNumber > MIN_RECORD_NUM) {
                            borrowLeft(tree, leftNode, fatherNode, index);
                            return false;
                        }
                        else {
                            mergeLeft(tree, leftNode, fatherNode);
                            return true;
                        }
                    }
                    else if (leftBrother >= 0 && rightBrother >= 0) {
                        //try borrow/merge left/right
                        leafNode leftNode = tree->leafPool->read(leftBrother);
                        if (leftNode.dataNumber > MIN_RECORD_NUM) {
                            borrowLeft(tree, leftNode, fatherNode, index);
                            return false;
                        }
                        else {
                            leafNode rightNode = tree->leafPool->read(rightBrother);
                            if (rightNode.dataNumber > MIN_RECORD_NUM) {
                                borrowRight(tree, rightNode, fatherNode, index);
                                return false;
                            }
                            else {
                                mergeRight(tree, rightNode, fatherNode, index);
                                return true;
                            }
                        }
                    }
                    else return false;
                }
                else return false;
            }

#ifdef debug
            
            void show() const {
                cout << "[leafNode]" << endl;
                cout << "offset: " << offset << endl;
                cout << "leftBrother: " << leftBrother << endl;
                cout << "rightBrother: " << rightBrother << endl;
                cout << "dataNumber: " << dataNumber << endl;
                cout << "leafKey & leafData:" << endl;
                for (int i = 0; i < dataNumber; i++) {
                    cout << "leafKey: " << leafKey[i] << "\t\t\t\t\t\t\t\t\t\t\t" << "leafData: " << leafData[i] << endl;
                }
                cout << endl;
            }

#endif
        };
        
        class internalNode {
        public:
            int offset = -1;
            int leftBrother = -1;
            int rightBrother = -1;
            bool childNodeIsLeaf = false;
            int keyNumber = 0;
            key nodeKey[MAX_KEY_NUM];
            int childNode[MAX_CHILD_NUM] = {0};
        
        public:
            void addElement(BPlusTree *tree, const pair<int, key> &o, int pos) {
                for (int i = keyNumber - 1; i >= pos; i--) {
                    childNode[i + 2] = childNode[i + 1];
                    nodeKey[i + 1] = nodeKey[i];
                }
                childNode[pos + 1] = o.first;
                nodeKey[pos] = o.second;
                keyNumber++;
                tree->internalPool->update(*this, offset);
            }
            
            void splitRoot(BPlusTree *tree) {
                //oldRoot(*this) -> oldRoot + tempNode
                internalNode newRoot, tempNode;
                newRoot.offset = tree->internalPool->write(newRoot);
                rightBrother = tree->internalPool->tellWritePoint();
                tempNode.offset = rightBrother;
                tempNode.leftBrother = offset;
                tempNode.childNodeIsLeaf = childNodeIsLeaf;
                for (int i = MIN_KEY_NUM + 1; i < keyNumber; i++) {
                    tempNode.nodeKey[i - MIN_KEY_NUM - 1] = nodeKey[i];
                    tempNode.childNode[i - MIN_KEY_NUM - 1] = childNode[i];
                }
                tempNode.childNode[keyNumber - MIN_KEY_NUM - 1] = childNode[keyNumber];
                tempNode.keyNumber = keyNumber - MIN_KEY_NUM - 1;
                keyNumber = MIN_KEY_NUM;
                newRoot.keyNumber = 1;
                newRoot.nodeKey[0] = nodeKey[MIN_KEY_NUM];
                newRoot.childNode[0] = offset;
                newRoot.childNode[1] = tempNode.offset;
                tree->internalPool->write(tempNode);
                tree->internalPool->update(newRoot, newRoot.offset);
                tree->internalPool->update(*this, offset);
                tree->info.root = newRoot.offset;
            }
            
            pair<int, key> splitNode(BPlusTree *tree) {
                internalNode tempNode;
                tempNode.leftBrother = offset;
                tempNode.rightBrother = rightBrother;
                tempNode.offset = tree->internalPool->tellWritePoint();
                if (rightBrother >= 0) {
                    internalNode tempRightNode = tree->internalPool->read(rightBrother);
                    tempRightNode.leftBrother = tempNode.offset;
                    tree->internalPool->update(tempRightNode, tempRightNode.offset);
                }
                rightBrother = tempNode.offset;
                tempNode.childNodeIsLeaf = childNodeIsLeaf;
                //delete No.MIN_KEY_NUM key, and return it to upper layer
                //in the following example, key 4 will be returned
                //key:    1 2 3 4 5 6 7 8
                //child: 1 2 3 4 5 6 7 8 9
                //               \/
                //key:    1 2 3   4   5 6 7 8
                //child: 1 2 3 4     5 6 7 8 9
                for (int i = MIN_KEY_NUM + 1; i < keyNumber; i++) {
                    tempNode.nodeKey[i - MIN_KEY_NUM - 1] = nodeKey[i];
                    tempNode.childNode[i - MIN_KEY_NUM - 1] = childNode[i];
                }
                tempNode.childNode[keyNumber - MIN_KEY_NUM - 1] = childNode[keyNumber];
                
                tempNode.keyNumber = keyNumber - MIN_KEY_NUM - 1;
                keyNumber = MIN_KEY_NUM;
                tree->internalPool->write(tempNode);
                tree->internalPool->update(*this, offset);
                pair<int, key> temp;
                temp.first = tempNode.offset;
                temp.second = nodeKey[MIN_KEY_NUM];
                return temp;
            }
            
            void borrowLeft(BPlusTree *tree, internalNode &leftNode, internalNode &fatherNode, int index) {
                //transfer data
                childNode[keyNumber + 1] = childNode[keyNumber];
                for (int i = keyNumber - 1; i >= 0; i--) {
                    childNode[i + 1] = childNode[i];
                    nodeKey[i + 1] = nodeKey[i];
                }
                childNode[0] = leftNode.childNode[leftNode.keyNumber];
                nodeKey[0] = fatherNode.nodeKey[index - 1];
                fatherNode.nodeKey[index - 1] = leftNode.nodeKey[leftNode.keyNumber - 1];
                keyNumber++;
                leftNode.keyNumber--;
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->internalPool->update(*this, offset);
                tree->internalPool->update(leftNode, leftNode.offset);
            }
            
            void borrowRight(BPlusTree *tree, internalNode &rightNode, internalNode &fatherNode, int index) {
                //transfer data
                nodeKey[keyNumber] = fatherNode.nodeKey[index];
                fatherNode.nodeKey[index] = rightNode.nodeKey[0];
                childNode[keyNumber + 1] = rightNode.childNode[0];
                for (int i = 0; i < rightNode.keyNumber - 1; i++) {
                    rightNode.childNode[i] = rightNode.childNode[i + 1];
                    rightNode.nodeKey[i] = rightNode.nodeKey[i + 1];
                }
                rightNode.childNode[rightNode.keyNumber - 1] = rightNode.childNode[rightNode.keyNumber];
                keyNumber++;
                rightNode.keyNumber--;
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->internalPool->update(*this, offset);
                tree->internalPool->update(rightNode, rightNode.offset);
            }
            
            //keep left node and delete right node anyway
            //only right-most point can merge left
            void mergeLeft(BPlusTree *tree, internalNode &leftNode, internalNode &fatherNode) {
                //update left/right brother
                leftNode.rightBrother = rightBrother;
                if (rightBrother >= 0) {
                    internalNode tempRightNode = tree->internalPool->read(rightBrother);
                    tempRightNode.leftBrother = leftBrother;
                    tree->internalPool->update(tempRightNode, tempRightNode.offset);
                }
                
                //transfer data
                //in the following example, key 4 is fatherNode.nodeKey[keyNumber - 1]
                //key:    1 2 3   4   5 6 7 8
                //child: 1 2 3 4     5 6 7 8 9
                //                \/
                //key:    1 2 3 4 5 6 7 8
                //child: 1 2 3 4 5 6 7 8 9
                leftNode.nodeKey[leftNode.keyNumber] = fatherNode.nodeKey[keyNumber - 1];
                for (int i = 0; i < keyNumber; i++) {
                    leftNode.nodeKey[i + leftNode.keyNumber + 1] = nodeKey[i];
                    leftNode.childNode[i + leftNode.keyNumber + 1] = childNode[i];
                }
                leftNode.childNode[leftNode.keyNumber + keyNumber + 1] = childNode[keyNumber];
                leftNode.keyNumber += keyNumber + 1;
                
                //update fatherNode info
                //delete last element
                fatherNode.keyNumber--;
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->internalPool->update(leftNode, leftNode.offset);
                tree->internalPool->erase(offset);
            }
            
            void mergeRight(BPlusTree *tree, internalNode &rightNode, internalNode &fatherNode, int index) {
                //update left/right brother
                rightBrother = rightNode.rightBrother;
                if (rightNode.rightBrother >= 0) {
                    internalNode tempRightRightNode = tree->internalPool->read(rightNode.rightBrother);
                    tempRightRightNode.leftBrother = offset;
                    tree->internalPool->update(tempRightRightNode, tempRightRightNode.offset);
                }
                
                //transfer data
                //in the following example, key 4 is fatherNode.nodeKey[index]
                //key:    1 2 3   4   5 6 7 8
                //child: 1 2 3 4     5 6 7 8 9
                //                \/
                //key:    1 2 3 4 5 6 7 8
                //child: 1 2 3 4 5 6 7 8 9
                nodeKey[keyNumber] = fatherNode.nodeKey[index];
                for (int i = 0; i < rightNode.keyNumber; i++) {
                    nodeKey[i + keyNumber + 1] = rightNode.nodeKey[i];
                    childNode[i + keyNumber + 1] = rightNode.childNode[i];
                }
                childNode[keyNumber + rightNode.keyNumber + 1] = rightNode.childNode[rightNode.keyNumber];
                keyNumber += rightNode.keyNumber + 1;
                
                //update fatherNode info
                //delete fatherNode.nodeKey[index] & fatherNode.childNode[index + 1]
                for (int i = index; i < fatherNode.keyNumber - 1; i++) {
                    fatherNode.nodeKey[i] = fatherNode.nodeKey[i + 1];
                    fatherNode.childNode[i + 1] = fatherNode.childNode[i + 2];
                }
                fatherNode.keyNumber--;
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->internalPool->update(*this, offset);
                tree->internalPool->erase(rightNode.offset);
            }
            
            void resizeRoot(BPlusTree *tree) {
                if ((!childNodeIsLeaf) && (keyNumber == 0)) {
                    tree->info.root = childNode[0];
                    tree->internalPool->erase(offset);
                    internalNode tempNode = tree->internalPool->read(childNode[0]);
                    tree->internalPool->update(tempNode, tempNode.offset);
                }
            }
            
            //return father node need resize
            bool resize(BPlusTree *tree, internalNode &fatherNode, int index) {
                if (keyNumber < MIN_KEY_NUM) {
                    if (index == 0 && rightBrother >= 0) {
                        //try borrow/merge right
                        internalNode rightNode = tree->internalPool->read(rightBrother);
                        if (rightNode.keyNumber > MIN_KEY_NUM) {
                            borrowRight(tree, rightNode, fatherNode, index);
                            return false;
                        }
                        else {
                            mergeRight(tree, rightNode, fatherNode, index);
                            return true;
                        }
                    }
                    else if (index == fatherNode.keyNumber && leftBrother >= 0) {
                        //try borrow/merge left
                        internalNode leftNode = tree->internalPool->read(leftBrother);
                        if (leftNode.keyNumber > MIN_KEY_NUM) {
                            borrowLeft(tree, leftNode, fatherNode, index);
                            return false;
                        }
                        else {
                            mergeLeft(tree, leftNode, fatherNode);
                            return true;
                        }
                    }
                    else if (leftBrother >= 0 && rightBrother >= 0) {
                        internalNode leftNode = tree->internalPool->read(leftBrother);
                        if (leftNode.keyNumber > MIN_KEY_NUM) {
                            borrowLeft(tree, leftNode, fatherNode, index);
                            return false;
                        }
                        else {
                            internalNode rightNode = tree->internalPool->read(rightBrother);
                            if (rightNode.keyNumber > MIN_KEY_NUM) {
                                borrowRight(tree, rightNode, fatherNode, index);
                                return false;
                            }
                            else {
                                mergeRight(tree, rightNode, fatherNode, index);
                                return true;
                            }
                        }
                    }
                    else return false;
                }
                else return false;
            }

#ifdef debug
            
            void show() const {
                cout << "[internalNode]" << endl;
                cout << "offset: " << offset << endl;
                cout << "leftBrother: " << leftBrother << endl;
                cout << "rightBrother: " << rightBrother << endl;
                cout << (childNodeIsLeaf ? "child node is leaf" : "child node is internal") << endl;
                cout << "keyNumber: " << keyNumber << endl;
                cout << "nodeKey & childNode:" << endl;
                for (int i = 0; i < keyNumber; i++) {
                    cout << "\t\t\t\t\t\t\t\t\t\t\t" << "childNode: " << childNode[i] << endl;
                    cout << "nodeKey: " << nodeKey[i] << endl;
                }
                cout << "\t\t\t\t\t\t\t\t\t\t\t" << "childNode: " << childNode[keyNumber] << endl;
                cout << endl;
                
            }

#endif
        };
    
    private:
        void initialize(const key &o1, const data &o2) {
            internalNode rootNode;
            rootNode.offset = internalPool->tellWritePoint();
            rootNode.childNodeIsLeaf = true;
            rootNode.childNode[0] = leafPool->tellWritePoint();
            internalPool->write(rootNode);
            leafNode tempNode;
            tempNode.offset = rootNode.childNode[0];
            tempNode.dataNumber = 1;
            tempNode.leafKey[0] = o1;
            tempNode.leafData[0] = o2;
            leafPool->write(tempNode);
            info.head = tempNode.offset;
            info.root = rootNode.offset;
        }
        
        //first represent child node number ++
        pair<bool, pair<int, key>> recursionInsert(int now, const key &o1, const data &o2) {
            internalNode nowNode = internalPool->read(now);
            if (nowNode.childNodeIsLeaf) {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
                leafNode targetNode = leafPool->read(nowNode.childNode[index]);
                targetNode.addElement(this, o1, o2);
                pair<bool, pair<int, key>> temp;
                if (targetNode.dataNumber == MAX_RECORD_NUM) {
                    nowNode.addElement(this, targetNode.splitNode(this), index);
                    if (nowNode.keyNumber == MAX_KEY_NUM) {
                        temp.first = true;
                        temp.second = nowNode.splitNode(this);
                    }
                    else temp.first = false;
                }
                else temp.first = false;
                return temp;
            }
            else {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
                pair<bool, pair<int, key>> temp = recursionInsert(nowNode.childNode[index], o1, o2);
                if (temp.first) {
                    nowNode.addElement(this, temp.second, index);
                    if (nowNode.keyNumber == MAX_KEY_NUM)temp.second = nowNode.splitNode(this);
                    else temp.first = false;
                }
                return temp;
            }
        }
        
        //first:   son node need resize
        //second:  delete successfully
        pair<bool, bool> recursionErase(int now, const key &o) {
            internalNode nowNode = internalPool->read(now);
            if (nowNode.childNodeIsLeaf) {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o) - nowNode.nodeKey;
                leafNode targetNode = leafPool->read(nowNode.childNode[index]);
                pair<bool, bool> temp;
                temp.second = targetNode.deleteElement(this, o);
                temp.first = targetNode.resize(this, nowNode, index);
                return temp;
            }
            else {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o) - nowNode.nodeKey;
                pair<bool, bool> temp = recursionErase(nowNode.childNode[index], o);
                if (!temp.first || !temp.second)return temp;
                else {
                    internalNode sonNode = internalPool->read(nowNode.childNode[index]);
                    temp.first = sonNode.resize(this, nowNode, index);
                    return temp;
                }
            }
        }
        
        data recursionFind(int now, const key &o) {
            internalNode nowNode = internalPool->read(now);
            if (nowNode.childNodeIsLeaf) {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o) - nowNode.nodeKey;
                leafNode targetNode = leafPool->read(nowNode.childNode[index]);
                int pos = lower_bound(targetNode.leafKey, targetNode.leafKey + targetNode.dataNumber, o) - targetNode.leafKey;
                if (pos == targetNode.dataNumber)return data(failedSignal);
                if (targetNode.leafKey[pos] == o)return targetNode.leafData[pos];
                else return data(failedSignal);
            }
            else {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o) - nowNode.nodeKey;
                return recursionFind(nowNode.childNode[index], o);
            }
        }
    
    public:
        explicit BPlusTree(const string &name) {
            leafPool = new MemoryPool<leafNode, basicInfo>("leaf_" + name + ".dat");
            internalPool = new MemoryPool<internalNode, basicInfo>("internal_" + name + ".dat");
            info = leafPool->readExtraMessage();
        }
        
        ~BPlusTree() {
            leafPool->updateExtraMessage(info);
            internalPool->updateExtraMessage(info);
            delete leafPool;
            delete internalPool;
        }
        
        int size() const {
            return info.size;
        }
        
        bool empty() const {
            return info.size == 0;
        }
        
        void clear() {
            leafPool->clear();
            internalPool->clear();
            info = leafPool->readExtraMessage();
        }
        
        void insert(const key &o1, const data &o2) {
            if (info.root == -1) initialize(o1, o2);
            else {
                internalNode rootNode = internalPool->read(info.root);
                if (rootNode.childNodeIsLeaf) {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                    leafNode targetNode = leafPool->read(rootNode.childNode[index]);
                    targetNode.addElement(this, o1, o2);
                    if (targetNode.dataNumber == MAX_RECORD_NUM)rootNode.addElement(this, targetNode.splitNode(this), index);
                    if (rootNode.keyNumber == MAX_KEY_NUM)rootNode.splitRoot(this);
                }
                else {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                    pair<bool, pair<int, key>> temp = recursionInsert(rootNode.childNode[index], o1, o2);
                    if (temp.first) {
                        rootNode.addElement(this, temp.second, index);
                        if (rootNode.keyNumber == MAX_KEY_NUM)rootNode.splitRoot(this);
                    }
                }
            }
            info.size++;
        }
        
        //return whether erase is successful
        bool erase(const key &o) {
            if (info.size == 0 || info.root == -1)return false;
            else {
                internalNode rootNode = internalPool->read(info.root);
                bool deleted;
                if (rootNode.childNodeIsLeaf) {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o) - rootNode.nodeKey;
                    leafNode targetNode = leafPool->read(rootNode.childNode[index]);
                    deleted = targetNode.deleteElement(this, o);
                    if (deleted) {
                        if (targetNode.resize(this, rootNode, index))rootNode.resizeRoot(this);
                    }
                }
                else {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o) - rootNode.nodeKey;
                    pair<bool, bool> temp = recursionErase(rootNode.childNode[index], o);
                    if (!temp.second)return false;
                    else {
                        deleted = true;
                        if (temp.first) {
                            internalNode sonNode = internalPool->read(rootNode.childNode[index]);
                            if (sonNode.resize(this, rootNode, index))rootNode.resizeRoot(this);
                        }
                    }
                }
                if (deleted)info.size--;
                return deleted;
            }
        }
        
        data find(const key &o) {
            if (info.size == 0 || info.root == -1)return data(failedSignal);
            else {
                internalNode rootNode = internalPool->read(info.root);
                if (rootNode.childNodeIsLeaf) {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o) - rootNode.nodeKey;
                    leafNode targetNode = leafPool->read(rootNode.childNode[index]);
                    int pos = lower_bound(targetNode.leafKey, targetNode.leafKey + targetNode.dataNumber, o) - targetNode.leafKey;
                    if (pos == targetNode.dataNumber)return data(failedSignal);
                    if (targetNode.leafKey[pos] == o)return targetNode.leafData[pos];
                    else return data(failedSignal);
                }
                else {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o) - rootNode.nodeKey;
                    return recursionFind(rootNode.childNode[index], o);
                }
            }
        }
        
        data operator[](const key &o) {
            return find(o);
        }
        
        void traversal(vector<data> &result) {
            if (info.head == -1)return;
            int cur = info.head;
            while (cur >= 0) {
                leafNode nowNode = leafPool->read(cur);
                for (int i = 0; i < nowNode.dataNumber; i++)result.push_back(nowNode.leafData[i]);
                cur = nowNode.rightBrother;
            }
        }

#ifdef debug
    private:
        void show(int offset, bool isLeaf) const {
            cout << "[pos] " << offset << endl;
            if (isLeaf) {
                leafNode tempNode = leafPool->read(offset);
                tempNode.show();
            }
            else {
                internalNode tempNode = internalPool->read(offset);
                tempNode.show();
                cout << endl;
                for (int i = 0; i <= tempNode.keyNumber; i++) {
                    if (tempNode.childNodeIsLeaf)show(tempNode.childNode[i], true);
                    else show(tempNode.childNode[i], false);
                }
            }
        };
    
    public:
        void show() const {
            cout << "[show]--------------------------------------------------------------------------------" << endl;
            show(info.root, false);
            cout << "[show]--------------------------------------------------------------------------------" << endl;
        }
        
        void showLeaves() const {
            int cur = info.head;
            while (cur >= 0) {
                leafNode nowNode = leafPool->read(cur);
                nowNode.show();
                cur = nowNode.rightBrother;
            }
        }

#endif
    };
}

#endif //RAINYMEMORY_BPLUSTREE_H
