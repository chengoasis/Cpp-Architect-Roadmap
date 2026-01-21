#include <iostream>
#include <string>
#include <utility>  // 提供 std::move 和 std::swap

class Resource {
public:
  // [构造函数]
  // explicit: 防止 Resource r = "string" 这种隐式转换，强制写 Resource r("string") 或者 Resource r = (Resource)"string"
  explicit Resource(std::string name) : name_(name) {
    std::cout << "构造:[" << name_ << "]" << std::endl;
  }

  // [析构函数]
  // 这是观察 RAII(Resource Acquisition Is Initialization 资源获取即初始化) 最关键的地方
  ~Resource() {
    std::cout << "析构:[" << (name_.empty() ? "Empty(被掏空了)" : name_) << "]" << std::endl;
  }

  // [Swap 函数] - 核心引擎
  // 作用：无脑交换两个对象的成员变量
  // noexcept: 保证不抛出异常，这对 STL 容器优化至关重要
  void Swap(Resource& other) noexcept {
    using std::swap; // (进阶技巧：在泛型编程中这样写更好，支持 ADL(Argument-Dependent Lookup 参数依赖查找))
    std::cout << "  -> (Swap发生前: 我是[" << name_ << "], 对方是[" << other.name_ << "])" << std::endl;
    swap(name_, other.name_); 
    std::cout << "  -> (Swap发生后: 我变成了[" << name_ << "], 对方变成了[" << other.name_ << "])" << std::endl;
  }

  // [移动赋值运算符] - Move and Swap Idiom
  // 场景: a = std::move(b);
  Resource& operator=(Resource&& other) noexcept {
    std::cout << "  -> (开始移动赋值 operator=)" << std::endl;
    // 1. 自赋值检查：如果是自己赋给自己 (a = std::move(a))，啥也不做
    if (this != &other) {
      // 2. 核心魔法：交换！
      // - 交换前：this="Old_A", other="new_B"
      // - 交换后：this="new_B", other="Old_A"
      // 核心：我们把不需要的 "Old_A" 塞给了 other
      Swap(other);
    }
    return *this;
  }

  /*
  Q: 为什么 operator= 写返回值？  -----> 对比 ex1_bool.cpp中的 operator bool()
  A: 规则：普通运算符重载 (Standard Operator Overloading) 必须写返回值。
     operator= 是一个真正的运算符（赋值号）。它的行为需要模拟原生类型（如 int）的赋值行为。

     核心原因：为了支持"连续赋值" (Chaining)
     int a, b, c;
     a = b = c = 10; // ✅ 连环赋值

     // 如果 operator= 返回 void：
     void operator=(Resource&& other) { ... }
     // 那么：
     Resource a, b, c;
     a = b = c;   // ❌ 错误！因为 (b = c) 返回了 void，接下来变成 a = (void)，编译器直接崩溃。
  */

private:
  std::string name_;
};

int main() {
  std::cout << "--- 1. main 开始 ---" << std::endl;
  Resource a("Old_A"); // a 出生

  {
    std::cout << "\n--- 2. 进入局部作用域 ---" << std::endl;
    Resource b("new_B"); // b 出生
    
    std::cout << "\n--- 3. 发生移动赋值 ---" << std::endl;
    // 此时执行 operator=
    // 预期：a 变成 "new_B"，b 变成 "Old_A"
    a = std::move(b); 
    
    std::cout << "\n--- 4. 准备离开局部作用域 ---" << std::endl;
  } // <--- 关键点！！
  // 这里 b 会析构。
  // 因为 b 刚才跟 a 换了身子，所以这里打印的析构应该是 "Old_A"！
  // 这证明了旧资源被临时对象带走了。

  std::cout << "\n--- 5. main 结束 ---" << std::endl;

  return 0;
}

/*
==================================================== 输出 ========================================================
--- 1. main 开始 ---
构造:[Old_A]

--- 2. 进入局部作用域 ---
构造:[new_B]

--- 3. 发生移动赋值 ---
  -> (开始移动赋值 operator=)
  -> (Swap发生前: 我是[Old_A], 对方是[new_B])
  -> (Swap发生后: 我变成了[new_B], 对方变成了[Old_A])

--- 4. 准备离开局部作用域 ---
析构:[Old_A]

--- 5. main 结束 ---
析构:[new_B]
*/