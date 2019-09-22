#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include <limits>

namespace Redis
{

const int ZSKIPLIST_MAXLEVEL  = 64;     /* Should be enough for 2^64 elements */
const double  ZSKIPLIST_P  = 0.25;      /* Skiplist P = 1/4 */

template <typename T>
struct SkipListNode
{
    std::vector<SkipListNode *> mNexts;
    SkipListNode* mBackword;
    T mValue;
    double mScore;
    std::vector<int> mSpans;
    SkipListNode(const T& v, double s, int l): mBackword(nullptr), mValue(v), mScore(s){
        mNexts.resize(l);
        mSpans.resize(l);
    }
    SkipListNode(const T&v, double s): mValue(v), mBackword(nullptr), mScore(s){}
    void setLevelNum(int l){ 
        mNexts.resize(l);
        mSpans.resize(l);
    }
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
    SkipListNode<T> *mHead, *mTail; //mhead and mtail is dummy node for code elegant
    int mMaxLevel;
    int mLength;
};

template<typename T>
SkipList<T>::SkipList(): mMaxLevel(0), mLength(0) {
    T a;
    mHead = new SkipListNode<T>(a, std::numeric_limits<double>::min());
    mTail = new SkipListNode<T>(a, std::numeric_limits<double>::max());
    mHead->setLevelNum(1);
    mHead->mNexts[0] = mTail;
    mTail->setLevelNum(1);
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
    int level = generateRandomLevel();
    auto newNode = new SkipListNode(v, s, level);
    int currLevel = this->mMaxLevel;
    SkipListNode* nd = this->mHead;
    vector<SkipListNode*> prevs(level, nullptr);
    vector<int> ranks(level, 0);
    //find the node just before insert node in each level
    while(currLevel >= 0 && nd) {
        if(nd->mValue <= v && nd->mNexts[currLevel]->mValue > v){
            if(currLevel <= level){
                prevs[currLevel] = nd;
            }
            currLevel--;
        }else{
            ranks[currLevel] += nd->mSpans[currLevel];
            nd = nd->mNexts[currLevel];
        }
    }

    //add pre node for the level who is higher than curr max level.
    if(level > this->mMaxLevel){
        mHead->setLevelNum(level);
        for(int i = level; i > this->mMaxLevel; i--){
            prevs[i].levels[i] = mHead;
        }
    }

    //update pointer and span for each pre, insert node. update the backward pointer.
    for(int i = 0; i <= level; i++){
        auto next = prevs[i]->mNexts[i];
        prevs[i]->mNexts[i] = newNode;
        newNode->mNexts[i] = next;
        newNode->mSpans[i] = prevs[i]->mSpans[i] - (ranks[0] - ranks[i]) ;
        prevs[i]->mSpans[i] = ranks[0] - ranks[i] + 1;
        next->mBackword = newNode;
        newNode->mBackword = prevs[i];
    }
    this->mLength += 1;
    reutrn true;
}

template<typename T>
SkipListNode<T>* SkipList<T>::query(double s){

}

template<typename T>    
std::vector<SkipListNode<T>> SkipList<T>::queryRange(double minmScore, double maxmScore){

}

}; // namespace Redis

#endif
