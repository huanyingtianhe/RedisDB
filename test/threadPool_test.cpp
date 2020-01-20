#include <gtest/gtest.h>
#include <threadPool.h>
#include <iostream>
#include <string>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class TestThreadPool :public testing::Test
{
public:
	static void SetUpTestCase()
	{
		std::cout << "SetUpTestCase" << std::endl;
	}
	static void TearDownTestCase()
	{
		std::cout << "TearDownTestCase" << std::endl;
	}
	virtual void SetUp()
	{
		std::cout << "SetUp" << std::endl;
	}
	virtual void TearDown()
	{
		std::cout << "TearDown" << std::endl;
	}
};

TEST_F(TestThreadPool, ConcurrentQueueTest) {
	Redis::ConcurrentQueueWithLock<int> q(5);
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	spdlog::logger logger("test", console_sink);
	std::thread producer([&]() {
		for (int i = 0; i < 10; i++){
			logger.info("start push data to queue {}\n", i);
			q.waitPush(i);
		}
	});

	std::thread consumer([&]() {
		for(int i = 0; i < 10; i++){
			auto t = q.waitPop();
			logger.info("front: {}, size: {}\n",*t, q.getSize());
		}
	});
	producer.join();
	consumer.join();
}



// TEST_F(TestThreadPool, LockTest) {
// 	std::mutex m;
// 	std::unique_lock<std::mutex> l1(m);
// 	std::lock_guard<std::mutex> l2(m);
// 	std::cout<<"gard"<<std::endl;
// }