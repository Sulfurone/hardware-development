# STM32F103C8 移植说明

当前工程是 STM32F103C8 + I2C OLED 的晶体管测试仪原型。主流程已经拆成多个模块，方便后续调硬件和补算法。

## 文件结构

| 文件 | 作用 |
| --- | --- |
| `空工程/User/main.c` | 主循环、OLED 页面显示、按键等待 |
| `空工程/User/app_config.h` | 电压、阈值、OLED、按键、电源、ADC 等集中配置 |
| `空工程/User/bsp_delay.c/.h` | 简单阻塞延时 |
| `空工程/User/oled_ssd1306.c/.h` | SSD1306 128x64 软件 I2C 驱动 |
| `空工程/User/tester_hw.c/.h` | GPIO、ADC、三端测试矩阵、按键、电池/参考电压 |
| `空工程/User/tester_measure.c/.h` | 二极管、电阻、电容、BJT 初步识别算法 |
| `空工程/User/tester_types.h` | 测试结果、二极管列表、元件类型等公共结构 |

## 默认引脚

OLED 使用 SSD1306 128x64，软件 I2C：

| 功能 | 引脚 |
| --- | --- |
| OLED SCL | PB6 |
| OLED SDA | PB7 |

控制与状态：

| 功能 | 引脚 | 说明 |
| --- | --- | --- |
| BUTTON | PB12 | 低电平按下 |
| POWER | PB13 | 高电平保持电源，若没有自锁电源可不接 |
| LED | PC13 | 测量时亮 |

测试端：

| 测试端 | ADC | 680R 驱动脚 | 470K 驱动脚 |
| --- | --- | --- | --- |
| TP1 | PA0 / ADC1_CH0 | PB0 | PB11 |
| TP2 | PA1 / ADC1_CH1 | PB1 | PA8 |
| TP3 | PA2 / ADC1_CH2 | PB10 | PA9 |

电压检测：

| 功能 | 引脚 | 默认换算 |
| --- | --- | --- |
| UBAT | PA3 / ADC1_CH3 | 680K:220K 分压 |
| REF | PA4 / ADC1_CH4 | 直接显示 ADC 电压 |

## 调参位置

优先改 `app_config.h`：

- `APP_VCC_MV`：MCU ADC 参考电压，默认 3300mV。
- `DIODE_MIN_MV` / `DIODE_MAX_MV`：二极管导通判断窗口。
- `RES_LOW_OHM_SUM`：低阻档两端串联电阻总和，默认 680R + 680R。
- `RES_HIGH_OHM_SUM`：高阻档两端串联电阻总和，默认 470K + 470K。
- `CAP_CHARGE_R_OHM`：电容测量充电电阻，默认 470K。
- `BAT_DIV_TOP_KOHM` / `BAT_DIV_BOTTOM_KOHM`：电池分压。

如果要改 TP 驱动引脚，改 `tester_hw.c` 里的 `g_testPins` 表。

## 已实现功能

- SSD1306 OLED 显示
- ADC 采样
- 三端测试矩阵控制
- 二极管识别与 Vf 显示
- 电阻粗测
- 电容粗测
- NPN/PNP 基极初步识别

## 后续建议

1. 先只接 OLED、按键、PC13，确认屏幕显示正常。
2. 再接 TP1/TP2/TP3 的 ADC 和电阻驱动脚，用万用表确认 GPIO 三态、拉高、拉低动作正确。
3. 先用普通二极管和 1K/10K 电阻校验基础识别。
4. 再补 BJT 的 C/E 判别、hFE、MOSFET 栅阈值、电感/ESR 与自校准参数。
