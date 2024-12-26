#include "LogisticsCenter.h"
#include "BranchOffice.h"
#include "Task.h"
#include <iostream>
#include <nlohmann/json.hpp> // JSON library
#include <vector>
#include <fstream>

using json = nlohmann::json;

int main()
{
    std::vector<std::unique_ptr<BranchOffice>> branchs;
    std::ifstream configFileStream("./config/config.json");
    if (!configFileStream)
    {
        std::cerr << "Could not open config file: " << std::endl;
        return;
    }

    json config;
    configFileStream >> config;

    std::string server_ip = config["logistics_center"]["ip"];
    int server_port = config["logistics_center"]["port"];
    for (const auto &branch : config["branch_offices"])
    {
        std::string name = branch["name"];
        branchs.push_back(std::make_unique<BranchOffice>(name, server_ip, server_port));
    }

    // 初始化 LogisticsCenter
    LogisticsCenter logisticsCenter("./config/config.json", "./config/routes.json");

    return 0;
}
