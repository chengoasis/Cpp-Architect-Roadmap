#include <iostream>
#include <utility>
#include <string>

void process(std::string& s) { // 传引用进去，地址完全没变！
  std::cout << "[左值处理] 处理变量:" << s << std::endl;
  std::cout << "左值引用: " << s << " address is " << &s << std::endl;
}

void process(std::string&& s) {
  std::cout << "[右值处理] 处理临时对象/移动后的对象:" << s << std::endl;
  std::cout << "右值引用: " << s << " address is " << &s << std::endl;
}

int main() {
  std::string lvalue = "cheng";

  std::cout << "L-value: " << lvalue << " address is " << &lvalue << std::endl;
  // 1. 传左值 (L-value)
  // lvalue 是一个有名字的变量，持久存在
  process(lvalue);

  // 2. 传右值 (R-value)
  // "rvalue" 是临时字面量，用完即毁
  process("rvalue");  // 这是一个全新的临时对象，地址不同

  // 3. std::move 的作用
  // std::move 并没有移动任何东西！
  // 它只是把 lvalue 强制转换成了"右值引用"类型，
  // 告诉编译器："我觉得这个 lvalue 以后不用了，你可以把它当右值处理（比如偷走它的资源）"
  process(std::move(lvalue)); // <-- 注意！！地址还是原来那个！

  // 为什么字符串没有被"偷走"？ 既然我 move 了，为什么最后一行 lvalue 还是 "cheng"？不是应该变成空吗？”
  // std::move 只是给了你"抢劫"的资格（把变量转成右值引用），但实际上抢不抢，取决于接收方做了什么。
  // 因为传参只是传引用（类似于传地址）。process 函数只是拿到了地址，并没有触发任何"移动构造"或"移动赋值"的代码。就像你把房产证交给中介看了一眼，房子还是你的。
  std::cout << "L-value: " << lvalue << " address is " << &lvalue << std::endl;
  return 0;
}

/*
==================================================== 输出 ========================================================
L-value: cheng address is 0x7ffca6f6bef0
[左值处理] 处理变量:cheng
左值引用: cheng address is 0x7ffca6f6bef0
[右值处理] 处理临时对象/移动后的对象:rvalue
右值引用: rvalue address is 0x7ffca6f6bf10
[右值处理] 处理临时对象/移动后的对象:cheng
右值引用: cheng address is 0x7ffca6f6bef0
L-value: cheng address is 0x7ffca6f6bef0
*/