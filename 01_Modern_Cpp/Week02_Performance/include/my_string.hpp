#ifndef Week02_PERFORMANCE_INCLUDE_MY_STRING_HPP_
#define Week02_PERFORMANCE_INCLUDE_MY_STRING_HPP_

#include <iostream>
#include <cstring> // for strlen, strcpy

class MyString {
public:
  // 1. 默认构造
  // 目标：创建一个空字符串 "" (注意不是空指针，而是包含一个 \0 的数组)
  MyString();

  // 2. 有参构造 (const char*)
  // 目标：根据传入的 C 风格字符串，申请堆内存并复制内容
  MyString(const char* str);

  // 3. 拷贝构造 (Deep Copy)
  // 目标：深拷贝。申请新内存，把 other 的内容复制过来。
  // 要求：请打印 "Copy Constructor" 以便后续观察性能
  MyString(const MyString& other);

  // 4. 拷贝赋值运算符 (Deep Copy)
  // 目标：深拷贝。注意处理"自赋值"和"旧内存清理"。
  // 要求：请打印 "Copy Assignment" 以便后续观察性能
  MyString& operator=(const MyString& other);

  // 5. 析构
  // 目标：释放堆内存
  ~MyString();

  // 6. 移动构造 (偷窃)
  MyString(MyString&& other) noexcept;

  // 7. Move Assignment Operator (偷窃 + 清理旧账) ———— 移动赋值
  MyString& operator=(MyString&& other) noexcept;

  // 辅助打印函数
  void Print() const;

private:
  char* data_;    // 原始指针，管理堆内存
  size_t length_; // 字符串长度 (不含 \0)
};

#endif // Week02_PERFORMANCE_INCLUDE_MY_STRING_HPP_