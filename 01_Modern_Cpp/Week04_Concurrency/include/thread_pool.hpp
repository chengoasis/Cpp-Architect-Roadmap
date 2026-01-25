#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <thread>
#include <functional> 
#include <atomic>

#include "thread_safe_queue.hpp" // 复用我们刚才写的队列

class ThreadPool {
public:
  // 它可以装入任何不接受参数且没有返回值（void()）的函数、Lambda 表达式或者仿函数。
  // void() 并不是指函数名，而是指函数签名
  using Task = std::function<void()>;

  // 构造函数：启动固定数量的线程
  explicit ThreadPool(int num_threads) : stop_(false) {
    for (int i = 0; i < num_threads; ++i) {
      // emplace_back 直接在 vector 尾部构造 std::thread 对象
      // 每个线程都在跑下面这个 lambda 表达式
      workers_.emplace_back([this] {
        // while(true) 是写在一个 Lambda 表达式里的，而这个 Lambda 被交给了 std::thread 去在一个“平行时空”里运行。
        while (true) {
          // task 是一个 std::function<void()> 类型的对象。
          //   它的状态：空的 (Empty)。它里面没有代码，什么都干不了。
          //   它的身份：它只是一个容器，准备用来装东西。
          //   它代表什么函数：此时此刻，它不代表任何函数。
          Task task;

          // 1. 从队列取任务
          // 如果队列为空，线程会在这里阻塞(睡觉)，直到 submit 放入新任务
          // 回看 WaitAndPop 的实现 : void WaitAndPop(T& value) { ... }
          // 函数内是将我们需要的给到 value
          queue_.WaitAndPop(task);

          // 2. 检查是不是“毒药丸”或者停止信号
          // 如果在 WaitAndPop 中没有取到数据，task就是 nullptr
          // 如果任务是空的 (nullptr) 或者 收到停止信号且队列空了
          if (task == nullptr || (stop_ && queue_.Empty())) {
             return; // 线程函数返回，意味着线程结束（下班）
          }

          // 3. 执行任务
          // 这就是 std::function 的魔力，像调用普通函数一样调用它
          task();
        }
      });
    }
  }

  // 析构函数
  ~ThreadPool() {
    // 1. 告诉大家准备下班
    stop_ = true;

    // 2. 发送“空任务”作为信号 
    // 为什么要发空任务？因为如果线程都在 WaitAndPop 睡觉，需要 push 东西进去把它们唤醒。
    for (int i = 0; i < workers_.size(); ++i) {
      queue_.Push(nullptr);
    }

    // 3. 等待所有线程真正结束 (Join)
    for (auto& worker : workers_) {
      if (worker.joinable()) { // joinable() 返回 true 表示这个线程还在活跃（或者已经跑完但还没汇报）
        worker.join(); // join() 的作用：主线程阻塞等待子线程结束。
      }
    }
  }

  // 提交任务的接口
  void Submit(Task task) {
    // 只要没喊停，就往队列里塞任务
    if (!stop_) {
        queue_.Push(task);
    }
  }

private:
  std::vector<std::thread> workers_;    // 工作线程组
  ThreadSafeQueue<Task> queue_;         // 任务队列（复用之前的代码）
  std::atomic<bool> stop_;              // 停止标志
};

#endif // THREAD_POOL_HPP