cmake_minimum_required(VERSION 3.10)
project(LogisticsSystem)

# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# 添加 nlohmann 的路径
include_directories(/usr/include)

# 包含项目的源文件
set(SOURCE_FILES
    main.cpp
    core/LogisticsCenter.cpp
    core/BranchOffice.cpp
    core/Task.cpp
    network/Server.cpp
    core/Vehicle.cpp
    network/Client.cpp
)

# 创建可执行文件
add_executable(LogisticsSystem ${SOURCE_FILES})

# 设置 CMake 目标的编译选项
target_compile_options(LogisticsSystem PRIVATE -pthread)

# 链接 pthread 库
target_link_libraries(LogisticsSystem PRIVATE pthread)
