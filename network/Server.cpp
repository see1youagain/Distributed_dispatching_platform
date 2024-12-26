#include "Server.h"
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <string>
#include "../core/LogisticsCenter.h"
#include <nlohmann/json.hpp> // JSON library

using json = nlohmann::json;

Server::Server(int port) : serverSock(-1), running(false), centerPtr(nullptr), port(port) {}

Server::~Server()
{
    stop();
}

void Server::start()
{
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0)
    {
        perror("Socket creation failed");
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(serverSock);
        return;
    }

    if (listen(serverSock, 5) < 0)
    {
        perror("Listen failed");
        close(serverSock);
        return;
    }

    running = true;
    // std::cout << "Server started on port " << port << std::endl;

    std::thread(&Server::acceptConnections, this).detach();
}

void Server::stop()
{
    if (running)
    {
        running = false;
        close(serverSock);
        for (auto &thread : clientThreads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
        std::cout << "Server stopped" << std::endl;
    }
}

void Server::acceptConnections()
{
    while (running)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSock < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                continue; // Non-blocking accept
            perror("Accept failed");
            continue;
        }
        clientThreads.emplace_back(&Server::handleClient, this, clientSock);
    }
}

void Server::handleClient(int clientSock)
{
    char buffer[1024] = {0};
    int bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // std::cout << "Received: " << buffer << std::endl;
        // Here we would handle the task request and assign it accordingly
        std::string taskMessage = std::string(buffer, bytesRead);
        centerPtr->addReceiveMsgQueue(taskMessage);
    }
    else
    {
        perror("Receive failed");
    }
    close(clientSock);
    // std::cout << "Closed connection to client" << std::endl;
}

void Server::assignTaskToClient(int clientSock, const std::string &taskMessage)
{
    send(clientSock, taskMessage.c_str(), taskMessage.size(), 0);
}

bool Server::sendTaskToBranch(const std::string &ip, int port, const std::string &taskMessage)
{
    std::lock_guard<std::mutex> lock(mtx);
    // std::cout << "start sendTaskToBranch" << std::endl;
    int tempSock = socket(AF_INET, SOCK_STREAM, 0);
    if (tempSock < 0)
    {
        perror("Socket creation failed");
        return false;
    }

    sockaddr_in clientAddr{};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &clientAddr.sin_addr) <= 0)
    {
        perror("Invalid address");
        return false;
    }

    if (connect(tempSock, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0)
    {
        std::cout << "connnetc client,ip:" << ip << " port :" << port << std::endl;
        perror("Connection failed");
        close(tempSock);
        return false;
    }
    // 发送任务消息
    if (send(tempSock, taskMessage.c_str(), taskMessage.size(), 0) < 0)
    {
        perror("Send failed");
        close(tempSock);
        return false;
    }

    // 关闭连接
    close(tempSock);
    // std::cout << "Connection closed with client" << std::endl;
    return true;
}

void Server::registerCenter(LogisticsCenter *center)
{
    centerPtr = center; // Register the BranchOffice instance to allow message passing
}