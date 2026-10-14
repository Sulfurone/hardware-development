#include "driverlib.h"
#include "uart_calc.h"
#include "Time_32.h"
#include "Time_A_PWM.h"

/* Standard Includes */
#include <stdint.h>
#include "stdio.h"
#include <stdbool.h>
#include "SEEKFREE_OLED.h"
#include "MOTOR.h"
#include "track.h"


//#include "HCSR04.h"
//#include  "string.h"
//#include "stdio.h"



extern float Speed_Value;
void T32_INT1_IRQnHandler(void);
void Time_A_IRQnHandler(void);

void delay(int z)
{
	int x;
	for(;z>0;z--)
	for(x=123;x>0;x--);




}


int main(void)
{

	uart_init(9600);
	OLED_Init();
	Motor_init();
	





	while(1)
	{
		OLED_Print_Num(0,2,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN2));//left1
		OLED_Print_Num(60,2,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN3));//left2
		OLED_Print_Num(0,4,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN0));//right1
		OLED_Print_Num(60,4,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN1));//right2
   
		
//		OLED_P8x16Str(0,0,"Speed:");
//		OLED_Print_Num(60,0,Speed_Value);
//		OLED_Print_Num(60,0,distance);
		
		app_LineWalking();
	}


}


