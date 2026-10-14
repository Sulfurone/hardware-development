#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
#include "lunzi.h"
#include "led1.h"
#include "stdio.h"
#include "control.h"
#include "lk.h"
#include "key.h"
#include "adc.h"
#include "oled.h"
#include <stdlib.h>

uint16_t AD0, AD1, AD2, AD3;
uint8_t key_value; //????
int TEMP=23;
extern int startflag;
extern int OUTPUT;// OUTPUT ??????PWM???; OUTPUT1 ?OUTPUT2 ???????????PWM???
extern    float target_voltage; // ????,???????
 float actual_voltage;
float voltage;
extern uint16_t input_voltage; // 在key.c里加上input_voltage的extern声明
extern uint16_t input_integer;
extern uint8_t input_decimal, inputting, inputting_decimal, decimal_pos;

int main(void)
{

    SystemInit();  //????????
    delay_init();  //????????
     uart_init(9600);  //?????????????????9600,??USB232???.hex??????
     Key_Init();

     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//??????,?????????????
     TIM2_PWM_Init(7199,0);  //PWM?????,????????,??7199?0,??PWM????10Khz    f=72000000/((7199+1)*(0+1))=10000
    TIM3_Init_Init(99,7199);      //??????????,???PWM?????,??????10ms,????10ms?????control.c???TIM3_IRQHandler??????
    OLED_Init();
    AD_Init();
     
    
    while (1)
    {
Key_Get();
//Key_Scan();
    AD0 = AD_GetValue(ADC_Channel_0);
    AD1 = AD_GetValue(ADC_Channel_1);
    actual_voltage = AD0 * 6 * 3.3f / 4095; // ????
    voltage = AD1 * 3.3f / 4095; // ????
    float vout = AD1 * 3.3f / 4095.0f;
    float current = (2.55f-vout) / 0.2f; // 5A?,??A
		if(current < 0.1f)
    {
        current = 0; // ??,??PWM
    }	
if(current > 3.0f)
    {
//        OUTPUT = 0; // ??,??PWM
         TIM_SetCompare3(TIM2, 0);
			startflag=0;
    }

    // OLED??
    char str[16];
    sprintf(str, "V0:%.2fV", actual_voltage);
    OLED_ShowString(1, 1, str);
    sprintf(str, "OUT:%d", OUTPUT);
    OLED_ShowString(2, 1, str);
    if(inputting) {
        if(inputting_decimal && decimal_pos == 1)
            sprintf(str, "Set:%d.%dV", input_integer, input_decimal);
        else if(inputting_decimal && decimal_pos == 0)
            sprintf(str, "Set:%d.V", input_integer); // 小数点已按下但未输入小数
        else
            sprintf(str, "Set:%dV", input_integer);
    } else {
        sprintf(str, "Set:%.1fV", target_voltage);
    }
    OLED_ShowString(3, 1, "                "); // 清空
    OLED_ShowString(3, 1, str);
    sprintf(str, "I:%.1fA", current);
    OLED_ShowString(4, 1, str);



    printf("%.3f,%.3f\n", actual_voltage, current);
   
    }
}