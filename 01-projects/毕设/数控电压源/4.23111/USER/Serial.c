//#include "stm32f10x.h"                  // Device header
//#include <stdarg.h>
//#include "Serial.h"

//char Serial_RxPacket[100];				//"@MSG\r\n"
//#define USE_printf 1

//#if USE_printf 
//#pragma import(__use_no_semihosting)
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 
//};

//void _ttywrch(int ch)
//{
//       // 空实现，避免半主机模式相关的引用错误
//}

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    USART_SendData(USART1, (uint8_t)ch);
//	return ch;
//}
//#else 
//int fputc(int ch, FILE *f){
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    USART_SendData(USART1, (uint8_t)ch);
//	return ch;
//}
//#endif 

//void Serial_Init(USART_TypeDef* USARTx,uint32_t BaudRate){
//	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//	if(USARTx==USART1){
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//TXD
//		GPIO_Init(GPIOA, &GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//RXD
//		GPIO_Init(GPIOA, &GPIO_InitStructure);

//		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	}

//	else if (USARTx==USART3){
//		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//TXD
//		GPIO_Init(GPIOB, &GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//RXD
//		GPIO_Init(GPIOB, &GPIO_InitStructure);
//		
//		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	}
//	
//	USART_InitTypeDef USART_InitStructure;
//	USART_InitStructure.USART_BaudRate = BaudRate;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//	USART_InitStructure.USART_Parity = USART_Parity_No;//no check at the end 
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//one bit for stop cheak
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//send 8bit at once
//	USART_Init(USARTx, &USART_InitStructure);
//	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//USART_interrupt
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&NVIC_InitStructure);
//	USART_Cmd(USARTx, ENABLE);
//}

//void USART1_IRQHandler(void){//interrupt
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清除标志位

//	}
//}


///*
//void USART1_IRQHandler(void){//interrupt
//static uint8_t flag = 0;
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
//		if(flag==0)flag=1;
//		else flag = 0;
//		GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)flag);
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//	}
//}
//*/

///*
//void USART2_IRQHandler(void){//interrupt
//	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
//		USART_ReceiveData(USART2);
//		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//	}
//}
//*/
