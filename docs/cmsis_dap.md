# CMSIS-DAP 
https://arm-software.github.io/CMSIS_5/DAP/html/index.html
## 是什么
CMSIS-DAP 是一种协议规范，是支持访问 CoreSight 调试访问端口 （DAP） 的固件实现。各种 Arm Cortex 处理器提供 CoreSight 调试和跟踪。CMSIS-DAP 支持包含一个或多个 Cortex 处理器的目标设备。器件提供调试访问端口 （DAP），通常具有 5 引脚 JTAG 或连接到调试单元的 2 引脚串行有线调试 （SWD） 接口。CMSIS-DAP 是将调试端口连接到 USB 的调试单元的接口固件。在主机上执行的调试器通过 USB 和调试单元连接到运行应用软件的设备。

## DAPLink VS CMSIS-DAP 区别
CMSIS是围绕Cortex处理器的ARM指定基础设施的通用名称。CMSIS-DAP是协议，DAPLink是这个协议的物理实现 [](https://electronics.stackexchange.com/questions/405134/swdap-vs-cmsis-dap-vs-daplink)

## CMSIS-DAP 的优势
- 为调试器提供标准化接口
- 访问所有 Cortex 处理器架构 （Cortex-A/R/M） 的 CoreSight 寄存器
- 通过 5 引脚 JTAG 或 2 引脚串行线调试 （SWD） 进行连接。
- 支持多核调试。
- 支持 Cortex-M 设备的串行线输出 （SWO）
- CMSIS-Driver USART，连接
    - Cortex-M 器件的 UART 到目标的 SWO 输出。
- 改用CMSIS-DAP v2.x，
    它提供高速SWO跟踪流

## Hardware requirements
- Cortex-M processor-based microcontroller.
- CPU Clock: 48 MHz or higher; microcontroller must have a SYSTICK timer.
- RAM: 8 KB or more.
- Flash ROM: 16 KB or more.
- Full-speed or High-speed USB Device peripheral.
- 7 standard I/O pins for JTAG/SWD interface.
- Optionally, 2 I/O pins for status LEDs.
- Optionally, a UART to support SWO capturing (Rx pin connected to SWO).
  CMSIS驱动器USART可用于利用微控制器上的UART RX输入捕获SWO引脚上的走线输出。UART串行线输出（SWO）走线可以在接头DAP_Config.h中启用和配置。#define SWO_UART 用于启用 UART SWO，#define SWO_UART_DRIVER 用于配置 USART 驱动程序实例编号 （Driver_USART#）
- Optionally, a UART to support an additional UART communication port (for printf debugging).

## 目录

- Config CMSIS-DAP 固件配置
- Examples 适用于各种调试单元的 CMSIS-DAP 固件
- Include 
- Source 
- Template MDK 中间件的接口模板
- Validation 验证项目

## 移植
- 选择微控制器并替换CMSIS核心（Cortex-M）文件。
- 配置 I/O 端口和调试单元：调整 I/O 端口并为调试单元指定其他参数
- 配置 USB 外设 ：使 USB 外设适应微控制器。
- 连接 SWO 跟踪：或者可以添加 CMSIS 驱动程序 USART 以连接到 SWO。
- 连接UART通信端口：或者，您可以添加CMSIS驱动程序USART以连接到UART通信端口
- CMSIS-DAP 供应商命令，可以将供应商特定的命令添加到调试单元。
- 闪存固件编程：将适配的固件编程到新调试单元的闪存ROM中。
- 验证调试单元操作：验证新调试单元的 CMSIS-DAP 固件。

## Revision History of CMSIS-DAP
CMSIS-DAP v1.x 已弃用，不建议用于新设计
- 2.1.1 
    允许默认时钟频率使用快速时钟模式
- 2.1.0 
    新增：UART COM 命令，支持通过额外的 UART 进行目标通信
    新增：命令DAP_Info中的UART接收/发送缓冲区大小值
    新增：命令DAP_Info中的目标主板供应商和目标主板名称字符串
    新增：命令中的产品固件版本字符串 DAP_Info
    已更改：字符串编码DAP_Info从 ASCII 到 UTF-8
- 2.0.0
    更改：通过 USB 批量端点进行通信以实现高速传输速率
    新增：通过单独的 USB 批量端点流式传输 SWO
    新增：DAP_SWO_Transport扩展了传输模式 2 - 通过单独的 USB 批量终结点发送跟踪数据


## CMSIS-DAP Commands

### General Commands 常规命令
CMSIS-DAP 调试单元的信息和控制命令。
调试单元和主机之间交换的每个命令。每个命令都以命令ID 和可选数据开头。根据命令，CMSIS-DAP 固件会回复重复命令ID 并提供其他数据的响应。命令和响应数据具有使用 DAP_PACKET_SIZE 定义的包大小限制。此配置设置可以使用命令DAP_Info获取，用于优化全速或高速 USB 的性能。调试器必须确保每个数据包都符合已配置DAP_PACKET_SIZE的限制。
- 字节序 为LSB 优先
- **未实现的命令使用 0xFF 回复，而不是重复命令字节**

### Common SWD/JTAG Commands
- 连接、断开连接和识别调试单元
- 控制调试单元的状态 LED
- 硬件重置到连接的设备
- 终止以前的 CMSIS-DAP 命令
- 等待指定的时间

1. DAP_Info
```
| BYTE | BYTE |
>0x00 | ID   |
|******|******|
```
- 0x01 = Get the Vendor Name (string).
- 0x02 = Get the Product Name (string).
- 0x03 = Get the Serial Number (string).
- 0x04 = Get the CMSIS-DAP Protocol Version (string).
- 0x05 = Get the Target Device Vendor (string).
- 0x06 = Get the Target Device Name (string).
- 0x07 = Get the Target Board Vendor (string).
- 0x08 = Get the Target Board Name (string).
- 0x09 = Get the Product Firmware Version (string, vendor-specific format).
- 0xF0 = Get information about the Capabilities (BYTE) of the Debug Unit (see below for details).
- 0xF1 = Get the Test Domain Timer parameter information (see below for details).
- 0xFB = Get the UART Receive Buffer Size (WORD).
- 0xFC = Get the UART Transmit Buffer Size (WORD).
- 0xFD = Get the SWO Trace Buffer Size (WORD).
- 0xFE = Get the maximum Packet Count (BYTE).
- 0xFF = Get the maximum Packet Size (SHORT).

RESPON:
```asm
| BYTE | BYTE | BYTE |
< 0x00 | Len  | Info |
|******|******|++++++|
Len: Info length in bytes.
Info:
    a string encoded as UTF-8. Len is the string length including the \x00 terminator. 
    Len = 0 indicates no information, and is distinct from an empty string indicated by Len = 1.
    a BYTE value (indicated with Len  = 1).
    a SHORT value (indicated with Len = 2).
    a WORD value (indicated with Len  = 4).
```


2. DAP_HostStatus
3. DAP_Connect
4. DAP_Disconnect
5. DAP_WriteABORT
6. DAP_Delay
7. DAP_ResetTarget

### SWD Commands
### SWO Commands
### JTAG Commands
### Transfer Commands
### Atomic Commands
### UART COM Commands
### Response Status

