#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include <limits>

namespace Redis
{

const int ZSKIPLIST_MAXLEVEL  = 64;     /* Should be enough for 2^64 elements */
const double  ZSKIPLIST_P  = 0.25;      /* Skiplist P = 1/4 */
const double ep = 1e-6;

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
    using  iterator = Iterator<T>;
    SkipList();
    ~SkipList()noexcept;
    SkipList(const SkipList &skiplist) = delete;
    SkipList(SkipList &&skiplist) = delete;
    SkipList &operator=(const SkipList &skiplist) = delete;
    SkipList &operator=(SkipList &&skiplist) = delete;

    int getMaxLevel() { return mMaxLevel; }
    void setMaxLevel(int ml) { mMaxLevel = ml; }
    int getLength() { return mLength; }

    bool insert(const T &v, double score);
    bool erase(const T &v, double scroe);
    SkipListNode<T>* queryFirstInRange(double minScore, double maxScore);
    SkipListNode<T>* queryLastInRange(double minScore, double maxScore);
    int getRank(double score, const T& v);

    iterator begin();
    iterator end();
    iterator next();
private:
    int generateRandomLevel(void);
    //only check the start of the list, because tail is not valid.
    bool isInRange(double minScore, double maxScore);
    bool doubleEqual(double d1, double d2) {return fabs(d1 - d2) < ep;}
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
}


template<typename T>
SkipList<T>::~SkipList()noexcept {
    if(mHead) delete mHead;
    mHead = nullptr;
    if(mTail) delete mTail;
    mTail = nullptr;
}

template<typename T>
int SkipList<T>::generateRandomLevel() {
    int level = 0;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

template<typename T>
bool SkipList<T>::insert(const T& v, double s){
    int level = generateRandomLevel();
    auto newNode = new SkipListNode(v, s, level);
    SkipListNode* nd = this->mHead;
    int newMaxLevel = std::max(this->mMaxLevel, level);
    vector<SkipListNode*> prevs(newMaxLevel, nullptr);
    vector<int> ranks(newMaxLevel, 0);
    //find the node just before insert node in each level
    int currLevel = this->mMaxLevel;
    while(currLevel >= 0) {
        //because of the mtail infinate large, we need not to do next pointer null judge 
        if(nd->mScore <= s && nd->mNexts[currLevel]->mScore > v){
            prevs[currLevel] = nd;
            currLevel--;
        }else{
            ranks[currLevel] += nd->mSpans[currLevel];
            nd = nd->mNexts[currLevel];
        }
    }

    //add pre node for the level who is higher than curr max level.
    if(level > this->mMaxLevel){
        mHead->setLevelNum(level);
        for(int i = level + 1; i > this->mMaxLevel; i--){
            prevs[i] = mHead;
            mHead->mNexts[i] = mTail;
        }
    }

    //update pointer and span for each pre, insert node. update the backward pointer.
    for(int i = 0; i <= newMaxLevel; i++){
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
bool SkipList<T>::erase(const T& v, double s){
    int level = generateRandomLevel();
    SkipListNode* nd = this->mHead;
    int newMaxLevel = std::max(this->mMaxLevel, level);
    vector<SkipListNode*> prevs(newMaxLevel, nullptr);
    //find the node just before insert node in each level
    int currLevel = this->mMaxLevel;
    while(currLevel >= 0) {
        //because of the mtail infinate large, we need not to do next pointer null judge 
        if(nd->mScore <= s && nd->mNexts[currLevel]->mScore > v){
            prevs[currLevel] = nd;
            currLevel--;
        }else{
            nd = nd->mNexts[currLevel];
        }
    }
    if(nd->mScore != s || nd->mValue != v) return false;
    //add pre node for the level who is higher than curr max level.
    if(level > this->mMaxLevel){
        mHead->setLevelNum(level);
        for(int i = level+1; i > this->mMaxLevel; i--){
            prevs[i] = mHead;
            mHead->mNexts[i] = mTail;
        }
    }

    //update pointer and span for each pre, insert node. update the backward pointer.
    for(int i = 0; i <= newMaxLevel; i++){
        if(prevs[i]->mNexts[i] == nd){
            prevs[i]->mSpans[i] += nd->mSpans[i] - 1;
            prevs[i]->mNexts[i] = nd->mNexts[i];
            nd->mNexts[i]->mBackword = prevs[i];
        }else{
            prevs[i]->mSpans[i]--;
        }
    }
    //if the highlist level is null, we need shrink the list maxLevel.
    while(this->mMaxLevel >= 0 && this->mHead->mNexts[this->mMaxLevel] == mTail) this->mMaxLevel--;
    this->mLength -= 1;
    reutrn true;
}

template<typename T>
bool SkipList<T>::isInRange(double minScore, double maxScore){
    if(minScore > maxScore) return false;
    SkipListNode<T> *first = this->mHead->mNexts[0];
    if(maxScore < first->mValue) return false;
    return true;
}


template<typename T>
SkipListNode<T>* SkipList<T>::queryFirstInRange(double minScore, double maxScore){
    if(!isInRange(minScore, maxScore)) return nullptr;
    SkipListNode<T>* nd = this->mHead;
    for(int level = this->mMaxLevel; level >= 0; ){
        SkipListNode<T>* next = nd->mNexts[level];
        if(!next) break;
        if(next->mScore >= minScore && next->mScore < maxScore){
            level--;
            continue;
        }else if(next->mScore >= maxScore){
            break;
        }else{
            nd = next;
        }
    }
    return nd ? ((nd->mScore >= minScore && nd->mScore < maxScore) ? nd : nullptr) : nullptr;
}

template<typename T>
SkipListNode<T>* SkipList<T>::queryLastInRange(double minmScore, double maxmScore){
    if(!isInRange(minScore, maxScore)) return nullptr;
    SkipListNode<T>* nd = this->mHead;
    for(int level = this->mMaxLevel; level >= 0;){
        SkipListNode<T>* next = nd->mNexts[level];
        if(!next) break;
        if(next->mScore >= maxScore){
            level--;
            continue;
        }else{
            nd = next;
        }
    }
    return nd ? ((nd->mScore >= minScore && nd->mScore < maxScore) ? nd : nullptr) : nullptr;
}

template<typename T>
int SkipList<T>::getRank(double score, const T& v){
    int result = 0;
    SkipListNode<T>* nd = this->mHead;
    for(int level = this->mMaxLevel; level >= 0; ){
        SkipListNode<T>* next = nd->mNexts[level];
        if(doubleEqual(next->mScore, score)){
            if(next->mValue == v){
                result += nd->mSpans[level];
                return result;
            }
            else{
                level--;
                continue;
            }
        }else if(next->mScore < score){
            result += nd->mSpans[level];
            nd = next;
        }else{
            return 0;
        }

    }
    return 0;
}


}; // namespace Redis

#endif
