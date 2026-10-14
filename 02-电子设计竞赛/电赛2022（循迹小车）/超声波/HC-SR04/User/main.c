#include  "main.h"
#include "HCSR04.h"
#include  "string.h"
#include "stdio.h"
/*  全局变量申明区  */
// u8  a=0;
// u16 len ;
/* 函数调用申明区 */


u32  flag=0,out=0,kind=0;//形参
u32  pwm1,pwm2,pwm3,pwm4;//电机
u32  pwm0;//舵机
u8 key=0;	

int main(void)
{ 
    int Distance1;
    WDT_A_holdTimer();
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);//led灯
    SystemClockInit(HFXT);//系统时钟初始化（系统主时钟）
    SystemClockInit(DCO);//系统时钟初始化（子系统时钟）
	Usart1tInit(); //串口0 初始化
	HCSR04Init();
    LedInit();
    while(1)
    {
        Distance1= Distance();
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);
        printf("Distance = %dmm\r\n",Distance1);    
        delay_ms(50);
    }
		

}
	


//					key = KEY_Scan();
////		OLED_ShowNum(20,3,key,2,16); //时
				
 			//得到键值						   
//			switch(key)
//				{	
//					case 1:	
////						OLED_ShowNum(20,3,1,2,16);
//					USART0PutString((u8 *)"#255P0500T0100!") ;
//					break; //时
//					case 2:	
////						OLED_ShowNum(20,3,2,2,16);
//					break; //时
//					case 3:	
//						OLED_ShowNum(20,3,3,2,16);
//					break; //时
//					case 4:	
//						OLED_ShowNum(20,3,4,2,16);
//					break; //时
//					case 5:	
//						OLED_ShowNum(20,3,5,2,16);
//					break; //时
//					case 6:	
//						OLED_ShowNum(20,3,6,2,16);
//					break; //时
//					case 7:	
//						OLED_ShowNum(20,3,7,2,16);
//					break; //时
//					case 8:	
//						OLED_ShowNum(20,3,8,2,16);
//					break; //时
//					case 9:
//						OLED_ShowNum(20,3,9,2,16);
//					break; //时
//					case 10: 
//						OLED_ShowNum(20,3,10,2,16);
//					break; //时
//					case 11: 
//						OLED_ShowNum(20,3,11,2,16);
//					break; //时
//					case 12: 
//						OLED_ShowNum(20,3,12,2,16);
//					break; //时
//					case 13: 
//						OLED_ShowNum(20,3,13,2,16);
//					break; //时
//					case 14: 
//						OLED_ShowNum(20,3,14,2,16);
//					break; //时
//					case 15: 
//						OLED_ShowNum(20,3,15,2,16);
//					break; //时
//					case 16: 
//						OLED_ShowNum(20,3,16,2,16);
//					break; //时
//				}


//		USART0PutString((u8 *)"#255P0500T0100!") ;//整体
//		delay_s(3);
//			USART0PutString((u8 *)"#255P2500T0100!") ;
//		delay_s(3);
//			huifu();
		

			
			



//     printf ("Hello MSP432!\r\n");




//	while(1) 
//	{	

//		key = KEY_Scan();
////		OLED_ShowNum(20,3,key,2,16); //时
//				
//			//得到键值						   
//			switch(key)
//				{	
//					case 1:	OLED_ShowNum(20,3,1,2,16);break; //时
//					case 2:	OLED_ShowNum(20,3,2,2,16);break; //时
//					case 3:	OLED_ShowNum(20,3,3,2,16);break; //时
//					case 4:	OLED_ShowNum(20,3,4,2,16);break; //时
//					case 5:	OLED_ShowNum(20,3,5,2,16);break; //时
//					case 6:	OLED_ShowNum(20,3,6,2,16);break; //时
//					case 7:	OLED_ShowNum(20,3,7,2,16);break; //时
//					case 8:	OLED_ShowNum(20,3,8,2,16);break; //时
//					case 9:	OLED_ShowNum(20,3,9,2,16);break; //时
//					case 10: OLED_ShowNum(20,3,10,2,16);break; //时
//					case 11: OLED_ShowNum(20,3,11,2,16);break; //时
//					case 12: OLED_ShowNum(20,3,12,2,16);break; //时
//					case 13: OLED_ShowNum(20,3,13,2,16);break; //时
//					case 14: OLED_ShowNum(20,3,14,2,16);break; //时
//					case 15: OLED_ShowNum(20,3,15,2,16);break; //时
//					case 16: OLED_ShowNum(20,3,16,2,16);break; //时
//				}
//			}





//	OLED_Init();   //初始化OLED  
//	OLED_Clear() ; //清屏
//	/* 系统启动页面  */
//	OLED_ShowString(36,0,"2019");
//	OLED_ShowCHinese(72,0,6); //年
//	OLED_ShowCHinese(10,3,0); //电
//	OLED_ShowCHinese(28,3,1); //子
//	OLED_ShowCHinese(46,3,2); //设
//	OLED_ShowCHinese(64,3,3); //计
//	OLED_ShowCHinese(82,3,4); //大
//	OLED_ShowCHinese(104,3,5);//赛
//	OLED_ShowString(26,6,"(");
//	OLED_ShowString(36,6,"TI");
//	OLED_ShowCHinese(70,6,7);//杯
//	OLED_ShowString(90,6,")");
//	delay_s(2);  
//	OLED_Clear(); //刷新界面	
////		USART0PutChar('F') ;

