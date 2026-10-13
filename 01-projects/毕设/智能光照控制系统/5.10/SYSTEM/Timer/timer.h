#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"

// 函数声明
void TIM3_Int_Init(u16 arr,u16 psc);
void Timer2_Init(void);
void KEY_Process(void);
void SetOnlineMode(void);  // 强制设置在线模式的函数

// 全局变量声明
extern volatile uint16_t pwm_value;    // PWM值
extern volatile uint8_t pwm_dir;       // PWM方向
extern volatile uint8_t light_mode;    // 灯光模式: 0=自动模式, 1=手动模式, 2=在线模式
extern volatile uint8_t light_power;   // 灯光电源状态
extern volatile uint8_t key_control;   // 按键控制标志
extern volatile uint8_t auto_control;  // 自动控制标志
extern volatile uint8_t online_control; // 在线控制标志

// 在线模式参数
extern volatile uint8_t online_mode;     // 在线模式下的灯光模式
extern volatile uint8_t online_power;    // 在线模式下的电源状态
extern volatile uint8_t online_brightness; // 在线模式下的亮度值(0-100)

#endif
