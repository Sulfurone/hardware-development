#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

#define MOTOR_A_PWM_PIN   GPIO_Pin_0
#define MOTOR_B_PWM_PIN   GPIO_Pin_1
#define MOTOR_A_IN1_PIN   GPIO_Pin_13
#define MOTOR_A_IN2_PIN   GPIO_Pin_12
#define MOTOR_B_IN1_PIN   GPIO_Pin_14
#define MOTOR_B_IN2_PIN   GPIO_Pin_15

#define MOTOR_PWMA_PORT   GPIOA
#define MOTOR_PWMB_PORT   GPIOA
#define MOTOR_A_IN_PORT   GPIOB
#define MOTOR_B_IN_PORT   GPIOB

void Motor_Init(void);
void Motor_A_SetSpeed(int16_t speed);
void Motor_B_SetSpeed(int16_t speed);
void Motor_SetSpeed(int16_t speedA, int16_t speedB);
void Motor_Forward(void);
void Motor_Backward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_Stop(void);

#endif