#include "./core/LogisticsCenter.h"
#include "./core/BranchOffice.h"
#include "core/Task.h"
#include <iostream>
#include <nlohmann/json.hpp> // JSON library
#include <vector>
#include <fstream>

using json = nlohmann::json;

#include <thread>

// 添加全局退出标志
std::atomic<bool> running(true);

void startBranchOffice(const std::string &name, const std::string &server_ip, int server_port, const std::string &branch_ip, int branch_port, const std::string &routesFile)
{
    BranchOffice branch(name, server_ip, server_port, branch_ip, branch_port, routesFile);
    while (running.load())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    LogisticsCenter logisticsCenter("../center_config/config.json", "../center_config/routes.json",
                                    "../center_config/routes_lists", "../center_config/log.txt");
    std::cout << "logisticsCenter server start" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(4));

    std::ifstream configFileStream("../center_config/config.json");
    if (!configFileStream)
    {
        std::cerr << "Could not open config file: " << std::endl;
        return -1;
    }

    json config;
    configFileStream >> config;

    std::string server_ip = config["logistics_center"]["ip"];
    int server_port = config["logistics_center"]["port"];

    std::vector<std::thread> branchThreads;
    for (const auto &branch : config["branch_offices"])
    {
        std::string name = branch["name"];
        int branch_port = branch["port"];
        std::string branch_ip = branch["ip"];
        branchThreads.emplace_back(startBranchOffice, name, server_ip, server_port, branch_ip, branch_port, "../branch_config/routes_lists");
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "logisticsCenter distribute start" << std::endl;
    logisticsCenter.run();

    // 设置退出标志为 false，通知分支线程停止
    running.store(false);

    for (auto &t : branchThreads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    return 0;
}
