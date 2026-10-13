#include "stm32f10x.h"
#include "Light.h"
#include "KEY.h"
#include "OLED.h"
#include "Delay.h"
#include "BODY_HW.h"  // 添加人体传感器头文件

// 系统状态变量
volatile uint16_t pwm_value = 50;     // 当前PWM值，初始为50
volatile uint8_t pwm_dir = 1;         // PWM变化方向: 1递增，0递减
volatile uint8_t light_mode = 0;      // 灯光模式: 0=自动, 1=手动
volatile uint8_t light_power = 1;     // 灯光电源状态: 0=关, 1=开
volatile uint8_t key_control = 0;     // 按键控制标志: 0=人体传感器控制, 1=按键控制

// 引用main.c中的光照传感器值
extern u16 light;

/**
  * @brief  初始化定时器2
  * @param  无
  * @retval 无
  */
void Timer2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_Period = 999;       // ARR值：999，计数到999产生更新事件
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;   // 预分频：72MHz/7200=10kHz，10kHz/1000=10Hz，即100ms中断一次
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	// 使能TIM2更新中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	// 配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// 使能TIM2
	TIM_Cmd(TIM2, ENABLE);
    
    // 初始化按键
    KEY_Init();
    
    // 初始状态为自动模式且开灯
    light_mode = 0;
    light_power = 1;
}

/**
  * @brief  处理按键事件，在主循环中调用
  * @param  无
  * @retval 无
  */
void KEY_Process(void)
{
    uint8_t key = KEY_Scan();
    
    if (key != KEY_NONE)
    {
        switch (key)
        {
            case KEY_MODE:  // 模式切换: 自动 <-> 手动
                light_mode = !light_mode;
                break;
                
            case KEY_BRIGHT_UP:  // 增加亮度 (减小PWM值)
                if (light_mode == 1 && light_power)  // 只在手动模式和开灯状态下有效
                {
                    if (pwm_value > 10)
                        pwm_value -= 10;  // 大步减小PWM值，增加亮度
                    else
                        pwm_value = 0;    // 最小为0(最亮)
                }
                break;
                
            case KEY_BRIGHT_DOWN:  // 减少亮度 (增加PWM值)
                if (light_mode == 1 && light_power)  // 只在手动模式和开灯状态下有效
                {
                    if (pwm_value < 89)
                        pwm_value += 10;  // 大步增加PWM值，减少亮度
                    else
                        pwm_value = 99;   // 最大为99(最暗)
                }
                break;
                
            case KEY_POWER:  // 开关灯
                light_power = !light_power;
                key_control = 1;  // 设置按键控制标志
                break;
                
            default:
                break;
        }
    }
}

/**
  * @brief  TIM2中断处理函数
  * @param  无
  * @retval 无
  */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        uint16_t pwm;
        uint8_t human_present = BODY_HW_GetData();
        
        // 只有在非按键控制状态下，才由人体传感器控制开关
        if (!key_control)
        {
            if (!human_present)
            {
                Motor_SetSpeed(99);  // 无人时关灯
                light_power = 0;
                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
                return;
            }
            else
            {
                light_power = 1;  // 有人时开灯
            }
        }
        
        // 如果灯是开启状态，进行亮度调节
        if (light_power)
        {
            if (light_mode == 0)  // 自动模式: 根据光照值调节亮度
            {
                if (light < 10)
                {
                    pwm = 0;  // 环境非常暗时，灯最亮
                }
                else if (light > 800)
                {
                    pwm = 99;  // 环境很亮时，灯最暗
                }
                else
                {
                    pwm = (light - 10) * 99 / (800 - 10);
                }
            }
            else  // 手动模式: 使用按键设置的pwm_value
            {
                pwm = pwm_value;
            }
            
            Motor_SetSpeed(pwm);
        }
        else
        {
            Motor_SetSpeed(99);  // 关灯状态，设置为最暗
        }
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
