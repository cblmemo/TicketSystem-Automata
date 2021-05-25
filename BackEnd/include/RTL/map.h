//
// Created by Rainy Memory on 2021/5/25.
//

#ifndef CODE_MAP_H
#define CODE_MAP_H

#include <functional>
#include "exceptions.h"

namespace RainyMemory {
    template<class Key, class T, class Compare = std::less<Key>>
    class map {
    public:
        typedef std::pair<const Key, T> value_type;
        
        class iterator;
        
        class const_iterator;
    
    private:
        enum colorType {
            RED, BLACK
        };
        
        enum positionType {
            NONE, OUTBOARD, INBOARD, BOTH
        };
        
        class Node {
        public:
            enum rotateType {
                LEFT, RIGHT, PARENT
            };
            
            colorType color = RED;
            value_type *value = nullptr;
            Node *parent = nullptr;
            Node *left = nullptr;
            Node *right = nullptr;
            
            Node() = default;
            
            explicit Node(const value_type &o, colorType c) : value(new value_type(o)), color(c) {}
            
            explicit Node(value_type *o, colorType c) : value(o), color(c) {}
            
            Node(const Node &o) : value(new value_type(*o.value)), color(o.color) {}
            
            Node &operator=(const Node &o) = delete;
            
            ~Node() {
                delete value;
            }
            
            Node *&selfFromParent() {
                if (isRoot())return parent->parent;
                else if (isLeft())return parent->left;
                else return parent->right;
            }
            
            Key getKey() const {
                return value->first;
            }
            
            void setColor(colorType c) {
                color = c;
            }
            
            bool isRed() const {
                return color == RED;
            }
            
            bool isLeft() const {
                if (parent->left == this)return true;
                else return false;
            }
            
            bool isRight() const {
                if (parent->right == this)return true;
                else return false;
            }
            
            bool isHeader() const {
                if (parent == nullptr)return true;
                return parent->parent == this && value == nullptr;
            }
            
            bool isRoot() const {
                if (parent == nullptr)return false;
                return parent->parent == this && value != nullptr;
            }
            
            int childNumber() const {
                int cnt = 0;
                if (left != nullptr)cnt++;
                if (right != nullptr)cnt++;
                return cnt;
            }
            
            int redChildNumber() const {
                int cnt = 0;
                if (left != nullptr && left->isRed())cnt++;
                if (right != nullptr && right->isRed())cnt++;
                return cnt;
            }
            
            positionType redChildPosition() {
                switch (redChildNumber()) {
                    case 0:
                        return NONE;
                    case 1:
                        if (isLeft()) {
                            if (left != nullptr && left->isRed())return OUTBOARD;
                            else return INBOARD;
                        }
                        else {
                            if (right != nullptr && right->isRed())return OUTBOARD;
                            else return INBOARD;
                        }
                    default:
                        return BOTH;
                }
            }
            
            rotateType findRotateType() {
                if (isRoot())return PARENT;
                else if (isLeft())return LEFT;
                else return RIGHT;
            }
            
            Node *&self(rotateType r) {
                switch (r) {
                    case LEFT:
                        return parent->left;
                    case RIGHT:
                        return parent->right;
                    default:
                        return parent->parent;
                }
            }
            
            void rotateLeft() {
                rotateType r = findRotateType();
                self(r) = right;
                right = right->left;
                if (right != nullptr)right->parent = this;
                self(r)->parent = parent;
                self(r)->left = this;
                parent = self(r);
            }
            
            void rotateRight() {
                rotateType r = findRotateType();
                self(r) = left;
                left = left->right;
                if (left != nullptr)left->parent = this;
                self(r)->parent = parent;
                self(r)->right = this;
                parent = self(r);
            }
            
            void swapColor(Node *other) {
                colorType c = other == nullptr ? BLACK : other->color;
                if (other != nullptr)other->setColor(color);
                setColor(c);
            }
            
            void swapPos(Node *other) {
                if (other->parent == this)other->swapPos(this);
                else if (parent == other) {
                    bool le = isLeft();
                    Node *l = left, *r = right, *sib = getSibling();
                    other->selfFromParent() = this, parent = other->parent, other->parent = this;
                    if (l != nullptr)l->parent = other;
                    if (r != nullptr)r->parent = other;
                    if (sib != nullptr)sib->parent = this;
                    other->left = l, other->right = r;
                    if (le)right = sib, left = other;
                    else left = sib, right = other;
                }
                else {
                    selfFromParent() = other;
                    other->selfFromParent() = this;
                    if (left != nullptr)left->parent = other;
                    if (right != nullptr)right->parent = other;
                    Node *l = left, *r = right, *p = parent;
                    left = other->left, right = other->right, parent = other->parent;
                    if (other->left != nullptr)other->left->parent = this;
                    if (other->right != nullptr)other->right->parent = this;
                    other->left = l, other->right = r, other->parent = p;
                }
            }
            
            Node *getSibling() const {
                if (isRoot())return nullptr;
                else if (isLeft())return parent->right;
                else return parent->left;
            }
            
            Node *findMin() const {
                Node *node = const_cast<Node *>(this);
                while (node->left != nullptr)node = node->left;
                return node;
            }
            
            Node *findMax() const {
                Node *node = const_cast<Node *>(this);
                while (node->right != nullptr)node = node->right;
                return node;
            }
            
            Node *findPrecursor() const {
                Node *node = const_cast<Node *>(this);
                if (node->isHeader())node = node->right;
                else if (node->left != nullptr)node = node->left->findMax();
                else {
                    while (node->isLeft())node = node->parent;
                    if (node->left != node->parent)node = node->parent;
                }
                return node;
            }
            
            Node *findSuccessor() const {
                Node *node = const_cast<Node *>(this);
                if (node->right != nullptr)node = node->right->findMin();
                else {
                    while (node->isRight())node = node->parent;
                    if (node->right != node->parent)node = node->parent;
                }
                return node;
            }

#ifdef debug
            
            void print() const {
                cout << (isRed() ? "R " : "B ") << getKey() << "  ";
            }

#endif
        };
        
        Node *header = nullptr; //header's parent is root, left is leftmost, right is rightmost
        int nodeNumber = 0;
        Compare compare;
        
        inline bool less(const Key &a, const Key &b) const {
            return compare(a, b);
        }
        
        inline bool equal(const Key &a, const Key &b) const {
            return !(less(a, b) || less(b, a));
        }
        
        void recursionClear(Node *now) {
            if (now == nullptr)return;
            recursionClear(now->left);
            recursionClear(now->right);
            delete now;
        }
        
        Node *recursionConstruct(Node *pa, Node *now) {
            if (now == nullptr)return nullptr;
            Node *temp = new Node(*now);
            temp->parent = pa;
            temp->left = recursionConstruct(temp, now->left);
            temp->right = recursionConstruct(temp, now->right);
            return temp;
        }
        
        Node *findNode(const Key &o) const {
            Node *temp = header->parent;
            while (temp != nullptr) {
                if (equal(o, temp->getKey()))return temp;
                temp = less(o, temp->getKey()) ? temp->left : temp->right;
            }
            return nullptr;
        }
        
        void rebalanceInsert(Node *now) {
            //rebalance red parent and black uncle
            if (now->isLeft()) {
                if (now->parent->isLeft()) {
                    now->parent->parent->rotateRight();
                    now->parent->swapColor(now->parent->right);
                }
                else { //parent->isRight()
                    now->parent->rotateRight();
                    now->parent->rotateLeft();
                    now->swapColor(now->left);
                }
            }
            else { //isRight()
                if (now->parent->isRight()) {
                    now->parent->parent->rotateLeft();
                    now->parent->swapColor(now->parent->left);
                }
                else { //parent->isLeft()
                    now->parent->rotateLeft();
                    now->parent->rotateRight();
                    now->swapColor(now->right);
                }
            }
        }
        
        Node *findInsertPos(const value_type &value) {
            Node *pa = header;
            Node *now = header->parent;
            while (now != nullptr) {
                if (now->redChildNumber() == 2) {
                    now->left->setColor(BLACK), now->right->setColor(BLACK);
                    if (!now->isRoot()) {
                        now->setColor(RED);
                        if (now->parent->isRed())rebalanceInsert(now);
                    }
                }
                pa = now;
                now = less(value.first, now->getKey()) ? now->left : now->right;
            }
            return pa;
        }
        
        iterator RedBlackTreeInsert(Node *insertPos, const value_type &value) {
            nodeNumber++;
            if (insertPos == header) { //empty tree
                header->parent = new Node(value, BLACK);
                header->parent->parent = header;
                header->left = header->right = header->parent;
                return iterator(header->parent);
            }
            Node *insertNode = new Node(value, RED);
            if (less(value.first, insertPos->getKey())) {
                if (insertPos == header->left)header->left = insertNode;
                insertPos->left = insertNode;
            }
            else {
                if (insertPos == header->right)header->right = insertNode;
                insertPos->right = insertNode;
            }
            insertNode->parent = insertPos;
            if (insertNode->parent->isRed())rebalanceInsert(insertNode);
            return iterator(insertNode);
        }
        
        void rebalanceErase(Node *now) {
            if (now->isRoot())return;
            if (now->getSibling() != nullptr && now->getSibling()->isRed()) {
                now->parent->swapColor(now->getSibling());
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
            }
            positionType pt;
            if (now->getSibling() == nullptr || (pt = now->getSibling()->redChildPosition()) == NONE) {
                if (now->parent->isRed()) {
                    now->parent->swapColor(now->getSibling());
                    return;
                }
                if (now->getSibling() != nullptr)now->getSibling()->setColor(RED);
                rebalanceErase(now->parent);
            }
            else if (pt == OUTBOARD) {
                now->getSibling()->setColor(now->parent->color), now->parent->setColor(BLACK);
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
                now->parent->getSibling()->setColor(BLACK);
            }
            else { //INBOARD && BOTH
                if (now->isLeft())now->getSibling()->rotateRight(), now->parent->rotateLeft();
                else now->getSibling()->rotateLeft(), now->parent->rotateRight();
                now->parent->parent->setColor(now->parent->color), now->parent->setColor(BLACK);
            }
        }
        
        void RedBlackTreeErase(Node *now) {
            nodeNumber--;
            if (now->childNumber() == 2) {
                Node *su = now->findSuccessor();
                now->swapPos(su), now->swapColor(su);
            }
            if (now->childNumber() == 1) {
                if (now->left != nullptr) {
                    now->left->parent = now->parent;
                    now->left->setColor(BLACK);
                    now->selfFromParent() = now->left;
                }
                else {
                    now->right->parent = now->parent;
                    now->right->setColor(BLACK);
                    now->selfFromParent() = now->right;
                }
            }
            else { //now->childNumber() == 0
                if (!now->isRed())rebalanceErase(now);
                now->selfFromParent() = nullptr;
            }
            delete now;
        }
    
    public:
        class iterator {
            friend class map;
        
        private:
            Node *node = nullptr;
            
            Key getKey() const {
                return node->getKey();
            }
            
            bool isInvalid() const {
                return node == nullptr;
            }
        
        public:
            iterator() = default;
            
            iterator(const iterator &o) : node(o.node) {}
            
            explicit iterator(Node *o) : node(o) {}
            
            iterator operator++(int) {
                if (isInvalid())throw invalid_iterator();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                iterator temp(*this);
                node = node->findSuccessor();
                return temp;
            }
            
            iterator &operator++() {
                if (isInvalid())throw invalid_iterator();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                node = node->findSuccessor();
                return *this;
            }
            
            iterator operator--(int) {
                if (isInvalid())throw invalid_iterator();
                iterator temp(*this);
                node = node->findPrecursor(); //begin()'s precursor is header
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                return temp;
            }
            
            iterator &operator--() {
                if (isInvalid())throw invalid_iterator();
                node = node->findPrecursor();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                return *this;
            }
            
            value_type &operator*() const {
                if (isInvalid() || node->isHeader())throw invalid_iterator();
                return *node->value;
            }
            
            bool operator==(const iterator &o) const {
                return node == o.node;
            }
            
            bool operator==(const const_iterator &o) const {
                return node == o.node;
            }
            
            bool operator!=(const iterator &o) const {
                return node != o.node;
            }
            
            bool operator!=(const const_iterator &o) const {
                return node != o.node;
            }
            
            value_type *operator->() const {
                if (isInvalid() || node->isHeader())throw invalid_iterator();
                return node->value;
            }
        };
        
        class const_iterator {
            friend class map;
        
        private:
            const Node *node = nullptr;
            
            Key getKey() const {
                return node->getKey();
            }
            
            bool isInvalid() const {
                return node == nullptr;
            }
        
        public:
            const_iterator() = default;
            
            const_iterator(const const_iterator &o) : node(o.node) {}
            
            explicit const_iterator(const iterator &o) : node(o.node) {}
            
            explicit const_iterator(Node *o) : node(o) {}
            
            const_iterator &operator=(const iterator &o) {
                node = o.node;
                return *this;
            }
            
            const_iterator operator++(int) {
                if (isInvalid())throw invalid_iterator();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                const_iterator temp(*this);
                node = node->findSuccessor();
                return temp;
            }
            
            const_iterator &operator++() {
                if (isInvalid())throw invalid_iterator();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                node = node->findSuccessor();
                return *this;
            }
            
            const_iterator operator--(int) {
                if (isInvalid())throw invalid_iterator();
                const_iterator temp(*this);
                node = node->findPrecursor();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                return temp;
            }
            
            const_iterator &operator--() {
                if (isInvalid())throw invalid_iterator();
                node = node->findPrecursor();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                return *this;
            }
            
            value_type &operator*() const {
                if (isInvalid() || node->isHeader())throw invalid_iterator();
                return *node->value;
            }
            
            bool operator==(const iterator &o) const {
                return node == o.node;
            }
            
            bool operator==(const const_iterator &o) const {
                return node == o.node;
            }
            
            bool operator!=(const iterator &o) const {
                return node != o.node;
            }
            
            bool operator!=(const const_iterator &o) const {
                return node != o.node;
            }
            
            value_type *operator->() const {
                if (isInvalid() || node->isHeader())throw invalid_iterator();
                return node->value;
            }
        };
        
        map() : header(new Node) {
            header->parent = nullptr;
            header->left = header;
            header->right = header;
        }
        
        map(const map &o) : nodeNumber(o.nodeNumber), header(new Node) {
            header->parent = recursionConstruct(header, o.header->parent);
            header->left = o.empty() ? header : findNode(o.header->left->getKey());
            header->right = o.empty() ? header : findNode(o.header->right->getKey());
        }
        
        map &operator=(const map &o) {
            if (this == &o)return *this;
            recursionClear(header->parent);
            nodeNumber = o.nodeNumber;
            header->parent = recursionConstruct(header, o.header->parent);
            header->left = o.empty() ? header : findNode(o.header->left->getKey());
            header->right = o.empty() ? header : findNode(o.header->right->getKey());
            return *this;
        }
        
        ~map() {
            recursionClear(header->parent);
            delete header;
        }
        
        T &at(const Key &key) {
            Node *pos = findNode(key);
            if (pos == nullptr)throw index_out_of_bound();
            return pos->value->second;
        }
        
        const T &at(const Key &key) const {
            Node *pos = findNode(key);
            if (pos == nullptr)throw index_out_of_bound();
            return pos->value->second;
        }
        
        T &operator[](const Key &key) {
            Node *pos = findNode(key);
            if (pos != nullptr)return pos->value->second;
            value_type value(key, T());
            iterator temp = RedBlackTreeInsert(findInsertPos(value), value);
            return temp.node->value->second;
        }
        
        const T &operator[](const Key &key) const {
            return at(key);
        }
        
        iterator begin() {
            return iterator(header->left);
        }
        
        const_iterator cbegin() const {
            return const_iterator(header->left);
        }
        
        iterator end() {
            return iterator(header);
        }
        
        const_iterator cend() const {
            return const_iterator(header);
        }
        
        bool empty() const {
            return nodeNumber == 0;
        }
        
        size_t size() const {
            return nodeNumber;
        }
        
        void clear() {
            recursionClear(header->parent);
            header->left = header, header->right = header, header->parent = nullptr;
            nodeNumber = 0;
        }
        
        std::pair<iterator, bool> insert(const value_type &value) {
            Node *temp = findNode(value.first);
            if (temp != nullptr)return std::pair<iterator, bool>(iterator(temp), false);
            return std::pair<iterator, bool>(RedBlackTreeInsert(findInsertPos(value), value), true);
        }
        
        void erase(iterator pos) {
            if (pos == end() || pos.isInvalid() || find(pos.node->getKey()) != pos)throw runtime_error();
            if (pos.node == header->left)header->left = header->left->findSuccessor();
            if (pos.node == header->right)header->right = header->right->findPrecursor();
            RedBlackTreeErase(pos.node);
        }
        
        size_t count(const Key &key) const {
            if (findNode(key) == nullptr)return 0;
            else return 1;
        }
        
        iterator find(const Key &key) {
            Node *temp = findNode(key);
            if (temp == nullptr)return end();
            else return iterator(temp);
        }
        
        const_iterator find(const Key &key) const {
            Node *temp = findNode(key);
            if (temp == nullptr)return cend();
            else return const_iterator(temp);
        }
    };
}

#endif //CODE_MAP_H
