#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp> // JSON库

using json = nlohmann::json;

class Logger
{
public:
    Logger(const std::string &logFile)
        : logFile(logFile)
    {
        // 初始化日志文件
        logStream.open(logFile, std::ios::out | std::ios::app);
        if (!logStream)
        {
            std::cerr << "Error opening log file: " << logFile << std::endl;
        }
    }

    ~Logger()
    {
        if (logStream.is_open())
        {
            logStream.close();
        }
    }

    // 记录任务分发的日志
    void logTaskDispatch(const std::string &taskId, const std::string &branch)
    {
        std::lock_guard<std::mutex> lock(mutex);
        json logEntry;
        logEntry["type"] = "task_dispatch";
        logEntry["task_id"] = taskId;
        logEntry["branch"] = branch;
        logEntry["status"] = "dispatched";
        logEntry["timestamp"] = getCurrentTime();
        logStream << logEntry.dump() << std::endl;
    }

    // 记录任务拒绝的日志
    void logTaskRejection(const std::string &taskId, const std::string &branch)
    {
        std::lock_guard<std::mutex> lock(mutex);
        json logEntry;
        logEntry["type"] = "task_rejection";
        logEntry["task_id"] = taskId;
        logEntry["branch"] = branch;
        logEntry["status"] = "rejected";
        logEntry["timestamp"] = getCurrentTime();
        logStream << logEntry.dump() << std::endl;
    }

    // 记录任务完成的日志
    void logTaskCompletion(const std::string &taskId, const std::string &branch)
    {
        std::lock_guard<std::mutex> lock(mutex);
        json logEntry;
        logEntry["type"] = "task_completion";
        logEntry["task_id"] = taskId;
        logEntry["branch"] = branch;
        logEntry["status"] = "completed";
        logEntry["timestamp"] = getCurrentTime();
        logStream << logEntry.dump() << std::endl;
    }

private:
    std::ofstream logStream;
    std::mutex mutex;
    std::string logFile;

    // 获取当前时间戳
    std::string getCurrentTime()
    {
        std::time_t t = std::time(nullptr);
        std::tm *tm = std::localtime(&t);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        return std::string(buffer);
    }
};
