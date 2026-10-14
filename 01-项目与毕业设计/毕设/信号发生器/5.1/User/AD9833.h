#ifndef __AD9833_H
#define __AD9833_H

#include "stm32f10x.h"

// 波形类型定义
#define SIN_WAVE 0
#define SQU_WAVE 1
#define TRI_WAVE 2

// 函数声明
void AD9833_Init_GPIO(void);
void AD9833_Write(unsigned int TxData);
void AD9833_AmpSet(unsigned char amp);
void AD9833_WaveSeting(double Freq, unsigned int Freq_SFR, unsigned int WaveMode, unsigned int Phase);

#endif

