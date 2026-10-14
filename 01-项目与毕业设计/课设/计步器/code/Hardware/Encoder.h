#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f10x.h"

// 编码器1引脚定义（左轮）
#define ENCODER1_PORT        GPIOA
#define ENCODER1_PIN         GPIO_Pin_7  // PA7

// 编码器参数
#define ENCODER_PPR         200         // 每转脉冲数，按单个上升沿计数
#define ENCODER_WHEEL_DIAMETER 65       // 车轮直径（mm）
#define ENCODER_CIRCUMFERENCE (3.14159 * ENCODER_WHEEL_DIAMETER) // 车轮周长（mm）
#define ENCODER_SAMPLE_TIME 100         // 采样周期（ms），用于闭环速度控制

// 函数声明
void Encoder_Init(void);
uint32_t Encoder1_GetCount(void);
float Encoder1_GetSpeed(void);  // 获取速度（cm/s）
float Encoder1_GetDistanceCm(void);  // 获取累计里程（cm）

#endif /* ENCODER_H */
