#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include "gizwits_protocol.h"
#include "usart2.h"

/*****************辰哥单片机设计******************
											STM32
 * 项目			:	机智云平台搭建实验                     
 * 版本			: V1.0
 * 日期			: 2024.10.7
 * MCU			:	STM32F103C8T6
 * 接口			:	参串口2						
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥 

**********************BEGIN***********************/

extern uint8_t Usart2_RxPacket[6];				//定义接收数据包数组
extern uint8_t Usart2_RxFlag;

void USART2_Config(void);
uint8_t Usart2_GetRxFlag(void);


#endif


