#ifndef __VALVE_H
#define __VALVE_H

#include "stm32f10x.h"

#define VALVE_PIN       GPIO_Pin_1
#define VALVE_PORT      GPIOB
#define VALVE_RCC       RCC_APB2Periph_GPIOB

void Valve_Init(void);
void Valve_Open(void);
void Valve_Close(void);

#endif
