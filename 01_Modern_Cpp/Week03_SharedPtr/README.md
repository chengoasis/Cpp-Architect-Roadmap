# Week 03: Shared Smart Pointer & Reference Counting

## 📖 项目简介 (Introduction)

这是 **C++ Architect Roadmap** 的第三周项目。 在掌握了独占资源 (`UniquePtr`) 和移动语义之后，本周我深入研究了 C++ 内存管理中最复杂的领域 —— **共享所有权 (Shared Ownership)**。

我手写了一个模板类 `SharedPtr`（对应标准库的 `std::shared_ptr`），实现了基于 **引用计数 (Reference Counting)** 的自动化内存管理。同时，通过构建一个“循环引用”的死锁场景，我亲手复现并验证了智能指针著名的 **“内存泄漏陷阱”**。

## 🚀 核心特性 (Key Features)

- **Reference Counting (引用计数)**：引入“控制块”概念，实现多个指针共享同一对象，最后一个离开的负责释放内存。
- **Rule of Five Compliant**：完整实现了拷贝构造、拷贝赋值（计数+1），以及**移动构造、移动赋值（所有权转移，计数不变）**。
- **Zero-Cost Abstraction**：通过内联与模板，尽量减少额外的运行时开销。
- **The Trap (循环引用演示)**：专门编写了测试用例，展示了强引用循环如何导致内存泄漏。

## 🧠 技术深度解析 (Technical Deep Dive)

### 1. 内存架构：栈 vs 堆 (Stack vs Heap)

通过本项目，我深刻理解了 C++ 对象模型的双重性：

- **Stack (栈)**：存放 `SharedPtr` 对象本身（轻量级，类似于“遥控器”）。**栈就像你的口袋。容量小，存取速度极快，函数结束自动清空。**
- **Heap (堆)**：存放用户数据（`T*`）和引用计数器（`int*`）。**堆就像仓库。容量巨大，但需要你手动管理（或者用智能指针管理）。**

### 2. 核心算法：控制块生命周期

- **构造 (Construction)**：`new T` 的同时，`new int(1)` 创建计数器。
- **拷贝 (Copy)**：`ptr_` 指向相同地址，计数器 `(*ref_count_)++`。
- **移动 (Move)**：接管对方的指针，对方置为 `nullptr`（防止析构破坏），计数器 **保持不变**。
- **析构 (Destruction)**：计数器 `(*ref_count_)--`。只有当计数归零时，才执行 `delete ptr_`。

### 3. 移动语义中的“安全状态”

**在实现移动赋值时，我发现必须将源对象（Move Source）的指针置为 `nullptr`。否则源对象析构时会错误地减少引用计数，导致目标对象持有的指针变成悬空指针（Dangling Pointer）。**

## 🧪 实验：循环引用陷阱 (The Circular Dependency Trap)

为了验证 `shared_ptr` 的局限性，我设计了如下实验：

### 场景描述

- 类 `Man` 持有 `SharedPtr<Woman>`。
- 类 `Woman` 持有 `SharedPtr<Man>`。
- 当两者互相赋值时 (`m->gf = w; w->bf = m;`)，双方的引用计数都变成了 **2**。

### 孤岛效应 (Island of Isolation)

当 `main` 函数结束，栈上的 `m` 和 `w` 销毁，计数从 2 降为 1。

- **结果**：堆上的 Man 和 Woman 对象依然互相指着对方，计数器无法归零。
- **现象**：程序结束时没有打印 "Destroyed" 日志，证实发生了**内存泄漏**。
- **可视化理解**：这就好比两个落水的人互相死死抓住对方，结果双双沉入水底（内存堆深处），无法浮出水面（释放）。

*(解决方案是引入 `WeakPtr`，通过打破强引用闭环来解决死锁)*

<div align="center">
  <img src="../../assets/circular_dependency.jpg" width="600" alt="SharedPtr Circular Dependency Hand-drawn Diagram" />
  <p><i>图：手绘解析循环引用导致的内存死锁 (The Deadlock)</i></p>
</div>

## 💻 快速开始 (Usage)

### 构建项目

```bash
mkdir build && cd build
cmake ..
make
```