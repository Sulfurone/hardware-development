/*
 * MOTOR.C
 *
 *  Created on: 2020年11月29日
 *      Author: Indira
 */
#include "MOTOR.H"
#include "SysTick.h"

void delay_ms(int z)
{
	int x;
	for(;z>0;z--)
	for(x=123;x>0;x--);




}

Timer_A_PWMConfig pwmConfigLeft =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64,
        512,
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

Timer_A_PWMConfig pwmConfigRight =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64,
        512,
        TIMER_A_CAPTURECOMPARE_REGISTER_3,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

void Motor_init(void){
    //电机驱动TB6612FNG初始化
    /*  P2.4    AIN1
     *  P2.5    AIN2
     *  P3.0    STBY
     *  P5.6    BIN1
     *  P5.7    BIN2
     *  P2.6    PWMA
     *  P2.7    PWMB
     * */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN0);

    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);

    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN7);

    //PWM输出初始
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7,
                GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6,
                GPIO_PRIMARY_MODULE_FUNCTION);

}

//控制PWM
void setPWM(int left,int right){

    pwmConfigLeft.dutyCycle=left;
    pwmConfigRight.dutyCycle=right;

    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigLeft);
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigRight);
}

//前进
/*5   4      6  7
 *1   0      1  0
 * */
void Car_Forward_Function(void)
{


    //left

    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN4);

   //right

    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN7);


}

//左转
/*5   4      6  7
 *0   1      1  0
 * */
void Car_LeftRun_Function(void)
{
    //left

    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);

   //right

    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN7);
//setPWM(100,0);
}


//右转
/*5   4      6  7
 *1   0      0  1
 * */
void Car_RightRun_Function(void)
{
    //left

    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN4);

   //right

    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN6);
//setPWM(0,100);
}

void Car_RightRun(int left,int right)
{
    //left

    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN4);

   //right
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);

    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN7);

    setPWM(left,right);
}

//后退
/*5   4      6  7
 *0   1      0  1
 * */
void Car_backRun_Function(void)
{
    //left
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);

   //right
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN6);
}
//停车
/*5   4      6  7
 *0   0      0  0
 * */
void Car_StopRun_Function(void)
{
    //left
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);

   //right

    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN7);

     setPWM(0,0);

}
