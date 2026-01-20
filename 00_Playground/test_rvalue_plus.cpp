#include <iostream>
#include <utility>
#include <string>

/*
std::move 只是把对象"标记"为可被偷窃，但它自己不偷窃。真正实施"偷窃"行为的，是 std::string 的移动构造函数。

=============================================== main 函数里 =============================================== 
process(std::move(lvalue));  对 lvalue 用了 std::move。
含义：你给 lvalue 贴了个条子，上面写着 "我是废弃物，谁想要资源可以拿走"（即强制转为右值引用）。
结果：
这个"条子"被传进了 process 函数。
process 函数的参数 std::string&& s 拿到了这个引用（也就是拿到了这个条子）。
但是！ process 函数内部的代码若仅仅是 std::cout << s; std::cout 只是看了一眼这个字符串，并没有把资源拿走。（参考test_rvalue.cpp）
结局：没人动手，资源还在原地。

=============================================== process 里=============================================== 
std::string thief(std::move(s)); // 关键时刻：调用了 std::string 的构造函数
再次标记：对 s 又做了一次 std::move(s)。
因为在 C++ 中，只要一个变量有名字（哪怕它是 T&& 类型），它在当前作用域就被视为左值。为了再次把它传给 thief，你必须再次申明"它是可以被偷的"。

小偷上场：std::string thief(...) 这行代码调用了 string 类的构造函数。
实施偷窃：
  编译器发现参数是右值（因为加了 move），于是选择了 移动构造函数 (Move Constructor)。

std::string 的移动构造函数内部大概长这样（伪代码）：
string(string&& other) {
    this->data_ = other.data_; // 1. 把 s 的指针拿过来（偷）
    other.data_ = nullptr;     // 2. 把 s 的指针置空（销毁现场）
}

结局：构造函数执行完毕，s (也就是外面的 lvalue) 变成了空指针，资源归 thief 所有。
*/

void process(std::string&& s) {
  std::cout << "[右值处理] 处理临时对象/移动后的对象:" << s << std::endl;
  std::cout << "[右值处理] 准备偷走资源..." << std::endl;
  std::cout << "============================= 移动构造 =============================" << std::endl;
  std::string thief(std::move(s));
  std::cout << "小偷 thief 里的内容: " << thief << std::endl;
  std::cout << "被偷后的 s: " << s << " (应该是空的)" << std::endl;
}

void processRvalueRef(std::string&& s) {
  std::cout << "[右值处理] 处理临时对象/移动后的对象:" << s << std::endl;
  std::cout << "============================= 拷贝构造 =============================" << std::endl;
  std::string thief(s);
  std::cout << "小偷 thief : " << thief << std::endl;
  std::cout << "原主 s : " << s << std::endl; // s 还是原样，因为是拷贝（深拷贝）
}

int main() {
  std::string lvalue = "cheng";
  // process(lvalue);  // ❌ 编译错误！直接传入左值会编译错误：这是C++的保护机制
  std::cout << "process 处理前 lvalue 的内容: " << lvalue  << std::endl;
  process(std::move(lvalue));
  std::cout << "process 处理后 lvalue 的内容: " << lvalue << " (应该是空的)" << std::endl;

  std::string lvalue_ = "hello";
  processRvalueRef(std::move(lvalue_));
  
  return 0;
}