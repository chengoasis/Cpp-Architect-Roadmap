// 00_Playground/ex4_lvalue_rvalue.cpp
#include <iostream>
#include <string>
#include <utility>  // Required for std::move

// Google Style: Function names should be PascalCase.
// Const reference for L-values (persisting objects)
// 接收左值：有名字、能取地址、持久存在的变量
void AnalyzeValue(const std::string& str) {
  std::cout << "[L-value] I caught a persisting variable: " << str << std::endl;
}

// R-value reference for R-values (temporary objects)
// 接收右值：字面量、临时对象、被 std::move 标记的对象
void AnalyzeValue(std::string&& str) {
  std::cout << "[R-value] I caught a temporary/moved object: " << str << std::endl;
}

int main() {
  // Google Style: Variable names should be snake_case.
  
  // 1. 字面量 (Literal)
  std::cout << "--- Test 1: Literal String ---" << std::endl;
  // "Hello" 是临时的，没有名字，它是纯粹的右值
  AnalyzeValue("Hello World");

  // 2. 具名变量 (Named Variable)
  std::cout << "\n--- Test 2: Named Variable ---" << std::endl;
  std::string my_name = "cheng";
  // my_name 有名字，在这一行之后还能用，它是左值
  AnalyzeValue(my_name);

  // 3. std::move 的本质
  std::cout << "\n--- Test 3: std::move() ---" << std::endl;
  // 关键点：观察这里调用了哪个函数？
  AnalyzeValue(std::move(my_name));

  // 4. 验证状态
  std::cout << "\n--- Test 4: State Check ---" << std::endl;
  // 思考：std::move(my_name) 执行后，my_name 里的东西还在吗？
  std::cout << "Is my_name empty? content: \"" << my_name << "\"" << std::endl;


  return 0;
}