cmake_minimum_required(VERSION 3.0.0)

include_directories(${PROJECT_SOURCE_DIR}/3rd/spdlog/include)

add_library(RedisCPlusPlusD SHARED SkipList.cpp log.cpp)

add_executable(RedisCPlusPlus main.cpp SkipList.cpp log.cpp)

#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

