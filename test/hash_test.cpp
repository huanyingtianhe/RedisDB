#include <gtest/gtest.h>
#include <hash.h>
#include <vector>

class TestHash : public testing::Test
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
	virtual void SetUp() //TEST跑之前会执行SetUp
	{
		std::cout << "SetUp" << std::endl;
	}
	virtual void TearDown() //TEST跑完之后会执行TearDown
	{
		std::cout << "TearDown" << std::endl;
	}
};

TEST_F(TestHash, HashMapHasherTest)
{
    RedisDataStructure::HashFunction<std::string> hasher;
	size_t code1 = hasher("hello") % 4;
    size_t code2 = hasher("zi") % 4;
	EXPECT_EQ(code1, 1);
	EXPECT_EQ(code2, 0);
}

TEST_F(TestHash, HashMapInsertTest)
{
    RedisDataStructure::HashMap<std::string, int> hash;
    hash.insert("hello", 1);
    hash.insert("world", 2);
	hash.insert("zi", 3);
	hash.printHash();

	int v1 = hash.get("hello");
	int v2 = hash.get("world");
	int v3 = hash.get("zi");
	//int v1 =1, v2 = 2;
    EXPECT_EQ(v1, 1);
    EXPECT_EQ(v2, 2);
	EXPECT_EQ(v3, 3);
}

TEST_F(TestHash, HashMapEraseTest)
{
    RedisDataStructure::HashMap<std::string, int> hash;
    hash.insert("hello", 1);
    hash.insert("world", 2);
	hash.insert("zi", 3);
	bool r1 = hash.erase("hello");
	bool r2 = hash.erase("doubi");
	hash.printHash();
	//int v1 =1, v2 = 2;
    EXPECT_EQ(r1, true);
    EXPECT_EQ(r2, false);
	EXPECT_EQ(hash.get("world"), 2);
}

TEST_F(TestHash, HashMapRehashTest)
{
    RedisDataStructure::HashMap<std::string, int> hash;
    hash.insert("hello", 1);
    hash.insert("world", 2);
	hash.insert("zi", 3);
	hash.insert("ti", 4);
	hash.printHash();
	hash.insert("yi", 5);
	hash.printHash();

	int v1 = hash.get("hello");
	hash.printHash();
	int v2 = hash.get("world");
	hash.printHash();
	int v3 = hash.get("zi");
	hash.printHash();
	int v4 = hash.get("ti");
	int v5 = hash.get("yi");
	hash.printHash();
	//int v1 =1, v2 = 2;
    EXPECT_EQ(v1, 1);
    EXPECT_EQ(v2, 2);
	EXPECT_EQ(v3, 3);
	EXPECT_EQ(v4, 4);
	EXPECT_EQ(v5, 5);
}

TEST_F(TestHash, HashMapIteratorTest){
	RedisDataStructure::HashMap<std::string, int> hash;
    hash.insert("hello", 1);
    hash.insert("world", 2);
	hash.insert("zi", 3);
	hash.insert("ti", 4);
	hash.insert("yi", 5);
	hash.printHash();
	std::vector<std::pair<std::string, int>> expect1 = {{"zi", 3}, {"world", 2}, {"hello", 1},  {"ti", 4}, {"yi", 5}};
	std::vector<std::pair<std::string, int>> expect2 = {{"zi", 3}, {"hello", 1}, {"ti", 4}, {"world", 2},  {"yi", 5}};

	int i = 0;
	for(auto it = hash.begin(); it != hash.end(); it++){
		EXPECT_EQ(*it, expect1[i++]);
	}
	hash.printHash();
	//trigger rehash
	EXPECT_EQ(hash.get("hello"), 1);
	hash.printHash();
	i = 0;
	for(auto it = hash.begin(); it != hash.end(); it++){
		EXPECT_EQ(*it, expect2[i++]);
	}
	hash.printHash();

}

TEST_F(TestHash, HashMapScanTest){
	RedisDataStructure::HashMap<std::string, int> hash;
    hash.insert("hello", 1);
    hash.insert("world", 2);
	hash.insert("zi", 3);
	hash.insert("ti", 4);
	hash.insert("yi", 5);
	std::vector<std::pair<std::string, int>> re1, re2;
	std::set<std::pair<std::string, int>> target = {{"hello", 1}, {"world", 2}, {"zi", 3}, {"ti", 4}, {"yi", 5}};
	hash.printHash();
	int v = 0;
	do {
		v = hash.scan(v, re1);
	}while(v != 0);
	std::set<std::pair<std::string, int>> tmp1(re1.begin(), re1.end());
	EXPECT_EQ(tmp1, target);
	//trigger rehash happens
	EXPECT_EQ(hash.get("hello"), 1);
	hash.printHash();
	v = 0;
	do {
		v = hash.scan(v, re2);
	}while(v != 0);
	std::set<std::pair<std::string, int>> tmp2(re2.begin(), re2.end());
	EXPECT_EQ(tmp2, target);
	//if rehash happens, the re1 and re2 should be different.
	EXPECT_NE(re1, re2);

}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}