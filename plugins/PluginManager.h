#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <string>
#include <nlohmann/json.hpp> // JSON library
#include <vector>
#include <mutex>
#include <unordered_map>

using json = nlohmann::json;

class PluginManager
{
private:
    std::string routesListDir;     // routes_lists 文件所在的文件夹路径
    std::string currentRoutesFile; // 当前使用的 routes 文件名
    std::string currentVersion;    // 当前版本号
    std::mutex mtx;                // 线程安全

    void parseVersionFromFilename(const std::string &filename); // 从文件名解析版本号
    std::vector<std::string> getRoutesFiles();                  // 获取文件夹中所有 routes 文件
    PluginManager(const PluginManager &) = delete;              // 删除拷贝构造函数
    PluginManager &operator=(const PluginManager &) = delete;   // 删除赋值运算符
    // 保存当前正在接收的分块
    std::unordered_map<std::string, std::vector<std::string>> receivedChunks;

public:
    // 分块大小
    const size_t CHUNK_SIZE = 512;
    PluginManager(const std::string &routesListDir);
    std::string getCurrentRoutesFile();

    std::string getCurrentVersion();     // 获取当前版本
    json getCurrentRoutes();             // 获取当前 routes 数据
    std::string checkAndUpdatePlugins(); // 检查并更新插件
    void handleChunk(const std::string &fileID, const std::string &chunk, bool isLastChunk, const std::string &version);
    bool createVersionFileFromChunks(const std::string &fileID, const std::vector<std::string> &chunks, const std::string &version);
    bool createVersionFile(const json &data, const std::string &version); // 创建特定版本的 routes 文件
};

#endif // PLUGIN_MANAGER_H
