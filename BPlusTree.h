//
// Created by Rainy Memory on 2021/1/28.
//

#ifndef UNTITLED_BPLUSTREE_H
#define UNTITLED_BPLUSTREE_H

#include "MemoryPool.h"

#define debug

#define MAX_KEY_LENGTH 65
#define M 100
#define L 100
#define MAX_RECORD_NUM (L+1)
#define MIN_RECORD_NUM ((L+1)/2)
#define MAX_KEY_NUM (M+1)
#define MIN_KEY_NUM ((M+1)/2)
#define MAX_CHILD_NUM (MAX_KEY_NUM+1)

class BPlusTreeString {
public:
    char key[MAX_KEY_LENGTH];
    
    BPlusTreeString() {
        key[0] = '\0';
    }
    
    BPlusTreeString(const BPlusTreeString &o) {
        strcpy(key, o.key);
    }
    
    explicit BPlusTreeString(const string &o) {
        strcpy(key, o.c_str());
    }
    
    bool operator<(const BPlusTreeString &o) const {
        return strcmp(key, o.key) < 0;
    }
    
    bool operator==(const BPlusTreeString &o) const {
        return strcmp(key, o.key) == 0;
    }
    
    bool operator!=(const BPlusTreeString &o) const {
        return !((*this) == o);
    }
    
    bool operator>(const BPlusTreeString &o) const {
        if ((*this) == o)return false;
        return !((*this) < o);
    }
    
    bool operator<=(const BPlusTreeString &o) const {
        if ((*this) == o)return true;
        return (*this) < o;
    }
    
    bool operator>=(const BPlusTreeString &o) const {
        if ((*this) == o)return true;
        return (*this) > o;
    }
    
    BPlusTreeString operator=(const BPlusTreeString &o) {
        if (this == &o)return *this;
        strcpy(key, o.key);
        return *this;
    }
    
    BPlusTreeString operator=(const string &o) {
        strcpy(key, o.c_str());
        return *this;
    }
    
    explicit operator string() {
        string temp = key;
        return temp;
    }
    
    friend ostream &operator<<(ostream &os, const BPlusTreeString &o) {
        os << o.key;
        return os;
    }
};

template<class key, class data>
class BPlusTree {
    class leafNode;
    
    class internalNode;
    
    class basicInfo {
    public:
        int root = -1;
        int head = -1;
        int size = 0;
    };

private:
    MemoryPool<leafNode, basicInfo> *leafPool;
    MemoryPool<internalNode, basicInfo> *internalPool;
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
                    for (int j = i; j < dataNumber; j++) {
                        leafData[i] = leafData[i + 1];
                        leafKey[i] = leafKey[i + 1];
                    }
                    dataNumber--;
                    result = true;
                    break;
                }
            }
            tree->leafPool->update(*this, offset);
            return result;
        }
        
        pair<int, key> splitNode(BPlusTree *tree) {
            leafNode tempNode;
            tempNode.leftBrother = offset;
            tempNode.rightBrother = rightBrother;
            tempNode.father = father;
            tempNode.offset = tree->leafPool->tellWritePoint();
            leafNode tempRightNode = tree->leafPool->read(rightBrother);
            tempRightNode.leftBrother = tempNode.offset;
            tree->leafPool->update(tempRightNode, tempRightNode.offset);
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
        
        void borrowLeft(BPlusTree *tree) {
            leafNode leftNode = tree->leafPool->read(leftBrother);
            for (int i = dataNumber - 1; i >= 0; i--) {
                leafKey[i + 1] = leafKey[i];
                leafData[i + 1] = leafData[i];
            }
            leafKey[0] = leftNode.leafKey[leftNode.dataNumber - 1];
            leafData[0] = leftNode.leafData[leftNode.dataNumber - 1];
            dataNumber++;
            leftNode.dataNumber--;
            tree->leafPool->update(*this, offset);
            tree->leafPool->update(leftNode, leftNode.offset);
        }
        
        void borrowRight(BPlusTree *tree) {
            leafNode rightNode = tree->leafPool->read(rightBrother);
            leafKey[dataNumber] = rightNode.leafKey[0];
            leafData[dataNumber] = rightNode.leafData[0];
            for (int i = 0; i < rightNode.dataNumber; i++) {
                rightNode.leafKey[i] = rightNode.leafKey[i + 1];
                rightNode.leafData[i] = rightNode.leafData[i + 1];
            }
            dataNumber++;
            rightNode.dataNumber--;
            tree->leafPool->update(*this, offset);
            tree->leafPool->update(rightNode, rightNode.offset);
        }
        
        //keep left node and delete right node anyway
        //only right-most point can merge left
        void mergeLeft(BPlusTree *tree) {
            leafNode leftNode = tree->leafPool->read(leftBrother);
            for (int i = 0; i < dataNumber; i++) {
                leftNode.leafKey[i + leftNode.dataNumber] = leafKey[i];
                leftNode.leafData[i + leftNode.dataNumber] = leafData[i];
            }
            leftNode.dataNumber += dataNumber;
            leftNode.rightBrother = rightBrother;
            tree->leafPool->update(leftNode, leftNode.offset);
            tree->leafPool->erase(offset);
        }
        
        void mergeRight(BPlusTree *tree) {
            leafNode rightNode = tree->leafPool->read(rightBrother);
            for (int i = 0; i < rightNode.dataNumber; i++) {
                leafKey[i + dataNumber] = rightNode.leafKey[i];
                leafData[i + dataNumber] = rightNode.leafData[i];
            }
            dataNumber += rightNode.dataNumber;
            rightBrother = rightNode.rightBrother;
            tree->leafPool->update(*this, offset);
            tree->leafPool->erase(rightNode.offset);
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
                cout << "leafKey: " << leafKey[i] << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << "leafData: " << leafData[i] << endl;
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
        int childNode[MAX_CHILD_NUM];
    
    public:
        internalNode() {
            memset(childNode, -1, sizeof(childNode));
        }
        
        void addElement(BPlusTree *tree, const pair<int, key> &o) {
            int pos = upper_bound(nodeKey, nodeKey + keyNumber, o.second) - nodeKey;
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
        
        pair<int, key> splitNode(BPlusTree *tree) {
            internalNode tempNode;
            tempNode.father = father;
            tempNode.leftBrother = offset;
            tempNode.rightBrother = rightBrother;
            tempNode.offset = tree->internalPool->tellWritePoint();
            internalNode tempRightNode = tree->internalPool->read(rightBrother);
            tempRightNode.leftBrother = tempNode.offset;
            tree->internalPool->update(tempRightNode, tempRightNode.offset);
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
            pair<int, key> temp;
            temp.first = tempNode.offset;
            temp.second = nodeKey[MIN_KEY_NUM];
            return temp;
        }
        
        void borrowLeft(BPlusTree *tree) {
            //todo borrow
            internalNode leftNode = tree->internalPool->read(leftBrother);
            childNode[keyNumber + 1] = childNode[keyNumber];
            for (int i = keyNumber - 1; i >= 0; i--) {
                childNode[i + 1] = childNode[i];
                nodeKey[i + 1] = nodeKey[i];
            }
            nodeKey[0] = leftNode.nodeKey[leftNode.keyNumber - 1];
            childNode[0] = leftNode.childNode[leftNode.keyNumber];
            keyNumber++;
            leftNode.keyNumber--;
            tree->internalPool->update(*this, offset);
            tree->internalPool->update(leftNode, leftNode.offset);
        }
        
        void borrowRight(BPlusTree *tree) {
            //todo
            internalNode rightNode = tree->internalPool->read(rightBrother);
            nodeKey[keyNumber] = rightNode.nodeKey[0];
            childNode[keyNumber + 1] = rightNode.childNode[0];
            for (int i = 0; i < rightNode.keyNumber - 1; i++) {
                rightNode.childNode[i] = rightNode.childNode[i + 1];
                rightNode.nodeKey[i] = rightNode.nodeKey[i + 1];
            }
            rightNode.childNode[rightNode.keyNumber - 1] = rightNode.childNode[rightNode.keyNumber];
            keyNumber++;
            rightNode.keyNumber--;
            tree->internalPool->update(*this, offset);
            tree->internalPool->update(rightNode, rightNode.offset);
        }
        
        void mergeLeft(BPlusTree *tree, const key &o) {
            internalNode leftNode = tree->internalPool->read(leftBrother);
            //in the following example, key 4 is const key &o
            //key:    1 2 3   4   5 6 7 8
            //child: 1 2 3 4     5 6 7 8 9
            //                \/
            //key:    1 2 3 4 5 6 7 8
            //child: 1 2 3 4 5 6 7 8 9
            leftNode.nodeKey[leftNode.keyNumber] = o;
            for (int i = 0; i < keyNumber; i++) {
                leftNode.nodeKey[i + leftNode.keyNumber + 1] = nodeKey[i];
                leftNode.childNode[i + leftNode.keyNumber + 1] = childNode[i];
            }
            leftNode.childNode[leftNode.keyNumber + keyNumber + 1] = childNode[keyNumber];
            leftNode.rightBrother = rightBrother;
            leftNode.keyNumber += keyNumber + 1;
            tree->internalPool->update(leftNode, leftNode.offset);
            tree->internalPool->erase(offset);
        }
        
        void mergeRight(BPlusTree *tree, const key &o) {
            internalNode rightNode = tree->internalPool->read(rightBrother);
            nodeKey[keyNumber] = o;
            for (int i = 0; i < rightNode.keyNumber; i++) {
                nodeKey[i + keyNumber + 1] = rightNode.nodeKey[i];
                childNode[i + keyNumber + 1] = rightNode.childNode[i];
            }
            childNode[keyNumber + rightNode.keyNumber + 1] = rightNode.childNode[rightNode.keyNumber];
            rightBrother = rightNode.rightBrother;
            keyNumber += rightNode.keyNumber + 1;
            tree->internalPool->update(*this, offset);
            tree->internalPool->erase(rightNode.offset);
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
                cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << "childNode: " << childNode[i] << endl;
                cout << "nodeKey: " << nodeKey[i] << endl;
            }
            cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << "childNode: " << childNode[keyNumber] << endl;
            cout << endl;
            
        }

#endif
    };

private:
    //first represent child node number ++
    pair<bool, pair<int, key>> recursionInsert(int now, const key &o1, const data &o2) {
        internalNode nowNode = internalPool->read(now);
        if (nowNode.childNodeIsLeaf) {
            int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
            leafNode targetNode = leafPool->read(nowNode.childNode[index]);
            targetNode.addElement(this, o1, o2);
            if (targetNode.dataNumber == MAX_RECORD_NUM) {
                pair<bool, pair<int, key>> temp;
                temp.first = true;
                temp.second = targetNode.splitNode(this);
                return temp;
            }
            else {
                pair<bool, pair<int, key>> temp;
                temp.first = false;
                return temp;
            }
        }
        else {
            int index = upper_bound(nowNode.nodeKey, nowNode.nodeKey + nowNode.keyNumber, o1) - nowNode.nodeKey;
            pair<bool, pair<int, key>> temp = recursionInsert(nowNode.childNode[index], o1, o2);
            if (temp.first) {
                nowNode.addElement(this, temp.second);
                if (nowNode.keyNumber == MAX_KEY_NUM) {
                    temp.second = nowNode.splitNode(this);
                    return temp;
                }
                else {
                    temp.first = false;
                    return temp;
                }
            }
            else return temp;
        }
    }
    
    bool recursionErase(int now) {
        //todo maybe failed
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
    BPlusTree(const string &name) {
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
    
    void insert(const key &o1, const data &o2) {
        if (info.size == 0) {
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
        else {
            internalNode rootNode = internalPool->read(info.root);
            if (rootNode.childNodeIsLeaf) {
                int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                leafNode targetNode = leafPool->read(rootNode.childNode[index]);
                targetNode.addElement(this, o1, o2);
                if (targetNode.dataNumber == MAX_RECORD_NUM) rootNode.addElement(this, targetNode.splitNode(this));
                if (rootNode.keyNumber == MAX_KEY_NUM) rootNode.splitRoot(this);
            }
            else {
                int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                pair<bool, pair<int, key>> temp = recursionInsert(rootNode.childNode[index], o1, o2);
                if (temp.first) {
                    internalNode tempNode = internalPool->read(rootNode.childNode[index]);
                    tempNode.addElement(this, temp.second);
                    if (tempNode.keyNumber == MAX_KEY_NUM)rootNode.addElement(this, tempNode.splitNode(this));
                    if (rootNode.keyNumber == MAX_KEY_NUM)rootNode.splitRoot(this);
                }
            }
        }
        info.size++;
    }
    
    //return whether erase is successful
    bool erase(const key &o1, const data &o2) {
        if (info.size == 0)return false;
        else {
            internalNode rootNode = internalPool->read(info.root);
            if (rootNode.childNodeIsLeaf) {
                int index = upper_bound(rootNode.nodeKey, rootNode.nodeKey + rootNode.keyNumber, o1) - rootNode.nodeKey;
                leafNode targetNode = leafPool->read(rootNode.childNode[index]);
                if (targetNode.deleteElement(this, o1, o2)) {
                    //todo resize
                    if (targetNode.dataNumber == MIN_RECORD_NUM) {
                    
                    }
                    return true;
                }
                bool result = false;
                while (targetNode.leftBrother > 0) {
                    targetNode = leafPool->read(targetNode.leftBrother);
                    if (targetNode.deleteElement(this, o1, o2)) {
                        result = true;
                        break;
                    }
                }
                //todo resize
                return result;
            }
            else {
            
            }
        }
    }
    
    void find(const key &o, vector<data> &result) {
        //change return type to data
        if (info.size == 0)return;
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
    
    data operator[](int index) const {
        int cnt = 0;
        int cur = info.head;
        while (cur >= 0) {
            leafNode nowNode = leafPool->read(cur);
            cnt += nowNode.dataNumber;
            if (nowNode.dataNumber > index) return nowNode.leafData[index - (cnt - nowNode.dataNumber)];
            cur = nowNode.rightBrother;
        }
    }
    
    void traversal(vector<data> &result) {
        int cur = info.head;
        while (cur >= 0) {
            leafNode nowNode = leafPool->read(cur);
            for (int i = 0; i < nowNode.dataNumber; i++)result.push_back(nowNode.leafData[i]);
            cur = nowNode.rightBrother;
        }
    }

#ifdef debug
public:
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


#endif //UNTITLED_BPLUSTREE_H
