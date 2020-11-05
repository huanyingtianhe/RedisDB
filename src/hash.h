#ifndef HASH_H
#define HASH_H

#include <cstdlib>

namespace RedisDataStructure{
    template<typename K, typename V>
    struct HashNode{
        K mKey;
        V mValue;
        HashNode* mNext;
        HashNode(const K& k, const V& v): mKey(k), mValue(v), mNext(nullptr){}
        HashNode(const K& k, const V& v, HashNode* n): mKey(k), mValue(v), mNext(n){}
    };
    

    template<typename K, typename V>
    struct HashTable{
        HashNode<K, V>** mNodes;
        int mSize;
        int mBucketsNumber;
        HashTable(int buck): mSize(0){
            mNodes = new HashNode<K, V>*[buck];
            fill(mNodes[0], mNodes[0]+buck, nullptr);
        }

        bool insertNode(int index, HashNode* node){
            if(index < mBucketsNumber){
                auto next = mNodes[index];
                mNodes[index] = node;
                node->mNext = next;
                return true;
            }else{
                reutrn false;
            }
        }
    };
    template<typename K, typename V, typename hashFunction>
    class Hash{
    public:
        Hash():mBucketsNumber(4), mBucketsMask(3), mRehashIndex(-1), mLoadFactor(0.75){}
        bool isReHashing(){
            return mRehashIndex != -1 || mActive.mSize >= mActive.mBucketsNumber * mLoadFactor;
        }

        void resetReHashing(){
            mRehashIndex = 0;
        }

        void compeleteReHashing(){
            mRehashIndex = -1;
        }

        void insert(const K& key, const V& v){

        }

    private:
        HashTable<K, V> mActive;
        HashTable<K, V> mBackup;
        int mBucketsNumber;
        int mBucketsMask;
        int mRehashIndex;
        float mLoadFactor;
    
    };
};

#endif
