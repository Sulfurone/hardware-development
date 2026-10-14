#include "PID.h"

void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float out_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->target = 0;
    pid->current = 0;
    pid->error = 0;
    pid->last_error = 0;
    pid->integral = 0;
    pid->output = 0;
    pid->output_max = out_max;
    pid->output_min = -out_max;
}

float PID_Calculate(PID_TypeDef *pid, float target, float current)
{
    pid->target = target;
    pid->current = current;
    pid->error = target - current;

    pid->integral += pid->error;
    if(pid->integral > 100) pid->integral = 100;
    if(pid->integral < -100) pid->integral = -100;

    float derivative = pid->error - pid->last_error;
    pid->last_error = pid->error;

    pid->output = pid->kp * pid->error
                + pid->ki * pid->integral
                + pid->kd * derivative;

    if(pid->output > pid->output_max) pid->output = pid->output_max;
    if(pid->output < pid->output_min) pid->output = pid->output_min;

    return pid->output;
}

void PID_Reset(PID_TypeDef *pid)
{
    pid->error = 0;
    pid->last_error = 0;
    pid->integral = 0;
    pid->output = 0;            
}
