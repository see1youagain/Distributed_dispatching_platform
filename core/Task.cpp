#include "Task.h"
#include <chrono>
#include <ctime>
#include <sstream>

Task::Task(const std::string &description, const std::string &taskId)
    : m_description(description), m_taskId(taskId), m_status("pending")
{
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S"); // 使用格式化时间
    m_createdAt = "hello";
}

Task::~Task() {}

std::string Task::GetDescription() const
{
    return m_description;
}

std::string Task::GetTaskId() const
{
    return m_taskId;
}

std::string Task::GetStatus() const
{
    return m_status;
}

void Task::SetStatus(const std::string &status)
{
    m_status = status;
}

std::string Task::GetCreatedAt() const
{
    return m_createdAt;
}
