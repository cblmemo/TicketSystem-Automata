cmake_minimum_required(VERSION 3.10.2)
project(code)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")

add_executable(code
        FrontEnd/BackEnd/src/main.cpp
        FrontEnd/BackEnd/src/Administrator.cpp
        FrontEnd/BackEnd/src/UserManager.cpp
        FrontEnd/BackEnd/src/TrainManager.cpp
        FrontEnd/BackEnd/src/OrderManager.cpp)