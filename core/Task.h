#ifndef TASK_H
#define TASK_H

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Task
{
public:
    Task(const std::string &description = "", const std::string &taskId = "");
    ~Task();

    std::string GetDescription() const;
    std::string GetTaskId() const;
    std::string GetStatus() const;
    void SetStatus(const std::string &status);
    std::string GetCreatedAt() const;

    // 从 JSON 反序列化构造 Task 对象
    static Task from_json(json j)
    {
        Task task;
        task.m_description = j.value("description", "");
        task.m_taskId = j.value("taskId", "");
        task.m_status = j.value("status", "");
        task.m_createdAt = j.value("createdAt", "");
        return task;
    }

    bool operator==(const std::string &taskId) const
    {
        return m_taskId == taskId;
    }

    // 将 Task 对象序列化为 JSON
    json to_json()
    {
        return json{
            {"description", m_description},
            {"taskId", m_taskId},
            {"status", m_status},
            {"createdAt", m_createdAt}};
    }

private:
    std::string m_description;
    std::string m_taskId;
    std::string m_status;
    std::string m_createdAt;
};

#endif
