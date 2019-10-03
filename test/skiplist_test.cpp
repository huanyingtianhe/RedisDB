#include <gtest/gtest.h>
#include <SkipList.h>
#include <string>
#include <iostream>
class TestMap :public testing::Test
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
	virtual void SetUp()   //TEST跑之前会执行SetUp
	{
		std::cout << "SetUp" << std::endl;
	}
	virtual void TearDown() //TEST跑完之后会执行TearDown
	{
		std::cout << "TearDown" << std::endl;
	}
};

 