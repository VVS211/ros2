# ROS 2 机器人速度指令缓存与异常检测系统

## 📖 项目简介
本项目是一个基于 ROS 2 (C++) 开发的节点系统，用于实时监控机器人的速度指令。
核心功能包括：
- 实时接收速度指令（`/robot_speed`）。
- 使用 `std::vector` 缓存最近 4 次的速度变化差值。
- 当速度突变（差值 >= 3）时，触发 `WARNING` 状态。
- 结合历史数据的累计异常次数，评估并输出系统整体状态（`STABLE` 或 `UNSTABLE`）。

## 🛠️ 运行环境
- **操作系统**: Ubuntu 22.04 (WSL 或 原生)
- **ROS 2 版本**: Humble (或其他你使用的版本)
- **编译工具**: CMake, colcon

## 📦 依赖项
- `rclcpp`
- `std_msgs`  
## ⚠️ 声明
本项目当前主要为了验证核心的缓存与异常检测逻辑，速度发布者（Publisher）使用的是**固定速度变化序列**。在实际机器人应用中，速度数据会存在随机波动和突发状况，具体应用时请以实际传感器数据为准。  

## 🚀 编译与运行指南

### 1. 编译工作空间
```bash
# 回到你的 ros2_ws 根目录
cd ~/ros2_ws
colcon build --packages-select robot_speed_pkg
source install/setup.bash  
```
2. 运行测试

你需要打开 两个终端（记得每个终端都要先 source install/setup.bash）。

终端 1 (启动发布者，发送模拟速度数据):

```bash
ros2 run robot_speed_pkg speed_publisher
```

终端 2 (启动订阅者，监控速度并报警):

```bash
ros2 run robot_speed_pkg speed_monitor
```
