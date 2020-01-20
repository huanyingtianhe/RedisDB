#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <spdlog/spdlog.h>


namespace Redis{
    class ThreadTask{
        public:
        void run() {}
    };

    template<typename T, typename Container = std::queue<T>>
    class ConcurrentQueueWithLock{
    public:
		explicit ConcurrentQueueWithLock(size_t s): mMaxSize(s) {}

		bool tryPush(const T& value) {
			std::lock_guard<std::mutex> lock(mMutex);
			if(mData.size() >= mMaxSize) return false;
			mData.emplace(value);
            mCond.notify_one();
			return true;
		}
        void waitPush(const T& value){
            std::unique_lock<std::mutex> lock(mMutex);
			while(mData.size() >= mMaxSize){
				lock.unlock();
				std::this_thread::yield();
				lock.lock();
			}
            mData.emplace(value);
            mCond.notify_one();
        }

		std::shared_ptr<T> waitPop() {
			std::unique_lock<std::mutex> lock(mMutex);
			mCond.wait(lock, [&]() {return !mData.empty(); });
			T tmp = mData.front();
			auto result = std::make_shared<T>(tmp);
			mData.pop();
			return result;
		}

		bool tryPop() {
			std::lock_guard<std::mutex> lock(mMutex);
			if (mData.empty()) return false;
			mData.pop();
			return true;
		}

		bool empty() const {
			std::lock_guard<std::mutex> lock(mMutex);
			return mData.empty();
		}
        
		T& front() {
			std::lock_guard<std::mutex> lock(mMutex);
			return mData.front();
		}

		const T& front() const {
			std::lock_guard<std::mutex> lock(mMutex);
			return mData.front();
		}

		std::size_t getMaxSize() const {
			std::lock_guard<std::mutex> lock(mMutex);
			return mMaxSize;
		}

		void setMaxSize(std::size_t s) {
			std::lock_guard<std::mutex> lock(mMutex);
			mMaxSize = s;
		}
		
		std::size_t getSize() const {
			std::lock_guard<std::mutex> lock(mMutex);
			return mData.size();
		}

    private:
        mutable std::mutex mMutex;
        std::condition_variable mCond;
        Container mData;
		//std::atomic<std::size_t> mSize;
		std::size_t mMaxSize;
    };

    class ThreadPool{
        
    };
}

#endif