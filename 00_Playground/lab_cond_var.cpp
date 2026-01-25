#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex g_mutex;
std::condition_variable g_cv;
bool g_ready = false; // 标志位：有时候虽然铃响了，但还得看一眼是不是真有事

void Worker() {
    std::cout << "Worker: I'm waiting..." << std::endl;
    
    // 1. 拿锁 (必须用 unique_lock，因为 wait 需要中途解锁)
    std::unique_lock<std::mutex> lock(g_mutex);
    
    // 2. 等待 (Wait)
    // 解释：如果 g_ready 是 false，就解锁并睡觉。
    //      如果被唤醒，醒来加锁，检查 g_ready。如果是 true，才往下走。
    g_cv.wait(lock, []{ return g_ready; });

    // 3. 醒来干活
    std::cout << "Worker: I'm awake and working!" << std::endl;
}

int main() {
    std::thread t1(Worker);

    // 模拟主线程做一些准备工作
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    {
        // 4. 修改状态
        std::lock_guard<std::mutex> lock(g_mutex);
        g_ready = true;
        std::cout << "Main: Ready flag set!" << std::endl;
    } // 出了大括号，锁释放，但 Worker 还在睡

    // 5. 发信号 (Notify)
    std::cout << "Main: Notifying worker..." << std::endl;
    g_cv.notify_one(); // 叮！起来干活！

    t1.join();
    return 0;
}