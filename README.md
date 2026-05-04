# PMSM Hall FOC Basic

基于 STM32F407 的永磁同步电机（PMSM）FOC 控制项目，使用霍尔传感器进行转子位置反馈。

## 硬件平台

- **主控板**：正点原子 F407 开发板（STM32F407IGT6，Cortex-M4F，168MHz）
- **驱动板**：正点原子电机驱动板
- **位置传感器**：霍尔传感器（TIM5 捕获）
- **电流采样**：双电阻法，ADC1/2/3 三重注入同步采样，DMA 自动搬运

## 开发平台

- **IDE**：Keil MDK-ARM
- **HAL 库**：STM32CubeMX 生成底层初始化代码
- **调试输出**：USART1（1152000 波特率），重映射 `printf` 打印速度等运行数据

## 代码模块

| 模块 | 功能 |
|------|------|
| `foc` | Clarke/Park 变换、逆 Park 变换、SVPWM 计算 |
| `pmsm` | 电机状态机：预对齐 → 开环启动 → 闭环运行 |
| `pid` | PI 控制器，含电流环（Id/Iq）和速度环 |
| `hall` | 霍尔信号解码，角度/速度估算，气泡排序滤波 |
| `adc` | ADC1/2/3 三重注入采样，TIM1 CC4 触发，DMA 循环传输 |
| `key` | 三按键扫描：CW 启动 / CCW 启动 / 停止 |

## 使用方法

1. Keil MDK-ARM 打开 `MDK-ARM/openloop1.uvprojx`
2. 编译下载到目标板
3. 连接电机、驱动板和霍尔传感器后上电
4. 按 KEY0 逆时针（CCW）启动，KEY1 顺时针（CW）启动

## 许可证

MIT License
