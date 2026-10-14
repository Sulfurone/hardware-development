#include "delay.h"
#include "timer.h"
#include "led.h"
#include "string.h"
#include "gizwits_product.h"
#include "stm32f10x.h"
#include "Light.h"
#include "KEY.h"
#include "OLED.h"
#include "BODY_HW.h"  // ??????????

volatile uint16_t pwm_value = 50;     // ??PWM?,???50
volatile uint8_t pwm_dir = 1;         // PWM????: 1??,0??
volatile uint8_t light_mode = 0;      // ????: 0=??, 1=??
volatile uint8_t light_power = 1;     // ??????: 0=?, 1=?
volatile uint8_t key_control = 0;     // ??????: 0=???????, 1=????
extern int keyflag;
extern u16 light;
/**
  * @brief  定时器3初始化函数
  * @param  arr: 自动重装载值
  * @param  psc: 预分频系数
  * @retval 无
  * @note   用于机智云平台通信的1ms定时器
  */
void TIM3_Int_Init(u16 arr,u16 psc)
{
	
		NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		// 开启定时器时钟,内部时钟CK_INT=72M
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
		// 自动重装载寄存器的值，累加TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period=arr;
	  // 时钟预分频数
    TIM_TimeBaseStructure.TIM_Prescaler= psc;	
		// 时钟分频因子 ，没用到不用管
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
		// 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
		// 重复计数器的值，没用到不用管
		TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	  // 初始化定时器
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
		// 清除计数器中断标志位
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	  
		// 开启计数器中断
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		
		// 使能计数器
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  定时器3中断处理函数
  * @param  无
  * @retval 无
  * @note   用于机智云平台的通信定时
  */
void TIM3_IRQHandler(void)   //TIM3中断，20ms中断
{
		if ( TIM_GetITStatus( TIM3, TIM_IT_Update) != RESET ) 
		{	
				TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);  	
				gizTimerMs();
		}		 	
}

/**
  * @brief  定时器2初始化函数
  * @param  无
  * @retval 无
  * @note   配置TIM2产生100ms的定时中断用于控制灯光模式和亮度
  */
void Timer2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_Period = 999;       // ARR值:999,计数器计到999时产生更新事件
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;   // 预分频:72MHz/7200=10kHz,10kHz/1000=10Hz,即100ms触发一次
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	// 启用TIM2更新中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	// 配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// 启动TIM2
	TIM_Cmd(TIM2, ENABLE);
    
    // 初始化按键
    KEY_Init();
    
    // 设置默认的灯光模式和电源状态
    light_mode = 0;
    light_power = 1;
}

/**
  * @brief  按键处理函数,用于处理各种按键功能
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
                if (light_mode == 1 && light_power)  // 仅在手动模式且灯光开启时有效
                {
                    if (pwm_value > 10)
                        pwm_value -= 10;  // 减小PWM值，增加亮度
                    else
                        pwm_value = 0;    // 最小为0(最亮)
                }
                break;
                
            case KEY_BRIGHT_DOWN:  // 降低亮度 (增加PWM值)
                if (light_mode == 1 && light_power)  // 仅在手动模式且灯光开启时有效
                {
                    if (pwm_value < 89)
                        pwm_value += 10;  // 增加PWM值，降低亮度
                    else
                        pwm_value = 99;   // 最大为99(最暗)
                }
                break;
                
            case KEY_POWER:  // 电源开关
                light_power = !light_power;
                key_control = 1;  // 标记为按键控制
						keyflag=1;
                break;
                
            default:
                break;
        }
    }
}

/**
  * @brief  TIM2中断处理函数,控制灯光模式和亮度
  * @param  无
  * @retval 无
  * @note   根据人体存在状态和光照强度自动调节灯光亮度
  */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        uint16_t pwm;
        uint8_t human_present = BODY_HW_GetData();
        
        // 在自动模式下，根据人体存在状态控制灯光
        if (light_mode == 0)  // 自动模式
        {
            if (human_present)  // 有人存在
            {
                light_power = 1;  // 打开灯光
							LED_ON();
            }
            else  // 无人存在
            {
                light_power = 0;  // 关闭灯光
              LED_OFF();
                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
                return;
            }
        }
        else  // 手动模式
        {
            // 保持现有的开关状态，由按键控制
            if (key_control && !keyflag)
            {
                // 如果是通过其他方式（不是按键）修改的light_power，不做处理
            }
        }
        
        // 根据灯光电源状态，决定输出亮度
        if (light_power)
        {
            if (light_mode == 0)  // 自动模式: 根据光强自动调节亮度
            {
                if (light < 10)
                {
                    pwm = 0;  // 光线很暗时，全亮
                }
                else if (light > 800)
                {
                    pwm = 99;  // 光线很强时，全暗
                }
                else
                {
                    pwm = (light - 10) * 99 / (800 - 10);
                }
            }
            else  // 手动模式: 使用按键设定的pwm_value
            {
                pwm = pwm_value;
            }
            
            LIGHT(pwm);
        }
        else
        {
            LIGHT(99);  // 灯光关闭，输出全暗亮度
        }
				
				if(keyflag == 1)
				{
					keyflag = 0;
				}
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

 
