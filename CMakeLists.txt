cmake_minimum_required(VERSION 3.17)
project(BPlusTree)

set(CMAKE_CXX_STANDARD 14)

add_executable(BPlusTree testBPlusTree.cpp BPlusTree.h MemoryPool.h BPlusTreeString.h)