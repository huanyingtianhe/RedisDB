#include <iostream>
#include "SkipList.h"

int main(int, char**) {
    std::cout << "Hello, world!\n";
    RedisDataStructure::SkipList<int, int> skiplist;
    std::cout << "skiplist.maxLevel"<<skiplist.getMaxLevel()<<std::endl;
}