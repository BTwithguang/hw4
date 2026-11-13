#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError {};

template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent)
        : Node<Key, Value>(key, value, parent), balance_(0)
    {}

    virtual ~AVLNode() {}

    int8_t getBalance() const { return balance_; }
    void setBalance(int8_t balance) { balance_ = balance; }
    void updateBalance(int8_t diff) { balance_ += diff; }

    virtual AVLNode<Key, Value>* getParent() const override
    {
        return static_cast<AVLNode<Key, Value>*>(this->parent_);
    }

    virtual AVLNode<Key, Value>* getLeft() const override
    {
        return static_cast<AVLNode<Key, Value>*>(this->left_);
    }

    virtual AVLNode<Key, Value>* getRight() const override
    {
        return static_cast<AVLNode<Key, Value>*>(this->right_);
    }

protected:
    int8_t balance_; // right subtree height - left subtree height
};

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value>& newItem);
    virtual void remove(const Key& key);

protected:
    virtual void nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2);

    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* node, int8_t diff);
};

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getRight();
    AVLNode<Key, Value>* middle = pivot->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    pivot->setParent(parent);
    if(parent == NULL) {
        this->root_ = pivot;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(pivot);
    }
    else {
        parent->setRight(pivot);
    }

    pivot->setLeft(node);
    node->setParent(pivot);
    node->setRight(middle);
    if(middle != NULL) {
        middle->setParent(node);
    }

    return pivot;
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getLeft();
    AVLNode<Key, Value>* middle = pivot->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    pivot->setParent(parent);
    if(parent == NULL) {
        this->root_ = pivot;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(pivot);
    }
    else {
        parent->setRight(pivot);
    }

    pivot->setRight(node);
    node->setParent(pivot);
    node->setLeft(middle);
    if(middle != NULL) {
        middle->setParent(node);
    }

    return pivot;
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert(
    const std::pair<const Key, Value>& newItem)
{
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(
            newItem.first, newItem.second, NULL);
        return;
    }

    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while(current != NULL) {
        parent = current;
        if(newItem.first < current->getKey()) {
            current = current->getLeft();
        }
        else if(current->getKey() < newItem.first) {
            current = current->getRight();
        }
        else {
            current->setValue(newItem.second);
            return;
        }
    }

    AVLNode<Key, Value>* added = new AVLNode<Key, Value>(
        newItem.first, newItem.second, parent);

    if(newItem.first < parent->getKey()) {
        parent->setLeft(added);
    }
    else {
        parent->setRight(added);
    }

    insertFix(parent, added);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(
    AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child)
{
    while(parent != NULL) {
        if(child == parent->getLeft()) {
            parent->updateBalance(-1);
        }
        else {
            parent->updateBalance(1);
        }

        int8_t balance = parent->getBalance();
        if(balance == 0) {
            return;
        }

        if(balance == -1 || balance == 1) {
            child = parent;
            parent = parent->getParent();
            continue;
        }

        if(balance == -2) {
            AVLNode<Key, Value>* left = parent->getLeft();

            if(left->getBalance() <= 0) {
                rotateRight(parent);
                parent->setBalance(0);
                left->setBalance(0);
            }
            else {
                AVLNode<Key, Value>* middle = left->getRight();
                int8_t middleBalance = middle->getBalance();

                rotateLeft(left);
                rotateRight(parent);

                if(middleBalance == -1) {
                    parent->setBalance(1);
                    left->setBalance(0);
                }
                else if(middleBalance == 0) {
                    parent->setBalance(0);
                    left->setBalance(0);
                }
                else {
                    parent->setBalance(0);
                    left->setBalance(-1);
                }
                middle->setBalance(0);
            }
            return;
        }

        AVLNode<Key, Value>* right = parent->getRight();
        if(right->getBalance() >= 0) {
            rotateLeft(parent);
            parent->setBalance(0);
            right->setBalance(0);
        }
        else {
            AVLNode<Key, Value>* middle = right->getLeft();
            int8_t middleBalance = middle->getBalance();

            rotateRight(right);
            rotateLeft(parent);

            if(middleBalance == -1) {
                parent->setBalance(0);
                right->setBalance(1);
            }
            else if(middleBalance == 0) {
                parent->setBalance(0);
                right->setBalance(0);
            }
            else {
                parent->setBalance(-1);
                right->setBalance(0);
            }
            middle->setBalance(0);
        }
        return;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* doomed = static_cast<AVLNode<Key, Value>*>(
        this->internalFind(key));
    if(doomed == NULL) {
        return;
    }

    if(doomed->getLeft() != NULL && doomed->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            BinarySearchTree<Key, Value>::predecessor(doomed));
        nodeSwap(doomed, pred);
    }

    AVLNode<Key, Value>* parent = doomed->getParent();
    AVLNode<Key, Value>* child = doomed->getLeft();
    if(child == NULL) {
        child = doomed->getRight();
    }

    int8_t diff = 0;
    if(parent != NULL) {
        diff = doomed == parent->getLeft() ? 1 : -1;
    }

    if(child != NULL) {
        child->setParent(parent);
    }

    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == doomed) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete doomed;

    if(parent != NULL) {
        removeFix(parent, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(
    AVLNode<Key, Value>* node, int8_t diff)
{
    while(node != NULL) {
        AVLNode<Key, Value>* parent = node->getParent();
        int8_t nextDiff = 0;
        if(parent != NULL) {
            nextDiff = node == parent->getLeft() ? 1 : -1;
        }

        node->updateBalance(diff);
        int8_t balance = node->getBalance();

        if(balance == -1 || balance == 1) {
            return;
        }

        if(balance == 0) {
            node = parent;
            diff = nextDiff;
            continue;
        }

        if(balance == -2) {
            AVLNode<Key, Value>* left = node->getLeft();
            int8_t leftBalance = left->getBalance();

            if(leftBalance <= 0) {
                rotateRight(node);

                if(leftBalance == 0) {
                    node->setBalance(-1);
                    left->setBalance(1);
                    return;
                }

                node->setBalance(0);
                left->setBalance(0);
            }
            else {
                AVLNode<Key, Value>* middle = left->getRight();
                int8_t middleBalance = middle->getBalance();

                rotateLeft(left);
                rotateRight(node);

                if(middleBalance == -1) {
                    node->setBalance(1);
                    left->setBalance(0);
                }
                else if(middleBalance == 0) {
                    node->setBalance(0);
                    left->setBalance(0);
                }
                else {
                    node->setBalance(0);
                    left->setBalance(-1);
                }
                middle->setBalance(0);
            }

            node = parent;
            diff = nextDiff;
            continue;
        }

        AVLNode<Key, Value>* right = node->getRight();
        int8_t rightBalance = right->getBalance();

        if(rightBalance >= 0) {
            rotateLeft(node);

            if(rightBalance == 0) {
                node->setBalance(1);
                right->setBalance(-1);
                return;
            }

            node->setBalance(0);
            right->setBalance(0);
        }
        else {
            AVLNode<Key, Value>* middle = right->getLeft();
            int8_t middleBalance = middle->getBalance();

            rotateRight(right);
            rotateLeft(node);

            if(middleBalance == -1) {
                node->setBalance(0);
                right->setBalance(1);
            }
            else if(middleBalance == 0) {
                node->setBalance(0);
                right->setBalance(0);
            }
            else {
                node->setBalance(-1);
                right->setBalance(0);
            }
            middle->setBalance(0);
        }

        node = parent;
        diff = nextDiff;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(
    AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempBalance = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempBalance);
}

#endif
