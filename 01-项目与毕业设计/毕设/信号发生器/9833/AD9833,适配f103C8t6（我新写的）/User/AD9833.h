#ifndef __AD9833_H
#define __AD9833_H

#include "stm32f10x.h"

// 波形类型定义
#define SIN_WAVE    0
#define TRI_WAVE    1
#define SQU_WAVE    2

// 频率寄存器选择
#define FREQ_REG0   0
#define FREQ_REG1   1

// 函数声明
void AD9833_Init(void);
void AD9833_Reset(void);
void AD9833_SetFrequency(double frequency, uint8_t freq_reg);
void AD9833_SetPhase(uint16_t phase, uint8_t phase_reg);
void AD9833_SetWaveform(uint8_t wave_type);
void AD9833_Sleep(uint8_t sleep_mode);
void AD9833_EnableOutput(uint8_t enable);

#endif /* __AD9833_H */

