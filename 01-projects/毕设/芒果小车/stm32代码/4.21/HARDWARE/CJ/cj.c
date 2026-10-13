
#include "cj.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
//unsigned int overcount=0;



/*void NVIC_Config(void)//使用的是TIM2
{
NVIC_InitTypeDef NVIC_InitStructer;

NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

NVIC_InitStructer.NVIC_IRQChannelPreemptionPriority=2;
NVIC_InitStructer.NVIC_IRQChannelSubPriority=2;
NVIC_InitStructer.NVIC_IRQChannel=TIM2_IRQn;
NVIC_InitStructer.NVIC_IRQChannelCmd=ENABLE;

NVIC_Init(&NVIC_InitStructer);
}*/

/*void CH_SR05_Init(void)//超声波是一个外部设备
{
GPIO_InitTypeDef GPIO_InitStructer;
TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructer;
TIM_ICInitTypeDef  TIM2_ICInitStructure;
	
RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
GPIO_InitStructer.GPIO_Pin=GPIO_Pin_14;
GPIO_Init(GPIOA, &GPIO_InitStructer);//PA14管脚使能=Trig
GPIO_ResetBits(GPIOA,GPIO_Pin_14);

			GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPD;
			GPIO_InitStructer.GPIO_Pin=GPIO_Pin_13;
			GPIO_Init(GPIOA, & GPIO_InitStructer);//PA13管脚使能=Echo
			GPIO_ResetBits(GPIOA,GPIO_Pin_13);
	
TIM_DeInit(TIM2);
TIM_TimeBaseInitStructer.TIM_Period=999;//定时周期为1000
TIM_TimeBaseInitStructer.TIM_Prescaler=71; //分频系数为72
TIM_TimeBaseInitStructer.TIM_ClockDivision=TIM_CKD_DIV1;//不分频
TIM_TimeBaseInitStructer.TIM_CounterMode=TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructer);//通用定时器2的配置

    TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
		
TIM_ClearFlag(TIM2,TIM_FLAG_Update);
TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启更新中断
NVIC_Config();
TIM_Cmd(TIM2,DISABLE);
*/


/*float Senor_Using(void)//不需要输入参数，但是最后的返回值为float型，
{
	
	
	float length=0,sum=0;
	u16 tim=0;
	unsigned int i=0;


	while(i!=5)
{

	PAout(14)=1; //
	delay_us(20); //输出触发电平
	PAout(14)=0;

	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13)==0);//如检测到低电平，程序一直在这里循环。检测到高电平就继续执行
	TIM_SetCounter(TIM2,0);
	overcount=0;
	TIM_Cmd(TIM2,ENABLE);
	i+=1;
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13)==1);//检测到低电平就跳出循环
	TIM_Cmd(TIM2,DISABLE);//
	tim=overcount*1000;
	tim+=TIM_GetCounter(TIM2);//取出TIM2 中定时器的计数值
		TIM2->CNT=0; 
	delay_ms(50);
	length=((float)tim/58.0);//

	sum=length+sum;
}
	length=sum/5.0;
	return length; //返回5次测量的平均值

}*/


/*void TIM2_IRQHandler(void) //??,????????,??????????,??????????
{
if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
  {
TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
overcount++;

  }
}*/



