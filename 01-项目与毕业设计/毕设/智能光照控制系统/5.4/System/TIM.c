#include "stm32f10x.h"
#include "Light.h"
#include "KEY.h"
#include "OLED.h"
#include "Delay.h"

// 系统状态变量
volatile uint16_t pwm_value = 50;     // 当前PWM值，初始为50
volatile uint8_t pwm_dir = 1;         // PWM变化方向: 1递增，0递减
volatile uint8_t light_mode = 0;      // 灯光模式: 0=自动, 1=手动
volatile uint8_t light_power = 1;     // 灯光电源状态: 0=关, 1=开

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
        // 用于调试，显示按键值
        OLED_ShowNum(1, 14, key, 1);
        
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
        
        // 如果灯是关闭状态，直接设置为最暗
        if (!light_power)
        {
            Motor_SetSpeed(99);  // 99对应最暗
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            return;
        }
        
        if (light_mode == 0)  // 自动模式: 根据光照值调节亮度
        {
    		// 根据光照传感器的值计算PWM值
    		// light的取值范围是0-999，值越小表示环境光线越暗
    		if (light < 10)
    		{
    			// 环境非常暗时，灯最亮 (PWM = 0)
    			pwm = 0;
    		}
    		else if (light > 800)
    		{
    			// 环境很亮时，灯最暗 (PWM = 99)
    			pwm = 99;
    		}
    		else
    		{
    			// 线性映射：light从10到800映射到pwm从0到99
    			// 使用公式：pwm = (light - min_light) * 99 / (max_light - min_light)
    			pwm = (light - 10) * 99 / (800 - 10);
    		}
        }
        else  // 手动模式: 使用按键设置的pwm_value
        {
            pwm = pwm_value;
        }
		
		// 更新PWM占空比
		Motor_SetSpeed(pwm);
		
		// 清除中断标志位
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
