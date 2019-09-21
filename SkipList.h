#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>

namespace Redis
{

const int ZSKIPLIST_MAXLEVEL  = 64;     /* Should be enough for 2^64 elements */
const double  ZSKIPLIST_P  = 0.25;      /* Skiplist P = 1/4 */

template <typename T>
struct SkipListNode
{
    std::vector<SkipListNode *> mLevels;
    SkipListNode* mBackword;
    T mValue;
    double mScore;
    int mSpan;
    SkipListNode(const T& v, double s): mValue(v), mScore(s){}
};

template <typename T>
class SkipList
{
public:
    SkipList();
    ~SkipList();
    SkipList(const SkipList &skiplist) = delete;
    SkipList(SkipList &&skiplist) = delete;
    SkipList &operator=(const SkipList &skiplist) = delete;
    SkipList &operator=(SkipList &&skiplist) = delete;

    int getMaxLevel() { return mMaxLevel; }
    void setMaxLevel(int ml) { mMaxLevel = ml; }
    int getLength() { return mLength; }

    bool insert(const T &v, double mScore);
    SkipListNode<T> *query(double s);
    std::vector<SkipListNode<T>> queryRange(double minmScore, double maxmScore);
private:
    int generateRandomLevel(void);
private:
    SkipListNode<T> *mHead, *mTail;
    int mMaxLevel;
    int mLength;
};

template<typename T>
SkipList<T>::SkipList(): mHead(nullptr), mTail(nullptr), mMaxLevel(0), mLength(0) {
    
}


template<typename T>
SkipList<T>::~SkipList() {

}

template<typename T>
int SkipList<T>::generateRandomLevel(void) {
    int level = 1;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

template<typename T>
bool SkipList<T>::insert(const T& v, double s){
    auto newNode = new SkipListNode(v, s);

}

template<typename T>
SkipListNode<T>* SkipList<T>::query(double s){

}

template<typename T>    
std::vector<SkipListNode<T>> SkipList<T>::queryRange(double minmScore, double maxmScore);

}; // namespace Redis

#endif
