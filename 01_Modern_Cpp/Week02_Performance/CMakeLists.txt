cmake_minimum_required(VERSION 3.15)
project(Performance_Demo)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(string_demo 
    src/main.cpp 
    src/my_string.cpp
)