cmake_minimum_required(VERSION 3.17)
project(SeaBattle C)

set(CMAKE_C_STANDARD 99)

include_directories(./include)
link_directories(./lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
add_executable(SeaBattle main.c)