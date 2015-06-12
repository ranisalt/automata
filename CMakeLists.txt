cmake_minimum_required(VERSION 3.0)
project(automata)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -lgtest -std=c++14")

set(SOURCE_FILES finitestateautomaton_test.cpp test.cpp)
add_executable(automata ${SOURCE_FILES})
set_property(TARGET automata PROPERTY CXX_STANDARD 14)
