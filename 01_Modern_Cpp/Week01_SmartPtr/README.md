# Week 01: RAII 与智能指针 (Smart Pointer)

## 🎯 学习目标
- [x] 理解 RAII (Resource Acquisition Is Initialization) 核心思想
- [x] 手写 `unique_ptr` 核心逻辑
- [x] 掌握 C++11 移动语义 (`std::move`) 的本质
- [x] 遵循 Google C++ Style Guide 重构代码

## 💡 核心知识点 (Notes)

### 1. 为什么需要 RAII？
> 传统的 `new` / `delete` 容易导致内存泄漏，尤其是在异常发生时。RAII 利用**栈对象自动析构**的特性来管理堆内存。
- **构造**：获取资源 (malloc/new)
- **析构**：释放资源 (free/delete)

### 2. 移动语义 (Move Semantics)
在实现 `SmartPtr` 时，我发现不能简单地禁用拷贝。为了支持函数返回和容器存储，必须实现**移动构造**和**移动赋值**。
- **核心逻辑**：`ptr_ = other.ptr_; other.ptr_ = nullptr;`
- **本质**：所有权的"窃取"而非"复制"。

### 3. Google Style 规范
- 成员变量必须以 `_` 结尾（如 `ptr_`）。
- 头文件宏保护格式：`PROJECT_PATH_FILE_H_`。
- `CMake` 中二进制文件命名应使用 `snake_case`。

## 💻 代码实现细节
- **类模板**：使用 `template <typename T>` 支持任意类型。
- **Explicit**：构造函数使用 `explicit` 防止 `SmartPtr<int> p = 1;` 这种隐式转换。
- **Noexcept**：移动构造函数标记为 `noexcept`，这对 `std::vector` 的扩容优化至关重要。

## 🐛 遇到的坑 (Pitfalls)
1. **Double Free**: 最初在移动赋值时，忘记将 `other.ptr_` 置空，导致源对象析构时重复释放内存。
2. **CMake 版本**: 使用了 `CMake 3.10` 但开启了 `C++20`，被提醒应该升级到 `3.15+`。

## 📚 参考资料
- [CppReference - std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)