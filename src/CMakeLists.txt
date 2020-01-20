cmake_minimum_required(VERSION 3.0.0)

include_directories(${PROJECT_SOURCE_DIR}/3rd/spdlog/include)

add_library(RedisCPlusPlusD SHARED SkipList.cpp)

add_executable(RedisCPlusPlus main.cpp SkipList.cpp)

