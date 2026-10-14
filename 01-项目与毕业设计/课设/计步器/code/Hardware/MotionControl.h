#ifndef __MOTION_CONTROL_H
#define __MOTION_CONTROL_H

#include "stm32f10x.h"

void MotionControl_Init(void);
void MotionControl_Straight(int16_t pwm);
void MotionControl_Turn(int16_t pwm_left, int16_t pwm_right);
void MotionControl_Stop(void);
void MotionControl_ResetAngle(void);
void MotionControl_Update(void);

#endif