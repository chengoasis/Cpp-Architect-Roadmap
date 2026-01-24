#include <iostream>

#include "shared_ptr.hpp"

// 一个简单的测试类
class Ball {
public:
  Ball() { std::cout << "Ball Created" << std::endl; }
  ~Ball() { std::cout << "Ball Destroyed" << std::endl; }
};

int main() {
//   std::cout << "--- Test 1: Construction ---" << std::endl;
  
//   // 创建一个 SharedPtr
//   SharedPtr<Ball> sp1(new Ball());
  
//   // 此时引用计数应该是 1
//   std::cout << "sp1 UseCount: " << sp1.UseCount() << std::endl; // Expect: 1


//   std::cout << "--- Test 2: Copy & Sharing ---" << std::endl;

//   { // 开启一个新的作用域
//     // 1. 创建 sp1
//     SharedPtr<Ball> sp1(new Ball());
//     std::cout << "sp1 created, UseCount: " << sp1.UseCount() << std::endl; // Expect: 1

//     // 2. 拷贝构造 sp2
//     std::cout << "Creating sp2 from sp1..." << std::endl;
//     SharedPtr<Ball> sp2(sp1);
    
//     std::cout << "sp1 UseCount: " << sp1.UseCount() << std::endl; // Expect: 2
//     std::cout << "sp2 UseCount: " << sp2.UseCount() << std::endl; // Expect: 2

//     // 3. 拷贝赋值 sp3
//     std::cout << "Creating sp3 from sp2..." << std::endl;
//     SharedPtr<Ball> sp3; // 空指针
//     sp3 = sp2;           // 赋值
//     std::cout << "sp3 UseCount: " << sp3.UseCount() << std::endl; // Expect: 3
    
//     std::cout << "Exiting scope..." << std::endl;
//   } // sp1, sp2, sp3 都在这里销毁

//   std::cout << "--- End of Test ---" << std::endl;

  std::cout << "--- Test 3: Move Semantics ---" << std::endl;

  SharedPtr<Ball> sp1(new Ball());
  std::cout << "sp1 UseCount: " << sp1.UseCount() << std::endl; // Expect: 1

  // 1. 移动构造
  std::cout << "Moving sp1 to sp2..." << std::endl;
  SharedPtr<Ball> sp2(std::move(sp1));
  
  // 核心验证：
  // sp1 应该空了
  // sp2 接管资源，但 UseCount 依然是 1 (因为没有新增加持有者，只是转移了)
  std::cout << "sp1 ptr: " << sp1.Get() << std::endl;        // Expect: 0 (nullptr)
  std::cout << "sp2 UseCount: " << sp2.UseCount() << std::endl; // Expect: 1

  // 2. 移动赋值
  std::cout << "Moving sp2 to sp3..." << std::endl;
  SharedPtr<Ball> sp3;
  sp3 = std::move(sp2);

  std::cout << "sp2 ptr: " << sp2.Get() << std::endl;        // Expect: 0
  std::cout << "sp3 UseCount: " << sp3.UseCount() << std::endl; // Expect: 1

  return 0;
}