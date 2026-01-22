cmake_minimum_required(VERSION 3.15)

# 项目名称
project(SmartPtrDemo)

# 设置 C++ 标准为 C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 开启所有编译警告
add_compile_options(-Wall -Wextra -Werror)

# --- 关键修改点 1: 添加头文件搜索路径 ---
# 这样在 main.cpp 里就可以直接写 #include "smart_ptr.hpp"
# 而不需要写 #include "../include/smart_ptr.hpp" (这种写法很丑)
include_directories(include)

# --- 关键修改点 2: 指定源文件的路径 ---
add_executable(smart_ptr_demo src/main.cpp)