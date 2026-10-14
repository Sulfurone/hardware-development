#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

#include <stdint.h>

// 简化 PID 函数声明（单参数）
int Position_PID(int measured);

void TIM3_IRQHandler(void); 
void Xianfu_PID(void);
void Su(void);
int Position_P (int Encoder,int Target);
int Position_PD (int Encoder,int Target);
extern int duty;
extern int OUTPUT,OUTPUT1,OUTPUT2;// OUTPUT 是控制舵机的PWM输出值; OUTPUT1 和OUTPUT2 是控制小车左右轮速度的PWM输出值
extern int caiflag; // 采样请求标志，由主循环设置

// 外部调用接口
void Control_Run(void);

// 外部变量声明
extern int output;

#endif

