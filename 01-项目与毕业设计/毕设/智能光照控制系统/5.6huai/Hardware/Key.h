#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

// 按键引脚定义
#define KEY_MODE_PIN       GPIO_Pin_8   // 模式切换按键 (PB8)
#define KEY_BRIGHTNESS_UP_PIN  GPIO_Pin_7   // 亮度增加按键 (PB7)
#define KEY_BRIGHTNESS_DOWN_PIN GPIO_Pin_6  // 亮度减少按键 (PB6)
#define KEY_POWER_PIN      GPIO_Pin_5   // 开关灯按键 (PB5)

// 按键GPIO端口
#define KEY_PORT           GPIOB
#define KEY_PORT_CLK       RCC_APB2Periph_GPIOB

// 按键值定义
#define KEY_MODE           0x01
#define KEY_BRIGHT_UP      0x02
#define KEY_BRIGHT_DOWN    0x03
#define KEY_POWER          0x04
#define KEY_NONE           0x00

// 函数声明
void KEY_Init(void);
uint8_t KEY_Scan(void);
void KEY_Process(void);

#endif
