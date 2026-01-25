#include <iostream>
#include <functional> 

// 1. 普通函数
void SayHello() {
    std::cout << "1. Hello from Normal Function!" << std::endl;
}

// 2. 仿函数 (Functor) - 一个重载了 () 的类
struct Greeter {
    void operator()() {
        std::cout << "2. Hello from Functor Object!" << std::endl;
    }
};

int main() {
    // 定义一个能存“无参无返回值”函数的容器
    // 不接收参数，也不返回任何值的“可调用对象”
    std::function<void()> task;  // 普通函数、Lambda 表达式、仿函数，只要签名对，它都能装。

    // A. 存普通函数
    task = SayHello;
    task(); // 调用

    // B. 存仿函数
    Greeter greeter_obj;
    task = greeter_obj;
    task(); // 调用

    // C. 存 Lambda 表达式 (最常用！)
    // [] 代表不捕获外部变量，{} 里是代码逻辑
    task = []() {
        std::cout << "3. Hello from Lambda!" << std::endl;
    };
    task(); // 调用

    return 0;
}