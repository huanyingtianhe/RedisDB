cmake_minimum_required(VERSION 3.10.0)

project(RedisDB)

find_package(spdlog CONFIG REQUIRED)
find_path(CACH_SPDLOG_INCLUDE_DIR spdlog/spdlog.h)
message("spd log: ${CACH_SPDLOG_INCLUDE_DIR}")
include_directories("${CACH_SPDLOG_INCLUDE_DIR}")
add_library(RedisDBD SHARED SkipList.cpp)

add_executable(RedisDB main.cpp SkipList.cpp)
target_link_libraries(RedisDB PRIVATE spdlog::spdlog spdlog::spdlog_header_only)


#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()