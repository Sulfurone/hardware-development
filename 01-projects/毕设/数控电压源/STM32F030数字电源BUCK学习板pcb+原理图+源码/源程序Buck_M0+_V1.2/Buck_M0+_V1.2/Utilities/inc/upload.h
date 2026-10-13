#ifndef __UPLOAD_H
#define __UPLOAD_H

#if USE_USART_PRINTF

#include "stm32f0xx.h"   


typedef struct  
{
	int16_t iBuckOffset;
	int16_t iBoostOffset;
}_CalibMsgType;


extern _CalibMsgType 	CalibMsg;

uint8_t My_Comma_Pos(uint8_t *buf, uint8_t cx);
int My_Str2num(uint8_t *buf, uint8_t*dx);
void Calib_Analysis(_CalibMsgType *msg, uint8_t *buf);

#endif

#endif
















