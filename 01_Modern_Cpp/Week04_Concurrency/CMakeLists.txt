cmake_minimum_required(VERSION 3.15)
project(Concurrency_Demo)

set(CMAKE_CXX_STANDARD 20)

# 包含头文件目录
include_directories(include)

# 查找系统线程库
find_package(Threads REQUIRED)

# 1. 数据竞争 Demo
add_executable(race_demo src/race_condition_demo.cpp)
target_link_libraries(race_demo PRIVATE Threads::Threads)

# 2. 互斥锁 Demo
add_executable(mutex_demo src/mutex_demo.cpp)
target_link_libraries(mutex_demo PRIVATE Threads::Threads)

# 3. 队列测试 Demo
add_executable(queue_test src/queue_test.cpp)
target_link_libraries(queue_test PRIVATE Threads::Threads)

# 4. 线程池测试
add_executable(pool_test src/pool_test.cpp)
target_link_libraries(pool_test PRIVATE Threads::Threads)