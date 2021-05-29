cmake_minimum_required(VERSION 3.15)
project(code)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts -Ofast")

add_executable(code
        BackEnd/src/main.cpp
        BackEnd/src/Administrator.cpp
        BackEnd/src/UserManager.cpp
        BackEnd/src/TrainManager.cpp
        BackEnd/src/OrderManager.cpp)