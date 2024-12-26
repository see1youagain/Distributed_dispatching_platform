#ifndef BRANCHOFFICE_H
#define BRANCHOFFICE_H

#include <string>
#include "Task.h"
#include "../network/Client.h"
#include "Vehicle.h"
#include <queue>

class BranchOffice
{
public:
    BranchOffice(const std::string &name, const std::string &serverIP, int serverPort, const std::string &clientIP, int clientPort);
    ~BranchOffice();

    bool ReceiveTask(const Task &task); // 判断能否执行

    std::string GetName() const;

    void addReceiveMsgQueue(const std::string &message);
    std::string fetchReceiveMsgQueue();
    void handleMsg(std::string message);

    void notifyTaskCompletion(const Task &task); // 通知任务完成

private:
    std::string m_name;
    Client *m_client;

    int num_vehicle = 2;
    std::vector<Vehicle *> m_vehicles; // 车辆列表

    std::queue<std::string> receiveMsgQueue; // 接收消息队列
    std::queue<std::string> sendMsgQueue;    // 发送消息队列
    // std::mutex mtx_receive;                  // 接收消息队列锁
    // std::mutex mtx_send;                     // 发送消息队列锁
    // std::condition_variable cv_send;         // 用于通知发送线程
};

#endif
