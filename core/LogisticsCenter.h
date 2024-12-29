#ifndef LOGISTICSCENTER_H
#define LOGISTICSCENTER_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Task.h"
#include "../network/Server.h" // 使用 Server 类
#include <queue>
#include "../utils/Logger.h"
#include "../plugins/PluginManager.h"

class LogisticsCenter
{
public:
    LogisticsCenter(const std::string &configFile, const std::string &tasksFile, const std::string &routesListPath, const std::string &logFile); // 构造函数，加载配置
    ~LogisticsCenter();                                                                                                                          // 析构函数

    template <typename T>
    bool SendToBranch(T &item, const std::string &branchName); // 向指定分支机构发送
    void PrintStatus();                                        // 打印任务状态
    std::string GetBranch();

    void addReceiveMsgQueue(const std::string &message);
    std::string fetchReceiveMsgQueue();
    void run();
    std::string selectBranchForTask();
    void shutdown();
    void checkAndUpdatePlugins();
    void scheduleTask();

private:
    void handleMsg(std::string msg);
    std::string m_name;                                                        // 调度中心名称
    std::string m_ip;                                                          // 调度中心IP
    int m_port;                                                                // 调度中心端口
    std::unordered_map<std::string, std::pair<std::string, int>> m_branchInfo; // 存储分支机构信息 (名称, {IP, Port})
    std::vector<Task> m_pendingTasks, m_activeTasks;                           // m_pendingTasks待处理任务,m_activeTasks在执行任务
    std::string tasksFile;
    Server *m_server; // 调度中心作为服务器接收来自分支的请求

    std::queue<std::string> receiveMsgQueue;         // 接收消息队列
    std::queue<std::string> sendMsgQueue;            // 发送消息队列
    std::unordered_map<std::string, bool> is_loaded; // 判断该branch是否满载
    Logger logger;
    PluginManager pluginManager;
};

#endif
