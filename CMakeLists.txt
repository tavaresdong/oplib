cmake_minimum_required(VERSION 2.8)

project(oplib)

set (CMAKE_CXX_COMPILER ${CXX})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -g -Wall -Wno-unused-variable")

enable_testing()

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
add_subdirectory(ds)
add_subdirectory(net)
