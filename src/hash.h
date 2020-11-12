#ifndef HASH_H
#define HASH_H

#include <cstdlib>
#include <string>
#include "Util.h"
#include <stdexcept>
#include <algorithm>

namespace RedisDataStructure
{
    template <typename K, typename V>
    struct HashNode: public Util::noncopyable
    {
        K mKey;
        V mValue;
        HashNode *mNext;
        HashNode(const K &k, const V &v) : mKey(k), mValue(v), mNext(nullptr) {}
        HashNode(const K &k, const V &v, HashNode *n) : mKey(k), mValue(v), mNext(n) {}
    };

    template <typename K, typename V>
    struct HashTable: public Util::noncopyable
    {
        HashTable(size_t buck) : mSize(0), mBucketsNumber(buck)
        {
            mNodes = new HashNode<K, V> *[buck];
            //fill(mNodes[0], mNodes[0] + buck, nullptr);
            for(int i = 0; i < buck; i++) mNodes[i] = nullptr;
        }

        ~HashTable(){
            delete []mNodes;
        }

        HashNode<K, V>* getBucketHead(size_t index){
            return mNodes[index];
        }

        HashNode<K, V>* get(size_t index, const K& key){
            auto head = mNodes[index];
            while(head){
                if(head->mKey == key){
                    return head;
                }
                head = head->mNext;
            }
            return nullptr;
        }

        void resetBucket(size_t index){
            mNodes[index] = nullptr;
        }

        bool insertNode(size_t index, HashNode<K, V> *node)
        {
            if (index < mBucketsNumber)
            {
                if(!mNodes[index]){
                    mNodes[index] = node;
                    node->mNext = nullptr;
                }else{
                    node->mNext = mNodes[index];
                    mNodes[index] = node;
                }
                mSize++;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool eraseNode(size_t index, const K& key){
            auto head = mNodes[index];
            if(!head) return false;
            if(head->mKey == key){
                mNodes[index] = mNodes[index]->mNext;
                delete head;
                mSize--;
                return true;
            }else{
                while(head->mNext){
                    if(head->mNext->mKey == key){
                        auto tmp = head->mNext;
                        head->mNext = head->mNext->mNext;
                        delete tmp;
                        mSize--;
                        return true;
                    }
                    head = head->mNext;
                }
            }
            return false;
        }

        void printHashTable(){
            for(int i = 0; i < mBucketsNumber; i++){
                std::cout<<"bucket "<<i<<" : ";
                auto head = mNodes[i];
                if(!head){
                    std::cout<<"nullptr"<<std::endl;
                }else{
                    while(head){
                        std::cout<< head->mKey<<","<< head->mValue<<"; ";
                        head = head->mNext;
                    }
                    std::cout<<std::endl;
                }
                
            }
        }
        HashNode<K, V> **mNodes;
        size_t mSize;
        size_t mBucketsNumber;
    };

    //this hash function comes from microsoft/stl
    template <typename Key>
    class HashFunction{
    public:
        std::size_t operator()(const Key& k) const
        {
            if constexpr(std::is_null_pointer_v<Key>){
                return 0xdeadbeefu;
            }else if constexpr (std::is_pointer_v<Key>) {
                return reinterpret_cast<size_t>(k) ^ 0xdeadbeefu;
            } else {
                return static_cast<size_t>(k) ^ 0xdeadbeefu;
            }
        }
    };

    //times33 hash function for string
    template <>
    class HashFunction<std::string> {
    public:
        std::size_t operator()(const std::string& k) const
        {
            size_t hash = 5381;
            size_t len = k.size();
            auto kIt = k.begin();
            while(len != 0){
                hash = ((hash << 5) + hash) + *kIt;
                len--;
                kIt++;
            }
            return hash;
        }
    };

    //remove copy constructor and copy assignment support
    template <typename K, typename V, typename Hasher = HashFunction<K>>
    class Hash: public Util::noncopyable
    {
    public:
        Hash() : mBucketsNumber(4), mBucketsMask(3), mRehashIndex(-1), mLoadFactor(1.0f), mShrinkFactor(0.1f) {
            mActive = new HashTable<K, V>(mBucketsNumber);
            mBackup = nullptr;
        }
        ~Hash(){
            if(mActive) delete mActive;
            if(mBackup) delete mBackup;
        }
        //add move constructor and move assignment support
        Hash(const Hash<K, V, Hasher>&& h): mActive(h.mActive), mBackup(h.mBackup),{
            mBucketsNumber = h.mBucketsNumber;
            mBucketsMask = h.mBucketsMask;
            mRehashIndex = h.mRehashIndex;
            mLoadFactor = h.mLoadFactor;
            mShrinkFactor = h.mShrinkFactor;
            h.mActive = nullptr;
            h.mBackup = nullptr;
        }
        Hash<K, V, Hasher>& operator=(const Hash<K, V, Hasher>&& h){
            mActive = h.mActive;
            mBackup = h.mBackup;
            mBucketsNumber = h.mBucketsNumber;
            mBucketsMask = h.mBucketsMask;
            mRehashIndex = h.mRehashIndex;
            mLoadFactor = h.mLoadFactor;
            mShrinkFactor = h.mShrinkFactor;
            h.mActive = nullptr;
            h.mBackup = nullptr;
        }
    private:
        bool isReHashing()
        {
            return mRehashIndex != -1;
        }

        bool isNeedReHash(){
            return mActive->mSize > mActive->mBucketsNumber * mLoadFactor;
        }

        bool isNeedShrink(){
            return mActive->mSize > MIN_BUCKETS_SIZE && mActive->mSize > mActive->mBucketsNumber * mShrinkFactor;
        }

        void startReHashing()
        {
            mRehashIndex = 0;
        }

        void compeleteReHashing()
        {
            mRehashIndex = -1;
        }

        size_t getNextPower2(size_t s){
            size_t init = MIN_BUCKETS_SIZE;
            while(init < s) {
                init = init << 1;
            }
            //inorder to make buckets is more than 
            return init;
        }

        void rehash(int step = 2, bool isExpand = true){
            Hasher hasher;
            //if first time rehash, new backup hashtable
            if(mRehashIndex == -1){
                mBucketsNumber = getNextPower2(mActive->mSize);
                mBackup = new HashTable<K, V>(mBucketsNumber);
                startReHashing();
                mBucketsMask = mBucketsNumber - 1;
            }
            if(mRehashIndex < mActive->mBucketsNumber){
                size_t endIndex = std::min(mRehashIndex + step, static_cast<int>(mActive->mBucketsNumber));
                while(mRehashIndex < endIndex){
                    auto head = mActive->getBucketHead(mRehashIndex);
                    while(head){
                        auto next = head->mNext;
                        mBackup->insertNode(hasher(head->mKey) & mBucketsMask, head);
                        head = next;
                    }
                    mActive->resetBucket(mRehashIndex);
                    mRehashIndex++;
                }
            }
            //if rehash done, swap backup to active, set backup to nullptr
            if(mRehashIndex == mActive->mBucketsNumber){
                swap(mActive, mBackup);
                delete mBackup;
                mBackup = nullptr;
                compeleteReHashing();
            }
        }
        
    public:
        void insert(const K &key, const V &v)
        {
            Hasher hasher;
            size_t index = hasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            std::cout<<"hash index:" <<index<<std::endl;
            HashNode<K, V>* tmp = new HashNode<K, V>(key, v);
            if(isReHashing()){
                mBackup->insertNode(index, tmp);
            }else{
                mActive->insertNode(index, tmp);
            }
        }

        bool erase(const K& key){
            Hasher hasher;
            size_t index = hasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            bool status = true;
            if(isReHashing()){
                if(!mBackup->eraseNode(index, key)){
                    status = mActive->eraseNode(hasher(key) & (mActive->mBucketsNumber - 1), key);
                }
            }else{
                status = mActive->eraseNode(index, key);
            }
            if(status && isNeedShrink()){
                rehash();
            }
            return status;
        }

        V& get(const K& key){
            Hasher hasher;
            size_t index = hasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            HashNode<K, V>* node = nullptr;
            if(isReHashing()){
                node = mBackup->get(index, key);
                if(!node){
                    node = mActive->get(hasher(key) & (mActive->mBucketsNumber - 1), key);
                }
            }else{
                node = mActive->get(index, key);
            }
            if(node) {
                return node->mValue;
            }else{
                throw std::out_of_range("key not exist in hash");
            }
        }
        void printHash(){
            std::cout<<"------------hash------------"<<std::endl;
            if(mActive){
                std::cout<<"mActive: not null" << std::endl;
                mActive->printHashTable();
            }else{
                std::cout<<"mActive: null" << std::endl;
            }
            std::cout<<"----------------------------"<<std::endl;
            if(mBackup){
                std::cout<<"mBackup: not null" << std::endl;
                mBackup->printHashTable();
            }else{
                std::cout<<"mBackup: null" << std::endl;
            }
            std::cout<<"------------end-------------"<<std::endl;

        }
    private:
        HashTable<K, V>* mActive;
        HashTable<K, V>* mBackup;
        int mBucketsNumber;
        int mBucketsMask;
        int mRehashIndex;
        float mLoadFactor;
        float mShrinkFactor;
        const int MIN_BUCKETS_SIZE = 4;
    };
}; // namespace RedisDataStructure

#endif
