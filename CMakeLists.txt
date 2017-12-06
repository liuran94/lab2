cmake_minimum_required(VERSION 3.8)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp bloom.h url.h ac.h ac.cpp url.cpp queue.cpp queue.h bloom.cpp matrix.h matrix.cpp MurmurHash2.cpp MurmurHasn2.h)
add_executable(lab2 ${SOURCE_FILES})