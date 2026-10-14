#include "driverlib.h"
#include "uart_calc.h"
#include "Time_32.h"
#include "Time_A_PWM.h"
#include "MLX90614.h"
/* Standard Includes */
#include <stdint.h>
#include "stdio.h"
#include <stdbool.h>
#include "SEEKFREE_OLED.h"
#include "MOTOR.h"
#include "track.h"
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
	
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN4);
  GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN4);
  GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN4);
  Interrupt_enableInterrupt(INT_PORT4);
  SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);
	Interrupt_enableMaster();   

//	Time32_Init_ms(TIMER32_0_BASE,200,T32_INT1_IRQnHandler);//编码器速度读取








	while(1)
	{
		OLED_Print_Num(0,2,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN2));//left1
		OLED_Print_Num(60,2,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN3));//left2
		OLED_Print_Num(0,4,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN0));//right1
		OLED_Print_Num(60,4,GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN1));//right2
//		
//		OLED_P8x16Str(0,0,"Speed:");
//		OLED_Print_Num(60,0,Speed_Value);
		app_LineWalking();
	}


}

//脉冲计数中断服务函数，获取码盘反馈的数值;
//void PORT4_IRQHandler(void)
//{
//    uint32_t status;
//    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);
//    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
//    if(status&GPIO_PIN4)
//    {
//        Pulse_Count++;  //脉冲个数计数;
//    }
// }



