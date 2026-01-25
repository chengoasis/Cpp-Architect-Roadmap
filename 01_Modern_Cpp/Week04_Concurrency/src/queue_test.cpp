#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <chrono>  // 用于模拟耗时操作 (sleep)

#include "thread_safe_queue.hpp"

// 1. 创建一个全局的线程安全队列
ThreadSafeQueue<int> g_queue;

// 为了让打印输出不乱序（避免 A 线程打印一半 B 线程插嘴），
// 加一个小锁专门用来保护 std::cout
std::mutex g_print_mutex;

void Print(const std::string& msg) {
  std::lock_guard<std::mutex> lock(g_print_mutex);
  std::cout << msg << std::endl;
}

// --- 生产者线程函数 ---
// 它的任务：我是生产者(id)，我要生产(count)个数据放入队列
void Producer(int id, int count) {
  for (int i = 0; i < count; ++i) {
    // 1. 生产数据 (放入 i)
    g_queue.Push(i);

    // 2. 打印日志
    Print("[Producer ID : " + std::to_string(id) + "] Pushed: " + std::to_string(i));

    // 3. 模拟生产需要一点时间 (休眠 10 毫秒)
    // 这样可以看到两个线程交替工作的效果
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  Print("[Producer ID : " + std::to_string(id) + "] Done!");
}

// --- 消费者线程函数 ---
// 它的任务：我是消费者(id)，我要取出(count)个数据放入队列
void Consumer(int id, int count) {
  for (int i = 0; i < count; ++i) {
    int value = 0;
    
    // 1. 等待并取出数据
    // 如果队列是空的，这里会阻塞(睡觉)，直到有数据被 Push 进来
    g_queue.WaitAndPop(value);

    // 2. 打印日志
    Print("    -> [Consumer ID : " + std::to_string(id) + "] Popped: " + std::to_string(value));
    
    // 3. 模拟消费也需要一点时间
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  Print("    -> [Consumer ID : " + std::to_string(id) + "] Done!");
}



int main() {
  Print("--- Thread Safe Queue Test Start ---");

  const int KTaskCount = 10;

  std::thread t1(Producer, 1, KTaskCount);

  std::thread t2(Consumer, 1, KTaskCount);

  t1.join();
  t2.join();

  Print("--- Test Finished Successfully ---");
  
  // 验证一下队列是不是空了
  if (g_queue.Empty()) {
    std::cout << "✅ Queue is empty as expected." << std::endl;
  } else {
    std::cout << "❌ Queue is NOT empty! Logic error." << std::endl;
  }

  return 0;
}