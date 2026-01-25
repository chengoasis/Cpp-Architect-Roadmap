#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <thread>
#include <functional> 
#include <atomic>

#include "thread_safe_queue.hpp" // 复用我们刚才写的队列

class ThreadPool {
public:
  // 定义“任务”类型：一个不接收参数、不返回值的函数包装器
  // void() 并不是指函数名，而是指函数签名
  using Task = std::function<void()>;

  // 构造函数：启动固定数量的线程
  explicit ThreadPool(int num_threads) : stop_(false) {
    for (int i = 0; i < num_threads; ++i) {
      // emplace_back 直接在 vector 尾部构造 std::thread 对象
      // 每个线程都在跑下面这个 lambda 表达式
      workers_.emplace_back([this] {
        while (true) {
          Task task;

          // 1. 从队列取任务
          // 如果队列为空，线程会在这里阻塞(睡觉)，直到 submit 放入新任务
          queue_.WaitAndPop(task);

          // 2. 检查是不是“毒药丸”或者停止信号
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
    // 1. 亮起红灯，告诉大家准备下班
    stop_ = true;

    // 2. 发送“空任务”作为信号 (Poison Pill)
    // 为什么要发空任务？因为如果线程都在 WaitAndPop 睡觉，
    // 我们需要 push 东西进去把它们唤醒。
    for (int i = 0; i < workers_.size(); ++i) {
      queue_.Push(nullptr);
    }

    // 3. 等待所有线程真正结束 (Join)
    for (auto& worker : workers_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  // 提交任务的接口
  // 用户传递一个 lambda 或者函数进来
  void Submit(Task task) {
    // 只要没喊停，就往队列里塞任务
    if (!stop_) {
        queue_.Push(task);
    }
  }

private:
  std::vector<std::thread> workers_;                    // 工作线程组
  ThreadSafeQueue<Task> queue_;      // 任务队列（复用之前的代码）
  std::atomic<bool> stop_;                              // 停止标志
};

#endif // THREAD_POOL_HPP