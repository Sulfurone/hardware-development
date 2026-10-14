#ifndef __KEY_H
#define __KEY_H


#include "driverlib/systemInit.h"



typedef unsigned char   uchar;
typedef unsigned int   uint;
/*#define  KEYIN  P5IN                           

#define  KEY_LD   BIT3        //片选（p5.3用来产生片选信号）  
#define  KEY_CLK  BIT2        //时钟（p5.2用来产生时钟信号）
#define  KEY_DIN  BIT1        //数据
#define  KEY_IRQ  BIT0        //中断标志

#define  KEY_LD_L  P5OUT  &=~ KEY_LD   //硬件中CS=0
#define  KEY_LD_H  P5OUT  |=  KEY_LD   //硬件中CS=1
#define  KEY_CLK_L P5OUT  &=~ KEY_CLK   //硬件中CLK=0
#define  KEY_CLK_H P5OUT  |=  KEY_CLK   //硬件中CLK=1

#define  Key_Initial  P5DIR |= 0x0C;    //按键端口初始化（CS=1，CLK=0，DIN=1，IRQ=0） */

#define  KEY_DATA_PERIPH             SYSCTL_PERIPH_GPIOC
#define  KEY_DATA_PORT               GPIO_PORTC_BASE
#define  KEY_DATA_PINS				 (GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6)
//#define  KEY_DOUT_EN      			 GPIOPinTypeOut(KEY_DATA_PORT, KEY_DATA_PINS);
//#define  KEY_DIN_EN       		 	 GPIOPinTypeIn(KEY_DATA_PORT, KEY_DATA_PINS);

#define  KEY_LD_PIN  				GPIO_PIN_6       //片选（p5.3用来产生片选信号）  
#define  KEY_CLK_PIN  				GPIO_PIN_4       //时钟（p5.2用来产生时钟信号）
#define  KEY_DIN_PIN  				GPIO_PIN_2       //数据
#define  KEY_IRQ_PIN 				GPIO_PIN_0       //中断标志

#define  KEY_LD_L  				GPIOPinWrite(KEY_DATA_PORT, KEY_LD_PIN, 0x00); //硬件中CS=0
#define  KEY_LD_H  				GPIOPinWrite(KEY_DATA_PORT, KEY_LD_PIN, 0xff);   //硬件中CS=1
#define  KEY_CLK_L 				GPIOPinWrite(KEY_DATA_PORT, KEY_CLK_PIN, 0x00); //硬件中CLK=0
#define  KEY_CLK_H 				GPIOPinWrite(KEY_DATA_PORT, KEY_CLK_PIN, 0xff);  //硬件中CLK=1

/*#define  Key_Initial			( SysCtlPeriEnable(KEY_DATA_PERIPH); \
                                  GPIOPinTypeOut(KEY_DATA_PORT,KEY_LD_PIN|KEY_CLK_PIN);\
                                  GPIOPinTypeIn(KEY_DATA_PORT,KEY_DIN_PIN|KEY_IRQ_PIN);)	 */

void Key_Initial(void );
void Key_Deley(unsigned int n);
unsigned char Key_Get(void);
unsigned char Key_Id(unsigned int n);

#endif
