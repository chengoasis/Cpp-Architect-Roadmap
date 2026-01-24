cmake_minimum_required(VERSION 3.15)
project(SharedPtr_Demo)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

# 1. 第一个可执行文件：测试基础功能 
add_executable(run_basics src/test_basics.cpp)

# 2. 第二个可执行文件：测试循环引用 
add_executable(run_circular src/test_circular.cpp)