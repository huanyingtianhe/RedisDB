#include <gtest/gtest.h>
#include <hash.h>

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

TEST_F(TestHash, HashHasherTest)
{
    RedisDataStructure::HashFunction<std::string> hasher;
	size_t code1 = hasher("hello") % 4;
    size_t code2 = hasher("zi") % 4;
	EXPECT_EQ(code1, 1);
	EXPECT_EQ(code2, 0);
}

TEST_F(TestHash, HashInsertTest)
{
    RedisDataStructure::Hash<std::string, int> hash;
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

TEST_F(TestHash, HashEraseTest)
{
    RedisDataStructure::Hash<std::string, int> hash;
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

TEST_F(TestHash, HashRehashTest)
{
    RedisDataStructure::Hash<std::string, int> hash;
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

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}