#include "Client.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "../core/BranchOffice.h"

Client::Client(const std::string &serverIP, int serverPort, const std::string &clientIP, int clientPort)
    : serverIP(serverIP), serverPort(serverPort), clientIP(clientIP), clientPort(clientPort), clientSock(-1), running(false), branchPtr(nullptr) {}

Client::~Client() {}

void Client::start()
{
    running = true;
    // sendRequest("hello this is client");
    passiveThread = std::thread(&Client::listenForMessages, this);
}

void Client::stop()
{
    if (!running) // 确保多次调用不会重复操作
        return;

    running = false;

    if (passiveSock >= 0)
    {
        shutdown(passiveSock, SHUT_RDWR); // 中断阻塞调用
        close(passiveSock);
        passiveSock = -1;
    }

    if (passiveThread.joinable())
    {
        try
        {
            passiveThread.join();
        }
        catch (const std::system_error &e)
        {
            std::cerr << "Error while joining passiveThread: " << e.what() << std::endl;
        }
    }
    std::cout << "Client stopped" << std::endl;
}

void Client::sendRequest(const std::string &message)
{
    std::lock_guard<std::mutex> lock(mtx);

    // 创建一个独立的 Socket
    int tempSock = socket(AF_INET, SOCK_STREAM, 0);
    if (tempSock < 0)
    {
        perror("Socket creation failed");
        return;
    }

    // 连接到服务器
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0)
    {
        perror("Invalid server IP address");
        close(tempSock);
        return;
    }

    if (connect(tempSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "connnect server,ip:" << serverIP << " port :" << serverPort << std::endl;
        perror("Connection failed");
        close(tempSock);
        return;
    }

    // std::cout << "started client port:" << clientPort << std::endl;

    if (send(tempSock, message.c_str(), message.size(), 0) < 0)
    {
        perror("Send failed");
    }
    else
    {
        // std::cout << "Message sent successfully: " << message << std::endl;
    }

    close(tempSock);
    // std::cout << "send successfully msg:" << message << std::endl;
}

void Client::listenForMessages()
{
    passiveSock = socket(AF_INET, SOCK_STREAM, 0);
    if (passiveSock < 0)
    {
        perror("Socket creation failed");
        return;
    }

    sockaddr_in clientAddr{};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(clientPort);
    if (inet_pton(AF_INET, clientIP.c_str(), &clientAddr.sin_addr) <= 0)
    {
        perror("Invalid client IP address");
        close(passiveSock);
        return;
    }

    if (bind(passiveSock, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0)
    {
        perror("Binding failed");
        close(passiveSock);
        return;
    }

    if (listen(passiveSock, 5) < 0)
    {
        perror("Listen failed");
        close(passiveSock);
        return;
    }
    std::cout << branchPtr->GetName() << " Client is listening on " << clientIP << ":" << clientPort << std::endl;
    while (running)
    {
        sockaddr_in serverAddr{};
        socklen_t addrLen = sizeof(serverAddr);
        int connSock = accept(passiveSock, (struct sockaddr *)&serverAddr, &addrLen);
        if (connSock < 0)
        {
            perror("Accept failed");
            continue;
        }

        char buffer[1024] = {0};
        int bytesRead = recv(connSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0)
        {
            // std::cout << branchPtr->GetName() << " Received from server: " << buffer << std::endl;
            std::string message(buffer, bytesRead); // 将接收到的字节转化为string
            branchPtr->addReceiveMsgQueue(message);
            // std::cout << branchPtr->GetName() << " Received from server: " << message << std::endl;
        }
        close(connSock);
    }
}

void Client::registerBranch(BranchOffice *branch)
{
    if (!branch)
    {
        std::cerr << "Error: Attempt to register a null BranchOffice" << std::endl;
        return;
    }
    branchPtr = branch;
}

void Client::addReceiveMsgQueue(const std::string &message)
{
    if (!branchPtr)
    {
        std::cerr << "Error: branchPtr is null, cannot add to receive queue" << std::endl;
        return;
    }
    branchPtr->addReceiveMsgQueue(message);
}