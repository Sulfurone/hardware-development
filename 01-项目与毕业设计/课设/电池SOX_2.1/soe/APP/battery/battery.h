#ifndef _BATTERY_H
#define _BATTERY_H

#include "system.h"

/********************* ADC引脚定义 *********************/
#define VOUTV_ADC_CH    ADC_Channel_0   // VOUTV 接 PA0 (ADC123_IN0)
#define VOUTI_ADC_CH    ADC_Channel_1   // VOUTI 接 PA1 (ADC123_IN1)

/********************* 电池参数校准（需实际标定） *********************/
// 真实电池电压 = VOUTV_K × ADC读电压 + VOUTV_B
#define VOUTV_K         6.0f   // 分压系数 k（例：分压比1/5时k≈5）
#define VOUTV_B         0.0f   // 偏移 b

// 电流计算：假设2.5V = 0A，灵敏度可调
// 电流(A) = (VOUTI_ADC_V - 2.5) × VOUTI_SENS
#define VOUTI_ZERO_V    2.5f   // 0A对应电压
#define VOUTI_SENS      1.0f   // 灵敏度 A/V（例：1A/V意味着1V偏移=1A）

/********************* SOE估算参数 *********************/
#define BAT_V_MAX       4.20f  // 满电电压
#define BAT_V_MIN       3.00f  // 放电截止电压
#define BAT_ENERGY_WH   7.4f   // 电池额定能量 (Wh)，例：3.7V×2.0Ah=7.4Wh

/********************* ADC参考电压 *********************/
#define ADC_VREF        3.30f  // ADC参考电压

/********************* 函数声明 *********************/
void  BATTERY_ADC_Init(void);
void  BATTERY_Update(float dt_s);   // 核心：更新电池数据，dt_s=距上次调用的秒数
float BATTERY_GetVOUTV_V(void);     // 获取VOUTV引脚电压(V)
float BATTERY_GetVOUTI_V(void);     // 获取VOUTI引脚电压(V)
float BATTERY_GetVoltage(void);     // 获取电池电压(V)
float BATTERY_GetCurrent(void);     // 获取电流(A)，正=充电，负=放电
float BATTERY_GetSOE(void);         // 获取SOE(0~100%)

#endif
