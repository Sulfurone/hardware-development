#ifndef __LDR_H
#define	__LDR_H
#include "stm32f10x.h"
#include "adcx.h"
#include "Delay.h"
#include "math.h"



#define LDR_READ_TIMES	5  //每次测量时ADC循环采取的次数

/***************自定义区域****************/
// LDR GPIO定义
#define		LDR_GPIO_CLK							RCC_APB2Periph_GPIOB
#define 	LDR_GPIO_PORT							GPIOB
#define 	LDR_GPIO_PIN							GPIO_Pin_1
/*********************END**********************/


// ADC 通道定义 - PB1对应ADC_Channel_9
#define   ADC_CHANNEL               ADC_Channel_9

//#define    ADC_IRQ                       ADC3_IRQn
//#define    ADC_IRQHandler                ADC3_IRQHandler

void LDR_Init(void);
uint16_t LDR_ADC_Read(void);
uint16_t LDR_Average_Data(void);
uint16_t LDR_LuxData(void);

#endif /* __ADC_H */

