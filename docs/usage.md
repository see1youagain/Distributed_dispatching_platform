# 使用说明

## 1. 环境要求

- 操作系统：Linux 或 Windows
- 编译工具：CMake 3.16 或更高版本，g++ 编译器
- 必要依赖：nlohmann/json 库

## 2. 项目配置

### 2.1 配置文件

本项目的配置文件存储在 `config/` 目录下。调度中心和分支机构的配置信息包含在 `config.json` 文件中。每个分支机构应配置自己的名称、IP 和端口。

### 2.2 路线数据

路线信息存储在 `routes.json` 文件中，其中包含任务的描述、ID 和其他信息。您可以通过更新此文件来扩展路线数据。

### 2.3 插件配置

插件的配置文件存储在 `routes_lists/` 目录下。每个版本的插件都存储在单独的文件中，如 `routes_lists_v1.json` 和 `routes_lists_v2.json`。

## 3. 运行项目

### 3.1 编译项目

在项目根目录下创建一个新的构建文件夹并使用 CMake 编译：

```bash
mkdir build
cd build
cmake ..
make
./LogisticsSystem
```

