#ifndef BST_H
#define BST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <utility>
#include <algorithm>

template <typename Key, typename Value>
class Node
{
public:
    Node(const Key& key, const Value& value, Node<Key, Value>* parent)
        : item_(key, value), parent_(parent), left_(NULL), right_(NULL)
    {}

    virtual ~Node() {}

    const std::pair<const Key, Value>& getItem() const { return item_; }
    std::pair<const Key, Value>& getItem() { return item_; }
    const Key& getKey() const { return item_.first; }
    const Value& getValue() const { return item_.second; }
    Value& getValue() { return item_.second; }

    virtual Node<Key, Value>* getParent() const { return parent_; }
    virtual Node<Key, Value>* getLeft() const { return left_; }
    virtual Node<Key, Value>* getRight() const { return right_; }

    void setParent(Node<Key, Value>* parent) { parent_ = parent; }
    void setLeft(Node<Key, Value>* left) { left_ = left; }
    void setRight(Node<Key, Value>* right) { right_ = right; }
    void setValue(const Value& value) { item_.second = value; }

protected:
    std::pair<const Key, Value> item_;
    Node<Key, Value>* parent_;
    Node<Key, Value>* left_;
    Node<Key, Value>* right_;
};

template <typename Key, typename Value>
class BinarySearchTree
{
public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    virtual void insert(const std::pair<const Key, Value>& keyValuePair);
    virtual void remove(const Key& key);
    void clear();
    bool isBalanced() const;
    void print() const;
    bool empty() const;

    template<typename PPKey, typename PPValue>
    friend void prettyPrintBST(BinarySearchTree<PPKey, PPValue>& tree);

public:
    class iterator
    {
    public:
        iterator();
        std::pair<const Key, Value>& operator*() const;
        std::pair<const Key, Value>* operator->() const;
        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;
        iterator& operator++();

    protected:
        friend class BinarySearchTree<Key, Value>;
        iterator(Node<Key, Value>* ptr);
        Node<Key, Value>* current_;
    };

public:
    iterator begin() const;
    iterator end() const;
    iterator find(const Key& key) const;
    Value& operator[](const Key& key);
    Value const& operator[](const Key& key) const;

protected:
    Node<Key, Value>* internalFind(const Key& key) const;
    Node<Key, Value>* getSmallestNode() const;
    static Node<Key, Value>* predecessor(Node<Key, Value>* current);

    virtual void printRoot(Node<Key, Value>* r) const;
    virtual void nodeSwap(Node<Key, Value>* n1, Node<Key, Value>* n2);

    // Added helpers.  These do not alter any provided members.
    static Node<Key, Value>* successor(Node<Key, Value>* current);
    static void clearHelper(Node<Key, Value>* current);
    static int balancedHeight(Node<Key, Value>* current);

protected:
    Node<Key, Value>* root_;
};

template<class Key, class Value>
BinarySearchTree<Key, Value>::iterator::iterator(Node<Key, Value>* ptr)
    : current_(ptr)
{}

template<class Key, class Value>
BinarySearchTree<Key, Value>::iterator::iterator()
    : current_(NULL)
{}

template<class Key, class Value>
std::pair<const Key, Value>&
BinarySearchTree<Key, Value>::iterator::operator*() const
{
    return current_->getItem();
}

template<class Key, class Value>
std::pair<const Key, Value>*
BinarySearchTree<Key, Value>::iterator::operator->() const
{
    return &(current_->getItem());
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::iterator::operator==(
    const BinarySearchTree<Key, Value>::iterator& rhs) const
{
    return current_ == rhs.current_;
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::iterator::operator!=(
    const BinarySearchTree<Key, Value>::iterator& rhs) const
{
    return current_ != rhs.current_;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator&
BinarySearchTree<Key, Value>::iterator::operator++()
{
    current_ = BinarySearchTree<Key, Value>::successor(current_);
    return *this;
}

template<class Key, class Value>
BinarySearchTree<Key, Value>::BinarySearchTree()
    : root_(NULL)
{}

template<class Key, class Value>
BinarySearchTree<Key, Value>::~BinarySearchTree()
{
    clear();
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::empty() const
{
    return root_ == NULL;
}

template<class Key, class Value>
void BinarySearchTree<Key, Value>::print() const
{
    printRoot(root_);
    std::cout << "\n";
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator
BinarySearchTree<Key, Value>::begin() const
{
    return iterator(getSmallestNode());
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator
BinarySearchTree<Key, Value>::end() const
{
    return iterator(NULL);
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator
BinarySearchTree<Key, Value>::find(const Key& key) const
{
    return iterator(internalFind(key));
}

template<class Key, class Value>
Value& BinarySearchTree<Key, Value>::operator[](const Key& key)
{
    Node<Key, Value>* current = internalFind(key);
    if(current == NULL) {
        throw std::out_of_range("Invalid key");
    }
    return current->getValue();
}

template<class Key, class Value>
Value const& BinarySearchTree<Key, Value>::operator[](const Key& key) const
{
    Node<Key, Value>* current = internalFind(key);
    if(current == NULL) {
        throw std::out_of_range("Invalid key");
    }
    return current->getValue();
}

template<class Key, class Value>
void BinarySearchTree<Key, Value>::insert(
    const std::pair<const Key, Value>& keyValuePair)
{
    if(root_ == NULL) {
        root_ = new Node<Key, Value>(
            keyValuePair.first, keyValuePair.second, NULL);
        return;
    }

    Node<Key, Value>* current = root_;
    Node<Key, Value>* parent = NULL;

    while(current != NULL) {
        parent = current;
        if(keyValuePair.first < current->getKey()) {
            current = current->getLeft();
        }
        else if(current->getKey() < keyValuePair.first) {
            current = current->getRight();
        }
        else {
            current->setValue(keyValuePair.second);
            return;
        }
    }

    Node<Key, Value>* added = new Node<Key, Value>(
        keyValuePair.first, keyValuePair.second, parent);

    if(keyValuePair.first < parent->getKey()) {
        parent->setLeft(added);
    }
    else {
        parent->setRight(added);
    }
}

template<class Key, class Value>
void BinarySearchTree<Key, Value>::remove(const Key& key)
{
    Node<Key, Value>* doomed = internalFind(key);
    if(doomed == NULL) {
        return;
    }

    if(doomed->getLeft() != NULL && doomed->getRight() != NULL) {
        Node<Key, Value>* pred = predecessor(doomed);
        nodeSwap(doomed, pred);
    }

    Node<Key, Value>* child = doomed->getLeft();
    if(child == NULL) {
        child = doomed->getRight();
    }

    Node<Key, Value>* parent = doomed->getParent();
    if(child != NULL) {
        child->setParent(parent);
    }

    if(parent == NULL) {
        root_ = child;
    }
    else if(parent->getLeft() == doomed) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete doomed;
}

template<class Key, class Value>
Node<Key, Value>*
BinarySearchTree<Key, Value>::predecessor(Node<Key, Value>* current)
{
    if(current == NULL) {
        return NULL;
    }

    if(current->getLeft() != NULL) {
        current = current->getLeft();
        while(current->getRight() != NULL) {
            current = current->getRight();
        }
        return current;
    }

    Node<Key, Value>* parent = current->getParent();
    while(parent != NULL && current == parent->getLeft()) {
        current = parent;
        parent = parent->getParent();
    }
    return parent;
}

template<class Key, class Value>
Node<Key, Value>*
BinarySearchTree<Key, Value>::successor(Node<Key, Value>* current)
{
    if(current == NULL) {
        return NULL;
    }

    if(current->getRight() != NULL) {
        current = current->getRight();
        while(current->getLeft() != NULL) {
            current = current->getLeft();
        }
        return current;
    }

    Node<Key, Value>* parent = current->getParent();
    while(parent != NULL && current == parent->getRight()) {
        current = parent;
        parent = parent->getParent();
    }
    return parent;
}

template<class Key, class Value>
void BinarySearchTree<Key, Value>::clearHelper(Node<Key, Value>* current)
{
    if(current == NULL) {
        return;
    }
    clearHelper(current->getLeft());
    clearHelper(current->getRight());
    delete current;
}

template<class Key, class Value>
void BinarySearchTree<Key, Value>::clear()
{
    clearHelper(root_);
    root_ = NULL;
}

template<class Key, class Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::getSmallestNode() const
{
    Node<Key, Value>* current = root_;
    if(current == NULL) {
        return NULL;
    }

    while(current->getLeft() != NULL) {
        current = current->getLeft();
    }
    return current;
}

template<class Key, class Value>
Node<Key, Value>*
BinarySearchTree<Key, Value>::internalFind(const Key& key) const
{
    Node<Key, Value>* current = root_;
    while(current != NULL) {
        if(key < current->getKey()) {
            current = current->getLeft();
        }
        else if(current->getKey() < key) {
            current = current->getRight();
        }
        else {
            return current;
        }
    }
    return NULL;
}

template<class Key, class Value>
int BinarySearchTree<Key, Value>::balancedHeight(Node<Key, Value>* current)
{
    if(current == NULL) {
        return 0;
    }

    int leftHeight = balancedHeight(current->getLeft());
    if(leftHeight == -1) {
        return -1;
    }

    int rightHeight = balancedHeight(current->getRight());
    if(rightHeight == -1) {
        return -1;
    }

    int difference = leftHeight - rightHeight;
    if(difference < -1 || difference > 1) {
        return -1;
    }

    return 1 + std::max(leftHeight, rightHeight);
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::isBalanced() const
{
    return balancedHeight(root_) != -1;
}

template<class Key, class Value>
void BinarySearchTree<Key, Value>::nodeSwap(
    Node<Key, Value>* n1, Node<Key, Value>* n2)
{
    if(n1 == n2 || n1 == NULL || n2 == NULL) {
        return;
    }

    Node<Key, Value>* n1p = n1->getParent();
    Node<Key, Value>* n1r = n1->getRight();
    Node<Key, Value>* n1lt = n1->getLeft();
    bool n1isLeft = n1p != NULL && n1 == n1p->getLeft();

    Node<Key, Value>* n2p = n2->getParent();
    Node<Key, Value>* n2r = n2->getRight();
    Node<Key, Value>* n2lt = n2->getLeft();
    bool n2isLeft = n2p != NULL && n2 == n2p->getLeft();

    Node<Key, Value>* temp = n1->getParent();
    n1->setParent(n2->getParent());
    n2->setParent(temp);

    temp = n1->getLeft();
    n1->setLeft(n2->getLeft());
    n2->setLeft(temp);

    temp = n1->getRight();
    n1->setRight(n2->getRight());
    n2->setRight(temp);

    if(n1r != NULL && n1r == n2) {
        n2->setRight(n1);
        n1->setParent(n2);
    }
    else if(n2r != NULL && n2r == n1) {
        n1->setRight(n2);
        n2->setParent(n1);
    }
    else if(n1lt != NULL && n1lt == n2) {
        n2->setLeft(n1);
        n1->setParent(n2);
    }
    else if(n2lt != NULL && n2lt == n1) {
        n1->setLeft(n2);
        n2->setParent(n1);
    }

    if(n1p != NULL && n1p != n2) {
        if(n1isLeft) n1p->setLeft(n2);
        else n1p->setRight(n2);
    }
    if(n1r != NULL && n1r != n2) n1r->setParent(n2);
    if(n1lt != NULL && n1lt != n2) n1lt->setParent(n2);

    if(n2p != NULL && n2p != n1) {
        if(n2isLeft) n2p->setLeft(n1);
        else n2p->setRight(n1);
    }
    if(n2r != NULL && n2r != n1) n2r->setParent(n1);
    if(n2lt != NULL && n2lt != n1) n2lt->setParent(n1);

    if(root_ == n1) root_ = n2;
    else if(root_ == n2) root_ = n1;
}

// The assignment already supplies print_bst.h.
#include "print_bst.h"

#endif
