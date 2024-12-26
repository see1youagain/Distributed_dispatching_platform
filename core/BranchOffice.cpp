#include "BranchOffice.h"
#include "../network/Client.h"
#include <iostream>
#include <nlohmann/json.hpp> // JSON library

using json = nlohmann::json;

BranchOffice::BranchOffice(const std::string &name, const std::string &serverIP, int serverPort, const std::string &clientIP, int clientPort)
    : m_name(name), m_client(new Client(serverIP, serverPort, clientIP, clientPort))
{
    // Register itself to the Client for receiving messages
    m_client->registerBranch(this);
    for (int i = 0; i < num_vehicle; i++)
    {
        m_vehicles.push_back(new Vehicle(name + "_Vehicle" + std::to_string(i), this));
    }
    // 启动所有车辆
    for (auto &vehicle : m_vehicles)
        vehicle->start();
    m_client->start();
    // std::cout << "branch " << name << " have started" << std::endl;
}

BranchOffice::~BranchOffice()
{
    m_client->stop();
    for (auto &vehicle : m_vehicles)
        vehicle->stop();
}

void BranchOffice::addReceiveMsgQueue(const std::string &message)
{
    receiveMsgQueue.push(message);
    handleMsg(fetchReceiveMsgQueue());
}

std::string BranchOffice::fetchReceiveMsgQueue()
{
    if (!receiveMsgQueue.empty())
    {
        std::string message = receiveMsgQueue.front();
        receiveMsgQueue.pop();
        return message;
    }
    return "";
}

void BranchOffice::notifyTaskCompletion(const Task &task)
{
    // std::cout << "Branch " << m_name << " completed task: " << task.GetTaskId() << std::endl;
    // 告诉LogisticsCenter 任务完成
    // 从任务队列中取出下一个任务，分配给空闲车辆
    json msg_json;
    msg_json["type"] = "finish_task";
    msg_json["branch"] = m_name;
    msg_json["taskid"] = task.GetTaskId();
    m_client->sendRequest(msg_json.dump());
    // std::cout << "Sent message: " << msg_json.dump() << std::endl;
}

bool BranchOffice::ReceiveTask(const Task &task)
{
    // 需要安排 Vehicle执行
    // 若Vehicle全部有任务，回发任务，拒绝执行
    for (auto v : m_vehicles)
    {
        if (v->isIdle())
        {
            v->assignTask(task);
            // std::cout << "Task received by " << m_name << ": " << task.GetDescription() << std::endl;
            return true;
        }
    }
    return false;
}

std::string BranchOffice::GetName() const
{
    return m_name;
}

void BranchOffice::handleMsg(std::string message)
{
    if (message.length() == 0)
    {
        return;
    }
    json msg_json = json::parse(message);
    std::string type = msg_json["type"];
    if (type == "add_task")
    {
        std::string task_str = msg_json["task"];
        json task_json = json::parse(task_str); // 解析嵌套 JSON
        Task task = Task::from_json(task_json);
        if (ReceiveTask(task))
        {
            // std::cout << m_name << " received the task " << task.GetTaskId() << std::endl;
        }
        else
        {
            json reject_msg;
            reject_msg["type"] = "reject_task";
            reject_msg["branch"] = m_name;
            reject_msg["taskid"] = task.GetTaskId();
            m_client->sendRequest(reject_msg.dump());
        }
    }
    else if (type == "update_plugin")
    {
        std::cout << "not finish" << std::endl;
    }
    else if (type == "send_msg")
    {
        if (msg_json["msg"] == "shutdown branch")
        {
            std::cout << "Branch " << m_name << " received shutdown signal." << std::endl;

            for (auto &vehicle : m_vehicles)
                vehicle->stop();
            m_client->stop();
        }
    }
}