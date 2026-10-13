#include "Motor.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = MOTOR_A_PWM_PIN;
    GPIO_Init(MOTOR_PWMA_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MOTOR_B_PWM_PIN;
    GPIO_Init(MOTOR_PWMB_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = MOTOR_A_IN1_PIN;
    GPIO_Init(MOTOR_A_IN_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MOTOR_A_IN2_PIN;
    GPIO_Init(MOTOR_A_IN_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MOTOR_B_IN1_PIN;
    GPIO_Init(MOTOR_B_IN_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MOTOR_B_IN2_PIN;
    GPIO_Init(MOTOR_B_IN_PORT, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE);

    Motor_Stop();
}

void Motor_A_SetSpeed(int16_t speed)
{
    if (speed > 1000) speed = 1000;
    if (speed < -1000) speed = -1000;

    if (speed >= 0)
    {
        GPIO_WriteBit(MOTOR_A_IN_PORT, MOTOR_A_IN1_PIN, Bit_SET);
        GPIO_WriteBit(MOTOR_A_IN_PORT, MOTOR_A_IN2_PIN, Bit_RESET);
    }
    else
    {
        GPIO_WriteBit(MOTOR_A_IN_PORT, MOTOR_A_IN1_PIN, Bit_RESET);
        GPIO_WriteBit(MOTOR_A_IN_PORT, MOTOR_A_IN2_PIN, Bit_SET);
        speed = -speed;
    }

    TIM_SetCompare1(TIM2, (uint16_t)speed);
}

void Motor_B_SetSpeed(int16_t speed)
{
    if (speed > 1000) speed = 1000;
    if (speed < -1000) speed = -1000;

    if (speed >= 0)
    {
        GPIO_WriteBit(MOTOR_B_IN_PORT, MOTOR_B_IN1_PIN, Bit_SET);
        GPIO_WriteBit(MOTOR_B_IN_PORT, MOTOR_B_IN2_PIN, Bit_RESET);
    }
    else
    {
        GPIO_WriteBit(MOTOR_B_IN_PORT, MOTOR_B_IN1_PIN, Bit_RESET);
        GPIO_WriteBit(MOTOR_B_IN_PORT, MOTOR_B_IN2_PIN, Bit_SET);
        speed = -speed;
    }

    TIM_SetCompare2(TIM2, (uint16_t)speed);
}

void Motor_SetSpeed(int16_t speedA, int16_t speedB)
{
    Motor_A_SetSpeed(speedA);
    Motor_B_SetSpeed(speedB);
}

void Motor_Forward(void)
{
    Motor_SetSpeed(500, 500);
}

void Motor_Backward(void)
{
    Motor_SetSpeed(-500, -500);
}

void Motor_TurnLeft(void)
{
    Motor_SetSpeed(-200, 200);
}

void Motor_TurnRight(void)
{
    Motor_SetSpeed(200, -200);
}

void Motor_ArcLeft(void)
{
    Motor_SetSpeed(300, 300);
}

void Motor_ArcRight(void)
{
    Motor_SetSpeed(300, 300);
}

void Motor_Stop(void)
{
    GPIO_WriteBit(MOTOR_A_IN_PORT, MOTOR_A_IN1_PIN, Bit_RESET);
    GPIO_WriteBit(MOTOR_A_IN_PORT, MOTOR_A_IN2_PIN, Bit_RESET);
    GPIO_WriteBit(MOTOR_B_IN_PORT, MOTOR_B_IN1_PIN, Bit_RESET);
    GPIO_WriteBit(MOTOR_B_IN_PORT, MOTOR_B_IN2_PIN, Bit_RESET);

    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, 0);
}
