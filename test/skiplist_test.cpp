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

TEST_F(TestMap, SkipListInsertTest) {
	RedisDataStructure::SkipList<int, std::string> sortedlist;
	std::vector<int> keys{1, 3, 8};
	sortedlist.insert(1, "he");
	sortedlist.insert(8, "world");
	sortedlist.insert(3, "llo");
	int keyIndex = 0;
	for(auto it = sortedlist.begin(); it != sortedlist.end(); it++){
		std::cout<< it->mKey <<" "<< it->mValue<< std::endl;
		EXPECT_EQ(it->mKey, keys[keyIndex++]);
	}
}

TEST_F(TestMap, SkipListEraseTest) {
	RedisDataStructure::SkipList<int, std::string> sortedlist;
	std::vector<int> keys{1, 8};
	sortedlist.insert(1, "he");
	sortedlist.insert(8, "world");
	sortedlist.insert(3, "llo");
	int keyIndex = 0;
	sortedlist.erase(3);
	EXPECT_EQ(sortedlist.getLength(), 2);
	for(auto it = sortedlist.begin(); it != sortedlist.end(); it++){
		EXPECT_EQ(it->mKey, keys[keyIndex++]);
	}
}

TEST_F(TestMap, SkipListReverseTest) {
	RedisDataStructure::SkipList<int, std::string> sortedlist;
	std::vector<int> keys{8, 3, 1};
	sortedlist.insert(1, "he");
	sortedlist.insert(8, "world");
	sortedlist.insert(3, "llo");
	int keyIndex = 0;
	for(auto it = sortedlist.rbegin(); it != sortedlist.rend(); it++){
		EXPECT_EQ(it->mKey, keys[keyIndex++]);
	}
}

TEST_F(TestMap, SkipListRankTest) {
	RedisDataStructure::SkipList<int, std::string> sortedlist;
	sortedlist.insert(1, "he");
	sortedlist.printList();
	sortedlist.insert(8, "world");
	sortedlist.printList();
	sortedlist.insert(3, "llo");
	EXPECT_EQ(sortedlist.getRank(1), 1);
	EXPECT_EQ(sortedlist.getRank(3), 2);
	EXPECT_EQ(sortedlist.getRank(8), 3);
	EXPECT_EQ(sortedlist.getRank(2), 0);
	sortedlist.printList();
}

TEST_F(TestMap, SkipListBoundTest) {
	RedisDataStructure::SkipList<int, std::string> sortedlist;
	std::vector<int> keys{1, 3, 8};
	sortedlist.insert(1, "he");
	sortedlist.insert(8, "world");
	sortedlist.insert(3, "llo");
	sortedlist.insert(3, "fun");
	int keyIndex = 0;
	EXPECT_EQ(sortedlist.getLength(), 4);
	auto low = sortedlist.lower_bound(3);
	auto high = sortedlist.upper_bound(3);
	EXPECT_EQ(low->mKey, 3);
	EXPECT_EQ(low->mValue, "llo");
	EXPECT_EQ(high->mKey, 8);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}