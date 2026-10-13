# 硬件开发资料

整理个人积累的单片机、嵌入式与电子设计资料，包含工程源码、硬件设计、开发板例程和参考文档。

## 目录导航

| 目录 | 内容 |
| --- | --- |
| [01-projects](01-projects/) | 课程设计、毕业设计、茶杯项目、晶体管测量仪 |
| [02-competitions](02-competitions/) | 2022 循迹小车、2023 同轴电缆、2024 晶体管测量仪及电源题资料 |
| [03-boards-and-examples](03-boards-and-examples/) | 51、STM32、GD32、STC 开发板资料、驱动库和示例工程 |
| [04-modules](04-modules/) | ESP8266、MPU9250、平衡小车相关资料 |
| [05-reference](05-reference/) | 单片机设计实例、模拟电路、运放、PCB 和嵌入式编程资料 |
| [docs/catalog.md](docs/catalog.md) | 各子目录的文件数量、大小和说明 |
| [tools/maintenance](tools/maintenance/) | 本地整理和上传范围检查脚本 |

本地另有 `06-local-tools/`，用于保存 Keil、烧录工具、串口助手和驱动安装包；该目录不上传。

## 使用工程

1. 在相应项目中查找 `.uvproj`、`.uvprojx`、`.ewp`、`.project` 或工程自身的说明文件。
2. 使用工程要求的工具链、芯片型号和库版本。历史工程可能使用 Keil C51、MDK、IAR、CCS 等不同环境。
3. 硬件设计文件可能需要 Altium Designer、Proteus、Multisim、嘉立创 EDA 或对应 CAD 软件打开。
4. 编译前根据实际开发板核对引脚、时钟、供电及下载器配置。

整理主要移动顶层资料目录，保留各工程内部的目录和文件名。芒果小车的 `9.Blinker_ESP32_CAM.ino` 中连接凭据已替换为 `YOUR_AUTH`、`YOUR_SSID`、`YOUR_PSWD` 占位符，使用前需填写自己的配置，原稿保存在本地 `.local-audit/private-originals/`。工程中的绝对路径如依赖旧位置，需要在本机重新配置。

## 上传范围

公开仓库收录源码、工程配置、硬件设计和通过检查的文档。安装包、压缩包、视频、编译产物、缓存、本机配置及检测到的个人信息材料保留在本地，通过 `.gitignore` 排除。只有压缩包、没有解压内容的资料不会出现在仓库中。

完整本地清单位于 `.local-audit/all-files.csv`，目录迁移记录位于 `.local-audit/moves.json`，额外排除项目位于 `.local-audit/excluded.json`。这些清单不上传。

原工程、例程和参考资料的署名、版权及许可归各自作者或供应商所有；请遵循各资料原有的许可要求。本仓库未为整套资料指定统一开源许可证。

## 整理与验证

整理日期：2026-10-13。执行了文件清单、大小、常见凭据及文档个人信息检查。未对历史固件逐一编译，也未进行实物硬件验证。
