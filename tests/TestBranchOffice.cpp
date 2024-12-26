#include "../core/BranchOffice.h"
#include "../core/Task.h"
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp> // JSON library
#include <fstream>

using json = nlohmann::json;

int main()
{
    std::string configFile = "./config/config.json";
    std::ifstream configFileStream(configFile);
    json config;
    configFileStream >> config;

    std::string server_ip = config["logistics_center"]["ip"];
    int server_port = config["logistics_center"]["port"];

    std::vector<BranchOffice> branchs;
    for (const auto &branch : config["branch_offices"])
    {
        std::string name = branch["name"];
        branchs.push_back(BranchOffice(name, server_ip, server_port));
    }

    while (true)
    {
        for (BranchOffice &branch : branchs)
        {
        }
    }

    return 0;
}
