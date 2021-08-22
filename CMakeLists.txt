cmake_minimum_required(VERSION 3.10.0)

project(RedisDB)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(${PROJECT_SOURCE_DIR}/3rd/asio_http)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

option(WITH_TEST, "ENABLE TEST")
if(WITH_TEST)
    enable_testing ()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()