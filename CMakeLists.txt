cmake_minimum_required(VERSION 3.22)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code main.cpp
        BPT.h
        CacheManager.h
        BPT.cpp)
