#include "lk.h"
#include "sys.h"
#include "delay.h"
extern int sensor[9];//引用主函数main定义的数组，才能在本文件内使用sensor[x]该变量

extern int Prestopconfig;//引用主函数main定义的Prestopconfig，才能在本文件内使用Prestopconfig该变量

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
	  
//	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出模式
//	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
//	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	  GPIO_Init(GPIOB,&GPIO_InitStructure);//PB8管脚的初始化，TB6612电机驱动板的使能端
//	
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭STM32f103c8t6的JTAGD功能，使PB3和PB4用作普通的IO管脚//必须开启复用功能的时钟才能关闭该功能
}

void read_sensor(void)//红外传感器识别到黑线返回数字信号电平1，未识别到黑线返回高电平0
{
	      /*将位置从最左按顺序到最右的传感器返回的数字信号依次存入数组sensor[0至8]里*/
//				sensor[0]=1;//最左的传感器
//				sensor[1]=1;
				sensor[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
				sensor[3]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);
//				sensor[4]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);//中央的传感器
				sensor[5]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
				sensor[6]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3);
//				sensor[7]=1;
//				sensor[8]=1;//最右的传感器
//error1=sensor[2]+sensor[3];
//error2=sensor[4]+sensor[5];
//	if(error1<error2)//zuobian
//	{
//		error=5*sensor[5]+10*sensor[6];
//	}
//	if(error1>error2)//youbian
//	{
//		error=-(10*sensor[2]+5*sensor[3]);	
//	}
//	if(error1==error2)//zuobian
//	{
//		error=0;	
//	}
		
	
//zhushi
	      if(sensor[2]==0&&sensor[3]==0&&sensor[5]==0&&sensor[6]==0)
				{	/*识别到黑线在中间*/	
				  error=0;a=0;
				}
				else if(sensor[2]==0&&sensor[3]==1&&sensor[5]==0&&sensor[6]==0)
				{	/*识别到黑线在中间*/	
				  error=-5;a=0;
				}
				else if(sensor[2]==1&&sensor[3]==0&&sensor[5]==0&&sensor[6]==0)
				{	/*识别到黑线在中间*/	
				  error=-9;a=0;
				}
	     else if(sensor[2]==0&&sensor[3]==0&&sensor[5]==1&&sensor[6]==0)
				{	/*识别到黑线在中间*/	
				  error=3;a=0;
				}
				else if(sensor[2]==0&&sensor[3]==0&&sensor[5]==0&&sensor[6]==1)
				{	/*识别到黑线在中间*/	
				  error=6;a=0;
				}

				else
					{	/*识别到黑线在中间*/	
				 error=0;a=0;
				}
				
				

//	
//				sensor[0]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//最左的传感器
//				sensor[1]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
//				sensor[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
//				sensor[3]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);
//				sensor[4]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);//中央的传感器
//				sensor[5]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
//				sensor[6]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3);
//				sensor[7]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
//				sensor[8]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);//最右的传感器
//				
				
//	      if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1)&&(sensor[3]==1&&sensor[4]==0)&&(sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{	/*识别到黑线在中间*/	
//				  error=0;a=0;
//				}
//				
//        else if((sensor[0]==0&&sensor[2]==1)&&sensor[4]==0&&(sensor[6]==1&&sensor[8]==0)&&(Prestopconfig==1))//只有Prestopconfig=1时，该条识别语句才会生效
//				{/*识别终点线(也是起跑线)*/  //开始时Prestopconfig=0，起跑线不会识别，变量stopconfig不会改变
//				  error=0;a=0;stopconfig++;					
//				}	
//				
//				else if((sensor[0]==0&&sensor[1]==0&&sensor[2]==0)&&(sensor[3]==0&&sensor[4]==0)&&(sensor[5]==0&&sensor[6]==0&&sensor[7]==0&&sensor[8]==0))
//				{/*识别到十字路口*/
//					error=0;a=0;					
//				}	
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1)&&(sensor[3]==0&&sensor[4]==0)&&(sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置有点偏右*/
//					error=-1;a=0;//误差为-1，舵机和轮子差速进行微小控制  //a=0,基础速度最大
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1)&&(sensor[3]==1&&sensor[4]==0)&&(sensor[5]==0&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置有点偏左*/
//					error=1;a=0;//误差为-1，舵机和轮子差速进行微小控制  //a=0,基础速度最大
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1)&&(sensor[3]==0&&sensor[4]==1)&&(sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏右*/
//					error=-2;a=0;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1)&&(sensor[3]==1&&sensor[4]==1)&&(sensor[5]==0&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏左*/
//					error=2;a=0;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==0&&sensor[3]==0&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1)
//					    |(sensor[0]==1&&sensor[1]==1&&sensor[2]==0&&sensor[3]==0&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))/*符号“|”是或者的意思*/
//				{/*小车位置偏右*/  //小车在有直线道路和弯道道路时，选择弯道进入，学校小车比赛规则要求
//					error=-5;a=1;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==0&&sensor[6]==0&&sensor[7]==1&&sensor[8]==1)
//					    |(sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==0&&sensor[6]==0&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏左*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=5;a=1;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==0&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1)
//					    |(sensor[0]==1&&sensor[1]==1&&sensor[2]==0&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==0&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏右*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=-7;a=2;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==0&&sensor[7]==1&&sensor[8]==1)
//					    |(sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==0&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏左*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=7;a=2;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==0&&sensor[2]==0&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1)
//							|(sensor[0]==1&&sensor[1]==0&&sensor[2]==0&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏右*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=-9;a=3;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==0&&sensor[7]==0&&sensor[8]==1)
//							|(sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==0&&sensor[7]==0&&sensor[8]==1))
//				{/*小车位置偏左*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=9;a=3;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==0&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1)
//							|(sensor[0]==1&&sensor[1]==0&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏右*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=-11;a=4;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==0&&sensor[8]==1)
//					    |(sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==0&&sensor[8]==1))
//				{/*小车位置偏左*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=11;a=4;
//				}
//				
//				else if((sensor[0]==0&&sensor[1]==0&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1)
//							|(sensor[0]==0&&sensor[1]==0&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏右*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=-13;a=5;
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==0&&sensor[8]==0)
//							|(sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==0&&sensor[8]==0))
//				{/*小车位置偏左*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=13;a=5;
//				}
//				
//				else if((sensor[0]==0&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1)
//							|(sensor[0]==0&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==1))
//				{/*小车位置偏向最右*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=-15;a=6;//小车位置偏向最大，这时a设为最大值，小车基础速度减得最厉害
//				}
//				
//				else if((sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==1&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==0)
//							|(sensor[0]==1&&sensor[1]==1&&sensor[2]==1&&sensor[3]==1&&sensor[4]==0&&sensor[5]==1&&sensor[6]==1&&sensor[7]==1&&sensor[8]==0))
//				{/*小车位置偏向最左*/  //小车在有直线道路和弯道道路时，选择弯道进入
//					error=15;a=6;//小车位置偏向最大，这时a设为最大值，小车基础速度减得最厉害
//				}

				
       
}
