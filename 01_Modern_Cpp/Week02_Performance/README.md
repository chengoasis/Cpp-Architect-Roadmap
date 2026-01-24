# Week 02: High-Performance String & Move Semantics

## 📖 项目简介 (Introduction)

这是 **C++ Architect Roadmap** 的第二周项目。本周的核心目标是打破 C++98 的性能枷锁，深入研究 **移动语义 (Move Semantics)**。

为了直观地量化性能差距，我手写了一个管理动态内存的字符串类 `MyString`。通过对比 "Deep Copy Only"（笨重版）和 "Move Enabled"（极速版）在 `std::vector` 扩容场景下的表现，我亲眼见证了**零拷贝 (Zero Copy)** 技术如何显著减少内存分配与数据复制的开销。

## 🚀 核心特性 (Key Features)

- **Rule of Five (五法则)**：实现了现代 C++ 资源管理的黄金准则，包括**析构、拷贝构造/赋值、移动构造/赋值**。
- **Move Semantics (移动语义)**：利用右值引用 (`&&`) 实现资源“窃取”，将时间复杂度从 O(N) 降低到 O(1)。
- **STL 优化适配 (`noexcept`)**：通过标记移动操作为 `noexcept`，激活了 `std::vector` 在扩容时的移动优化策略。
- **Deep Copy vs. Move**：清晰展现了左值 (L-value) 触发拷贝、右值 (R-value) 触发移动的编译器决策逻辑。

## 🧪 性能实验：Vector 扩容危机 (The Experiment)

我设计了一个能够把 CPU 跑满的测试用例：将 `MyString` 对象连续推入 `std::vector`。

### 1. 痛点：Deep Copy 的代价

在没有移动语义时，当 `vector` 容量不足需要扩容（Reallocation）时，它被迫将旧内存里的所有元素**深拷贝**到新内存，然后销毁旧元素。

- **后果**：大量的 `new`、`strcpy` 和 `delete` 操作，性能极差。

### 2. 优化：引入移动语义

我在 `MyString` 中实现了移动构造函数：

```c++
MyString::MyString(MyString&& other) noexcept {
  std::cout << "[移动构造] Stealing resources!" << std::endl;
  // 1. 偷窃
  length_ = other.length_;
  data_ = other.data_;

  // 2. 释放 other 的资源
  other.length_ = 0;
  other.data_ = nullptr; // 关键：置空，防止对方析构时 delete 我们的数据
}
```

- **结果**：扩容时，`vector` 不再复制数据，而是直接“偷走”旧元素的指针。

> 详细解释一下：为什么需要 `other.data_ = nullptr;`
>
> 记住 C++ 的一条底层铁律：**`delete` 一个空指针 (`nullptr`) 是安全的，且什么都不会发生（No-op）。**
>
> **如果忘了置空 (Fatal Error)**
>
> 假设我们执行 `MyString s2 = std::move(s1);`，但移动构造函数里**没写** `other.data_ = nullptr;`。
>
> 1. **偷窃阶段**：`s2.data_` 拿到了 `s1.data_` 的地址（比如 `0x999`）。
>    - 此时：`s1` 指向 `0x999`，`s2` 也指向 `0x999`。
> 2. **s1 析构**：`s1` 生命周期结束。
>    - 执行 `~MyString()` -> `delete[] data_`。
>    - 系统释放了 `0x999` 的内存。
>    - **现状**：`0x999` 变成了非法区域（野指针），但 `s2` 手里还攥着这个地址！
> 3. **s2 析构**：`main` 函数结束，`s2` 也要死了。
>    - 执行 `~MyString()` -> `delete[] data_`。
>    - 它试图再次释放 `0x999`。
>    - **崩溃**：**Double Free (重复释放)**！程序当场暴毙。
>
> **正确写法 (Safe)**
>
> 假设执行 `MyString s2 = std::move(s1);`，且写了 `other.data_ = nullptr;`。
>
> 1. **偷窃阶段**：`s2.data_` 拿到了 `0x999`。
> 2. **断开连接**：`s1.data_` 被赋值为 `nullptr` (0x0)。
>    - 此时：`s2` 指向 `0x999`（它是唯一的主人），`s1` 指向 `0x0`（空手）。
> 3. **s1 析构**：`s1` 生命周期结束。
>    - 执行 `~MyString()` -> `delete[] data_`。
>    - **关键点**：此时执行的是 `delete[] nullptr;`。
>    - **C++ 标准规定**：对空指针执行 `delete` 操作，**什么都不做，直接跳过**。
>    - **结果**：平安无事，`0x999` 里的数据还在，留给 `s2` 用。
> 4. **s2 析构**：`s2` 生命周期结束。
>    - 执行 `~MyString()` -> `delete[] data_`。
>    - 释放 `0x999`。
>    - **结果**：内存被正确回收一次。完美闭环。

### 3. 终极优化：std::move

在 `main.cpp` 中，我通过 `std::move` 将左值显式转换为右值：

```c++
vec.push_back(std::move(temp));
```

- **成就**：实现了 **Zero Copy**。从对象创建到进入容器，全程没有发生一次深拷贝。

## 🧠 技术深度解析 (Technical Deep Dive)

### 1. 为什么必须加 `noexcept`？

这是一个关键的架构细节。`std::vector` 为了保证**强异常安全性 (Strong Exception Guarantee)**，如果在搬运数据过程中发生异常，它必须能回滚到初始状态。

- 如果移动构造函数**没加** `noexcept`：Vector 不敢用它（怕搬到一半崩溃导致数据丢失），会强制降级使用拷贝构造。
- 如果**加了** `noexcept`：Vector 才会大胆地使用移动构造来扩容。

### 2. Rule of Five

本项目严格遵守了 C++11 后的资源管理规范。既然我需要手动管理 `char* data_`，我就必须显式处理以下五个函数，缺一不可：

1. **`~MyString()` (析构)**
2. **`MyString(const MyString&)` (拷贝构造)**
3. **`operator=(const MyString&)` (拷贝赋值)**
4. **`MyString(MyString&&) noexcept` (移动构造)**
5. **`operator=(MyString&&) noexcept` (移动赋值)**

## 💻 快速开始 (Usage)

### 环境要求

- CMake 3.15+
- C++20 Compiler (GCC/Clang)
- **Google C++ Style Guide** Compliant

### 构建与运行

```bash
mkdir build && cd build
cmake ..
make
./string_demo
```

## 📚 知识储备 (Prerequisites)

在进入本项目前，我在 `00_Playground` 进行了以下专项训练，为理解移动语义打下了基础：

- **`ex4_lvalue_rvalue.cpp`**: 深入剖析左值与右值的物理本质，拆穿 `std::move` 的“伪装”（它只是进行类型转换，真正干活的是移动构造函数）。