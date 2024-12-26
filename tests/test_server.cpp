#include "../core/LogisticsCenter.h"
#include "../network/Server.h"

int main()
{
    LogisticsCenter logisticsCenter(8080);

    logisticsCenter.start();

    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    logisticsCenter.stop();

    return 0;
}
