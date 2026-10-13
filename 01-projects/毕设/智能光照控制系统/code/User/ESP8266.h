#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"

void ESP8266_Init(void);
void ESP8266_SendData(uint8_t temp, uint8_t humi, uint16_t light, uint8_t human);
void ESP8266_ReceiveCmd(void);

#endif // __ESP8266_H
