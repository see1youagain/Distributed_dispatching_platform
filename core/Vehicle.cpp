#include "Vehicle.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp> // JSON library
#include "BranchOffice.h"

using json = nlohmann::json;

Vehicle::Vehicle(const std::string &vehicleName, BranchOffice *parentBranch)
    : m_name(vehicleName), m_parentBranch(parentBranch), m_running(false), m_hasTask(false) {}

Vehicle::~Vehicle()
{
    stop();
}

void Vehicle::start()
{
    m_running = true;
    m_hasTask = false;
    m_workerThread = std::thread(&Vehicle::processTasks, this);
}

void Vehicle::stop()
{
    m_running = false;
    if (m_workerThread.joinable())
    {
        try
        {
            m_workerThread.join();
        }
        catch (const std::system_error &e)
        {
            std::cerr << "Error while joining vehicle thread: " << e.what() << std::endl;
        }
    }
}

void Vehicle::assignTask(const Task &task)
{
    m_currentTask = task;
    m_hasTask = true;
}

// 判断车辆是否空闲
bool Vehicle::isIdle() const
{
    return !m_hasTask; // 如果没有任务，返回true，表示空闲
}

void Vehicle::processTasks()
{
    while (m_running)
    {
        if (m_hasTask)
        {
            // std::cout << "processTasks ing" << std::endl;
            int duration = getTaskDuration(m_currentTask.GetTaskId());
            // std::cout << "task " << m_currentTask.GetTaskId() << " duration " << duration << std::endl;
            // std::cout << m_name << " started task: " << m_currentTask.GetTaskId() << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(duration)); // 模拟任务执行

            std::cout << m_name << " completed task: " << m_currentTask.GetTaskId() << " " << m_currentTask.GetDescription() << std::endl;

            m_parentBranch->notifyTaskCompletion(m_currentTask); // 通知分支机构任务完成
            m_hasTask = false;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 空闲时等待
        }
    }
}

int Vehicle::getTaskDuration(const std::string &taskId)
{
    std::ifstream routesFile("../config/tasks_list.json");
    if (!routesFile.is_open())
    {
        std::cerr << "Failed to open routes.json" << std::endl;
        return 1; // 默认任务时间
    }

    json routes;
    routesFile >> routes;

    for (const auto &task : routes["tasks_range"])
    {
        if (task["taskId"] == std::stoi(taskId)) // 如果 taskId 匹配
        {
            return task["cost"]; // 返回对应的 cost
        }
    }
    std::cerr << "Task ID " << taskId << " not found in routes.json" << std::endl;
    return 1; // 默认任务时间
}
