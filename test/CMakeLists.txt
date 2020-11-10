cmake_minimum_required(VERSION 3.10.0)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

aux_source_directory(. EXECUTABLE)
foreach(T_FILE_NAME ${EXECUTABLE})
    get_filename_component(EXECUTABLE_NAME ${T_FILE_NAME} NAME_WE)
    # message(${EXECUTABLE_NAME})
    add_executable(${EXECUTABLE_NAME} ${T_FILE_NAME})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
    gtest_discover_tests(${EXECUTABLE_NAME})
    #special op for msvc
    IF(MSVC)
        message("run here")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    endif()
endforeach()

