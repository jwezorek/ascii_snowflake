cmake_minimum_required(VERSION 3.16)

project(ascii_snowflake VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ascii_snowflake
    src/main.cpp
    src/hex_grid.cpp
    src/snowflake.cpp
 "src/util.cpp")