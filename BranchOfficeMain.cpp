#include "BranchOffice.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./BranchOffice <name> <server_ip> <server_port>" << std::endl;
        return -1;
    }

    std::string name = argv[1];
    std::string server_ip = argv[2];
    int server_port = std::stoi(argv[3]);

    // 启动分支服务
    BranchOffice branch(name, server_ip, server_port);

    // 保持分支服务运行
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
