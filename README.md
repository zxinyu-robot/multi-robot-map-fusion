# Multi-Robot Map Fusion

[![CI](https://github.com/zxinyu-robot/multi-robot-map-fusion/actions/workflows/ci.yml/badge.svg)](https://github.com/zxinyu-robot/multi-robot-map-fusion/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

面向多机器人 3D 地图协同的共享内存数据路径与版本化子图融合核心。

项目公开实现一个 POSIX 共享内存最新帧槽和一个后端无关的子图注册表，展示关键帧如何从同机 SLAM 进程低拷贝交付给通信进程，以及边缘端如何幂等地接收子图和相对约束。

## 公开实现

- `shm_open + mmap` 的有界共享帧槽。
- 生产者单次写入、消费者只读 `std::span` 视图。
- POSIX named semaphore、序列号、容量和校验和验证。
- `robot/session/local` 三级子图身份。
- 重复子图、重复约束、缺失端点和非法协方差校验。
- 每次有效注册推进 `graph_version`。
- 单元测试、Demo 和 Linux/macOS CI。

## 架构位置

```text
Robot SLAM frontend
  → shared-memory keyframe
  → DDS transport
  → edge submap receiver
  → SubmapRegistry
  → Graph-SLAM backend
  → global map correction
```

共享内存只解决同机大数据路径；跨机器人链路仍需要 DDS QoS、版本、ACK/NACK 和弱网恢复。

## 快速运行

```bash
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
./build/default/fusion_demo
```

## 工程案例背景

该公开重构对应多无人机/机器人协同地图工程经历：

- 使用 Linux System V/POSIX 共享内存传递关键帧，减少 ROS 2 Topic 序列化开销。
- 针对弱网调整 DDS QoS、历史深度和关键帧策略。
- 在 Jetson Orin/RK3588 与 x86 边缘节点间同步关键帧和子图。
- 基于 Graph-SLAM 维护全局位姿图和地图一致性。
- 完成 Docker 部署和 ROS 2 通信压测。

历史项目报告的 CPU 降幅约 `35%–50%`、关键帧丢帧率降幅约 `20%–40%`、同步频率约 `10–20 Hz`、子图融合延迟约 `100–300 ms`。这些结果依赖原始硬件、ROS 2 配置、数据和完整后端，不是本仓库 CI 的复现结论。

生产版本仍需多槽内存池、租约、崩溃恢复、背压、DDS 适配器与实际图优化器。详见 [架构说明](docs/architecture.md)。

## License

[MIT](LICENSE)
