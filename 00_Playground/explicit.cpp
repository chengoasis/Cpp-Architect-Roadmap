#include <iostream>

class NoExplicit {
public:
  NoExplicit(int x) { std::cout << "NoExplicit constructed with " << x << std::endl; }
};

class WithExplicit {
public:
  // 加上 explicit，禁止隐式转换
  explicit WithExplicit(int x) { std::cout << "WithExplicit constructed with " << x << std::endl; }
};

void funcA(NoExplicit obj) { std::cout << "funcA used NoExplicit " << std::endl; }
void funcB(WithExplicit obj) { std::cout << "funcB used WithExplicit " << std::endl; }

int main() {
  // 1. 隐式转换 (Implicit Conversion)
  // 编译器会自动把整数 10 转换成 NoExplicit 对象！ ------- 这在某些时候很方便，但也很危险（容易意外触发）。
  NoExplicit a = 10;
  funcA(a);
  funcA(20);

  // WithExplicit b = 300; // ❌ 编译错误！不允许用 int 直接赋值  
  // funcB(400); // ❌ 编译错误！

  // 2. 显示转换（Explicit Conversion）
  WithExplicit b = (WithExplicit)300; // 强转
  WithExplicit c(400); // 这样构造
  funcB(b);
  funcB(c);
  funcB((WithExplicit)500);

  std::cout << "Done." << std::endl;
  return 0;
}

/*
==================================================== 输出 ========================================================
NoExplicit constructed with 10
funcA used NoExplicit 
NoExplicit constructed with 20
funcA used NoExplicit 
WithExplicit constructed with 300
WithExplicit constructed with 400
funcB used WithExplicit 
funcB used WithExplicit 
WithExplicit constructed with 500
funcB used WithExplicit 
Done.
*/