# Week 01: Hand-written Smart Pointer

## 📖 项目简介 (Introduction)

这是 **C++ Architect Roadmap** 的第一周项目。为了深入理解 Modern C++ 的内存管理机制，我完全从零开始手写了一个智能指针类 `SmartPtr`。

该项目的核心目标是**去魅**：通过亲手造轮子，彻底搞懂 `std::unique_ptr` 背后的 RAII 机制、所有权模型以及移动语义，从而解决 C++ 开发中最令人头疼的内存泄漏 (Memory Leak) 和双重释放 (Double Free) 问题。

## 🚀 核心特性 (Key Features)

我在实现过程中逐步迭代了四个版本，最终实现了以下工业级特性：

- **RAII 自动化管理**：利用栈对象的生命周期自动释放堆内存，杜绝内存泄漏。
- **零成本抽象**：通过运算符重载 (`operator*`, `operator->`)，使其像原生指针一样易用。
- **独占所有权 (Exclusive Ownership)**：显式禁止拷贝，从编译期根除 Double Free 隐患。
- **移动语义 (Move Semantics)**：实现了高效的资源所有权转移，支持 `std::move`。

## 🛠️ 技术实现细节 (Implementation Details)

### 1. RAII 机制 (Resource Acquisition Is Initialization)

为了解决手动 `delete` 易遗忘的问题，我将资源的生命周期绑定在栈对象 `SmartPtr` 上：

- **构造时**：接管原始指针 (`T* ptr_`)。
- **析构时**：自动检查并执行 `delete ptr_`。

### 2. 伪装成指针 (Operator Overloading)

为了让 `SmartPtr` 的使用体验与原生指针一致，我重载了关键操作符：

- `T& operator*()`：返回引用，允许访问对象本体。
- `T* operator->()`：返回指针，支持直接调用成员函数（编译器的自动转发机制）。

### 3. 禁止拷贝 (No Copy)

由于 `SmartPtr` 模拟的是独占指针，两个指针指向同一块内存会导致致命的 **Double Free** 错误。 我通过 `delete` 关键字显式禁用了拷贝构造和拷贝赋值：

```c++
SmartPtr(const SmartPtr&) = delete;
SmartPtr& operator=(const SmartPtr&) = delete;
```

### 4. 移动语义 (Move Semantics)

虽然不能拷贝，但资源需要传递。我利用 C++11 的右值引用实现了所有权转移：

- **移动构造**：窃取源对象的指针，并将源对象置空 (`nullptr`)。
- **移动赋值**：先清理当前持有的资源（防止内存泄漏），再窃取新资源，最后置空源对象。

## 💻 Usage

### 环境要求

- CMake 3.15+
- C++20 Compiler (GCC/Clang)

### 构建与运行

```bash
mkdir build && cd build
cmake ..
make
./smart_ptr_demo
```

## 学习总结 (Learnings)

1. **栈与堆的关系**：利用栈对象的确定性死亡来管理堆对象的非确定性生命周期。
2. **编译期拦截**：最好的错误是在编译时发现的（如禁止拷贝），而不是运行时崩溃。
3. **所有权的本质**：谁负责 `delete`，谁就拥有所有权。
4. **移动的真相**：移动不是魔法，只是底层指针的“浅拷贝”加上原指针的“置空”。

## 📚 知识储备 (Prerequisites)

为了顺利完成本项目，我在 `00_Playground` 目录下进行了针对性的专项训练。这些微型实验涵盖了实现智能指针所需的 C++ 核心语法：

- **`explicit.cpp`**: 学习 `explicit` 关键字，防止智能指针被隐式转换为布尔值或整数（Safe Boolean Idiom）。
- **`ex1_bool.cpp`**: 深入理解布尔转换重载，确保 `if (ptr)` 语法能正确工作。
- **`ex2_swap.cpp`**: 掌握数据交换逻辑，这是实现移动语义（Move Semantics）中 "Steal & Reset" 逻辑的基础。
- **`ex3_operators.cpp`**: 练习运算符重载（Operator Overloading），特别是 `operator+` 和 `operator<<`，为重载 `*` 和 `->` 打下基础。
- **`rvalue.cpp` / `rvalue_plus.cpp`**: 攻克本项目最难的概念——**右值引用 (R-value Reference)** 与 **移动语义**，理解左值与右值的本质区别。