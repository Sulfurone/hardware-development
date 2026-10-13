#ifndef BSP_DELAY_H
#define BSP_DELAY_H

#include "stm32f10x.h"

void Delay_Init(void);
void Delay_Us(uint32_t us);
void Delay_Ms(uint32_t ms);

#endif
