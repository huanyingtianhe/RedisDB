#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>

template <typename T>
class SkipListNode{
    std::vector<SkipListNode*> levels;
    T value;
    double point;
    int span;
};

template <typename T>
class SkipList{
public:
    SkipList(): head(nullptr), tail(nullptr), maxLevel(0) {}
    int getMaxLevel(){ return maxLevel;}
    int setMaxLevel(int ml) {maxLevel = ml;}
    bool insert(T& v);
    SkipListNode<T>* query(double point);

private:
    SkipListNode<T> *head, *tail;
    int maxLevel;
};   


#endif
