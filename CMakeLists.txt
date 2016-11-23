cmake_minimum_required(VERSION 3.2)
project(Tetris)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Tetris/controls.txt
    Tetris/credits.txt
    Tetris/Source.cpp
    Tetris/tetris.cpp
    Tetris/tetris.exe
    Tetris/tetris.h
    Tetris/TetrisHeadline.txt)

add_executable(Tetris ${SOURCE_FILES})