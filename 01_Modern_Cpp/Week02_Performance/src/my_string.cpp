#include <iostream>
#include <cstring> // 用于 strlen, strcpy

#include "my_string.hpp"

// 1. 默认构造
MyString::MyString() {
  std::cout << "---- 默认构造 -----" << std::endl;
  data_ = new char[1]; 
  data_[0] = '\0';     // 设置结束符
  length_ = 0;
}

// 2. 有参构造
MyString::MyString(const char* str) {
  std::cout << "---- 有参构造(const char*) -----" << std::endl;
  if (str == nullptr) {
    data_ = new char[1];
    data_[0] = '\0';
    length_ = 0;
  } else {
    length_ = std::strlen(str);
    data_ = new char[length_ + 1];
    std::strcpy(data_, str);
  }
}

// 3. 拷贝构造 (Deep Copy)
// 痛点：每次拷贝都要重新申请内存，非常慢！
MyString::MyString(const MyString& other) {
  std::cout << "---- 拷贝构造(Deep Copy) -----" << std::endl;
  length_ = other.length_;
  data_ = new char[length_ + 1];
  std::strcpy(data_, other.data_);
}

// 4. 拷贝赋值运算符 (Deep Copy)
MyString& MyString::operator=(const MyString& other) {
  std::cout << "---- 拷贝赋值运算符(Deep Copy) -----" << std::endl;

  // [非常重要] 1. 自赋值检测
  // 防止 s1 = s1 时，先把自己的肉割了，导致后面没法读
  if (this == &other) {
    return *this;
  }

  // 2. 释放旧内存
  // 注意：对应 new[]，这里必须用 delete[]
  if (data_) {
    delete[] data_;
  }

  // 3. 申请新内存并复制
  length_ = other.length_;
  data_ = new char[length_ + 1];
  std::strcpy(data_, other.data_);

  // 4. 返回对象本身
  return *this;
}

// 5. 析构
// 写法是 MyString::~MyString
MyString::~MyString() {
  // delete[] 对 nullptr 是安全的，所以其实不用 if (data_) 判断
  // 但为了逻辑清晰，保留判断也可以
  if (data_) {
    delete[] data_; // 必须用 delete[] 匹配 new[]
  }
}

// 6. 移动构造
MyString::MyString(MyString&& other) noexcept {
  std::cout << "[移动构造] Stealing resources!" << std::endl;
  // 1. 偷窃
  length_ = other.length_;
  data_ = other.data_;

  // 2. 释放 other 的资源
  other.length_ = 0;
  other.data_ = nullptr; // 关键：置空，防止对方析构时 delete 我们的数据
}

// 7. 移动赋值
MyString& MyString::operator=(MyString&& other) noexcept {
  // 1. 判断是否为同一个对象
  if (this == &other) return *this;

  // 2. 先释放this的资源，防止内存泄漏
  delete[] data_;
  
  length_ = other.length_;
  data_ = other.data_;

  // 3. 释放 other 的资源
  other.length_ = 0;
  other.data_ = nullptr;
  return *this;
}


void MyString::Print() const {
  // 直接打印 char* 即可，cout 会自动处理
  if (data_) {
    std::cout << data_ << std::endl;
  }
}

