#ifndef __LIGHT_H
#define __LIGHT_H

#include "stm32f10x.h"

void Light_Init(void);
void Light_Set(uint8_t state); // 1:开灯 0:关灯
void Light_Toggle(void);

#endif // __LIGHT_H
