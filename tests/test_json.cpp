#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main()
{
    json j;
    j["name"] = "example";
    std::cout << j << std::endl;
    return 0;
}
