#include "shuduo.h"
#include "usart.h"
#include "string.h"
#include  "main.h"



void huifu(void)//重置设置
{
USART0PutString((u8 *)"#001PCLE0!") ;
	USART0PutString((u8 *)"#002PCLE0!") ;
	delay_s(1);
}
void shuduo(void)//初始设置
{
USART0PutString((u8 *)"#001PID001!") ;
	delay_s(1); 
USART0PutString((u8 *)"#002PID002!") ;
	delay_s(1); 
}
void zheng(void)//舵机同时动，180顺
{
USART0PutString((u8 *)"#255PMOD3!") ;
delay_ms(1); 
}
void yi(void)//001动  定圈顺
{
//USART0PutString((u8 *)"#001PMOD5!") ;
	USART0PutString((u8 *)"#001PMOD1!") ;
delay_s(1); 
}
void er(void) //022动   定圈逆
{
//USART0PutString((u8 *)"#002PMOD6!") ;
	USART0PutString((u8 *)"#002PMOD1!") ;
delay_s(1); 
}

//		USART0PutString((u8 *)"{G0000#001P0500T0000!#002P0500T0000!}") ;//两个或三个以上一起动的方法
//		delay_s(3);
//			USART0PutString((u8 *)"{G0000#001P2500T0000!#002P2500T0000!}") ;
//		delay_s(3);
//			huifu();
