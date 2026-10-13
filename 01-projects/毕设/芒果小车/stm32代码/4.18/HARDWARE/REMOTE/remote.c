#include "remote.h"
#include "stm32f10x.h"
#include "delay.h" 
#include "usart.h" //红外遥控初始化
//初始化IO口配置以及定时器5输入捕获
void Remote_Init(void)       
	{   
 	 
		GPIO_InitTypeDef GPIO_InitStructure;  
		NVIC_InitTypeDef NVIC_InitStructure;  
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
		TIM_ICInitTypeDef  TIM_ICInitStructure;      
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //?? PORTB ??  
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //TIM4
 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;   		//PA1 ??    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //????   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
		GPIO_SetBits(GPIOB,GPIO_Pin_9);		//??? GPIOA1          
		TIM_TimeBaseStructure.TIM_Period = 10000; //?????????? ?? 10ms ??  
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //???? 1M ?????,1us ? 1.  
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //?????? 
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //?????????? TIMx 
 
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3|TIM_Channel_4;  // IC2 ??? TI4 ?   
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //?????   
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //??????,???   
		TIM_ICInitStructure.TIM_ICFilter = 0x03; //IC4F=0011 ??????? 8 ??????????   
		TIM_ICInit(TIM4, &TIM_ICInitStructure);//???????????? 
 
     
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4 ?? 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //????? 0 ? 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //???? 3 ? 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ????? 
		NVIC_Init(&NVIC_InitStructure);  //????? NVIC ???  
		
		    TIM_Cmd(TIM4,ENABLE );  //????? 5    
				TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC2,ENABLE); //允许更新中断，,允许CC2IE捕获中断
			
		}
				u8  RmtSta=0;     
				u16 Dval;  //下降沿时计数器的值
				u32 RmtRec=0; //红外遥控接收到的数据      
				u8  RmtCnt=0; //按键按下的次数  
				void TIM4_IRQHandler(void) 
					{           
						if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)  
							{  
								if(RmtSta&0x80)//??????????  
								{     RmtSta&=~0X10;  //????????????    
									if((RmtSta&0X0F)==0X00)RmtSta|=1<<6; //?????????????????  
									if((RmtSta&0X0F)<14)RmtSta++;  
									else  
										{    
										RmtSta&=~(1<<7);//??????    
										RmtSta&=0XF0; //?????   
										}            
							  }           
						  }   
							if(TIM_GetITStatus(TIM4,TIM_IT_CC2)!=RESET) 
								{      if(RDATA1|RDATA2)//?????  
									{ 
 
                        TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling); //?????  
												TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling); 
												TIM_SetCounter(TIM4,0);     //??????  
												RmtSta|=0X10;      
									}
										else //?????   
											{        
												Dval=TIM_GetCapture2(TIM4);//?? CCR1 ???? CC1IF ??? 
												TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //上升沿捕获
												TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
												
												      if(RmtSta&0X10)     //?????????  
																{     
																	if(RmtSta&0X80)//???????    
																		{         
																			if(Dval>300&&Dval<800)   //560 ????,560us  
																				{       RmtRec<<=1; //????.    
																					RmtRec|=0; //??? 0     
																				}
																			else if(Dval>1400&&Dval<1800) //1680 ????,1680us  
																				{      
																					RmtRec<<=1; //????.     
																					RmtRec|=1; //??? 1   
																				}
																	else if(Dval>2200&&Dval<2600)  //??????????? 2500 ???? 2.5ms   
																		{       RmtCnt++;   //?????? 1 ?    
																					RmtSta&=0XF0; //?????   
																		}  
																		
																}
																		else if(Dval>4200&&Dval<4700)  //4500 ???? 4.5ms  
																{   
																	RmtSta|=1<<7; //???????????    
																	RmtCnt=0;  //?????????   
																}         
  }   
		RmtSta&=~(1<<4); 
					
		}       
	}  
	TIM_ClearFlag(TIM4,TIM_IT_Update|TIM_IT_CC2);  
	} 
	    u8 Remote_Scan(void)
		{         
			u8 sta=0;        
			u8 t1,t2;    
			if(RmtSta&(1<<6))//???????????? 
				{  
					
					
					 t1=RmtRec>>24;   //?????   
					t2=(RmtRec>>16)&0xff; //??????   
					if((t1==(u8)~t2)&&t1==REMOTE_ID)//???????(ID)???   
						{       
							t1=RmtRec>>8;     
							t2=RmtRec;     
							if(t1==(u8)~t2)sta=t1;//????   
							}    
						if((sta==0)||((RmtSta&0X80)==0))//??????/????????? 
							{    
								RmtSta&=~(1<<6);//??????????? 
								RmtCnt=0;  //????????? 
								} 
		    }      
		return sta; 
			}
		
			
			
			
			
			
			
			
