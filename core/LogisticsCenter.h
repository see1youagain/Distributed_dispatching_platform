#ifndef LOGISTICSCENTER_H
#define LOGISTICSCENTER_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Task.h"
#include "../network/Server.h" // 使用 Server 类
#include <queue>

class LogisticsCenter
{
public:
    LogisticsCenter(const std::string &configFile, const std::string &tasksFile); // 构造函数，加载配置
    ~LogisticsCenter();                                                           // 析构函数

    template <typename T>
    bool SendToBranch(T item, const std::string &branchName); // 向指定分支机构发送
    void PrintStatus();                                       // 打印任务状态
    std::string GetBranch();

    void addReceiveMsgQueue(const std::string &message);
    std::string fetchReceiveMsgQueue();
    void run();
    std::string selectBranchForTask();
    void shutdown();

private:
    void handleMsg(std::string msg);
    std::string m_name;                                                        // 调度中心名称
    std::string m_ip;                                                          // 调度中心IP
    int m_port;                                                                // 调度中心端口
    std::unordered_map<std::string, std::pair<std::string, int>> m_branchInfo; // 存储分支机构信息 (名称, {IP, Port})
    std::vector<Task> m_pendingTasks, m_activeTasks;                           // m_pendingTasks待处理任务,m_activeTasks在执行任务

    Server *m_server; // 调度中心作为服务器接收来自分支的请求

    std::queue<std::string> receiveMsgQueue;         // 接收消息队列
    std::queue<std::string> sendMsgQueue;            // 发送消息队列
    // std::mutex mtx_receive;                          // 接收消息队列锁
    // std::mutex mtx_send;                             // 发送消息队列锁
    // std::condition_variable cv_send;                 // 用于通知发送线程
    std::unordered_map<std::string, bool> is_loaded; // 判断该branch是否满载
    std::mutex mtx_tasks;                            // 新增的锁，保护任务队列
};

#endif
