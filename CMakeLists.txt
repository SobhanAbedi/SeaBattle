cmake_minimum_required(VERSION 3.17)
project(SeaBattle C)

set(CMAKE_C_STANDARD 99)

include_directories(./include)
link_directories(./lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
add_executable(SeaBattle src/main.c src/Board.c include/Board.h src/Config.c include/Config.h src/Player.c include/Player.h src/Mechanism.c include/Mechanism.h src/Android.c include/Android.h)