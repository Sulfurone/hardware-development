#include "control.h"	
#include "cs.h"
#include "usart.h"
#include "lunzi.h"
#include "key.h"


int OUTPUT=0;// OUTPUT 是控制舵机的PWM输出值; OUTPUT1 和OUTPUT2 是控制小车左右轮速度的PWM输出值

float Kp = 50.0;
float Ki = 5.0;
float Kd = 0.5;
float max_integral = 1000.0f;  // 积分限幅

float voltage0;
float target_voltage ; // 目标电压（比如2.0V，可根据需要修改）
		
		

float PID_Calc(float target_voltage,float voltage0)
{
    static float integral = 0, last_error = 0;
    // 调整后的参数 - 需要根据实际系统调试
   
    
    float error = target_voltage - voltage0;
    
    // 抗积分饱和
//    if(fabs(error) > 0.1f) {  // 只有误差较大时才积分
//        integral += error;
//        // 积分限幅
//        integral = (integral > max_integral) ? max_integral : 
//                  ((integral < -max_integral) ? -max_integral : integral);
//    }
    
    float derivative = error - last_error;
    last_error = error;
    
    int output = (int)(Kp * error + Ki * integral + Kd * derivative);
    
    // 输出限幅
    output = (output > 7199) ? 7199 : 
            ((output < 0) ? 0 : output);
    
    return output;
}

// 建议的中断处理函数
void TIM3_IRQHandler(void)
{
    if(TIM3->SR & 0X0001)
    {
        TIM3->SR &= ~(1<<0);
        
        // 1. 读取当前输出电压
        float current_voltage = voltage0;  
        
     
  
    TIM_SetCompare3(TIM2, OUTPUT);
        
        // 4. 安全检测
//        if(current_voltage > target_voltage * 1.2f) {
//            // 过压保护处理
//        }
    }
}
