#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

// 函数声明
void TIM3_Int_Init(u16 arr,u16 psc);
void Timer2_Init(void);
void KEY_Process(void);
void Light_OnlineControl(uint8_t mode, uint8_t power, uint8_t brightness);

// 全局变量声明
extern volatile uint16_t pwm_value;    // PWM值
extern volatile uint8_t pwm_dir;       // PWM方向
extern volatile uint8_t light_mode;    // 灯光模式
extern volatile uint8_t light_power;   // 灯光电源状态
extern volatile uint8_t key_control;   // 按键控制标志
extern volatile uint8_t auto_control;  // 自动控制标志
extern volatile uint8_t online_control; // 在线控制标志

#endif
