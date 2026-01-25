#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <sstream> // 用于格式化字符串
#include "thread_pool.hpp"

// 用来防止打印乱序的锁
std::mutex g_io_mutex;

void Print(const std::string& msg) {
  std::lock_guard<std::mutex> lock(g_io_mutex);
  std::cout << msg << std::endl;
}

int main() {
  Print("--- Thread Pool Demo Start ---");

  // 1. 创建一个只有 4 个工人的线程池
  // 这意味着同时并发度最大为 4
  ThreadPool pool(4);

  // 2. 疯狂提交 20 个任务
  for (int i = 0; i < 20; ++i) {
    // Submit 接收一个 lambda 表达式作为任务
    pool.Submit([i] {
      // 获取当前线程的 ID
      // std::this_thread::get_id()：这是 C++ 标准库函数。它返回当前正在跑这段代码的线程的唯一身份证号。
      // 它返回的类型 不是 int，也不是 string，而是一个特殊的类型 std::thread::id。你不能直接把它和字符串相加。
      std::stringstream ss;
      ss << std::this_thread::get_id();   
      std::string thread_id = ss.str();

      // 模拟计算任务
      std::string msg = "[Task " + std::to_string(i) + "] is running by Thread " + thread_id;
      Print(msg);

      // 模拟耗时 100ms (假装在处理业务)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
  }

  Print("All tasks submitted to queue. Main thread is waiting...");
  
  // 让主线程睡 3 秒，给子线程足够的时间跑完任务
  // (在真实项目中，ThreadPool 析构时会自动 join 等待，这里 sleep 是为了让打印好看)
  std::this_thread::sleep_for(std::chrono::seconds(3));

  Print("--- Demo Finished (Pool will destroy now) ---");
  return 0;
}