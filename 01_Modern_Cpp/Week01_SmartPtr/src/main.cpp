#include <iostream>
#include <string>
#include <utility> // 必须包含，为了使用 std::move

#include "smart_ptr.hpp"

// 定义一个测试类
class Ball {
 public:
  explicit Ball(const std::string& color) : color_(color) {
    std::cout << "构造 Ball: " << color_ << std::endl;
  }

  ~Ball() {
    std::cout << "析构 Ball: " << color_ << std::endl;
  }

  void Bounce() const {
    std::cout << "  > " << color_ << " 球正在弹跳..." << std::endl;
  }

 private:
  std::string color_;
};

int main() {
  std::cout << "=== 测试开始: 移动语义 ===" << std::endl;

  // 1. 创建一个 SmartPtr
  SmartPtr<Ball> ptr1(new Ball("Red"));
  
  if (ptr1.Get() != nullptr) {
    std::cout << "[ptr1] 拥有 Red 球的所有权" << std::endl;
    ptr1->Bounce();
  }

  std::cout << "\n--- 执行所有权转移 (Move) ---\n" << std::endl;

  // 2. 使用移动构造函数
  // std::move(ptr1) 将 ptr1 转换为右值，触发 Move Constructor
  SmartPtr<Ball> ptr2(std::move(ptr1));

  // 3. 验证转移结果
  if (ptr1.Get() == nullptr) {
    std::cout << "[ptr1] 变为空 (所有权已移交)" << std::endl;
  } else {
    std::cout << "错误: ptr1 仍然持有资源!" << std::endl;
  }

  if (ptr2.Get() != nullptr) {
    std::cout << "[ptr2] 现在拥有 Red 球的所有权" << std::endl;
    ptr2->Bounce();
  }

  std::cout << "\n--- 测试结束: 作用域结束，开始析构 ---\n" << std::endl;

  // 此时，ptr2 析构，应该打印 "析构 Ball: Red"
  // ptr1 也会析构，但因为它为空，所以什么都不做
  return 0;
}