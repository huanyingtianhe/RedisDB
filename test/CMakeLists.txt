include_directories(${PROJECT_SOURCE_DIR}/3rd/googletest-master/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd/googletest-master/googlemoc/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

project(ReistCPlusPlusTest)
add_executable(ReistCPlusPlusTest skiplist_test.cpp)
target_link_libraries(ReistCPlusPlusTest gtest gtest_main)
#special op for msvc
IF(MSVC)
    message("run here")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()