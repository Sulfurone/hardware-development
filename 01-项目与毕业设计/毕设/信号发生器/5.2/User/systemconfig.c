
#include  <stm32f10x.h>
#include "systemconfig.h"
#include "common.h"

/*******************************************************************************
* 函数名称       : RCC_Configuration
* 功能描述       : 配置外设时钟.
* 入口参数       : 无
* 出口参数       : 无
*******************************************************************************/
void RCC_Configuration(void)
{
 

	//使能GPIO时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

	//使能复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	//USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//延时初始化
	Delay_Init(); 
					 
}

/*******************************************************************************
* 函数名称       : GPIO_Configuration
* 功能描述       : 配置项目中所有用到的GPIO
* 入口参数       : 无
* 出口参数       : 无
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//使能SWJ，禁止 JTAG

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  //LED1,LED2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		  //key
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	
	//串口UART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
                                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              //TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	 

}

/*******************************************************************************
* 函数名称       : NVIC_Configuration
* 功能描述       : 配置向量表.
* 入口参数       : 无
* 出口参数       : 无
*******************************************************************************/
void NVIC_Configuration(void)
{
	//EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	/*串口USART1中断配置 */
#ifdef EN_USART_RX	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif

	/*串口USART3中断配置 */	

//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 	//设置串口3中断
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  	NVIC_Init(&NVIC_InitStructure);
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);


}


void SystemConfiguration(void)
{	
	RCC_Configuration(); 

	GPIO_Configuration(); 	

	NVIC_Configuration();
}



