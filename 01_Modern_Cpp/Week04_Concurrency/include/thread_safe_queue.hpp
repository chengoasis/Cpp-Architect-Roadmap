#ifndef Week04_Concurrency_INCLUDE_THREAD_SAFE_QUEUE_HPP
#define Week04_Concurrency_INCLUDE_THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
public:
  ThreadSafeQueue() {} // 构造函数

  // 生产者调用
  void Push(T value) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.push(std::move(value)); // std::move 稍微优化一下性能
    }
    // 关键！唤醒一个正在等待的消费者(放在锁外面通知也是可以的，甚至在某些情况下性能更好，但放在锁里面最简单直观)
    cond_var_.notify_one();
  }

  // 消费者调用 ———— 等待并取出数据
  void WaitAndPop(T& value) {
    // 这里必须用 std::unique_lock，不能用 std::lock_guard。
    //   lock_guard：死板，构造锁，析构解，中间不能动。
    //   unique_lock：灵活，可以随时手动 lock/unlock。cond_var_.wait 需要在睡觉前临时解锁，所以必须配合 unique_lock 使用。
    std::unique_lock<std::mutex> lock(mutex_);

    // 当 queue 为空，返回 false ，wait 函数会立刻让当前线程“去睡觉”（阻塞），并释放锁。
    // 如果队列空，就睡觉 (wait)
    // wait 做三件事：
    // 1. 解锁 (让生产者能进去 push 数据)
    // 2. 睡觉 (阻塞当前线程)
    // 3. 醒来后自动重新上锁
    cond_var_.wait(lock, [this] {  
        return !queue_.empty();
    });

    // 取出数据
    value = std::move(queue_.front()); // move 优化性能
    queue_.pop();
  }

  // 辅助函数：判断是否为空 (要在锁里检查)
  // Q: 只是读一下状态，又不是修改数据，为什么要加锁？
  // A: std::queue::empty() 底层通常是比较 begin_ptr == end_ptr。
  //    如果不加锁：可能你在读取 begin_ptr 的瞬间，另一个线程正在修改 begin_ptr（比如正在 Pop）。
  //    所以，即使是“看一眼”，在多线程环境下也必须先锁住，看一眼，再解锁。这就是为什么 Empty() 里面也要写 std::lock_guard。
  bool Empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

private:
  std::queue<T> queue_;
  mutable std::mutex mutex_;  // mutable（可变的）。含义：“即使在一个 const 对象或 const 函数中，这个特定的变量依然允许被修改。”
  std::condition_variable cond_var_;
};

#endif // Week04_Concurrency_INCLUDE_THREAD_SAFE_QUEUE_HPP