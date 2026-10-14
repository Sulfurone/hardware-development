#include "stm32f10x.h"
#include "systemconfig.h" 
#include "common.h"
#include "ad9833.h"

#define LED1	BIT_ADDR(GPIOB_ODR_Addr,0)	  
#define LED2	BIT_ADDR(GPIOB_ODR_Addr,1)
#define KEY		BIT_ADDR(GPIOB_IDR_Addr,2)

#define CLK 			BIT_ADDR(GPIOB_ODR_Addr,14)	  
int main(void)
{	

	 u8 i;

   SystemConfiguration();		    //系统初始化

	LED1=0; //灭
	LED2=1;	//亮

	AD9833_Init_GPIO(); //AD9833 GPIO 初始化

//	AD9833_WaveSeting(8000000.5,0,TRI_WAVE,0 );//1.0005KHz,频率寄存器0，三角波输出 ,初相位0
/*	AD9833_WaveSeting(5000,0,SQU_WAVE,90);	//5KHz,		频率寄存器0，方波输出 	,初相位90 */
/*	AD9833_WaveSeting(100000,0,SIN_WAVE,0 );//100KHz,	频率寄存器0，正弦波输出 ,初相位0 */
	

   	AD9833_WaveSeting(1000.1,0,TRI_WAVE,0);


	AD9833_AmpSet(200); //设置幅值，幅值最大 255


	while(1)
	{
		LED1 = !LED1_READ; //LED1 取反 
		Delay_ms(300);	   //延时 300ms
	}
}




