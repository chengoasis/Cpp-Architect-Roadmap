#include <iostream>
#include <utility> 
#include <string>

#include "smart_ptr.hpp"  // 预处理器在编译之前，会把 smart_ptr.hpp 里的所有代码，原封不动地复制并替换掉这一行

class Ball {
public:  // <--- 必须公开，否则外界无法构造和析构
  // 构造函数：带个名字，方便区分
  explicit Ball(const std::string& name = "") : name_(name) {
    std::cout << "构造 Ball: " << name_ << " (地址: " << this << ")" << std::endl;
  }

  ~Ball() {
    std::cout << "析构 Ball: " << name_ << " (地址: " << this << ")" << std::endl;
  }

  // const 修饰符：表示这个函数只读取、不修改成员变量，这是好习惯
  void Bounce() const {
    std::cout << "  [验证成功] " << name_ << " 正在弹跳! "
              << "(访问的对象地址: " << this << ")" << std::endl;
  }

private:
  std::string name_;
};

int main() {
  std::cout << "=== 开始测试 Level 2 ===" << std::endl;

  // 1. 创建对象，由智能指针接管
  SmartPtr<Ball> basket(new Ball("篮球"));
  /*
  SmartPtr<Ball>
  在模板实例化 SmartPtr<Ball> 中，Ball 是一个类型参数，用于指定智能指针所管理的对象类型。
  类比于 SmartPtr<int> 管理 int 类型对象，SmartPtr<Ball> 管理 Ball 类型对象。

  源码：basket->Bounce();
  basket-> 相当于 ptr_ (Ball类型)，因为是 Ball 类型，所以可以调用 ptr_->Bounce()

  编译器展开第一步：basket.operator->()->Bounce(); (调用重载函数)
  编译器展开第二步：ptr_->Bounce(); (拿到了真正的 Ball*)
  */

  std::cout << "\n--- 测试 1: 使用箭头操作符 (basket->) ---" << std::endl;
  // 原理：编译器自动调用 basket.operator->() 拿到原始指针，再调用 Bounce
  basket->Bounce();

  std::cout << "\n--- 测试 2: 使用解引用操作符 (*basket) ---" << std::endl;
  // 原理：(*basket) 返回了 Ball& 引用，然后像普通对象一样调用 .Bounce()
  (*basket).Bounce();

  std::cout << "\n=== 测试结束 (即将自动析构) ===" << std::endl;

  /*
  SmartPtr<Ball> ptr1(new Ball());
  SmartPtr<Ball> ptr2 = ptr1;   ⚠️ 危险操作：默认拷贝构造
  ptr1->Bounce();
  ptr2->Bounce();
  
  这会让 ptr2 和 ptr1 指向同一个 Ball ------- ptr2 和 ptr1 现在都存着相同地址
  main 结束，开始析构:
  ptr2 先死 (栈是后进先出)：调用 delete ...。内存被回收，Ball 死了。
  ptr1 后死：它不知道 Ball 已经没了，它手里还拿着 ... 的地址。它再次调用 delete ...。
  操作系统介入：检测到你试图释放一块由空闲的/已回收的内存，直接强制杀掉进程 -> Segfault。
  */

  std::cout << "\n=== 开始测试 Level 4 ===" << std::endl;
  /*
  new Ball("Original") : 的地址是真正的球的地址
  &ptr1 : 这是栈 (Stack) 上的地址， 这是 盒子 A (ptr1) 住的地方。
  &ptr2 : 这是栈 (Stack) 上的地址， 这是 盒子 B (ptr2) 住的地方。

  Ball (堆内存) 是 "一张昂贵的支票"。
  ptr1 (栈变量) 是 "你的钱包"。
  ptr2 (栈变量) 是 "我的钱包"。
  */
  SmartPtr<Ball> ptr1(new Ball("Original"));

  std::cout << "--- 移动构造前 ---" << std::endl;
  // 打印盒子放在哪 (&)
  std::cout << "ptr1 住在: " << &ptr1 << std::endl; 
  // 打印盒子里装着啥 (Get) -> 应该有值
  std::cout << "ptr1 持有: " << ptr1.Get() << std::endl; 

  std::cout << "--- 移动构造后 ---" << std::endl;
  SmartPtr<Ball> ptr2 = std::move(ptr1);

  // 打印盒子放在哪 (&) -> 地址肯定变了，这是个新变量
  std::cout << "ptr2 住在: " << &ptr2 << std::endl; 
  // 打印盒子里装着啥 (Get) -> 应该是刚才那个值！
  std::cout << "ptr2 持有: " << ptr2.Get() << std::endl; 

  // 此时 ptr1 应该是空的
  std::cout << "ptr1 持有: " << ptr1.Get() << std::endl;

  if (ptr1.Get() == nullptr) {
    std::cout << "ptr1 已经被掏空了,是nullptr" << std::endl;
  }
  ptr2->Bounce(); // ptr2 接管了 ptr1 的地址
  
  std::cout << "--- 移动赋值前 ---" << std::endl;
  SmartPtr<Ball> ptr3(new Ball("New Ball"));
  std::cout << "ptr3 住在: " << &ptr3 << std::endl; 
  std::cout << "ptr3 持有：" << ptr3.Get() << std::endl;
  
  std::cout << "--- 移动赋值后 ---" << std::endl;
  // ptr3 原本指向 New Ball，现在接管 ptr2 (Original)
  // 触发了delete，所以会析构掉new Ball("New Ball")
  ptr3 = std::move(ptr2);
  std::cout << "ptr3 住在: " << &ptr3 << std::endl; 
  std::cout << "ptr3 持有：" << ptr3.Get() << std::endl;
  // 预期：New Ball 被析构，ptr3 拿到 Original
  if (ptr2.Get() == nullptr) {
    std::cout << "ptr2 已经被掏空了,是nullptr" << std::endl;
  }
  ptr3->Bounce(); // ptr3 现在指向 Original

  return 0;
}