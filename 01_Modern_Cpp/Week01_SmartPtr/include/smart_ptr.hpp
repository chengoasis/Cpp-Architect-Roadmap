#ifndef CPP_ARCHITECT_ROADMAP_01_MODERN_CPP_WEEK01_SMART_PTR_SMART_PTR_H_
#define CPP_ARCHITECT_ROADMAP_01_MODERN_CPP_WEEK01_SMART_PTR_SMART_PTR_H_

#include <utility> // 用于 std::exchange (可选，这里我们手动实现以展示原理)

// Google Style: 类名使用 PascalCase
template <typename T>
class SmartPtr {
 public:
  // 构造函数：接管原始指针的所有权
  // explicit 防止隐式类型转换 (例如 SmartPtr<int> p = 1;)
  explicit SmartPtr(T* ptr = nullptr) : ptr_(ptr) {}

  // 析构函数：负责释放资源
  ~SmartPtr() {
    if (ptr_) {
      delete ptr_;
      ptr_ = nullptr; // 最佳实践：释放后置空，避免悬空指针
    }
  }
  // --- 移动语义核心部分 (The Core of Move Semantics) ---

  // 1. 移动构造函数 (Move Constructor)
  // 目的：从一个临时对象(右值)那里"窃取"资源，而不是复制。
  // 参数：SmartPtr&& other (右值引用)
  // noexcept：承诺不抛出异常（对于放入 std::vector 非常重要）
  SmartPtr(SmartPtr&& other) noexcept : ptr_(other.ptr_) {
    // 关键步骤：将源指针置空！
    // 否则，当 'other' 析构时，资源会被释放，导致我们也指向无效内存。
    other.ptr_ = nullptr; 
  }

  // 2. 移动赋值运算符 (Move Assignment Operator)
  // 目的：支持 p1 = std::move(p2);
  SmartPtr& operator=(SmartPtr&& other) noexcept {
    // 步骤 A: 检测自赋值 (Self-assignment check)
    // 如果是自己赋给自己 (p = std::move(p))，什么都不做
    if (this == &other) {
      return *this;
    }

    // 步骤 B: 释放当前持有的资源
    // 因为我们要接管新资源，必须先清理旧的，否则会内存泄漏
    if (ptr_) {
      delete ptr_;
    }

    // 步骤 C: 窃取资源
    ptr_ = other.ptr_;

    // 步骤 D: 将源指针置空 (打断原来的所有权)
    other.ptr_ = nullptr;

    // 返回当前对象的引用，支持链式调用
    return *this;
  }

  // --- 禁止拷贝 (RAII 独占所有权) ---

  // 禁用拷贝构造
  SmartPtr(const SmartPtr&) = delete;
  // 禁用拷贝赋值
  SmartPtr& operator=(const SmartPtr&) = delete;

  // --- 常用操作符重载 ---

  // 解引用操作符，允许像使用 *p 一样使用
  T& operator*() const { return *ptr_; }

  // 箭头操作符，允许像使用 p->method() 一样使用
  T* operator->() const { return ptr_; }

  // 获取原始指针 (Google Style: 函数名 PascalCase)
  T* Get() const { return ptr_; }

 private:
  // Google Style: 成员变量以小写加下划线结尾
  T* ptr_;
};

#endif  // CPP_ARCHITECT_ROADMAP_01_MODERN_CPP_WEEK01_SMART_PTR_SMART_PTR_H_