#include <iostream>
#include <cstring>
#include <sys/socket.h> // 核心 socket 函数
#include <netinet/in.h> // sockaddr_in 结构体
#include <unistd.h>     // close, read, write 函数
#include <arpa/inet.h>  // inet_ntoa

const int kPort = 8080;
const int kBufferLen = 1024;

int main() {
  // 1. 创建 Socket (买电话)
  // socket(domain, type, protocol) 返回一个文件描述符（fd），后续对这个 fd 调用各种网络函数。
  //   AF_INET: 表示使用 IPv4 地址族
  //   SOCK_STREAM: TCP 流式协议
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);  
  if (server_fd == 0) {
      std::cerr << "Socket creation failed" << std::endl;
      return -1;
  }

  // 配置地址结构体 (查电话号码本)Socket 创建与错误处理
  struct sockaddr_in address; // address 里会存 IP 地址 + 端口 + 地址族 
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // 绑定本机所有 IP
  // htons 是必须的，因为网络传输是大端序，而 CPU (x86) 是小端序
  address.sin_port = htons(kPort);      // 端口号转换为网络字节序 (Host to Network Short)

  // 2. 绑定端口 (插电话线)
  // TODO: 使用 bind() 函数将 server_fd 和 address 绑定 —————— 这里的 address：本机地址（你要监听的 IP+端口）
  int bind_res = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
  if (bind_res < 0) {
    perror("bind");
    return -1;
  }
  
  // 3. 开始监听 (打开铃声)
  // TODO: 使用 listen() 函数，backlog 设置为 3
  // backlog = 3 大致表示：最多允许大约 3 个连接在队列里等待（超过的可能被拒绝或让客户端超时/重试）。
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    return -1;
  }

  std::cout << "Server is listening on port " << kPort << std::endl;

  // 4. 接受连接 (接电话)
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
  // TODO: 不要忘了关闭两个 socket fd (server_fd 和 new_socket)
  // new_socket：和当前客户端通话用的
  close(new_socket);
  // server_fd：监听用的（如果程序结束也要关）
  close(server_fd);

  
  return 0;
}