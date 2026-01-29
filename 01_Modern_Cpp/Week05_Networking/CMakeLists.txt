cmake_minimum_required(VERSION 3.15)

project(Week05_Networking)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

include_directories(include)

find_package(Threads REQUIRED)

# 定义源文件
set(SOURCES
    src/main.cpp
    src/Socket.cpp
)

# 6. 生成可执行文件
add_executable(server ${SOURCES})

# 链接线程库
# 这一步非常重要！如果不加，将来合入 Week 04 线程池代码时会报 Link Error
target_link_libraries(server PRIVATE Threads::Threads)