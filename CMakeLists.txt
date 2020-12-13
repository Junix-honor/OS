cmake_minimum_required(VERSION 3.17)
project(OS)

set(CMAKE_CXX_STANDARD 14)

add_executable(OS main.cpp PRM.cpp PRM.h RCB.cpp RCB.h PCB.cpp PCB.h)