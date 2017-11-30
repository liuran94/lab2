cmake_minimum_required(VERSION 3.8)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp bloom.h url.h ac.h ac.cpp url.cpp queue.cpp queue.h bloom.cpp)
add_executable(lab2 ${SOURCE_FILES})