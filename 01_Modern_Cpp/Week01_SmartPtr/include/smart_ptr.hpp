#ifndef CPP_ARCHITECT_ROADMAP_01_MODERN_CPP_Week01_SMART_PTR_SMART_PTR_H_
#define CPP_ARCHITECT_ROADMAP_01_MODERN_CPP_Week01_SMART_PTR_SMART_PTR_H_

#include <utility> // std::exchange 

// Google Style: 类名使用 PascalCase
template <typename T> 
class SmartPtr {
public:
  // ptr参数有默认值nullptr
  explicit SmartPtr(T* ptr = nullptr) : ptr_(ptr) {}

  ~SmartPtr() { 
    if (ptr_) {
      delete ptr_;
      ptr_ = nullptr;  // 防止悬垂指针
    }
  }

  T* Get() {
    return ptr_;
  }

  // ------------------------- Level 2: 运算符重载 -------------------------
  // 1. 解引用运算符
  // 返回 T& (引用)，允许用户直接操作对象本身
  // const 修饰函数：表示调用这个函数不会修改 SmartPtr 自身的成员变量(ptr_)
  T& operator*() const {
    return *ptr_;
  }

  // 2. 箭头运算符
  // 返回 T* (指针)，这是 C++ 编译器处理 -> 的硬性规定
  // 它会让 basket->func() 自动转发为 ptr_->func()
  T* operator->() const {
    return ptr_;
  }

  // ------------------------- Level 3: 独占所有权 -------------------------
  // --- unique_ptr 最核心的特性，也是它区别于普通指针的关键。----
  // Q: 如果不能拷贝，那怎么把一个指针传递给别人呢？  -----> level 4

  // 1. 禁用拷贝构造函数
  //    禁止 SmartPtr<Ball> p2(p1);
  SmartPtr(const SmartPtr&) = delete;

  // 2. 禁用拷贝赋值运算符
  //    禁止 p2 = p1;
  SmartPtr& operator=(const SmartPtr&) = delete;

  // ------------------------- Level 4: 转移所有权 -------------------------
  // 1. 移动构造函数
  // 目的：创建一个新对象，直接接管 other 的资源
  // 注意：构造函数没有返回值！
  // 构造函数不需要检查 self-assignment，因为新对象不可能等于旧对象
  SmartPtr(SmartPtr&& other) noexcept {
    // [偷窃]: 把别人的指针拿过来
    ptr_ = other.ptr_;
    // [销毁现场]: 把别人的指针置空，防止他析构时把球炸了
    other.ptr_ = nullptr;
    
    // 构造函数结束，this 对象就诞生了
  }

  // 2. 移动赋值运算符
  // 目的：this 已经存在了，现在要扔掉旧的，换个新的
  // 注意：返回值是 SmartPtr& (引用)，支持链式赋值 (a = b = c)
  SmartPtr& operator=(SmartPtr&& other) noexcept {
    // [自赋值检测]: 如果是 p = std::move(p)，直接返回
    if (this == &other) {
      return *this;
    }

    // [清理门户]: 
    // 如果我手里原本拿着球，我必须先把它销毁，否则就内存泄漏了。
    if (ptr_) {
      delete ptr_;   // <---------- 会触发析构
    }

    // [偷窃]: 接管资源
    ptr_ = other.ptr_;

    // [销毁现场]: 把源对象置空 ---- 解除绑定（已经移交给 this->ptr_ 了）
    other.ptr_ = nullptr;

    // 返回我自己 (*this)
    return *this;
  }

private:
  // Google Style: 成员变量以小写加下划线结尾
  T* ptr_;
};

#endif  // CPP_ARCHITECT_ROADMAP_01_MODERN_CPP_Week01_SMART_PTR_SMART_PTR_H_