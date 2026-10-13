#include "Valve.h"

void Valve_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(VALVE_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = VALVE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(VALVE_PORT, &GPIO_InitStructure);

    Valve_Close(); // 默认关闭
}

void Valve_Open(void)
{
    GPIO_SetBits(VALVE_PORT, VALVE_PIN);
}

void Valve_Close(void)
{
    GPIO_ResetBits(VALVE_PORT, VALVE_PIN);
}
