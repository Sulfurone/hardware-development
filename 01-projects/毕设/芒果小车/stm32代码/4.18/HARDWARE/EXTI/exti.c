#include "exti.h"
#include "stm32f10x.h"
//#include "key.h"
#include "led1.h"
#include "delay.h"
void exti_Init(void)
{
	
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
//	KEY_Init();//初始化按键对应的IO口模式
	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA对应的时钟，同时开启复用时钟（引脚复用）
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);//GPIOC.5中断线以及中断初始化配置
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);//GPIOA.15中断线映射

	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

	
/*GPIO_InitTypeDef GPIO_InitStucture;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);//设置IO口与中断线的映射关系
	GPIO_InitStucture.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStucture.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitStucture.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStucture);//初始化GPIOA.15为输入
	*///都已经被KEY_Init();代替了
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿触发
	
	
	EXTI_Init(&EXTI_InitStructure);//设置触发中断的相关条件
	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿触发	
	EXTI_Init(&EXTI_InitStructure);//设置触发中断的相关条件
	
	  //GPIOA.0   
 
    /*EXTI_InitStructure.EXTI_Line=EXTI_Line0; 
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;   
		EXTI_Init(&EXTI_InitStructure); */  
	
	/*NVIC_InitStructure.NVIC_IRQChannel=EXTI0_IRQn; //使能按键坐在的外部中断通道0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;//设置抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;//设置子优先级为1
	NVIC_Init(&NVIC_InitStructure);*/
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn; //使能通道5
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //????? 2,    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;  //???? 1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //????????    
	NVIC_Init(&NVIC_InitStructure);   
	
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn; ; //使能通道5
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //????? 2,    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;  //???? 1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //????????    
	NVIC_Init(&NVIC_InitStructure);   
}


/*void EXTI0_IRQHandler(void) 
	{   
		delay_ms(10);  
if(WK_UP==1) 
	{     
		LED0=!LED0;   
		LED1=!LED1; 
  } 
	EXTI_ClearITPendingBit(EXTI_Line0);  
	}  
	 
	 
 
 
  void EXTI9_5_IRQHandler(void)
		{    
			delay_ms(10);   
		if(KEY0==0)
  {  
	LED0=!LED0; 
	}
		EXTI_ClearITPendingBit(EXTI_Line5);   
	} 
 
void EXTI15_10_IRQHandler(void) 
	{   delay_ms(10);    //??  
		if(KEY1==0)
			{  
				LED1=!LED1; 
			} 
			EXTI_ClearITPendingBit(EXTI_Line15); 
			
			
	} 
	*/
	
	
	


