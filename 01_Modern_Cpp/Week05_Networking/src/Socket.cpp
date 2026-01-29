#include <arpa/inet.h>  // inet_pton, htons
#include <cstring>      // strerror, memset
#include <iostream>     // std::cerr 

#include "Socket.hpp"

Socket::Socket() : fd_(-1) {
  // ::socket 表示调用全局命名空间中的 socket 函数, 它来自 C 标准库，在 <sys/socket.h> 中声明
  fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    throw std::runtime_error("Failed to create socket: " + 
                             std::string(strerror(errno)));
  }

  // 【优化】设置 SO_REUSEADDR 选项
  // 允许服务器重启后立刻绑定到同一个端口，不需要等待 TIME_WAIT 结束
  int opt = 1;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    // 这个错误通常不致命，打印个日志就行，或者也可以 throw
    std::cerr << "Warning: Failed to set SO_REUSEADDR" << std::endl;
  }
}

// 私有成员函数可以在 .cpp 中实现 ——————  私有指的是访问权限，不是实现位置
Socket::Socket(int fd) : fd_(fd) {
  if (fd_ < 0) {
    throw std::runtime_error("Invalid file descriptor provided");
  }
}

Socket::~Socket() {
  if (fd_ >= 0) {
    ::close(fd_);
  }
}

// 移动构造函数
Socket::Socket(Socket&& other) noexcept {
  fd_ = other.fd_;
  other.fd_ = -1;  // 转移所有权，避免重复关闭
}

// 移动赋值运算符
Socket& Socket::operator=(Socket&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  if (fd_ >= 0) {
    ::close(fd_);
  }
  fd_ = other.fd_;
  other.fd_ = -1;
  return *this;
}

void Socket::BindAddress(int port) {
  if (fd_ < 0) {
    throw std::runtime_error("Socket is not valid");
  }

  struct sockaddr_in address;
  std::memset(&address, 0, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定到所有接口
  address.sin_port = htons(static_cast<uint16_t>(port));

  int ret = ::bind(fd_, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
  if (ret < 0) {
    throw std::runtime_error("Failed to bind address: " + 
                             std::string(strerror(errno)));
  }
}

void Socket::Listen() {
  if (fd_ < 0) {
    throw std::runtime_error("Socket is not valid");
  }

  // 设置监听队列大小为10
  int ret = ::listen(fd_, 10);
  if (ret < 0) {
     throw std::runtime_error("Failed to listen: " + 
                              std::string(strerror(errno)));
  }
}

Socket Socket::Accept() {
  if (fd_ < 0) {
    throw std::runtime_error("Socket is not v valid");
  }

  struct sockaddr_in client_address;  // 创建一个缓冲区 
  socklen_t client_address_len = sizeof(client_address); // 缓冲区大小

  // 接受客户端连接 ————— accept() 函数原型
  // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  // sockfd: 监听套接字文件描述符
  // addr: 指向 sockaddr 结构的指针，用于接收客户端地址信息
  // addrlen: 输入输出参数，既是输入也是输出
  int client_fd = ::accept(fd_, reinterpret_cast<struct sockaddr*>(&client_address), &client_address_len);
  if (client_fd < 0) {
    throw std::runtime_error("Failed to accept connection: " + 
                             std::string(strerror(errno)));
  }

  // 创建新的Socket对象来管理客户端连接 ———————— hpp中私有成员函数
  return Socket(client_fd);
}

void Socket::Connect(const std::string& ip, int port) {
  if (fd_ < 0) {
    throw std::runtime_error("Socket is not valid");
  }

  struct sockaddr_in server_address;
  std::memset(&server_address, 0, sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(static_cast<uint16_t>(port));

  // 转换IP地址
  // inet_pton 返回 1 表示成功
  int ret = inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);
  if (ret <= 0) {
    throw std::runtime_error("Invalid IP address format: " + ip);
  }

  // 连接到服务器
  ret = ::connect(fd_, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address));
  if (ret < 0) {
    throw std::runtime_error("Failed to connect: " + 
                              std::string(strerror(errno)));
  }
}
