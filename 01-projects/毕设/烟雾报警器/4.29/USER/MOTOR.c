#include "Motor.h"
#include "stm32f10x.h"
#include "pwm.h"

void Motor_Init(void)//对相应的IO口进行初始化,PA0-3
{

	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA的时钟
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB的时钟
	
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//输出模式为推挽输出，电平可高可低
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PA0-3管脚IO口
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//管脚传输速度
	  GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void stop(void)//小车两个电机反转，因为越过终点线时，单纯停止电机工作，小车会因为惯性停不下来，所以短时间的反转，能抵消惯性，从而使小车迅速停下来
{
	  

		TIM_SetCompare3(TIM2,0);//电机控制
//	    TIM_SetCompare4(TIM2,0);//电机控制
}
void start(void)//小车两个电机正转
{


	  GPIO_SetBits(GPIOA,GPIO_Pin_5);//PA0高电平	     1
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);//PA1低电平   0


}






