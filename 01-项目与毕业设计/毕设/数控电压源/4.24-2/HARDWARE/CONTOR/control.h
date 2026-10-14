#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

void TIM3_IRQHandler(void); 
void Xianfu_PID(void);
float PID_Calc(float target_voltage,float voltage0);
int Position_P (int Encoder,int Target);
int Position_PD (int Encoder,int Target);
int Position_PID (int Encoder,int Target);

#endif

