cmake_minimum_required(VERSION 3.29)
project(chips)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(chips ${SOURCE})
