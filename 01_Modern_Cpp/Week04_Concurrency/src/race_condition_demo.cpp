#include <iostream>
#include <thread>
#include <vector>

// [Google Style] 全局变量通常以 g_ 开头，或者尽量避免。
int g_global_counter = 0; 

// 线程工作函数：执行具体的计数任务
void Worker(int iterations) {
  for (int i = 0; i < iterations; ++i) {
    // ⚠️ 危险操作！发生数据竞争
    // ++g_global_counter 并不是原子操作。 --------- 它在 CPU 层面分为三步：1. 读取内存 2. 寄存器加一 3. 写回内存
    ++g_global_counter;
  }
}

int main() {
  std::cout << "--- The Data Race Experiment ---" << std::endl;

  const int kIterations = 100000;

  // 1. 创建并启动线程
  // 在构造函数中直接传入函数名和参数，线程立即启动。
  // 语法：std::thread 变量名(函数名, 参数1, 参数2...);
  //   第一个参数：是你想让线程执行的函数名（函数指针）。
  //   后续参数：是传给这个函数的参数。
  std::thread t1(Worker, kIterations);
  std::thread t2(Worker, kIterations);

  // 2. 等待线程结束 (Join)
  // 主线程必须等待 t1 和 t2 执行完毕。
  // 如果不调用 join()，main 函数退出时 t1/t2 还在跑，会导致 std::terminate 崩溃。
  t1.join();
  t2.join();

  // 3. 输出结果
  std::cout << "Expected Result: " << (kIterations * 2) << std::endl;
  std::cout << "Actual Result:   " << g_global_counter << std::endl;

  if (g_global_counter != kIterations * 2) {
    std::cout << "❌ DATA RACE DETECTED! Results do not match." << std::endl;
  } else {
    std::cout << "✅ Lucky! Results match (but it's still unsafe)." << std::endl;
  }

  return 0;
}