//#include "Light.h"

//#define LIGHT_GPIO_PORT GPIOB
//#define LIGHT_GPIO_PIN  GPIO_Pin_13

//void Light_Init(void)
//{
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitTypeDef GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Pin = LIGHT_GPIO_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(LIGHT_GPIO_PORT, &GPIO_InitStructure);
//    GPIO_ResetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN); // 默认关灯
//}

//void Light_Set(uint8_t state)
//{
//    if(state)
//        GPIO_SetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
//    else
//        GPIO_ResetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
//}

//void Light_Toggle(void)
//{
//    if(GPIO_ReadOutputDataBit(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN))
//        GPIO_ResetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
//    else
//        GPIO_SetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
//}

