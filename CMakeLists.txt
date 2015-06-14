cmake_minimum_required(VERSION 3.0)
project(automata)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -lgtest -std=c++14")

set(SOURCE_FILES finitestateautomaton_test.cpp test.cpp)
find_package(GTest REQUIRED)

add_executable(tests ${SOURCE_FILES})
set_property(TARGET tests PROPERTY CXX_STANDARD 14)
