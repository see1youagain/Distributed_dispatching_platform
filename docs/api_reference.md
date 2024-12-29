# API Reference

## LogisticsCenter

### `LogisticsCenter::LogisticsCenter(const std::string &configFile, const std::string &routesFile, const std::string &routesListDir, const std::string &logFile)`
构造函数，初始化调度中心。

#### 参数：
- `configFile`: 配置文件路径。
- `routesFile`: 路线数据文件路径。
- `routesListDir`: 路线列表目录。
- `logFile`: 日志文件路径。

### `void run()`
启动调度中心，处理任务分配与监控。

---

## BranchOffice

### `BranchOffice::BranchOffice(const std::string &name, const std::string &serverIP, int serverPort, const std::string &clientIP, int clientPort, const std::string &routesListPath)`
构造函数，初始化分支机构。

#### 参数：
- `name`: 分支机构名称。
- `serverIP`: 调度中心IP。
- `serverPort`: 调度中心端口。
- `clientIP`: 分支机构IP。
- `clientPort`: 分支机构端口。
- `routesListPath`: 路线列表文件路径。

### `bool ReceiveTask(const Task &task)`
接收并处理任务。

#### 参数：
- `task`: 要接收的任务对象。

#### 返回值：
- `true`: 任务成功接收。
- `false`: 任务接收失败。

---

## Task

### `Task::Task(const std::string &description, const std::string &taskId)`
构造函数，初始化任务。

#### 参数：
- `description`: 任务描述。
- `taskId`: 任务ID。

---

## PluginManager

### `PluginManager::PluginManager(const std::string &routesListDir)`
构造函数，初始化插件管理器。

#### 参数：
- `routesListDir`: 路线列表文件夹路径。

### `bool updatePlugin(const std::string &pluginFile)`
更新插件，加载新的插件文件。

#### 参数：
- `pluginFile`: 插件文件路径。

---

## Server

### `Server::Server(int port)`
构造函数，初始化服务器。

#### 参数：
- `port`: 服务器端口。

### `bool start()`
启动服务器，开始监听连接。

#### 返回值：
- `true`: 启动成功。
- `false`: 启动失败。

---

## Client

### `Client::Client(const std::string &serverIP, int serverPort, const std::string &clientIP, int clientPort)`
构造函数，初始化客户端。

#### 参数：
- `serverIP`: 服务器IP。
- `serverPort`: 服务器端口。
- `clientIP`: 客户端IP。
- `clientPort`: 客户端端口。

### `void start()`
启动客户端并连接到服务器。

