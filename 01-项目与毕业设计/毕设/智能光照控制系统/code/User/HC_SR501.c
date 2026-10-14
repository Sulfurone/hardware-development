//#include "stm32f10x.h"

//#define HC_SR501_GPIO_PIN GPIO_Pin_5
//#define HC_SR501_GPIO GPIOA
//#define HC_SR501_RCC RCC_APB2Periph_GPIOA

//void HC_SR501_GPIO_Init(void) {
//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(HC_SR501_RCC, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = HC_SR501_GPIO_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_Init(HC_SR501_GPIO, &GPIO_InitStructure);
//}
//uint16_t HC_SR501_GetData(void) {
//    return GPIO_ReadInputDataBit(HC_SR501_GPIO, HC_SR501_GPIO_PIN);
//}

