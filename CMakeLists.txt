cmake_minimum_required(VERSION 3.29)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(chips)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/chip8.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/display.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(chips ${SOURCE})

target_link_libraries(chips ${SDL2_LIBRARIES})
