cmake_minimum_required(VERSION 3.17)
project(BPlusTree)

set(CMAKE_CXX_STANDARD 14)

add_executable(BPlusTree testBPlusTree_SingleKeyVersion.cpp MemoryPool.h BPlusTree_SingleKeyVersion.h testBPlusTree_SingleKeyVersion.cpp)