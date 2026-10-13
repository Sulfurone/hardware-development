//#include "stm32f10x.h"
//#include <stdio.h>
//#include <stdarg.h>

//u8 sign1;                   // 标志变量，具体用途需结合主程序
//u8 Rx1Data;                 // 接收的单字节数据
//u8 Rx1Flag;                 // 接收标志位，1表示收到新数据
//u8 Tx1Packet[40];           // 发送数据包缓冲区
//u8 Rx1Packet[40];           // 接收数据包缓冲区


//void USART1_init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    USART_InitTypeDef USART_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

//    // PA9  TX
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    // PA10 RX
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    USART_InitStructure.USART_BaudRate = 9600;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;
//    USART_InitStructure.USART_Parity = USART_Parity_No;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//    USART_Init(USART1, &USART_InitStructure);

//    USART_Cmd(USART1, ENABLE);
//}
//void UART1_SendString(char *str)
//{
//    while(*str)
//    {
//        USART_SendData(USART1, *str++);
//        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    }


//}

//void SendByte(u8 byte)		//发送一个字节数据
//{
//	USART_SendData(USART2,byte);
//	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送完成
//}

///**
// * @brief  发送一个数组的数据
// * @param  Array 数据数组指针
// * @param  len   数据长度
// */
//void SendArray(u8 *Array,u16 len)	//发送一个数组的数据
//{
//	u16 i;
//	for(i=0;i<len;i++)
//	{
//		SendByte(Array[i]);
//	}		
//}

///**
// * @brief  发送一个字符串
// * @param  String 字符串指针
// */
//void SendString(char *String)		//发送一个字符串
//{
//	u8 i;
//	for(i=0;String[i]!='\0';i++)
//	{
//		SendByte(String[i]);
//	}
//}

///**
// * @brief  格式化输出（类似printf），通过串口发送
// * @param  format 格式化字符串
// * @param  ...    可变参数
// */
//void printf_plus(char *format,...)	//适用任何串口
//{
//	char String[100];
//	va_list arg;
//	va_start(arg,format);
//	vsprintf(String,format,arg);
//	va_end(arg);
//	SendString(String);
//}

///**
// * @brief  获取并清除接收标志位
// * @retval 1：有新数据，0：无新数据
// */
//u8 Get1Flag()		//返回并清除接收数据包标志位
//{
//	if(Rx1Flag==1)
//	{
//		Rx1Flag=0;
//		return 1;
//	}
//	return 0;
//}

///**
// * @brief  获取接收到的数据
// * @retval Rx1Data：接收到的字节
// */
//uint8_t GetRxData(void)
//{
//	return Rx1Data;
//}

///**
// * @brief  USART2中断服务函数
// *         接收数据并回显，同时设置接收标志位
// */
//void USART1_IRQHandler(void)		//串口中断接收数据
//{
//	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//	{
//		Rx1Data = USART_ReceiveData(USART1);
//		// 这里可以选择是否回显
//		// USART_SendData(USART1, Rx1Data);
//		Rx1Flag = 1;
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
//	}
//}

//void SendByte1(u8 byte)		// 发送一个字节到USART1
//{
//	USART_SendData(USART1, byte);
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//}

//void SendString1(char *String)	// 发送字符串到USART1
//{
//	u8 i;
//	for(i = 0; String[i] != '\0'; i++)
//	{
//		SendByte1(String[i]);
//	}
//}

//int fputc(int ch, FILE *f)
//{
//    USART_SendData(USART1, (uint8_t) ch);
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    return ch;
//}

