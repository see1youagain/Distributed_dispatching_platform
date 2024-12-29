#include "LogisticsCenter.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp> // JSON library
#include "BranchOffice.h"

using json = nlohmann::json;

std::string LogisticsCenter::GetBranch()
{
    std::string res = "";
    for (auto map : m_branchInfo)
    {
        res += map.first;
        res += "\n";
    }
    return res;
}

LogisticsCenter::LogisticsCenter(const std::string &configFile, const std::string &tasksFile,
                                 const std::string &routesListPath, const std::string &logFile) : tasksFile(tasksFile), logger(logFile), pluginManager(routesListPath)
{
    // Load configuration from config.json
    std::ifstream configFileStream(configFile), tasksFileStream(tasksFile);
    if (!configFileStream)
    {
        std::cerr << "Could not open config file: " << configFile << std::endl;
        return;
    }

    json config, tasks;
    configFileStream >> config;
    tasksFileStream >> tasks;

    // std::cout << "IP: " << config["logistics_center"]["ip"].dump() << std::endl;
    // std::cout << "Port: " << config["logistics_center"]["port"].dump() << std::endl;

    m_name = config["logistics_center"]["name"];
    m_ip = config["logistics_center"]["ip"];
    m_port = config["logistics_center"]["port"];

    // Read branch office details from config file
    for (const auto &branch : config["branch_offices"])
    {
        // std::cout << "Branch Name: " << branch["name"] << std::endl;
        // std::cout << "Branch IP: " << branch["ip"] << std::endl;
        // std::cout << "Branch Port: " << branch["port"] << " (Type: " << branch["port"].type_name() << ")" << std::endl;

        std::string name = branch["name"];
        std::string ip = branch["ip"];
        int port = branch["port"];
        // std::cout << "create branch" << name << std::endl;
        m_branchInfo[name] = std::make_pair(ip, port);
        is_loaded[name] = false;
    }
    m_server = new Server(m_port);
    // Start the server to handle requests from branches
    m_server->registerCenter(this);
    m_server->start();

    for (const auto &task : tasks["version" + pluginManager.getCurrentVersion()])
    {
        m_pendingTasks.push_back(
            Task(task["m_description"], std::to_string(task["taskId"].get<int>())));
    }
    std::cout << "pluginManager version" << pluginManager.getCurrentVersion() << std::endl;
}

LogisticsCenter::~LogisticsCenter()
{
    m_server->stop();
}

void LogisticsCenter::scheduleTask()
{
    while (true)
    {
        // std::lock_guard<std::mutex> lock(mtx_tasks);
        if (m_pendingTasks.empty() && m_activeTasks.empty())
        {
            break; // 所有任务都完成了
        }
        handleMsg(fetchReceiveMsgQueue());

        if (!m_pendingTasks.empty())
        {
            Task task = m_pendingTasks.back();
            std::string branch_name = selectBranchForTask();
            // std::cout << "branch_name:" << branch_name << std::endl;
            if (!branch_name.empty())
            {
                if (SendToBranch(task, branch_name))
                {
                    logger.logTaskDispatch(task, branch_name);
                    // std::cout << "assign successfully" << std::endl;
                }
                else
                {
                    std::cout << "assign unsuccessfully" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            else
            {
                std::cout << "no vehicle available now" << std::endl;

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        else
        {
            PrintStatus();
            std::cout << "waiting for finish" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void LogisticsCenter::run()
{
    scheduleTask();

    checkAndUpdatePlugins();

    scheduleTask();

    std::cout
        << "all tasks have finished" << std::endl;
    shutdown();
}

void LogisticsCenter::checkAndUpdatePlugins()
{
    std::string plugin_log = pluginManager.checkAndUpdatePlugins();
    logger.logPluginUpdate(pluginManager, plugin_log);

    for (auto branch : m_branchInfo)
    {
        SendToBranch(pluginManager, branch.first);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "plugin update to version" << pluginManager.getCurrentVersion() << " path:" << pluginManager.getCurrentRoutesFile() << std::endl;

    json tasks;
    std::ifstream tasksFileStream(tasksFile);
    tasksFileStream >> tasks;
    // std::cout << " version + pluginManager.getCurrentVersion() " << "version" + pluginManager.getCurrentVersion() << std::endl;
    for (const auto &task : tasks["version" + pluginManager.getCurrentVersion()])
    {
        m_pendingTasks.push_back(
            Task(task["m_description"], std::to_string(task["taskId"].get<int>())));
    }
}

// 选择将 task/plugin/message 发送给分支
template <typename T>
bool LogisticsCenter::SendToBranch(T &item, const std::string &branchName)
{
    // std::lock_guard<std::mutex> lock(mtx_send);
    if (m_branchInfo.find(branchName) == m_branchInfo.end())
    {
        std::cout << "do not exist " << branchName << " in branch list" << std::endl;
        return false;
    }
    auto branchInfo = m_branchInfo[branchName];
    try
    {
        json message_json;
        if constexpr (std::is_same<std::decay_t<T>, Task>::value)
        {

            auto it = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(), [&item](const Task &task1)
                                   { return item.GetTaskId() == task1.GetTaskId(); });
            if (it == m_pendingTasks.end())
            {
                std::cout << "LogisticsCenter.m_pendingTasks do not exist " << item.GetTaskId() << std::endl;
                return false;
            }
            m_activeTasks.push_back(*it);
            m_pendingTasks.erase(it);
            std::cout << "Task assigned to " << branchName << ": " << item.GetTaskId() << " " << item.GetDescription() << std::endl;

            // 构建任务消息，交由Server转发给分支
            message_json["type"] = "add_task";
            message_json["task"] = item.to_json().dump();
            std::string taskMessage = message_json.dump();
            // std::cout << "Server send  " << taskMessage << std::endl;
            return m_server->sendTaskToBranch(branchInfo.first, branchInfo.second, taskMessage);
        }
        else if constexpr (std::is_same<std::decay_t<T>, std::string>::value)
        {
            message_json["type"] = "send_msg";
            message_json["msg"] = item;
            std::string taskMessage = message_json.dump();
            // std::cout << "Server send  " << taskMessage << std::endl;
            return m_server->sendTaskToBranch(branchInfo.first, branchInfo.second, taskMessage);
        }
        else if constexpr (std::is_same<std::decay_t<T>, PluginManager>::value)
        {
            message_json["type"] = "update_plugin_chunk";

            json pluginData = pluginManager.getCurrentRoutes();
            std::string serializedData = pluginData.dump();

            // 分块发送
            size_t totalSize = serializedData.size();
            size_t chunkCount = (totalSize + pluginManager.CHUNK_SIZE - 1) / pluginManager.CHUNK_SIZE;

            for (size_t i = 0; i < chunkCount; ++i)
            {
                size_t start = i * pluginManager.CHUNK_SIZE;
                size_t end = std::min(start + pluginManager.CHUNK_SIZE, totalSize);

                json message_json;
                message_json["type"] = "update_plugin_chunk";
                message_json["file_id"] = pluginManager.getCurrentVersion();
                message_json["chunk"] = serializedData.substr(start, end - start);
                message_json["is_last_chunk"] = (i == chunkCount - 1);
                message_json["version"] = pluginManager.getCurrentVersion();

                std::string message = message_json.dump();
                m_server->sendTaskToBranch(branchInfo.first, branchInfo.second, message);
            }
        }
    }
    catch (const nlohmann::json::exception &e)
    {
        // std::cout << "task msg:" << task.to_json().dump() << std::endl;
        std::cerr << "JSON parsing/serialization error: " << e.what() << std::endl;
    }
    return false;
}
void LogisticsCenter::shutdown()
{
    std::cout << "Initiating shutdown process for all branches..." << std::endl;
    for (auto &[branchName, branchInfo] : m_branchInfo)
    {
        // 标记需要关闭
        std::string msg_shutdown("shutdown branch");
        if (!SendToBranch(msg_shutdown, branchName))
        {
            std::cerr << "Failed to send shutdown signal to branch: " << branchName << std::endl;
        }
    }

    // 等待所有分支完全关闭
    for (auto &[branchName, branchInfo] : m_branchInfo)
    {
        // 这里可以加入检测机制，比如等待客户端响应关闭完成的信号
        std::cout << "Waiting for branch: " << branchName << " to shut down..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟等待
    }
    m_server->stop();
    std::cout << "All branches shut down successfully." << std::endl;
}

void LogisticsCenter::PrintStatus()
{
    if (m_activeTasks.size())
    {
        std::cout << "ids of tasks in execution are ";
        for (Task task : m_activeTasks)
        {
            std::cout << task.GetTaskId() << " ";
        }
        std::cout << std::endl;
    }
    if (m_pendingTasks.size())
    {
        std::cout << "ids of tasks in waiting are ";
        for (Task task : m_pendingTasks)
        {
            std::cout << task.GetTaskId() << " ";
        }
        std::cout << std::endl;
    }
}

void LogisticsCenter::addReceiveMsgQueue(const std::string &message)
{
    // std::lock_guard<std::mutex> lock(mtx_receive); // 保证线程安全
    receiveMsgQueue.push(message);
}

std::string LogisticsCenter::fetchReceiveMsgQueue()
{
    // std::lock_guard<std::mutex> lock(mtx_receive);
    if (!receiveMsgQueue.empty())
    {
        std::string message = receiveMsgQueue.front();
        receiveMsgQueue.pop();
        return message;
    }
    return "";
}

std::string LogisticsCenter::selectBranchForTask()
{
    for (auto v : is_loaded)
    {
        if (v.second == false)
        {
            return v.first;
        }
    }
    return "";
}

void LogisticsCenter::handleMsg(std::string msg)
{
    // std::lock_guard<std::mutex> lock(mtx_receive);
    if (msg.length() == 0)
    {
        return;
    }
    json msg_json = json::parse(msg);
    std::string msg_str = msg_json["type"];
    if (msg_str == "reject_task")
    {
        // PrintStatus();
        auto it = std::find_if(m_activeTasks.begin(), m_activeTasks.end(), [&msg_json](const Task &task)
                               { return task.GetTaskId() == msg_json["taskid"]; });
        if (it == m_activeTasks.end())
        {
            std::cout << "error reject not exist task " << msg_json["taskid"] << std::endl;
            return;
        }
        Task rejected_task(*it);
        // std::cout << "handleMsg reject_task " << (*it).to_json().dump() << std::endl;
        m_pendingTasks.push_back(*it);
        m_activeTasks.erase(it);
        // std::cout << "handleMsg reject_task " << (*it).to_json().dump() << std::endl;
        is_loaded[msg_json["branch"]] = true;
        std::cout << msg_json["branch"] << " reject task " << rejected_task.GetTaskId() << " " << rejected_task.GetDescription() << std::endl;
        logger.logTaskRejection(rejected_task, msg_json["branch"]);
        // PrintStatus();
    }
    else if (msg_str == "finish_task")
    {
        auto it = std::find_if(m_activeTasks.begin(), m_activeTasks.end(), [&msg_json](const Task &task)
                               { return task.GetTaskId() == msg_json["taskid"]; });
        if (it == m_activeTasks.end())
        {
            // PrintStatus();
            std::cout << "error finish not exist task " << msg_json["taskid"] << std::endl;
            return;
        }
        Task finished_task(*it);
        m_activeTasks.erase(it);
        is_loaded[msg_json["branch"]] = false;
        std::cout << msg_json["branch"] << " finish task " << finished_task.GetTaskId() << finished_task.GetDescription() << std::endl;
        logger.logTaskCompletion(finished_task, msg_json["branch"]);
    }
    else
    {
        std::cout << msg_json["branch"] << " send unencoded msg " << msg_json["taskid"] << std::endl;
    }
}
