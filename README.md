# 🚀 C++ 后端架构师成长之路 (C++ System Architect Roadmap)

> **"Don't just read about it, build it."**

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg?logo=c%2B%2B)](https://isocpp.org/)
[![Linux](https://img.shields.io/badge/OS-Ubuntu_22.04-orange.svg?logo=linux)](https://ubuntu.com/)
[![Build](https://img.shields.io/badge/Build-CMake-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## 📖 项目背景
**LeetCode 刷题400+，无任项目背景**

在为期 20 周（5个月）的时间里，我将深入研究 Modern C++ (C++11/14/17/20)、Linux 系统编程、高并发网络模型，并最终**从零手写一个工业级 RPC 框架**。

---

## 🛠️ 开发环境
* **OS**: Ubuntu 22.04 LTS (WSL2 / Server)
* **Compiler**: GCC 12.3.0 (C++20 Standard)
* **Build System**: CMake 3.22+
* **Tools**: GDB, Valgrind, Perf, Git
* **Editor**: Vim / VSCode

---

## 🗺️ 学习路线与进度 (Roadmap)

### 第一阶段：Modern C++ 觉醒与重构 (Weeks 1-4)
> 目标：摒弃 C with Classes 思维，掌握 RAII 与资源管理。
- [ ] **Week 01**: RAII 与智能指针本质 (手写 `SmartPtr`)
- [ ] **Week 02**: 右值引用与移动语义 (`std::move`, Rule of Five)
- [ ] **Week 03**: Lambda 表达式与函数式编程 (`std::function`, 闭包)
- [ ] **Week 04**: STL 进阶与 Modern CMake 构建系统

### 第二阶段：Linux 系统编程与并发实战 (Weeks 5-8)
> 目标：深入 OS 内核，理解并发与 I/O 的本质。
- [ ] **Week 05**: Linux 系统调用与文件 I/O (手写 `cat`)
- [ ] **Week 06**: 多线程编程基石 (`std::thread`, `mutex`, `cond_var`)
- [ ] **Week 07**: Reactor 模型与非阻塞 I/O (Epoll 实战)
- [ ] **Week 08**: 手写线程池 (Thread Pool) 与异步编程

### 第三阶段：工业级项目 "TinyRPC" (Weeks 9-16) 🏆
> 目标：造轮子。实现一个高性能、支持 Protobuf 的 RPC 框架。
- [ ] **Week 09-10**: 通信协议设计与序列化 (Protobuf 封装)
- [ ] **Week 11-12**: 网络传输模块 (Reactor + Non-blocking IO)
- [ ] **Week 13-14**: RPC 核心调用流 (Service 注册与发现)
- [ ] **Week 15**: 异步日志系统
- [ ] **Week 16**: 性能压测与优化 (Benchmark)

### 第四阶段：源码阅读与架构设计 (Weeks 17-20)
> 目标：站在巨人的肩膀上，提升系统设计能力。
- [ ] **Week 17**: `muduo` 网络库源码剖析
- [ ] **Week 18**: `LevelDB` 存储引擎核心模块阅读
- [ ] **Week 19**: 分布式系统设计 (CAP, 一致性 Hash)
- [ ] **Week 20**: 模拟面试与简历打磨

---

## 📂 目录结构说明
```text
Cpp_Architect_Roadmap/
├── 01_Modern_Cpp/      # C++11/14/17/20 核心特性练习
├── 02_Linux_System/    # Linux 系统调用与并发编程
├── 03_TinyRPC/         # [核心项目] 高性能 RPC 框架实现
└── 04_Source_Reading/  # 经典开源库源码分析笔记
