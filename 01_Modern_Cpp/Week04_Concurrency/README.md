# Week 04: Concurrency & Thread Pool

## 📖 项目简介 (Introduction)
这是 **C++ Architect Roadmap** 的第四周项目。
核心目标是从零打造一个工业级的 **线程池 (Thread Pool)**。我们利用 **RAII 锁管理**、**条件变量** 和 **原子操作**，实现了一个能复用线程资源、避免频繁上下文切换的高性能组件。

## 🏗️ 核心架构 (Core Architecture)

### 1. 线程安全队列 (ThreadSafeQueue)
* **位置**: `include/thread_safe_queue.hpp`
* **问题**: 标准库 `std::queue` 非线程安全，多线程并发 `push/pop` 会导致数据竞争甚至 crash。
* **核心**: **线程安全队列 = 普通队列 + 锁（保护） + 信号（通知）**
* **解决方案**:
    * **互斥锁 (`std::mutex`)**: 保护队列所有操作的原子性。
    * **条件变量 (`std::condition_variable`)**: 实现“空时等待，有时唤醒”机制，杜绝 CPU 空转 (Busy Waiting)。
    * **虚假唤醒防御**: 在 `wait` 中使用 `while (!empty)` (通过 lambda 谓词) 再次检查条件。

### 2. 线程池 (ThreadPool)
* **位置**: `include/thread_pool.hpp`
* **设计思想**: **池化技术 (Pooling)** —— 预先创建，循环使用。
* **核心组件**:
    * **Workers**: `std::vector<std::thread>`，一组死循环运行的工人线程。
    * **Tasks**: `std::function<void()>`，利用类型擦除技术，存储任意可调用的任务。
    * **Stop Flag**: `std::atomic<bool>`，线程安全的停止标志。
* **生命周期管理**:
    * **Submit**: 生产者提交任务入队。
    * **Execute**: 空闲 Worker 被唤醒，取出任务执行。
    * **Shutdown**: 析构时发送 "Poison Pill" (空任务) 或设置标志，并调用 `join()` 等待所有线程优雅退出。

<div align="center">
  <img src="../../assets/thread_pool_architecture.jpg" width="800" alt="Thread Pool Architecture Diagram" />
  <p><i>图：线程池架构与工作流全景图 (Thread Pool Architecture & Workflow)</i></p>
</div>

## 📂 目录结构 (Directory Structure)

```text
Week04_Concurrency/
├── include/
│   ├── thread_safe_queue.hpp   # 核心组件：安全队列
│   └── thread_pool.hpp         # 核心组件：线程池
├── src/
│   ├── race_condition_demo.cpp # 实验：复现数据竞争 (Data Race)
│   ├── mutex_demo.cpp          # 实验：使用 Mutex 修复竞争
│   ├── queue_test.cpp          # 测试：验证队列的生产/消费
│   └── pool_test.cpp           # 测试：验证线程池复用与高并发
├── CMakeLists.txt              # 构建脚本
└── README.md                   # 项目文档
```

## 📚 知识储备 (Prerequisites)

在进入本项目前，我在 `00_Playground` 进行了以下专项训练，为理解并发编程打下了基础：

- `lab_function.cpp`：深入理解 `std::function` 的类型擦除机制（Type Erasure），掌握它是如何统一存储普通函数、Lambda 表达式和仿函数的。
- `lab_atomic.cpp`：验证 `std::atomic` 的无锁特性，对比原子变量与普通变量在多线程竞争下的表现，理解其在线程池停止标志（Stop Flag）中的应用。
- `lab_cond_var.cpp`：模拟“生产者-消费者”的唤醒机制，深度剖析 `wait` 配合 `unique_lock` 使用的原因，以及如何利用 Lambda 谓词（Predicate）来防止虚假唤醒