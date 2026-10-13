#ifndef __SMOKESENSOR_H
#define __SMOKESENSOR_H
#include "stm32f10x.h"

// 烟雾传感器引脚定义
#define SMOKE_SENSOR_PIN        GPIO_Pin_5
#define SMOKE_SENSOR_PORT       GPIOA
#define SMOKE_SENSOR_RCC        RCC_APB2Periph_GPIOA
#define SMOKE_SENSOR_ADC        ADC1
#define SMOKE_SENSOR_ADC_RCC    RCC_APB2Periph_ADC1
#define SMOKE_SENSOR_CHANNEL    ADC_Channel_5

// 烟雾浓度阈值（可根据实际传感器调整）
#define SMOKE_THRESHOLD_LOW     1000  // 低浓度阈值
#define SMOKE_THRESHOLD_MEDIUM  2000  // 中浓度阈值
#define SMOKE_THRESHOLD_HIGH    3000  // 高浓度阈值

// 函数声明
void SmokeSensor_Init(void);
uint16_t SmokeSensor_ReadADC(void);
float SmokeSensor_GetConcentration(void);
char* SmokeSensor_GetLevelString(uint16_t adc_value);

#endif