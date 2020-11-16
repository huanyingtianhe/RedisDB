#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include <limits>
#include <iterator>
#include <cstdlib>
#include "redisIterator.h"

namespace RedisDataStructure
{
    const int ZSKIPLIST_MAXLEVEL = 64; /* Should be enough for 2^64 elements */
    const double ZSKIPLIST_P = 0.25;   /* Skiplist P = 1/4 */
    const double EP = 1e-6;

    template <typename Key, typename Value>
    struct SkipListNode
    {
        std::vector<SkipListNode *> mNexts;
        SkipListNode *mBackword;
        Value mValue;
        Key mKey;
        std::vector<int> mSpans;
        SkipListNode(const Key &k, const Value &v, int l) : mBackword(nullptr), mKey(k), mValue(v)
        {
            setLevelNum(l);
        }
        SkipListNode(const Key &k, const Value &v) : mKey(k), mValue(v), mBackword(nullptr) {}
        void setLevelNum(int l)
        {
            mNexts.resize(l, nullptr);
            mSpans.resize(l, 0);
        }

        SkipListNode<Key, Value>* next(){
            return this->mNexts[0];
        }

        SkipListNode<Key, Value>* prev(){
            return this->mBackword;
        }

        void printNode()
        {
            std::cout << "key: " << mKey << ", value: " << mValue << std::endl;
            std::cout << "----------------------------" << std::endl;

            for (int i = mNexts.size() - 1; i >= 0; i--)
            {
                std::cout << "| span: " << mSpans[i] << " | next: " << mNexts[i] << " |" << std::endl;
            }
            std::cout << "----------------------------" << std::endl;
        }
    };

    //SkipList do allow duplicate key
    template <typename Key, typename Value>
    class SkipList
    {
    public:
        using iterator = RedisBidirectionalIterator<SkipListNode<Key, Value>>;
        using const_iterator = RedisBidirectionalIterator<const SkipListNode<Key, Value>>;
        using reverse_iterator = RedisBidirectionalReverseIterator<SkipListNode<Key, Value>>;
        using const_reverse_iterator = RedisBidirectionalReverseIterator<const SkipListNode<Key, Value>>;
        SkipList();
        ~SkipList() noexcept;
        SkipList(const SkipList<Key, Value> &skiplist) = delete;
        SkipList(SkipList<Key, Value> &&skiplist) = delete;
        SkipList &operator=(const SkipList<Key, Value> &skiplist) = delete;
        SkipList &operator=(SkipList<Key, Value> &&skiplist) = delete;
        //getter and setter
        int getMaxLevel() { return mMaxLevel; }
        void setMaxLevel(int ml) { mMaxLevel = ml; }
        int getLength() { return mLength; }
        //basic operation: insert, erase, query.
        bool insert(const Key &k, const Value &v);
        bool erase(const Key &k);
        SkipListNode<Key, Value> *lower_bound(const Key &k);
        SkipListNode<Key, Value> *upper_bound(const Key &k);
        int getRank(const Key &k);
        //iterators of skiplist
        iterator begin() { return iterator(mHead->mNexts[0]); }
        iterator end() { return iterator(mTail); }
        const_iterator cbegin() { return const_iterator(mHead->mNexts[0]); }
        const_iterator cend() { return const_iterator(mTail); }
        reverse_iterator rbegin() { return reverse_iterator(mTail->mBackword); }
        reverse_iterator rend() { return reverse_iterator(mHead); }
        const_reverse_iterator crbegin() { return const_reverse_iterator(mTail->mBackword); }
        const_reverse_iterator crend() { return const_reverse_iterator(mHead); }
        void printList();

    private:
        //generate random level
        int generateRandomLevel(void);
        //only check the start of the list, because tail is not valid.
        bool isInRange(const Key &k);
        bool doubleEqual(double d1, double d2) { return fabs(d1 - d2) < ep; }

    private:
        SkipListNode<Key, Value> *mHead, *mTail; //mhead and mtail is dummy node for code elegant
        int mMaxLevel;                           //mMaxLevel starts from 0
        int mLength;
    };

    template <typename Key, typename Value>
    SkipList<Key, Value>::SkipList() : mMaxLevel(0), mLength(0)
    {
        Key k;
        Value v;
        mHead = new SkipListNode<Key, Value>(k, v);
        mTail = new SkipListNode<Key, Value>(k, v);
        mHead->setLevelNum(1);
        mTail->setLevelNum(1);
        mHead->mNexts[0] = mTail;
        mTail->mBackword = mHead;
    }

    template <typename Key, typename Value>
    SkipList<Key, Value>::~SkipList() noexcept
    {
        for (auto it = mHead; it != nullptr;)
        {
            auto next = it->mNexts[0];
            delete it;
            it = next;
        }
        mHead = nullptr;
        mTail = nullptr;
    }

    template <typename Key, typename Value>
    int SkipList<Key, Value>::generateRandomLevel()
    {
        int level = 0;
        while ((std::rand() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
            level += 1;
        return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL - 1;
    }

    template <typename Key, typename Value>
    bool SkipList<Key, Value>::insert(const Key &k, const Value &v)
    {
        int level = generateRandomLevel();
        auto newNode = new SkipListNode<Key, Value>(k, v, level + 1);
        SkipListNode<Key, Value> *nd = this->mHead;
        int newMaxLevel = std::max(this->mMaxLevel, level);
        std::vector<SkipListNode<Key, Value> *> prevs(newMaxLevel + 1, nullptr);
        std::vector<int> ranks(newMaxLevel + 1, 0);
        //find the node just before insert node in each level
        int currLevel = this->mMaxLevel;
        while (currLevel >= 0)
        {
            //because of the mtail is specified, we need not to do next pointer null judge
            if (nd->mNexts[currLevel] == mTail || nd->mNexts[currLevel]->mKey > k)
            {
                prevs[currLevel] = nd;
                currLevel--;
                if (currLevel >= 0)
                    ranks[currLevel] = ranks[currLevel + 1];
            }
            else
            {
                ranks[currLevel] += nd->mSpans[currLevel];
                nd = nd->mNexts[currLevel];
            }
        }
        //add pre node for the level who is higher than curr max level.
        if (level > this->mMaxLevel)
        {
            mHead->setLevelNum(level + 1);
            for (int i = level; i > this->mMaxLevel; i--)
            {
                prevs[i] = mHead;
                mHead->mNexts[i] = mTail;
                mHead->mSpans[i] = this->mLength;
            }
        }
        //update pointer and span for each pre, insert node. update the backward pointer.
        for (int i = 0; i <= newMaxLevel; i++)
        {
            auto next = prevs[i]->mNexts[i];
            if (i <= level)
            {
                newNode->mSpans[i] = prevs[i]->mSpans[i] - (ranks[0] - ranks[i]);
                prevs[i]->mSpans[i] = ranks[0] - ranks[i] + 1;
                prevs[i]->mNexts[i] = newNode;
                newNode->mNexts[i] = next;
            }
            else
            {
                prevs[i]->mSpans[i]++;
            }
        }
        //update backword pointer for newNode and the node after new node.
        newNode->mBackword = prevs[0];
        newNode->mNexts[0]->mBackword = newNode;
        //update max level
        this->mMaxLevel = newMaxLevel;
        //update list node len
        this->mLength += 1;
        return true;
    }

    template <typename Key, typename Value>
    bool SkipList<Key, Value>::erase(const Key &k)
    {
        SkipListNode<Key, Value> *nd = this->mHead;
        std::vector<SkipListNode<Key, Value> *> prevs(this->mMaxLevel + 1, nullptr);
        //find the node just before insert node in each level
        int currLevel = this->mMaxLevel;
        while (currLevel >= 0)
        {
            //because of the mtail infinate large, we need not to do next pointer null judge
            if (nd->mNexts[currLevel] == mTail || nd->mNexts[currLevel]->mKey >= k)
            {
                prevs[currLevel] = nd;
                currLevel--;
            }
            else
            {
                nd = nd->mNexts[currLevel];
            }
        }
        //if node not exist return
        nd = nd->mNexts[0];
        if (nd == mTail || nd->mKey != k)
            return false;
        //update pointer and span for each pre, insert node.
        for (int i = 0; i <= this->mMaxLevel; i++)
        {
            if (prevs[i]->mNexts[i] == nd)
            {
                prevs[i]->mSpans[i] += nd->mSpans[i] - 1;
                prevs[i]->mNexts[i] = nd->mNexts[i];
            }
            else
            {
                prevs[i]->mSpans[i]--;
            }
        }
        //update the backward pointer.
        nd->mNexts[0]->mBackword = prevs[0];
        //if the highlist level is null, we need shrink the list maxLevel.
        while (this->mMaxLevel > 0 && this->mHead->mNexts[this->mMaxLevel] == mTail)
            this->mMaxLevel--;
        //update list len
        this->mLength--;
        return true;
    }

    template <typename Key, typename Value>
    bool SkipList<Key, Value>::isInRange(const Key &k)
    {
        auto first = this->mHead->mNexts[0];
        if (k < first->mKey)
            return false;
        auto last = this->mTail->mBackword;
        if (k > last->mKey)
            return false;
        return true;
    }

    template <typename Key, typename Value>
    SkipListNode<Key, Value> *SkipList<Key, Value>::lower_bound(const Key &k)
    {
        if (!isInRange(k))
            return nullptr;
        SkipListNode<Key, Value> *nd = this->mHead;
        for (int level = this->mMaxLevel; level >= 0;)
        {
            SkipListNode<Key, Value> *next = nd->mNexts[level];
            if (next == mTail || next->mKey >= k)
            {
                level--;
                continue;
            }
            else
            {
                nd = next;
            }
        }
        return nd->mNexts[0];
    }

    template <typename Key, typename Value>
    SkipListNode<Key, Value> *SkipList<Key, Value>::upper_bound(const Key &k)
    {
        if (!isInRange(k))
            return nullptr;
        SkipListNode<Key, Value> *nd = this->mHead;
        for (int level = this->mMaxLevel; level >= 0;)
        {
            SkipListNode<Key, Value> *next = nd->mNexts[level];
            if (next == mTail || next->mKey > k)
            {
                level--;
                continue;
            }
            else
            {
                nd = next;
            }
        }
        return nd->mNexts[0];
    }

    template <typename Key, typename Value>
    int SkipList<Key, Value>::getRank(const Key &k)
    {
        int result = 0;
        SkipListNode<Key, Value> *nd = this->mHead;
        for (int level = this->mMaxLevel; level >= 0;)
        {
            SkipListNode<Key, Value> *next = nd->mNexts[level];
            if (next == mTail || next->mKey > k)
            {
                level--;
            }
            else
            {
                result += nd->mSpans[level];
                nd = next;
            }
        }
        return nd->mKey == k ? result : 0;
    }

    template <typename Key, typename Value>
    void SkipList<Key, Value>::printList()
    {
        this->mHead->printNode();
        for (auto it = this->begin(); it != this->end(); it++)
        {
            it->printNode();
        }
    }

}; // namespace RedisDataStructure

#endif