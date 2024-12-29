#include "PluginManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

PluginManager::PluginManager(const std::string &routesListDir)
    : routesListDir(routesListDir)
{
    // 获取所有 routes 文件并解析版本号
    auto files = getRoutesFiles();
    if (files.empty())
    {
        throw std::runtime_error("No routes files found in directory: " + routesListDir);
    }

    // 选择版本号最小的文件作为当前版本
    std::sort(files.begin(), files.end());
    currentRoutesFile = files.front();
    parseVersionFromFilename(currentRoutesFile);
}

void PluginManager::parseVersionFromFilename(const std::string &filename)
{
    // 假设文件名格式为 routes_lists_vX.json
    size_t vPos = filename.find("_v");
    size_t dotPos = filename.find(".json");

    if (vPos != std::string::npos && dotPos != std::string::npos)
    {
        currentVersion = filename.substr(vPos + 2, dotPos - vPos - 2);
    }
    else
    {
        throw std::runtime_error("Invalid routes file name format: " + filename);
    }
}

std::vector<std::string> PluginManager::getRoutesFiles()
{
    std::vector<std::string> files;
    for (const auto &entry : fs::directory_iterator(routesListDir))
    {
        if (entry.is_regular_file() && entry.path().filename().string().find("routes_lists_v") != std::string::npos)
        {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}

std::string PluginManager::getCurrentVersion()
{
    std::lock_guard<std::mutex> lock(mtx);
    return currentVersion;
}

json PluginManager::getCurrentRoutes()
{
    std::lock_guard<std::mutex> lock(mtx);
    std::ifstream routesFile(routesListDir + "/" + currentRoutesFile);
    if (!routesFile)
    {
        throw std::runtime_error("Failed to load routes file: " + currentRoutesFile);
    }
    json routes;
    routesFile >> routes;
    return routes;
}

std::string PluginManager::getCurrentRoutesFile()
{
    return routesListDir + "/" + currentRoutesFile;
}

std::string PluginManager::checkAndUpdatePlugins()
{
    std::lock_guard<std::mutex> lock(mtx);

    auto files = getRoutesFiles();
    if (files.empty())
    {
        throw std::runtime_error("No routes files found for update.");
        return std::string("No routes files found for update.");
    }

    // 获取比当前版本大的所有文件
    std::sort(files.begin(), files.end());
    for (const auto &file : files)
    {
        if (file > currentRoutesFile)
        {
            currentRoutesFile = file;
            parseVersionFromFilename(currentRoutesFile);
            std::cout << "Updated to new routes file: " << currentRoutesFile << std::endl;
            return std::string("Updated to new routes file: " + currentRoutesFile);
        }
    }
    return "no newest plugin available";
}

bool PluginManager::createVersionFile(const json &data, const std::string &version)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::string newFileName = "routes_lists_v" + version + ".json";
    std::string filePath = routesListDir + "/" + newFileName;

    // 检查文件是否已存在
    if (fs::exists(filePath))
    {
        std::cerr << "File already exists: " << filePath << std::endl;
        return false;
    }

    // 写入 JSON 数据到文件
    std::ofstream file(filePath);
    if (!file)
    {
        std::cerr << "Failed to create file: " << filePath << std::endl;
        return false;
    }

    file << data.dump(4); // 以格式化方式写入 JSON
    file.close();

    std::cout << "Created new routes file: " << filePath << std::endl;
    return true;
}
bool PluginManager::createVersionFileFromChunks(const std::string &fileID, const std::vector<std::string> &chunks, const std::string &version)
{
    // 合并所有分块数据
    std::string combinedData;
    for (const auto &chunk : chunks)
    {
        combinedData += chunk;
    }

    // 将合并后的数据解析成 JSON
    json data = json::parse(combinedData);

    // 创建新版本的文件路径
    std::string newFileName = routesListDir + "/routes_lists_v" + version + ".json";
    std::ofstream outFile(newFileName);
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open file: " << newFileName << std::endl;
        return false;
    }

    // 写入 JSON 数据到文件
    outFile << data.dump(4); // 使用缩进为 4 格式化输出
    outFile.close();

    return true;
}

void PluginManager::handleChunk(const std::string &fileID, const std::string &chunk, bool isLastChunk, const std::string &version)
{
    std::lock_guard<std::mutex> lock(mtx);
    // 将分块存储到 receivedChunks 中
    receivedChunks[fileID].push_back(chunk);

    // 如果是最后一个分块，则创建版本文件
    if (isLastChunk)
    {
        // 获取所有的分块
        auto &chunks = receivedChunks[fileID];

        // 调用 createVersionFileFromChunks 来生成新版本文件
        if (createVersionFileFromChunks(fileID, chunks, version))
        {
            std::cout << "Version file created successfully: " << fileID << "_v" << version << ".json" << std::endl;
        }
        else
        {
            std::cerr << "Failed to create version file: " << fileID << "_v" << version << ".json" << std::endl;
        }

        // 清空已处理的分块数据
        receivedChunks.erase(fileID);
    }
}
