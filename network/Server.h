#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <vector>
#include <atomic>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

class LogisticsCenter;

class Server
{
public:
    Server(int port);
    ~Server();
    void start(); // 启动服务器以监听分支请求
    void stop();  // 停止服务器

    bool sendTaskToBranch(const std::string &ip, int port, const std::string &taskMessage);

    void registerCenter(LogisticsCenter *branch); // 注册Branch以便发送数据

private:
    int serverSock;
    int port;
    std::atomic<bool> running;
    std::vector<std::thread> clientThreads;

    std::mutex mtx;

    void acceptConnections();          // 接受新的分支连接
    void handleClient(int clientSock); // 处理客户端请求
    void assignTaskToClient(int clientSock, const std::string &taskMessage);

    LogisticsCenter *centerPtr;
};

#endif
