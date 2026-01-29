// #include <iostream>
// #include <vector>
// #include <unistd.h> 

// #include "Socket.hpp"

// const int kPort = 8080;
// const int kBufferSize = 1024;

// void HandleClient(const Socket& client_sock) {
//     std::vector<char> buffer(kBufferSize);
    
//     // 获取原始句柄进行读写
//     int client_fd = client_sock.fd();

//     // 简单的 Echo 逻辑：收到什么发回什么
//     // 注意：这里我们只读一次就退出了，为了演示简单。
//     // 实际服务器通常是一个 while 循环读取，直到客户端断开。
//     ssize_t bytes_read = ::read(client_fd, buffer.data(), kBufferSize);
    
//     if (bytes_read > 0) {
//         std::string received(buffer.data(), bytes_read);
//         std::cout << "[Client] Received: " << received << std::endl;

//         // 发送回显
//         ::write(client_fd, buffer.data(), bytes_read);
//     } else if (bytes_read == 0) {
//         std::cout << "[Client] Disconnected (EOF)." << std::endl;
//     } else {
//         std::cerr << "[Client] Read error." << std::endl;
//     }
    
//     // 关键点！
//     // 函数结束，client_sock 离开作用域。
//     // 析构函数自动调用 close(fd)，无需我们手动写 close()！
// }

// int main() {
//     try {
//         // 1. 创建 Server Socket (构造函数自动 socket())
//         Socket server;

//         // 2. 绑定端口 (封装了 bind)
//         server.BindAddress(kPort);

//         // 3. 开始监听 (封装了 listen)
//         server.Listen();

//         std::cout << "Server is running on port " << kPort << "..." << std::endl;

//         // 4. 循环接受连接
//         while (true) {
//             // 阻塞等待连接...
//             std::cout << "Waiting for connection..." << std::endl;
            
//             // Accept 返回一个新的 Socket 对象 (移动语义生效！)
//             // 这里发生了所有权转移：client_fd 从内核到了这个 Socket 对象手中
//             Socket client = server.Accept();
            
//             std::cout << "Client connected!" << std::endl;

//             // 处理客户端逻辑
//             HandleClient(client);
            
//         } // client 对象在这里销毁，连接自动关闭

//     } catch (const std::exception& e) {
//         // 统一的错误处理，再也不用满屏的 if (ret < 0) perror...
//         std::cerr << "Server Error: " << e.what() << std::endl;
//         return -1;
//     }

//     return 0;
// }

// ================================================  引入线程池 ================================================ 
#include <iostream>
#include <string>
#include <vector>
#include <memory> // std::shared_ptr

#include "Socket.hpp"
#include "thread_pool.hpp"

const int kPort = 8080;

// 参数改为 shared_ptr，这样在 Lambda 里通过值传递 shared_ptr (拷贝)，
// 就可以骗过 std::function 的“必须可拷贝”检查。
void HandleClient(std::shared_ptr<Socket> client_sock) {
    // 使用 -> 访问成员
    int fd = client_sock->fd();
    std::cout << "[Thread " << std::this_thread::get_id() << "] Handling client fd: " << fd << std::endl;

    try {
        char buffer[1024];
        while (true) {
            // 使用 client_sock->fd()
            ssize_t valread = ::read(client_sock->fd(), buffer, sizeof(buffer));
            
            if (valread > 0) {
                std::string msg(buffer, valread);
                if (!msg.empty() && msg.back() == '\n') msg.pop_back();

                std::cout << "[fd " << fd << "] Recv: " << msg << std::endl;
                
                std::string reply = "Server Echo: " + msg + "\n";
                ::write(client_sock->fd(), reply.c_str(), reply.size());
            } 
            else if (valread == 0) {
                std::cout << "[fd " << fd << "] Client disconnected." << std::endl;
                break; 
            } 
            else {
                std::cerr << "[fd " << fd << "] Read error." << std::endl;
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[fd " << fd << "] Exception: " << e.what() << std::endl;
    }

    std::cout << "[Thread " << std::this_thread::get_id() << "] Finished client fd: " << fd << std::endl;
    // shared_ptr 计数归零，Socket 析构，自动 close(fd)
}

int main() {
    try {
        ThreadPool pool(4);
        std::cout << "ThreadPool started with 4 threads." << std::endl;

        Socket server;
        server.BindAddress(kPort);
        server.Listen();
        std::cout << "Server listening on port " << kPort << "..." << std::endl;

        while (true) {
            // 1. Accept 拿到一个右值 Socket
            Socket client = server.Accept();
            
            // 2. 错误 2 修正：
            // 将 Socket 移动到 shared_ptr 中。
            // shared_ptr 是可拷贝的，可以在 std::function 中传递。
            auto client_ptr = std::make_shared<Socket>(std::move(client));

            // 3. 将 shared_ptr 传入 Lambda
            // 这里的 client_ptr 是按值捕获（发生了拷贝，引用计数+1）
            pool.Submit([client_ptr]() {
                HandleClient(client_ptr);
            });

        }
    } catch (const std::exception& e) {
        std::cerr << "Main Error: " << e.what() << std::endl;
    }

    return 0;
}