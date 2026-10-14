//#include "stm32f10x.h"                  // Device header
//#include <stdio.h>
//#include <stdarg.h>

//u8 sign1;
//u8 Rx1Data;
//u8 Rx1Flag;
//u8 Tx1Packet[40];	//发送数据包(发送控制电机转动命令)数组
//u8 Rx1Packet[40];	//接收数据包(接收电机位置信息)数组
//void USART2_init()
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
//		
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;		//TX
//	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;		//RX
//	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
//	//GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
//	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	USART_InitTypeDef USART_InitStruct;
//	USART_InitStruct.USART_BaudRate=9600;	//波特率
//	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//8位数据位
//	USART_InitStruct.USART_StopBits=USART_StopBits_1;		//1位停止位
//	USART_InitStruct.USART_Parity=USART_Parity_No;	//不校验
//	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
//	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//不使用流控
//	USART_Init(USART2, &USART_InitStruct);
//	
//	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);	//开启串口接收中断
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断优先级分组
//	
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;		//
//	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	
//	USART_Cmd(USART2,ENABLE);
//}
//void SendByte(u8 byte)		//发送一个字节数据
//{
//	USART_SendData(USART2,byte);
//	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送完成
//}
//void SendArray(u8 *Array,u16 len)	//发送一个数组的数据
//{
//	u16 i;
//	for(i=0;i<len;i++)
//	{
//		SendByte(Array[i]);
//	}		
//}
//void SendString(char *String)		//发送一个字符串
//{
//	u8 i;
//	for(i=0;String[i]!='\0';i++)
//	{
//		SendByte(String[i]);
//	}
//}

//void printf_plus(char *format,...)	//适用任何串口
//{
//	char String[100];
//	va_list arg;
//	va_start(arg,format);
//	vsprintf(String,format,arg);
//	va_end(arg);
//	SendString(String);
//}
//u8 Get1Flag()		//返回并清除接收数据包标志位
//{
//	if(Rx1Flag==1)
//	{
//		Rx1Flag=0;
//		return 1;
//	}
//	return 0;
//}

//uint8_t GetRxData(void)
//{
//	return Rx1Data;
//}

//void USART2_IRQHandler()		//串口中断接收数据
//{

//	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)==SET);
//	{
//		Rx1Data=USART_ReceiveData(USART2);
//		SendByte(Rx1Data);
//		Rx1Flag = 1;
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);	
//	}
//}
