#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <thread>
#include <atomic>
#include "Task.h"
#include "../plugins/PluginManager.h"

class BranchOffice; // 前向声明

class Vehicle
{
public:
    Vehicle(const std::string &vehicleName, BranchOffice *parentBranch, const std::string routesListsFile);
    ~Vehicle();

    void start();                      // 开始线程
    void stop();                       // 停止线程
    void assignTask(const Task &task); // 分配任务
    bool isIdle() const;               // 判断是否空闲
    void updatePlugin();

private:
    void processTasks();                            // 处理任务的线程函数
    int getTaskDuration(const std::string &taskId); // 从routes.json中读取任务时间

    std::string m_name;
    BranchOffice *m_parentBranch; // 所属分支
    std::atomic<bool> m_running;  // 是否运行中
    std::thread m_workerThread;
    Task m_currentTask;
    std::atomic<bool> m_hasTask; // 是否有任务
    PluginManager pluginManager;
};

#endif
