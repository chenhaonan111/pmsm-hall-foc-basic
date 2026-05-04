# PMSM Hall FOC Basic

基于 **STM32F407** 的永磁同步电机（PMSM）FOC 控制项目，使用**霍尔传感器**进行转子位置反馈。

## 功能特性

- **FOC 矢量控制**：Clarke 变换 → Park 变换 → SVPWM
- **霍尔传感器位置检测**：支持速度滤波与角度估算
- **三阶段启动**：预对齐 → 开环启动 → 闭环运行
- **PI 电流环控制**
- **ADC 电流采样**（双电阻法）

## 硬件平台

| 项目 | 型号 |
|------|------|
| MCU | STM32F407 |
| 功率器件 | 三相逆变桥 |
| 电流采样 | 双电阻采样 |
| 位置传感器 | 霍尔传感器 |
| 开发环境 | Keil MDK-ARM + STM32CubeMX |

## 工程结构

```
├── Core/
│   ├── Inc/          # 头文件
│   │   ├── foc.h     # FOC 算法（Clarke/Park/SVPWM）
│   │   ├── pmsm.h    # 电机状态机
│   │   ├── pid.h     # PI 控制器
│   │   ├── hall.h    # 霍尔传感器
│   │   └── main.h    # 主头文件
│   └── Src/          # 源文件
│       ├── foc.c
│       ├── pmsm.c
│       ├── pid.c
│       ├── hall.c
│       └── main.c
├── Drivers/          # HAL 库（CMSIS + STM32F4xx_HAL）
├── MDK-ARM/          # Keil 工程文件
└── openloop1.ioc     # STM32CubeMX 配置
```

## 使用方法

1. 用 Keil MDK-ARM 打开 `MDK-ARM/openloop1.uvprojx`
2. 编译下载到 STM32F407 目标板
3. 连接电机、逆变桥、霍尔传感器后上电运行

## 许可证

MIT License
