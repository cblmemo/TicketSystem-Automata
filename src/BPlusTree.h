//
// Created by Rainy Memory on 2021/1/28.
//

#ifndef RAINYMEMORY_BPLUSTREE_H
#define RAINYMEMORY_BPLUSTREE_H

//NOTE: class key need to overload operator= and operator< to support assignment and sort

#include "LRUCacheMemoryPool.h"
#include "algorithm.h"

#include <iostream>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using RainyMemory::upper_bound;
using RainyMemory::lower_bound;
using RainyMemory::LRUCacheMemoryPool;

//#define debug

namespace RainyMemory {
    template<class key, class data, int M = 200, int L = 200>
    class BPlusTree {
    private:
        struct splitNodeReturn {
            int offset;
            key nodeKey;
        };
        
        struct insertReturn {
            bool childNodeNumberIncreased;
            splitNodeReturn node;
        };
        
        struct eraseReturn {
            int fatherNodeOffset;
            int nowNodeOffset;//to find index in fatherNode
            bool sonNodeNeedResize;
            bool eraseSucceed;
        };
        
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
        LRUCacheMemoryPool<leafNode, basicInfo> *leafPool;
        LRUCacheMemoryPool<internalNode, basicInfo> *internalPool;
        basicInfo info;
    
    private:
        class leafNode {
        public:
            int offset = -1;
            int father = -1;
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
            
            bool deleteElement(BPlusTree *tree, const key &o1, const data &o2) {
                int start = lower_bound(leafKey, leafKey + dataNumber, o1) - leafKey;
                int end = upper_bound(leafKey, leafKey + dataNumber, o1) - leafKey;
                bool result = false;
                for (int i = start; i < end; i++) {
                    if (o2 == leafData[i]) {
                        for (int j = i; j < dataNumber - 1; j++) {
                            leafData[j] = leafData[j + 1];
                            leafKey[j] = leafKey[j + 1];
                        }
                        dataNumber--;
                        result = true;
                        break;
                    }
                }
                if (result)tree->leafPool->update(*this, offset);
                return result;
            }
            
            splitNodeReturn splitNode(BPlusTree *tree) {
                leafNode tempNode;
                tempNode.leftBrother = offset;
                tempNode.rightBrother = rightBrother;
                tempNode.father = father;
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
                splitNodeReturn temp;
                temp.offset = tempNode.offset;
                temp.nodeKey = tempNode.leafKey[0];
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
                    else if (leftBrother >= 0 && leftBrother >= 0) {
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
                cout << "father: " << father << endl;
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
            int father = -1;
            int leftBrother = -1;
            int rightBrother = -1;
            bool childNodeIsLeaf = false;
            int keyNumber = 0;
            key nodeKey[MAX_KEY_NUM];
            int childNode[MAX_CHILD_NUM] = {0};
        
        public:
            void addElement(BPlusTree *tree, const splitNodeReturn &o, int pos) {
                for (int i = keyNumber - 1; i >= pos; i--) {
                    childNode[i + 2] = childNode[i + 1];
                    nodeKey[i + 1] = nodeKey[i];
                }
                childNode[pos + 1] = o.offset;
                nodeKey[pos] = o.nodeKey;
                keyNumber++;
                tree->internalPool->update(*this, offset);
            }
            
            void splitRoot(BPlusTree *tree) {
                //oldRoot(*this) -> oldRoot + tempNode
                internalNode newRoot, tempNode;
                tempNode.father = tree->internalPool->write(newRoot);
                father = tempNode.father;
                rightBrother = tree->internalPool->tellWritePoint();
                tempNode.offset = rightBrother;
                tempNode.leftBrother = offset;
                tempNode.childNodeIsLeaf = childNodeIsLeaf;
                for (int i = MIN_KEY_NUM + 1; i < keyNumber; i++) {
                    tempNode.nodeKey[i - MIN_KEY_NUM - 1] = nodeKey[i];
                    tempNode.childNode[i - MIN_KEY_NUM - 1] = childNode[i];
                }
                tempNode.childNode[keyNumber - MIN_KEY_NUM - 1] = childNode[keyNumber];
                //update child node's father
                if (childNodeIsLeaf) {
                    for (int i = MIN_KEY_NUM + 1; i <= keyNumber; i++) {
                        leafNode tempChildNode = tree->leafPool->read(childNode[i]);
                        tempChildNode.father = tempNode.offset;
                        tree->leafPool->update(tempChildNode, tempChildNode.offset);
                    }
                }
                else {
                    for (int i = MIN_KEY_NUM + 1; i <= keyNumber; i++) {
                        internalNode tempChildNode = tree->internalPool->read(childNode[i]);
                        tempChildNode.father = tempNode.offset;
                        tree->internalPool->update(tempChildNode, tempChildNode.offset);
                    }
                }
                tempNode.keyNumber = keyNumber - MIN_KEY_NUM - 1;
                keyNumber = MIN_KEY_NUM;
                newRoot.offset = father;
                newRoot.keyNumber = 1;
                newRoot.nodeKey[0] = nodeKey[MIN_KEY_NUM];
                newRoot.childNode[0] = offset;
                newRoot.childNode[1] = tempNode.offset;
                tree->internalPool->write(tempNode);
                tree->internalPool->update(newRoot, newRoot.offset);
                tree->internalPool->update(*this, offset);
                tree->info.root = newRoot.offset;
            }
            
            splitNodeReturn splitNode(BPlusTree *tree) {
                internalNode tempNode;
                tempNode.father = father;
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
                
                //update child node's father
                if (childNodeIsLeaf) {
                    for (int i = MIN_KEY_NUM + 1; i <= keyNumber; i++) {
                        leafNode tempChildNode = tree->leafPool->read(childNode[i]);
                        tempChildNode.father = tempNode.offset;
                        tree->leafPool->update(tempChildNode, tempChildNode.offset);
                    }
                }
                else {
                    for (int i = MIN_KEY_NUM + 1; i <= keyNumber; i++) {
                        internalNode tempChildNode = tree->internalPool->read(childNode[i]);
                        tempChildNode.father = tempNode.offset;
                        tree->internalPool->update(tempChildNode, tempChildNode.offset);
                    }
                }
                tempNode.keyNumber = keyNumber - MIN_KEY_NUM - 1;
                keyNumber = MIN_KEY_NUM;
                tree->internalPool->write(tempNode);
                tree->internalPool->update(*this, offset);
                splitNodeReturn temp;
                temp.offset = tempNode.offset;
                temp.nodeKey = nodeKey[MIN_KEY_NUM];
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
                
                //update child node's father
                if (childNodeIsLeaf) {
                    leafNode tempNode = tree->leafPool->read(childNode[0]);
                    tempNode.father = offset;
                    tree->leafPool->update(tempNode, tempNode.offset);
                }
                else {
                    internalNode tempNode = tree->internalPool->read(childNode[0]);
                    tempNode.father = offset;
                    tree->internalPool->update(tempNode, tempNode.offset);
                }
                
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
                
                //update child node's father
                if (childNodeIsLeaf) {
                    leafNode tempNode = tree->leafPool->read(childNode[keyNumber]);
                    tempNode.father = offset;
                    tree->leafPool->update(tempNode, tempNode.offset);
                }
                else {
                    internalNode tempNode = tree->internalPool->read(childNode[keyNumber]);
                    tempNode.father = offset;
                    tree->internalPool->update(tempNode, tempNode.offset);
                }
                
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
                
                //update child node's father
                if (childNodeIsLeaf) {
                    for (int i = 0; i <= keyNumber; i++) {
                        leafNode tempNode = tree->leafPool->read(childNode[i]);
                        tempNode.father = leftNode.offset;
                        tree->leafPool->update(tempNode, tempNode.offset);
                    }
                }
                else {
                    for (int i = 0; i <= keyNumber; i++) {
                        internalNode tempNode = tree->internalPool->read(childNode[i]);
                        tempNode.father = leftNode.offset;
                        tree->internalPool->update(tempNode, tempNode.offset);
                    }
                }
                
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
                
                //update child node's father
                if (childNodeIsLeaf) {
                    for (int i = 0; i <= rightNode.keyNumber; i++) {
                        leafNode tempNode = tree->leafPool->read(rightNode.childNode[i]);
                        tempNode.father = offset;
                        tree->leafPool->update(tempNode, tempNode.offset);
                    }
                }
                else {
                    for (int i = 0; i <= rightNode.keyNumber; i++) {
                        internalNode tempNode = tree->internalPool->read(rightNode.childNode[i]);
                        tempNode.father = offset;
                        tree->internalPool->update(tempNode, tempNode.offset);
                    }
                }
                
                tree->internalPool->update(fatherNode, fatherNode.offset);
                tree->internalPool->update(*this, offset);
                tree->internalPool->erase(rightNode.offset);
            }
            
            void resizeRoot(BPlusTree *tree) {
                if ((!childNodeIsLeaf) && (keyNumber == 0)) {
                    tree->info.root = childNode[0];
                    tree->internalPool->erase(offset);
                    internalNode tempNode = tree->internalPool->read(childNode[0]);
                    tempNode.father = -1;
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
                    else if (leftBrother >= 0 && leftBrother >= 0) {
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
                cout << "father: " << father << endl;
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
            tempNode.father = rootNode.offset;
            tempNode.offset = rootNode.childNode[0];
            tempNode.dataNumber = 1;
            tempNode.leafKey[0] = o1;
            tempNode.leafData[0] = o2;
            leafPool->write(tempNode);
            info.head = tempNode.offset;
            info.root = rootNode.offset;
        }
        
        //first represent child node number ++
        insertReturn recursionInsert(int now, const key &o1, const data &o2) {
            internalNode nowNode = internalPool->read(now);
            if (nowNode.childNodeIsLeaf) {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
                leafNode targetNode = leafPool->read(nowNode.childNode[index]);
                targetNode.addElement(this, o1, o2);
                insertReturn temp;
                if (targetNode.dataNumber == MAX_RECORD_NUM) {
                    nowNode.addElement(this, targetNode.splitNode(this), index);
                    if (nowNode.keyNumber == MAX_KEY_NUM) {
                        temp.childNodeNumberIncreased = true;
                        temp.node = nowNode.splitNode(this);
                    }
                    else temp.childNodeNumberIncreased = false;
                }
                else temp.childNodeNumberIncreased = false;
                return temp;
            }
            else {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
                insertReturn temp = recursionInsert(nowNode.childNode[index], o1, o2);
                if (temp.childNodeNumberIncreased) {
                    nowNode.addElement(this, temp.node, index);
                    if (nowNode.keyNumber == MAX_KEY_NUM)temp.node = nowNode.splitNode(this);
                    else temp.childNodeNumberIncreased = false;
                }
                return temp;
            }
        }
        
        eraseReturn recursionErase(int now, const key &o1, const data &o2) {
            internalNode nowNode = internalPool->read(now);
            bool deleted = false;
            if (nowNode.childNodeIsLeaf) {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
                leafNode targetNode = leafPool->read(nowNode.childNode[index]);
                bool flag = targetNode.deleteElement(this, o1, o2);
                bool changeIndexFlag = false;
                if (flag)deleted = true;
                else {
                    while (targetNode.leftBrother >= 0) {
                        changeIndexFlag = true;
                        targetNode = leafPool->read(targetNode.leftBrother);
                        if (targetNode.deleteElement(this, o1, o2)) {
                            deleted = true;
                            break;
                        }
                    }
                }
                if (now != targetNode.father)nowNode = internalPool->read(targetNode.father);
                if (changeIndexFlag)index = RainyMemory::find(nowNode.childNode, nowNode.childNode + nowNode.keyNumber + 1, targetNode.offset) - nowNode.childNode;
                eraseReturn temp;
                temp.fatherNodeOffset = nowNode.father;
                temp.nowNodeOffset = nowNode.offset;
                temp.sonNodeNeedResize = targetNode.resize(this, nowNode, index);
                temp.eraseSucceed = deleted;
                return temp;
            }
            else {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
                eraseReturn temp = recursionErase(nowNode.childNode[index], o1, o2);
                if (!temp.sonNodeNeedResize || !temp.eraseSucceed)return temp;
                else {
                    if (nowNode.offset != temp.fatherNodeOffset) nowNode = internalPool->read(temp.fatherNodeOffset);
                    temp.fatherNodeOffset = nowNode.father;
                    internalNode sonNode = internalPool->read(temp.nowNodeOffset);
                    //find index, use temp.first.second
                    index = RainyMemory::find(nowNode.childNode, nowNode.childNode + nowNode.keyNumber + 1, temp.nowNodeOffset) - nowNode.childNode;
                    temp.nowNodeOffset = nowNode.offset;
                    temp.sonNodeNeedResize = sonNode.resize(this, nowNode, index);
                    return temp;
                }
            }
        }
        
        void recursionFind(int now, const key &o, vector<data> &result) {
            internalNode nowNode = internalPool->read(now);
            if (nowNode.childNodeIsLeaf) {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o) - nowNode.nodeKey;
                int cur = nowNode.childNode[index];
                bool flag = true;
                while (cur >= 0 && flag) {
                    leafNode targetNode = leafPool->read(cur);
                    int pos = upper_bound(targetNode.leafKey, targetNode.leafKey + targetNode.dataNumber, o) - targetNode.leafKey;
                    for (int i = pos - 1; i >= 0; i--) {
                        if (o > targetNode.leafKey[i]) {
                            flag = false;
                            break;
                        }
                        result.push_back(targetNode.leafData[i]);
                    }
                    cur = targetNode.leftBrother;
                }
            }
            else {
                int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o) - nowNode.nodeKey;
                recursionFind(nowNode.childNode[index], o, result);
            }
        }
    
    public:
        explicit BPlusTree(const string &name) {
            leafPool = new LRUCacheMemoryPool<leafNode, basicInfo>("leaf_" + name + ".dat");
            internalPool = new LRUCacheMemoryPool<internalNode, basicInfo>("internal_" + name + ".dat");
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
                    insertReturn temp = recursionInsert(rootNode.childNode[index], o1, o2);
                    if (temp.childNodeNumberIncreased) {
                        rootNode.addElement(this, temp.node, index);
                        if (rootNode.keyNumber == MAX_KEY_NUM)rootNode.splitRoot(this);
                    }
                }
            }
            info.size++;
        }
        
        //return whether erase is successful
        bool erase(const key &o1, const data &o2) {
            if (info.size == 0 || info.root == -1)return false;
            else {
                internalNode rootNode = internalPool->read(info.root);
                bool deleted = false;
                if (rootNode.childNodeIsLeaf) {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                    leafNode targetNode = leafPool->read(rootNode.childNode[index]);
                    bool flag = targetNode.deleteElement(this, o1, o2);
                    bool changeIndexFlag = false;
                    if (flag)deleted = true;
                    else {
                        while (targetNode.leftBrother >= 0) {
                            changeIndexFlag = true;
                            targetNode = leafPool->read(targetNode.leftBrother);
                            if (targetNode.deleteElement(this, o1, o2)) {
                                deleted = true;
                                break;
                            }
                        }
                    }
                    if (changeIndexFlag)index = RainyMemory::find(rootNode.childNode, rootNode.childNode + rootNode.keyNumber + 1, targetNode.offset) - rootNode.childNode;
                    if (deleted) {
                        if (targetNode.resize(this, rootNode, index))rootNode.resizeRoot(this);
                    }
                }
                else {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                    eraseReturn temp = recursionErase(rootNode.childNode[index], o1, o2);
                    if (!temp.eraseSucceed)return false;
                    else {
                        deleted = true;
                        if (temp.sonNodeNeedResize) {
                            internalNode sonNode = internalPool->read(temp.nowNodeOffset);
                            index = RainyMemory::find(rootNode.childNode, rootNode.childNode + rootNode.keyNumber + 1, temp.nowNodeOffset) - rootNode.childNode;
                            if (sonNode.resize(this, rootNode, index))rootNode.resizeRoot(this);
                        }
                    }
                }
                if (deleted)info.size--;
                return deleted;
            }
        }
        
        void find(const key &o, vector<data> &result) {
            if (info.size == 0 || info.root == -1)return;
            else {
                internalNode rootNode = internalPool->read(info.root);
                if (rootNode.childNodeIsLeaf) {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o) - rootNode.nodeKey;
                    int cur = rootNode.childNode[index];
                    bool flag = true;
                    while (cur >= 0 && flag) {
                        leafNode targetNode = leafPool->read(cur);
                        int pos = upper_bound(targetNode.leafKey, targetNode.leafKey + targetNode.dataNumber, o) - targetNode.leafKey;
                        for (int i = pos - 1; i >= 0; i--) {
                            if (o > targetNode.leafKey[i]) {
                                flag = false;
                                break;
                            }
                            result.push_back(targetNode.leafData[i]);
                        }
                        cur = targetNode.leftBrother;
                    }
                }
                else {
                    int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o) - rootNode.nodeKey;
                    recursionFind(rootNode.childNode[index], o, result);
                }
            }
        }
        
        vector<data> operator[](const key &o) {
            vector<data> result;
            find(o, result);
            return result;
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
