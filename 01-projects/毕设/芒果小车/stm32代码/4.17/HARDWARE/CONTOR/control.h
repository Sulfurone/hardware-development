#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

void TIM3_IRQHandler(void); 
void Xianfu_PID(void);
void Su(void);
int Position_P (int Encoder,int Target);
int Position_PD (int Encoder,int Target);
int Position_PID (int Encoder,int Target);

#endif

