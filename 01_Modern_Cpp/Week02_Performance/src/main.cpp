// src/main.cpp
#include <iostream>
#include <vector>

#include "my_string.hpp"

int main() {
  std::cout << "=== The Cost of Copying: Vector Reallocation Demo ===" << std::endl;

  // 创建一个容器
  std::vector<MyString> vec;

  // 这里的预期是：只打印 3 次拷贝构造日志？
  // 实际上：你会看到大量的 "Deep copy"！
  // 观察当 i 增加时，旧的元素是如何被反复拷贝的。
  for (int i = 0; i < 3; ++i) {
    std::cout << "\n--- Iteration " << i << ": Pushing back ---" << std::endl;
    
    // 1. Construct a temporary string (Parameterized Constructor)
    MyString temp("Hello World");
    
    // 2. Push into vector (Triggers Copy Constructor)
    // 如果 vector 需要扩容，它还会把旧元素一个个拷贝到新家
    // vec.push_back(temp);
    // std::move 把 temp 伪装成了右值，Vector 一看是右值，立马调用 Move Constructor
    vec.push_back(std::move(temp));
  }

  std::cout << "\n=== Demo Finished ===" << std::endl;
  return 0;
}

/*
========================================================================================================================
vector底层扩容机制: 
1. 对待“老员工” (扩容搬家) -> 智能移动    场景：Vector 容量不够了，需要搬家。
   Vector 的思考：
     “我要把老内存里的 MyString 搬到新内存去。我看了一下 MyString 的代码，发现它有 移动构造函数，而且还贴心地标了 noexcept。既然这样，我就不费劲复制了，直接把它们‘偷’（Move）过去！”
     结果：最后那两行 [移动构造]。这是极大的性能优化！(如果没有移动构造，这里就会是两次深拷贝)。

2. 对待“新员工” (push_back) -> 保守拷贝  场景：你调用 vec.push_back(temp)。
   Vector 的思考：
     “用户传给我的是 temp。这是一个 左值 (L-value)，也就是一个有名字的变量。根据 C++ 法律，只要变量有名字，就说明用户后面可能还要用它。所以我绝不能偷它的资源，我必须老老实实地 拷贝 (Copy) 一份。”
     结果：你看到了第二行的 ---- 拷贝构造(Deep Copy) -----。
    如果我们不需要这个temp了，也可以让 push_back 变智能 ———————— 需要把 左值 变成 右值。

注意：
这个 noexcept 起什么作用：
  它告诉 std::vector：“放心大胆地用移动构造吧！我保证在搬运数据的过程中绝不会报错（抛出异常）。”
  如果没有这个 noexcept，std::vector 为了数据安全，即使你写了移动构造函数，它也会视而不见，强制降级去调用拷贝构造函数（Deep Copy）。
*/