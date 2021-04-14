cmake_minimum_required(VERSION 3.17)
project(TicketSystem_Automata)

set(CMAKE_CXX_STANDARD 14)

add_executable(TicketSystem_Automata BackEnd/src/main.cpp BackEnd/src/Administrator.cpp BackEnd/src/UserManager.cpp)