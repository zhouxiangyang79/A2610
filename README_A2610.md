# A2610 加热器控制器 - 完整工程

基于 A2200BS 修改，主芯片 CH584，增加 WiFi 功能。

## 硬件配置

### 主控芯片
- **CH584** - RISC-V3A 内核，48MHz

### 按键 (A2610)
| 功能 | 引脚 |
|------|------|
| 加键 | PB9 |
| 减键 | PB8 |
| 菜单键 | PB17 |
| 电源键 | PB16 |

### WiFi 模块 (ESP8266)
| 信号 | 引脚 |
|------|------|
| TX | PA4 |
| RX | PA5 |
| RST | PA6 |

### 其他外设
| 功能 | 引脚 |
|------|------|
| TM1628 DATA | PA14 |
| TM1628 STB | PA3 |
| TM1628 WR | PA15 |
| HP203B SDA | PB20 |
| HP203B SCL | PB21 |
| 热敏电阻 | PA1 |
| 继电器 | PB13 |
| 3线串口使能 | PB15 |
| 语音 NRST | PA12 |
| 语音 CTRL | PA13 |

## 工程结构

```
A2610_Full/
├── APP/                    # 应用程序
│   ├── include/           # 头文件
│   │   ├── wifi_a2610.h   # WiFi 头文件
│   │   └── ...
│   ├── peripheral_main.c  # 主程序 (已添加WiFi)
│   ├── button.c           # 按键 (已修改A2610映射)
│   ├── wifi_a2610.c       # WiFi 驱动
│   ├── running.c          # 运行控制
│   ├── lcd.c              # 显示驱动
│   ├── hp203b.c           # 气压传感器
│   └── ...
├── HAL/                    # 硬件抽象层
│   ├── include/
│   ├── MCU.c
│   ├── KEY.c
│   ├── LED.c
│   └── ...
├── BLE/                    # BLE 协议栈
│   ├── LIB/
│   │   ├── CH58xBLE_LIB.h
│   │   └── LIBCH58xBLE.a
│   └── Profile/
├── StdPeriphDriver/        # 标准外设库
│   ├── inc/
│   └── CH58x_*.c
├── Startup/                # 启动文件
│   └── startup_CH583.S
├── Ld/                     # 链接脚本
├── Profile/                # GATT Profile
├── RVMSIS/                 # RISC-V 内核支持
└── README_A2610.md         # 本文件
```

## 修改说明

### 1. 主程序 (APP/peripheral_main.c)
```c
#include "wifi_a2610.h"    // 添加头文件

int main(void)
{
    // ... 原有初始化 ...
    WiFi_Init();            // 添加 WiFi 初始化
    // ...
}
```

### 2. 按键映射 (APP/button.c)
```c
// A2610 按键映射
// 加键: PB9, 减键: PB8, 菜单键: PB17, 电源键: PB16
case 0:  // 电源键
    return ((GPIOB_ReadPortPin(GPIO_Pin_16) == 0) ? 0 : 1);
case 1:  // 菜单键
    return ((GPIOB_ReadPortPin(GPIO_Pin_17) == 0) ? 0 : 1);
case 2:  // 加键
    return ((GPIOB_ReadPortPin(GPIO_Pin_9) == 0) ? 0 : 1);
case 3:  // 减键
    return ((GPIOB_ReadPortPin(GPIO_Pin_8) == 0) ? 0 : 1);
```

### 3. 新增 WiFi 模块
- `wifi_a2610.c/h` - ESP8266 驱动
- UART1: PA4(TX), PA5(RX)
- RST: PA6
- 波特率: 115200

## WiFi 通信协议

### 帧格式
```
[0x55][0xAA][CMD][LEN][DATA...][CRC]
```

###