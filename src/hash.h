#ifndef HASH_H
#define HASH_H

#include <cstdlib>
#include <string>
#include "Util.h"
#include <stdexcept>
#include <algorithm>

namespace RedisDataStructure
{
    template <class T, bool>
    struct HtValueTraitsImp
    {
        using KeyType = T ;
        using MappedType = T ;
        using ValueType = T ;

        template <class Ty>
        static const KeyType &getKey(const Ty &value)
        {
            return value;
        }

        template <class Ty>
        static const ValueType &getValue(const Ty &value)
        {
            return value;
        }
    };

    //hash map's node is std::pair, this partial template specialization is for the hashMap node structure
    template <class T>
    struct HtValueTraitsImp<T, true>
    {
        using KeyType = typename std::remove_cv<typename T::first_type>::type;
        using MappedType = typename T::second_type;
        using ValueType = typename T::second_type;

        template <class Ty>
        static const KeyType &getKey(const Ty &value)
        {
            return value.first;
        }

        template <class Ty>
        static const ValueType &getValue(const Ty &value)
        {
            return value.second;
        }
    };

    template <class T>
    struct HtValueTraits
    {
        static constexpr bool isMap = Util::isPair<T>::value;
        using valueTraitsType = HtValueTraitsImp<T, isMap>;

        using KeyType =  typename valueTraitsType::KeyType ;
        using MappedType = typename valueTraitsType::MappedType;
        using ValueType = typename valueTraitsType::ValueType;

        template <class Ty>
        static const KeyType &getKey(const Ty &value)
        {
            return valueTraitsType::getKey(value);
        }

        template <class Ty>
        static const ValueType &getValue(const Ty &value)
        {
            return valueTraitsType::getValue(value);
        }
    };

    template <typename T>
    struct HashNode : public Util::noncopyable
    {
        T mValue;
        HashNode *mNext;
        HashNode(const T& v) : mValue(v), mNext(nullptr) {}
        HashNode(const T& v, HashNode *n) : mValue(v), mNext(n) {}
    };

    template <typename T>
    struct HashTable: public Util::noncopyable
    {
    public:
        using ValueTraits = HtValueTraits<T>;
        using KeyType = typename ValueTraits::KeyType;
        using ValueType = typename ValueTraits::ValueType;

        HashTable(size_t buck) : mSize(0), mBucketsNumber(buck)
        {
            mNodes = new HashNode<T> *[buck];
            //fill(mNodes[0], mNodes[0] + buck, nullptr);
            for(int i = 0; i < buck; i++) mNodes[i] = nullptr;
        }

        ~HashTable(){
            for(int i = 0; i < mBucketsNumber; i++){
                auto head = mNodes[i];
                while(head){
                    auto next = head->mNext;
                    delete head;
                    head = next;
                }
            }
            delete []mNodes;
        }

        HashNode<T>* getBucketHead(size_t index){
            return mNodes[index];
        }

        HashNode<T>* get(size_t index, const KeyType& key){
            auto head = mNodes[index];
            while(head){
                if(ValueTraits::getKey(head->mValue) == key){
                    return head;
                }
                head = head->mNext;
            }
            return nullptr;
        }

        void resetBucket(size_t index){
            mNodes[index] = nullptr;
        }

        bool insertNode(size_t index, HashNode<T> *node)
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

        bool eraseNode(size_t index, const KeyType& key){
            auto head = mNodes[index];
            if(!head) return false;
            if(ValueTraits::getKey(head->mValue) == key){
                mNodes[index] = mNodes[index]->mNext;
                delete head;
                mSize--;
                return true;
            }else{
                while(head->mNext){
                    if(ValueTraits::getKey(head->mNext->mValue) == key){
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
                        std::cout<< ValueTraits::getKey(head->mValue)<<","<< ValueTraits::getValue(head->mValue)<<"; ";
                        head = head->mNext;
                    }
                    std::cout<<std::endl;
                }
                
            }
        }
        HashNode<T> **mNodes;
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
    template <typename T, typename Hasher>
    class Hash: public Util::noncopyable
    {
    public:
        using ValueTraits =  HtValueTraits<T>;
        using KeyType = typename ValueTraits::KeyType;
        using ValueType = typename ValueTraits::ValueType;
    public:
        Hash() : mBucketsNumber(4), mBucketsMask(3), mRehashIndex(-1), 
                 mLoadFactor(1.0f), mShrinkFactor(0.1f) {
            mActive = new HashTable<T>(mBucketsNumber);
            mBackup = nullptr;
            mHasher = Hasher();
        }
        ~Hash(){
            if(mActive) delete mActive;
            if(mBackup) delete mBackup;
        }
        //add move constructor and move assignment support
        Hash(const Hash<T, Hasher>&& h): mActive(h.mActive), mBackup(h.mBackup),{
            mBucketsNumber = h.mBucketsNumber;
            mBucketsMask = h.mBucketsMask;
            mRehashIndex = h.mRehashIndex;
            mLoadFactor = h.mLoadFactor;
            mShrinkFactor = h.mShrinkFactor;
            mHasher = h.mHasher;
            h.mActive = nullptr;
            h.mBackup = nullptr;
        }
        Hash<T, Hasher>& operator=(const Hash<T, Hasher>&& h){
            mActive = h.mActive;
            mBackup = h.mBackup;
            mBucketsNumber = h.mBucketsNumber;
            mBucketsMask = h.mBucketsMask;
            mRehashIndex = h.mRehashIndex;
            mLoadFactor = h.mLoadFactor;
            mShrinkFactor = h.mShrinkFactor;
            mHasher = h.mHasher;
            h.mActive = nullptr;
            h.mBackup = nullptr;
        }
    protected:
        bool isReHashing()
        {
            return mRehashIndex != -1;
        }

        bool isNeedReHash(){
            return mActive->mSize > mActive->mBucketsNumber * mLoadFactor;
        }

        bool isNeedShrink(){
            return mActive->mSize > MIN_BUCKETS_SIZE && mActive->mSize < mActive->mBucketsNumber * mShrinkFactor;
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
            //if first time rehash, new backup hashtable
            if(mRehashIndex == -1){
                mBucketsNumber = getNextPower2(mActive->mSize);
                mBackup = new HashTable<T>(mBucketsNumber);
                startReHashing();
                mBucketsMask = mBucketsNumber - 1;
            }
            if(mRehashIndex < mActive->mBucketsNumber){
                size_t endIndex = std::min(mRehashIndex + step, static_cast<int>(mActive->mBucketsNumber));
                while(mRehashIndex < endIndex){
                    auto head = mActive->getBucketHead(mRehashIndex);
                    while(head){
                        auto next = head->mNext;
                        mBackup->insertNode(mHasher(ValueTraits::getKey(head->mValue)) & mBucketsMask, head);
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
        bool erase(const KeyType& key){
            size_t index = mHasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            bool status = true;
            if(isReHashing()){
                if(!mBackup->eraseNode(index, key)){
                    status = mActive->eraseNode(mHasher(key) & (mActive->mBucketsNumber - 1), key);
                }
            }else{
                status = mActive->eraseNode(index, key);
            }
            if(status && isNeedShrink()){
                rehash();
            }
            return status;
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
    protected:
        HashTable<T>* mActive;
        HashTable<T>* mBackup;
        int mBucketsNumber;
        int mBucketsMask;
        int mRehashIndex;
        float mLoadFactor;
        float mShrinkFactor;
        Hasher mHasher;
        const int MIN_BUCKETS_SIZE = 4;
    };

    template<typename K, typename V, typename Hasher = HashFunction<K>>
    class HashMap: public Hash<std::pair<K, V>, Hasher> {
    public:
        using KeyType = K;
        using ValueType = V;
        using EncyType = std::pair<KeyType, ValueType>;
    public:
        void insert(const KeyType &key, const ValueType &v)
        {
            size_t index = mHasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            //std::cout<<"hash index:" <<index<<std::endl;
            HashNode<EncyType>* tmp = new HashNode<EncyType>(std::make_pair(key, v));
            if(isReHashing()){
                mBackup->insertNode(index, tmp);
            }else{
                mActive->insertNode(index, tmp);
            }
        }

        ValueType& get(const KeyType& key){
            size_t index = mHasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            HashNode<EncyType>* node = nullptr;
            if(isReHashing()){
                node = mBackup->get(index, key);
                if(!node){
                    node = mActive->get(mHasher(key) & (mActive->mBucketsNumber - 1), key);
                }
            }else{
                node = mActive->get(index, key);
            }
            if(node) {
                return node->mValue.second;
            }else{
                throw std::out_of_range("key not exist in hash");
            }
        }
    };

    template<typename K, typename Hasher = HashFunction<K>>
    class HashSet: public Hash<K, Hasher> {
    public:
        using KeyType = K;
    public:
        void insert(const KeyType &key)
        {
            size_t index = mHasher(key) & mBucketsMask;
            if(isReHashing() || isNeedReHash()){
                rehash();
            }
            //std::cout<<"hash index:" <<index<<std::endl;
            HashNode<KeyType>* tmp = new HashNode<KeyType>(key);
            if(isReHashing()){
                mBackup->insertNode(index, tmp);
            }else{
                mActive->insertNode(index, tmp);
            }
        }
    };


}; // namespace RedisDataStructure

#endif
