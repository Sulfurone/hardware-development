#ifndef __TIM_H
#define __TIM_H

#include "stm32f10x.h"

extern volatile uint16_t pwm_value;
extern volatile uint8_t pwm_dir;

void Timer2_Init(void);

#endif
