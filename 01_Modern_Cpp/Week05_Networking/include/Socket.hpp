#ifndef WEEK05_NETWORKING_SOCKET_H_
#define WEEK05_NETWORKING_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>     // 用于 close()
#include <string>
#include <stdexcept>    // 用于 std::runtime_error

// 封装原始 POSIX Socket，使用 RAII (资源获取即初始化) 惯用法。
// 该类负责管理 Socket 文件描述符的生命周期。
class Socket {
public:
  // 构造函数：创建一个新的 TCP Socket (AF_INET, SOCK_STREAM)。
  // 如果创建失败，抛出 std::runtime_error 异常。
  Socket();

  // 析构函数：如果文件描述符有效，自动关闭 Socket。
  // 这是防止资源泄露的关键。
  ~Socket();

  // 禁用拷贝（浅拷贝）：Socket 代表唯一的系统资源（类似 std::unique_ptr）。
  // 拷贝会导致两个对象管理同一个 fd，从而引发 "Double Close" (重复关闭) 问题。
  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  // 允许移动：允许转移 Socket 文件描述符的所有权。
  // 这对于从函数返回 Socket 对象（例如 Accept）是必须的。
  // noexcept: 承诺不抛出异常，有助于编译器优化。
  Socket(Socket&& other) noexcept;
  Socket& operator=(Socket&& other) noexcept;

  // 服务端方法：将 Socket 绑定到本机所有 IP (INADDR_ANY) 的指定端口。
  // 如果失败，抛出 std::runtime_error。
  void BindAddress(int port);

  // 服务端方法：将 Socket 标记为被动监听模式，准备接受传入连接。
  // 如果失败，抛出 std::runtime_error。
  void Listen();

  // 服务端方法：阻塞并等待客户端连接。
  // 返回值：一个代表已连接客户端的 *新* Socket 对象。
  // 如果失败，抛出 std::runtime_error。
  Socket Accept();

  // 客户端方法：连接到指定的 IP 和端口。
  // 如果失败，抛出 std::runtime_error。
  void Connect(const std::string& ip, int port);

  // 获取原始文件描述符 (通常用于 select/poll/epoll)。
  int fd() const { return fd_; }

private:
  // 私有构造函数：专门给 Accept() 内部使用。
  // 用于将一个已存在的 raw fd 包装成 Socket 对象。
  explicit Socket(int fd);
  int fd_;  // 原始 Socket 文件描述符。如果无效则初始化为 -1。
};

#endif  // WEEK05_NETWORKING_SOCKET_H_