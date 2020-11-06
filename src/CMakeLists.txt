cmake_minimum_required(VERSION 3.0.0)

project(RedisDB)

find_package(spdlog CONFIG REQUIRED)
add_library(RedisDBD SHARED SkipList.cpp)

add_executable(RedisDB main.cpp SkipList.cpp)
target_link_libraries(RedisDB PRIVATE spdlog::spdlog spdlog::spdlog_header_only)


#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()