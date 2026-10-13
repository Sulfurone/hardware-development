#ifndef __LED_H
#define __LED_H
#include "stm32f0xx.h"   


#define LED_OUT_ON		GPIOB->BRR = GPIO_Pin_1
#define LED_OUT_OFF		GPIOB->BSRR = GPIO_Pin_1
#define LED_OUT_TOGGLE	GPIOB->ODR^=GPIO_Pin_1

#define LED_CC_ON		GPIOB->BRR = GPIO_Pin_5;GPIOB->BSRR = GPIO_Pin_4
#define LED_CV_ON		GPIOB->BRR = GPIO_Pin_4;GPIOB->BSRR = GPIO_Pin_5
#define LED_CV_CC_OFF	GPIOB->BSRR = GPIO_Pin_4;GPIOB->BSRR = GPIO_Pin_5


void LED_Config(void);

#endif
