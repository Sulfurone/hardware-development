#include "lk.h"
#include "sys.h"
#include "delay.h"

int sensor[4]={0,0,0,0};   //定义一个数组，用于储存红外传感器返回的值，

int Prestopconfig=0;//引用主函数main定义的Prestopconfig，才能在本文件内使用Prestopconfig该变量

int c=0;//小车的位置偏移量 误差(error),位置偏移越大，误差越大，偏移越小，误差越小

int a=0;//小车基础速度的标志变量，a越小时，基础速度越大
int stopconfig=0; //检测终点线的标志变量  //当检测到终点线时，该变量的数值会改变
int error=0,error1,error2;
void hongwai_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA的时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB的时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能复用功能的时钟
	
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//IO管脚模式配置为浮空输入，这样就能获取传感器传回来的数字信号(高低电平)
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA,&GPIO_InitStructure);//PA4,PA5,PA6,PA7管脚的初始化
	
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入模式
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_3;
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	  GPIO_Init(GPIOB,&GPIO_InitStructure);//PB0,PB1,PB3,PB4,PB5管脚的初始化

	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭STM32f103c8t6的JTAGD功能，使PB3和PB4用作普通的IO管脚//必须开启复用功能的时钟才能关闭该功能
}

void read_sensor(void)//红外传感器识别到黑线返回数字信号电平1，未识别到黑线返回高电平0
{
	sensor[0]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);//左1
	sensor[1]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3);//左二
				sensor[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);//右2
				sensor[3]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);//you1



	      if(sensor[0]==0&&sensor[1]==0&&sensor[2]==0&&sensor[3]==0)
				{	/*识别到黑线在中间*/	
				  error=0;a=0;
				}
				else if(sensor[0]==0&&sensor[1]==1&&sensor[2]==0&&sensor[3]==0)
				{	/*识别到黑线在中间*/	
				  error=-4;a=0;
				}
				else if(sensor[0]==1&&sensor[1]==0&&sensor[2]==0&&sensor[3]==0)
				{	/*识别到黑线在中间*/	
				  error=-5;a=0;
				}
	     else if(sensor[0]==0&&sensor[1]==0&&sensor[2]==1&&sensor[3]==0)
				{	/*识别到黑线在中间*/	
				  error=2;a=0;
				}
				else if(sensor[0]==0&&sensor[1]==0&&sensor[2]==0&&sensor[3]==1)
				{	/*识别到黑线在中间*/	
				  error=4;a=0;
				}

				else
					{	/*识别到黑线在中间*/	
				 error=0;a=0;
				}
				

       
}
