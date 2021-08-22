cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_VERBOSE_MAKEFILE off)
project(asio_http C CXX)

include_directories(SYSTEM
    deps/asio/asio/include
    include
)

find_package(Boost 1.76.0 REQUIRED system)
include_directories(
    src
    include/http
    ${Boost_INCLUDE_DIRS}
)
add_definitions(-DASIO_STANDALONE -DRAPIDJSON_HAS_STDSTRING=1)
file(GLOB SOURCE_FILES src/*.cpp)

if (BUILD_SHARED_LIBS)
    install(TARGETS ${PROJECT_NAME} DESTINATION .)
endif()
add_library(asio_http STATIC ${SOURCE_FILES})

find_package(Threads REQUIRED)

target_link_libraries(asio_http Threads::Threads ${Boost_LIBRARIES})
if (CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(asio_http PROPERTIES COMPILE_FLAGS "--coverage")
    set_target_properties(asio_http PROPERTIES LINK_FLAGS "--coverage")
endif()
if (MSVC)
    add_compile_options(/EHsc)
endif()

set_property(TARGET asio_http PROPERTY CXX_STANDARD 11)
set_property(TARGET asio_http PROPERTY CXX_STANDARD_REQUIRED TRUE)

if(BUILD_EXAMPLE)
    add_executable(example examples/main.cpp)
    target_link_libraries(example asio_http)
    if (CMAKE_COMPILER_IS_GNUCXX)
        set_target_properties(example PROPERTIES COMPILE_FLAGS "--coverage")
        set_target_properties(example PROPERTIES LINK_FLAGS "--coverage")
    endif()
    set_property(TARGET example PROPERTY CXX_STANDARD 11)
endif()

if(WIN32)
    install(TARGETS asio_http
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/lib
    )
    install(DIRECTORY include/http DESTINATION ${CMAKE_BUILD_DIR}/include)
else()
    install(TARGETS asio_http
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/lib
    )
    install(DIRECTORY include/http DESTINATION ${CMAKE_BUILD_DIR}/include)
endif()
message("asio_http ok")