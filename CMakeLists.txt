cmake_minimum_required(VERSION 3.17)
project(TicketSystem_Automata)

set(CMAKE_CXX_STANDARD 14)

add_executable(TicketSystem_Automata BackEnd/include/RTL/exceptions.h BackEnd/include/RTL/vector.h
        BackEnd/src/main.cpp BackEnd/src/Administrator.cpp BackEnd/src/UserManager.cpp BackEnd/src/TrainManager.cpp BackEnd/src/TrainManager.h BackEnd/src/OrderManager.cpp BackEnd/src/OrderManager.h BackEnd/include/RTL/MyString.h BackEnd/include/RTL/Clock.h)