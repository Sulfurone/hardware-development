#ifndef __MYIIC_H
#define __MYIIC_H
#include "stm32f0xx.h" 
#include "stdbool.h"

void iicDevStart(void);	
void iicDevStop(void);
void iicDevAck(void);
void iicDevNAck(void);
uint8_t iicDevWaitAck(void);
void iicDevSendByte(uint8_t txd);
uint8_t iicDevReceiveByte(uint8_t ack);

void myIIC_Config(void);

#endif
















