#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"

typedef struct {
    float target;
    float current;
    float error;
    float last_error;
    float integral;
    float kp;
    float ki;
    float kd;
    float output;
    float output_max;
    float output_min;
} PID_TypeDef;

void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float out_max);
float PID_Calculate(PID_TypeDef *pid, float target, float current);
void PID_Reset(PID_TypeDef *pid);

#endif