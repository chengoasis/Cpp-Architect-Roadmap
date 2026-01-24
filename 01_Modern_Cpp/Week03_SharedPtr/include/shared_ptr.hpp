#ifndef WEEK03_SHAREDPTR_INCLUDE_SHARED_PTR_HPP_
#define WEEK03_SHAREDPTR_INCLUDE_SHARED_PTR_HPP_

template <typename T>
class SharedPtr {
 public:
  // 1. Default Constructor
  SharedPtr() : ptr_(nullptr), ref_count_(nullptr) {}

  // 2. Parameterized Constructor
  explicit SharedPtr(T* ptr) : ptr_(ptr), ref_count_(nullptr) {
    if (ptr) {
      // new int(1): 申请一个 int 大小的堆内存，并初始化值为 1
      // 不要用 new int[1]，那是数组！后续必须用 delete[] 释放。但引用计数通常只是一个 int，不是数组。
        ref_count_ = new int(1);
    }
    // 如果 ptr 是 nullptr，ref_count_ 保持 nullptr (初始化列表里已做)
  }

  // 3. Destructor
  ~SharedPtr() {
    // 只有当引用计数器存在时，才需要处理
    // (如果是空指针 SharedPtr()，ref_count_ 是 nullptr，直接跳过)
    if (ptr_ && ref_count_) {
      // 1. 计数减一
      (*ref_count_)--;

      // 2. 检查是否归零
      if (*ref_count_ == 0) {
        // 3. 我是最后一个，负责清理现场
        // std::cout << "[Destructor] Ref count is 0. Freeing memory..." << std::endl; // Debug用
        delete ptr_;       // 杀掉对象 (打印 Ball Destroyed)
        delete ref_count_; // 杀掉计数器
      }
    }
  }

  // 4. Copy Constructor
  SharedPtr(const SharedPtr& other) {
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    // 只有当对方不是空指针时，才增加计数
    if (ref_count_) {
      (*ref_count_)++;
    }
  }

  // 5. Copy Assignment Operator
  SharedPtr& operator=(const SharedPtr& other) {
    // 1. 自赋值检测 (Self-assignment check)
    // 如果是 sp1 = sp1，什么都不做，直接返回
    if (this == &other) {
      return *this;
    }

    // 2. 释放旧资源 (Release old resource)
    // 这段逻辑和析构函数完全一样！
    if (ref_count_) { 
      (*ref_count_)--;
      if (*ref_count_ == 0) {
        delete ptr_;
        delete ref_count_;
        std::cout << "Old resource destroyed" << std::endl;
      }
    }

    // 3. 接管新资源 (Acquire new resource)
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;

    // 4. 增加新计数 (Increment new ref count)
    if (ref_count_) {
      (*ref_count_)++;
    }

    return *this;
  }

  // 6. Move Constructor
  SharedPtr(SharedPtr&& other) noexcept {
    // 1. Steal (偷)
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    
    // 2. Reset (擦除对方记忆) -> 关键！
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
  }

  // 7. Move Assignment Operator
  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this == &other) return *this;

    // 1. Release old resources 
    if (ref_count_) {
      (*ref_count_)--;
      if (*ref_count_ == 0) {
        delete ptr_;
        delete ref_count_;
      }
    }

    // 2. Steal (偷)
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    
    // 3. Reset (擦除对方记忆) -> 关键！
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
    
    return *this;
  }

  // Helper: Get current reference count
  int UseCount() const {
    // 安全检查
    // 如果 ref_count_ 存在，解引用它；否则返回 0
    if (ref_count_) {
      return *ref_count_;
    }
    return 0;
  }
  
  // Helper: Get raw pointer
  T* Get() const { 
      return ptr_; 
  }

  T* operator->() const {
    return ptr_;
  }

 private:
  T* ptr_;
  int* ref_count_; // The Control Block ------ 指向引用计数的指针 (int*)
};

#endif  // WEEK03_SHAREDPTR_INCLUDE_SHARED_PTR_HPP_