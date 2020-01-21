cmake_minimum_required(VERSION 3.0.0)

project(RedisCPlusPlust)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rd/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd/googletest-master)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

enable_testing ()
add_test (NAME MyTest COMMAND ReistCPlusPlusTest)

#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()