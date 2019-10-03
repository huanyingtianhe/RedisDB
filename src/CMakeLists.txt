cmake_minimum_required(VERSION 3.0.0)

add_library(RedisCPlusPlusD SHARED SkipList.cpp)

add_executable(RedisCPlusPlus main.cpp SkipList.cpp)

