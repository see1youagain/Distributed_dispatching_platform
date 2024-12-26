### 实现日志

时间：2024年12月10日
任务：理解项目要求，完成架构设计。按照REAMD.md文档开始构建基本的物流调度中心和分支机构功能。

时间：2024年12月21日
完成LogisticsCenter类，BranchOffice类，并编写测试程序

时间：2024年12月22日
完成Server类，Client类，并编写测试程序

时间：2024年12月23日
尝试合并LogisticsCenter、Server类，BranchOffice、Client类，出现头文件中直接包含相互依赖的头文件的问题，采用前向声明，并尝试使用branchPtr和centerPtr指针进行回调。

时间：2024年12月24日
验证前向声明和回调指针成功，并尝试解决Client类架构失误的问题，重构Client类和Server类，尝试完成LogisticsCenter类和BranchOffice类的间接Socket通信

时间：2024年12月25日
LogisticsCenter类和BranchOffice类的间接Socket通信成功，尝试改为异步通信，并增加Vehicle类及结构

时间：2024年12月26日
完成总体架构，增加注释和GDB调试，解决多线程冲突，资源竞争，死锁等问题。调试完毕，完成无Plugin的任务调度。

