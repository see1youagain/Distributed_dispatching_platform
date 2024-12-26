#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class BranchOffice;

class Client
{
public:
    Client(const std::string &serverIP, int serverPort, const std::string &clientIP, int clientPort);
    ~Client();
    void start();
    void stop();

    void registerBranch(BranchOffice *branch); // 注册Branch以便发送数据

    void sendRequest(const std::string &message);
    void addReceiveMsgQueue(const std::string &message);

private:
    void listenForMessages();

    std::string serverIP;
    int serverPort;
    std::string clientIP;
    int clientPort;
    int clientSock;
    bool running;
    int passiveSock;

    std::mutex mtx;

    std::thread passiveThread;

    BranchOffice *branchPtr; // 指向Branch实例，用于回调发送消息
};

#endif
