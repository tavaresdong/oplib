cmake_minimum_required(VERSION 2.8)

project(oplib)

set (CMAKE_CXX_STANDARD 14)

enable_testing()

add_subdirectory(test)
add_subdirectory(oplib)
