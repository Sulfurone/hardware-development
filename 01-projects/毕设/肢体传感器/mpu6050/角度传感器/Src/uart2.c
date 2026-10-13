#include "stm32f10x.h"      // STM32F10x 设备头文件
#include <stdio.h>
#include <stdarg.h>

// 全局变量定义
u8 sign1;                   // 标志变量，具体用途需结合主程序
u8 Rx1Data;                 // 接收的单字节数据
u8 Rx1Flag;                 // 接收标志位，1表示收到新数据
u8 Tx1Packet[40];           // 发送数据包缓冲区
u8 Rx1Packet[40];           // 接收数据包缓冲区

/**
 * @brief  初始化USART2串口
 *         配置GPIO、串口参数、中断优先级等
 */
void USART2_init()
{
	// 使能GPIOA和USART2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
	// 配置PA2为复用推挽输出（TX）
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;		//TX
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	// 配置PA3为浮空输入（RX）
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;		//RX
	//GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	// 配置USART2参数
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=9600;	//波特率
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//8位数据位
	USART_InitStruct.USART_StopBits=USART_StopBits_1;		//1位停止位
	USART_InitStruct.USART_Parity=USART_Parity_No;	//不校验
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//不使用流控
	USART_Init(USART2, &USART_InitStruct);
	
	// 使能USART2接收中断
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);	//开启串口接收中断
	
	// 配置中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断优先级分组
	
	// 配置USART2中断
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;		//
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	// 使能USART2
	USART_Cmd(USART2,ENABLE);
}

/**
 * @brief  发送一个字节数据
 * @param  byte 要发送的数据
 */
void SendByte(u8 byte)		//发送一个字节数据
{
	USART_SendData(USART2,byte);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送完成
}

/**
 * @brief  发送一个数组的数据
 * @param  Array 数据数组指针
 * @param  len   数据长度
 */
void SendArray(u8 *Array,u16 len)	//发送一个数组的数据
{
	u16 i;
	for(i=0;i<len;i++)
	{
		SendByte(Array[i]);
	}		
}

/**
 * @brief  发送一个字符串
 * @param  String 字符串指针
 */
void SendString(char *String)		//发送一个字符串
{
	u8 i;
	for(i=0;String[i]!='\0';i++)
	{
		SendByte(String[i]);
	}
}

/**
 * @brief  格式化输出（类似printf），通过串口发送
 * @param  format 格式化字符串
 * @param  ...    可变参数
 */
void printf_plus(char *format,...)	//适用任何串口
{
	char String[100];
	va_list arg;
	va_start(arg,format);
	vsprintf(String,format,arg);
	va_end(arg);
	SendString(String);
}

/**
 * @brief  获取并清除接收标志位
 * @retval 1：有新数据，0：无新数据
 */
u8 Get1Flag()		//返回并清除接收数据包标志位
{
	if(Rx1Flag==1)
	{
		Rx1Flag=0;
		return 1;
	}
	return 0;
}

/**
 * @brief  获取接收到的数据
 * @retval Rx1Data：接收到的字节
 */
uint8_t GetRxData(void)
{
	return Rx1Data;
}

/**
 * @brief  USART2中断服务函数
 *         接收数据并回显，同时设置接收标志位
 */
void USART2_IRQHandler()		//串口中断接收数据
{

	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)==SET);
	{
		Rx1Data=USART_ReceiveData(USART2);
		SendByte(Rx1Data);
		Rx1Flag = 1;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);	
	}
}
