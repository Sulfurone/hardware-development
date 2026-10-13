//////////////////////////////////
//


#include "STC15F2K60S2.H"
#include <string.h>
#include <intrins.h>



/** Initlize IE TCON SCON function **/

void initCOM(void)
{
	TMOD=0x20;    //定时器T1使用工作方式2
    TH1=250;     //设置初值
    TH0=250;
    TR1=1;      //开始计时
    PCON=0x80;     //SMOD=1；
    SCON=0x50;     //工作方式1，波特率9600bit/s,允许接收
    TI=1;
}

/** Send a char to MC **/

void sendChar(unsigned char ch)
{
	SBUF = ch;
	while(TI == 0);
	TI = 0;
}

//发送一个字符串
void sendStr(unsigned char *s)
{
    while(*s!='\0')         
    {
        sendChar(*s);
        s++;
    }
}

/** COM receive message interrupt function**/

void inteReceive (void) interrupt 4 using 1
{
	if(RI == 1)
	{
		RI = 0;
	}
}