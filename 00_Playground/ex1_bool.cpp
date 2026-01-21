#include <iostream>

class SmartHandle {
public:
  explicit SmartHandle(int* ptr = nullptr) : ptr_(ptr) {}

  ~SmartHandle() {
    if (ptr_) {  // 虽然 delete nullptr 是安全的，但加个判断是很多项目代码的习惯
      delete ptr_;
    }
  }

  // --- 核心知识点 ---
  // explicit: 防止 obj 被当成整数 (obj + 1)
  // operator bool: 允许 if (obj)
  explicit operator bool() const noexcept {
    return this->ptr_ != nullptr;
  }
  /*
  Q: 为什么 operator bool() 不写返回值？
  A: 规则：类型转换函数 (Conversion Function) 不需要写返回值类型，因为函数名本身就是返回值类型。
     语法规定：operator TypeName()
     写 operator bool() 时，并不是在重载一个叫 bool 的运算符（没有这种运算符），本质是在定义"如何把自己转换成 bool 类型"。

     如果写 bool operator bool()，这就很啰嗦了，C++ 编译器觉得是多此一举：既然叫 operator bool，那你肯定返回 bool ！！！

     // 假设你要把自己转成 int
     operator int() const { return 100; } // ✅ 对的
     // int operator int() ...            // ❌ 错的，语法错误
  */

private:
  int* ptr_;
};

int main() {
  // 1. 测试有效的情况 (True)
  // 写法优化：直接括号初始化，比 SmartHandle h = SmartHandle(...) 少一次移动/拷贝语义的干扰
  // SmartHandle valid_handle = SmartHandle(new int(10));
  SmartHandle valid_handle(new int(10));
  if (valid_handle) {
    std::cout << "✅ valid_handle is TRUE (as expected)" << std::endl;
  } else {
    std::cout << "❌ Error: valid_handle should be true" << std::endl;
  }

  // 2. 测试为空的情况 (False)
  SmartHandle null_handle(nullptr);
  if (null_handle) {
    std::cout << "❌ Error: null_handle should be false" << std::endl;
  } else {
    std::cout << "✅ null_handle is FALSE (as expected)" << std::endl;
  }

  // 3. 安全性测试 (关键！)
  // 取消下面这行的注释，编译器应该直接报错，而不是默默通过。
  // int risky_sum = valid_handle + 1; 
  // 报错信息是: no match for ‘operator+’ (operand types are ‘SmartHandle’ and ‘int’)
  return 0;
}

/*
==================================================== 输出 ========================================================
✅ valid_handle is TRUE (as expected)
✅ null_handle is FALSE (as expected)
*/