#include <gtest/gtest.h>
#include <threadPool.h>
#include <iostream>
#include <string>
#include <thread>

class TestThreadPool : public testing::Test
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

TEST_F(TestThreadPool, ConcurrentQueueTest)
{
	Redis::ConcurrentQueueWithLock<int> q(5);
	std::thread producer([&]() {
		for (int i = 0; i < 10; i++)
		{
			std::cout << "start push data to queue " << i << std::endl;
			q.waitPush(i);
		}
	});

	std::thread consumer([&]() {
		for (int i = 0; i < 10; i++)
		{
			auto t = q.waitPop();
			std::cout << "front: " << *t << ", size: " << q.getSize() << std::endl;
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

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}