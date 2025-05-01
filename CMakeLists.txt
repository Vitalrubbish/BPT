cmake_minimum_required(VERSION 3.22)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code main.cpp
        BPT.h
        BPT.cpp
        CacheManager.h
        HashTable.h

        ./list/list.hpp
        ./vector/vector.hpp
        ./vector/exceptions.hpp
        ./vector/utility.hpp
)
