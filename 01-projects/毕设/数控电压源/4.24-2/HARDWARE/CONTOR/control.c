#include "control.h"	
#include "cs.h"
#include "usart.h"
#include "lunzi.h"
#include "key.h"

int startflag=0;
int OUTPUT;// OUTPUT 是控制舵机的PWM输出值; OUTPUT1 和OUTPUT2 是控制小车左右轮速度的PWM输出值

float Kp = 1.0;
float Ki = 50.0;
float Kd = 0.0;
float max_integral = 1000.0f;  // 积分限幅

float target_voltage ; // 目标电压（比如2.0V，可根据需要修改）
extern float actual_voltage;
		

float PID_Calc(float target_voltage, float actual_voltage)
{
    static float e_1 = 0, e_2 = 0; // e_1:上次误差，e_2:上上次误差
    static int output = 0;         // 上次输出
    float error = target_voltage - actual_voltage;

    // 增量式PID计算
    float delta_output = Kp * (error - e_1)
                      + Ki * error
                      + Kd * (error - 2 * e_1 + e_2);

    output += (int)delta_output;

    // 输出限幅
    if (output > 7199) output = 7199;
    if (output < 0) output = 0;

    // 更新误差
    e_2 = e_1;
    e_1 = error;

    return output;
}

// 建议的中断处理函数
void TIM3_IRQHandler(void)
{
    if(TIM3->SR & 0X0001)
    {
			
        TIM3->SR &= ~(1<<0); 
if(startflag==1)			
{OUTPUT = PID_Calc(target_voltage, actual_voltage);
   TIM_SetCompare3(TIM2, OUTPUT);
}     
        
    }
}
