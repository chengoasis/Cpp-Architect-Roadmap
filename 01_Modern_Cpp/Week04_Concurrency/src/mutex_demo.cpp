#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

// 锁和资源是分开的，但必须配套使用
// 编译器并不知道 g_counter_mutex 是用来保护 g_global_counter 的
//   完全靠程序员自觉：如果你在一个线程里锁了 mutex 再去改 counter，但在另一个线程里直接改 counter（忘了锁），编译器是不会报错的！
//   后果：依然会发生数据竞争。、
//   结论：“君子协定”。既然大家约定了这个锁保护这个变量，所有线程访问这个变量前必须都要先拿锁。
int g_global_counter = 0;   // 1. 这是要保护的资源 (宝箱)
std::mutex g_counter_mutex; // 2. [新增] 这是保护它的锁 (宝箱锁)

void Worker(int iterations) {
  for (int i = 0; i < iterations; i++) {
    // 开启一个小作用域，为了让锁尽早释放
    {
      // 3. 锁的是 mutex
      // 这一行执行时，会自动调用 g_counter_mutex.lock()
      std::lock_guard<std::mutex> lock(g_counter_mutex);

      // 现在只有当前线程能进到这里 (临界区)
      ++g_global_counter;
    }
  }
}

int main() {
  const int kIterations = 100000;

  std::thread t1(Worker, kIterations);
  std::thread t2(Worker, kIterations);

  t1.join();
  t2.join();

  std::cout << "Expected Result: " << (kIterations * 2) << std::endl;
  std::cout << "Actual Result:   " << g_global_counter << std::endl;

  if (g_global_counter != kIterations * 2) {
    std::cout << "❌ DATA RACE DETECTED! Results do not match." << std::endl;
  } else {
    std::cout << "✅ Results match." << std::endl;
  }

  return 0;
}

/*
为什么要用 lock_guard 而不是直接 lock()？  

如果你手动写：
  g_counter_mutex.lock();   // 上锁
  g_global_counter++;       // 干活
  g_counter_mutex.unlock(); // 解锁

这样写逻辑是对的，但在 C++ 中被视为危险代码。为什么？
危险场景：假设 g_global_counter++ 这一行突然抛出了一个异常，或者你写了个 return 直接跳出了函数。
后果：unlock() 永远不会被执行！这把锁永远锁着，其他所有线程（包括主线程）都会死等，程序直接死锁 (Deadlock) 卡死。

RAII 的妙处：std::lock_guard<std::mutex> lock(g_counter_mutex);
std::lock_guard 是一个看门大爷。
  上班（构造）：只要你创建它，它立刻帮你关门上锁。
  下班（析构）：不管你是正常走、报错走、还是 return 走，只要出了 { } 这个作用域，它自动帮你开门解锁。绝对安全。
*/