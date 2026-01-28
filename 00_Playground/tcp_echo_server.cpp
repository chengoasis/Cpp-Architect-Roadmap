#include <iostream>
#include <cstring>
#include <sys/socket.h> // 核心 socket 函数
#include <netinet/in.h> // sockaddr_in 结构体
#include <unistd.h>     // close, read, write 函数
#include <arpa/inet.h>  // inet_ntoa

const int kPort = 8080;
const int kBufferLen = 1024;

int main() {
  // 1. 创建套接字 Socket (买电话)
  // socket(domain, type, protocol) 返回一个文件描述符（fd），后续对这个 fd 调用各种网络函数。
  // 参数1：AF_INET - 地址族，表示使用 IPv4
  // 参数2：SOCK_STREAM - 套接字类型，表示面向连接的 TCP 套接字
  // 参数3：0 - 协议，0 表示自动选择（TCP 对应 IPPROTO_TCP）
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);  
  if (server_fd == 0) {
      std::cerr << "Socket creation failed" << std::endl;
      return -1;
  }

  // 配置地址结构体 (查电话号码本)Socket 创建与错误处理 —————— 存 IP 地址 + 端口 + 地址族 
  /*
  struct sockaddr_in {
    // 相当于"邮寄方式"：平邮/快递/挂号信
    sa_family_t    sin_family;    // 地址族：AF_INET（IPv4）
    
    // 相当于"端口号"
    in_port_t      sin_port;      // 端口号（16位）
    
    // 相当于"具体地址"
    struct in_addr sin_addr;      // IP地址（32位） 注意！！！sin_addr是一个结构体变量
    
    // 相当于"填充物"，让信封大小统一
    unsigned char  sin_zero[8];   // 填充字段（为了和sockaddr对齐）
  };
  */
  struct sockaddr_in address; 
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // 绑定本机所有 IP
  // htons 是必须的，因为网络传输是大端序，而 CPU (x86) 是小端序
  address.sin_port = htons(kPort);      // 端口号转换为网络字节序 (Host to Network Short)

  // 2. 绑定端口 (插电话线) —————— 将套接字绑定到特定的 IP 地址和端口
  // TODO: 使用 bind() 函数将 server_fd 和 address 绑定 —————— 这里的 address：本机地址（你要监听的 IP+端口）
  // 参数1：套接字文件描述符
  // 参数2：指向 sockaddr 结构体的指针（包含地址信息）
  // 参数3：地址结构体的大小
  int bind_res = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
  if (bind_res < 0) {
    perror("bind");
    return -1;
  }
  
  // 3. 开始监听 (打开铃声) —————— 将套接字置于被动监听模式，准备接受连接
  // 参数1：已绑定的套接字
  // 参数2：等待连接队列的最大长度（backlog）（超过的可能被拒绝或让客户端超时/重试）
  // TODO: 使用 listen() 函数，backlog 设置为 3
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    return -1;
  }

  std::cout << "Server is listening on port " << kPort << std::endl;

  // 4. 接受连接 (接电话) ———————— 从已完成连接队列中取出一个连接
  // 阻塞行为：如果队列为空，线程会阻塞等待
  // 返回值：新的文件描述符（专门用于与这个客户端通信）
  // ！！！重要：监听套接字继续监听，每个客户端获得独立的通信套接字
  int new_socket;
  socklen_t addrlen = sizeof(address);  
  // TODO: 使用 accept() 阻塞等待连接 ———— 这里的address：客户端地址（谁连进来了）
  new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
  if (new_socket < 0) {
      perror("accept");
      return -1;
  }
  
  std::cout << "Connection accepted!" << std::endl;

  // 5. 读写数据 (通话)
  char buffer[kBufferLen] = {0};
  // TODO: 使用 read() 从 new_socket 读取数据到 buffer
  // TODO: 使用 write() 或 send() 把 buffer 的内容发回给 new_socket (Echo)
  // 5.1 读取数据（阻塞，直到收到数据/对端关闭/出错）
  int valread = read(new_socket, buffer, kBufferLen - 1); // 留 1 字节给 '\0'
  if (valread < 0) {
      perror("read");
      // 出错也要关 socket
      close(new_socket);
      close(server_fd);
      return -1;
  }
  if (valread == 0) {
      // 客户端正常关闭连接（挂电话）
      std::cout << "Client closed connection.\n";
  } else {
      buffer[valread] = '\0'; // 确保当作 C 字符串打印安全
      std::cout << "Received: " << buffer << std::endl;

      // 5.2 回显（Echo）
      int sent = write(new_socket, buffer, valread); // 或 send(new_socket, buffer, valread, 0)
      if (sent < 0) {
          perror("write");
      }
  }
  
  std::cout << "Received: " << buffer << std::endl;

  // 6. 关闭 (挂电话)
  // 功能：关闭文件描述符，释放系统资源
  // 重要：不关闭会导致资源泄漏（文件描述符是有限资源）
  // new_socket：和当前客户端通话用的
  close(new_socket);
  // server_fd：监听用的（如果程序结束也要关）
  close(server_fd);

  
  return 0;
}