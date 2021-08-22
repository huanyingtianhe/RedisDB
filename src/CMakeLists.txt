cmake_minimum_required(VERSION 3.10.0)

project(RedisDB)

find_package(spdlog CONFIG REQUIRED)
find_path(CACH_SPDLOG_INCLUDE_DIR spdlog/spdlog.h)
message("spd log: ${CACH_SPDLOG_INCLUDE_DIR}")
include_directories("${CACH_SPDLOG_INCLUDE_DIR}")
message("3rd dir: ${CMAKE_SOURCE_DIR}/3rd/asio_http/include")
include_directories(${CMAKE_SOURCE_DIR}/3rd/asio_http/include)
find_package(Boost 1.76.0 REQUIRED system)

# generate shared target
add_library(RedisDBD STATIC server.cpp SkipList.cpp)
add_dependencies (RedisDBD asio_http)
target_link_libraries(RedisDBD PRIVATE spdlog::spdlog spdlog::spdlog_header_only ${Boost_LIBRARIES} asio_http)

# generate executable file
add_executable(RedisDBServer server.cpp SkipList.cpp)
add_dependencies (RedisDBServer asio_http)
target_link_libraries(RedisDBServer PRIVATE spdlog::spdlog spdlog::spdlog_header_only ${Boost_LIBRARIES} asio_http)

#add_executable(RedisDBClient client.cpp)
#target_link_libraries(RedisDBClient PRIVATE spdlog::spdlog spdlog::spdlog_header_only ${Boost_LIBRARIES} asio_http)

#special op for msvc
IF(MSVC)
    message("MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()
message("src ok")