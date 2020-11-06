cmake_minimum_required(VERSION 3.0.0)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(GTest CONFIG REQUIRED)

add_executable(RedisDBTest skiplist_test.cpp threadPool_test.cpp)
target_link_libraries(RedisDBTest PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()