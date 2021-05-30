cmake_minimum_required(VERSION 3.15)
project(code)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-std=c++2a -Ofast)

add_executable(code
        FrontEnd/BackEnd/src/main.cpp
        FrontEnd/BackEnd/src/Administrator.cpp
        FrontEnd/BackEnd/src/UserManager.cpp
        FrontEnd/BackEnd/src/TrainManager.cpp
        FrontEnd/BackEnd/src/OrderManager.cpp)