#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

// 对于简单的状态标志（如“是否停止”）或计数器，用 std::atomic 比 mutex 更快、更简单。
std::atomic<int> g_counter(0); 
// 尝试把 atomic 改成普通的 int，结果就不对劲了
// int g_counter = 0; // 如果用这个，结果往往小于 20000

void Worker() {
    for (int i = 0; i < 10000; ++i) {
        // 原子操作：++ 实际上是 fetch_add
        // 这一步绝对不会被别的线程打断
        g_counter++; 
    }
}

int main() {
    std::thread t1(Worker);
    std::thread t2(Worker);

    t1.join();
    t2.join();

    std::cout << "Final Counter: " << g_counter << std::endl;
    // 必是 20000，不需要 mutex！
    
    // 原子布尔值用法
    std::atomic<bool> stop_flag(false);
    stop_flag = true; // 线程安全的赋值
    if (stop_flag) { /* 线程安全的读取 */ }

    return 0;
}