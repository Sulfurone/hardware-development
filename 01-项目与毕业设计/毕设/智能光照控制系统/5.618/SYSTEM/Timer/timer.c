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
#include "light_mode.h"  // 包含灯光模式定义

volatile uint16_t pwm_value = 50;     // PWM值，默认50
volatile uint8_t pwm_dir = 1;         // PWM变化方向: 1增加,0减少
volatile LightMode_t light_mode = MODE_AUTO;  // 控制模式: 0=自动, 1=手动, 2=在线
volatile uint8_t light_power = 1;     // 灯光电源状态: 0=关, 1=开
volatile uint8_t key_control = 0;     // 按键控制标志: 0=非按键控制, 1=按键控制
volatile uint8_t online_control = 0;  // 在线控制标志: 0=非在线控制, 1=在线控制

extern int keyflag;
extern u16 light2;
extern dataPoint_t currentDataPoint;  // 添加对currentDataPoint的引用
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
    light_mode = MODE_AUTO;
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
        keyflag = 1;  // 标记按键已按下
        
        switch (key)
        {
            case KEY_MODE:  // 模式切换: 自动 -> 手动 -> 在线 -> 自动
                light_mode = (LightMode_t)((light_mode + 1) % 3);
                
                // 切换到在线模式时，允许云平台控制
                if (light_mode == MODE_ONLINE)
                {
                    online_control = 1;  // 设置在线控制标志
                }
                else
                {
                    online_control = 0;  // 清除在线控制标志，不接受云平台控制
                }
                break;
                
            case KEY_BRIGHT_UP:  // 增加亮度 (减小PWM值)
                // 如果当前是在线模式，切换到手动模式
                if (light_mode == MODE_ONLINE)
                {
                    light_mode = MODE_MANUAL;
                    online_control = 0;  // 清除在线控制标志
                }
                
                // 仅在手动模式且灯光开启时有效
                if (light_mode == MODE_MANUAL && light_power)
                {
                    if (pwm_value > 10)
                        pwm_value -= 10;  // 减小PWM值，增加亮度
                    else
                        pwm_value = 0;    // 最小为0(最亮)
                }
                break;
                
            case KEY_BRIGHT_DOWN:  // 降低亮度 (增加PWM值)
                // 如果当前是在线模式，切换到手动模式
                if (light_mode == MODE_ONLINE)
                {
                    light_mode = MODE_MANUAL;
                    online_control = 0;  // 清除在线控制标志
                }
                
                // 仅在手动模式且灯光开启时有效
                if (light_mode == MODE_MANUAL && light_power)
                {
                    if (pwm_value < 89)
                        pwm_value += 10;  // 增加PWM值，降低亮度
                    else
                        pwm_value = 99;   // 最大为99(最暗)
                }
                break;
                
            case KEY_POWER:  // 电源开关
                // 如果当前是在线模式，切换到手动模式
                if (light_mode == MODE_ONLINE)
                {
                    light_mode = MODE_MANUAL;
                    online_control = 0;  // 清除在线控制标志
                }
                
                // 切换灯光状态
                light_power = !light_power;
                key_control = 1;  // 标记为按键控制
                
                // 同步灯光状态到云平台
                currentDataPoint.valuelight = light_power;
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
  * @note   根据当前控制模式执行不同的灯光控制逻辑
  */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        uint16_t pwm;
        uint8_t human_present = (BODY_HW_GetData() == 0) ? 1 : 0;  // 修改为检测低电平
        
        // 根据不同的控制模式执行相应的控制逻辑
        switch (light_mode)
        {
            case MODE_AUTO:  // 自动模式：根据人体存在状态和光照强度自动控制
                if (human_present)  // 有人存在
                {
                    light_power = 1;  // 打开灯光
                    LED_ON();
                    
                    // 根据光强自动调节亮度
                    if (light2 < 10)
                    {
                        pwm = 0;  // 光线很暗时，全亮
                    }
                    else if (light2 > 800)
                    {
                        pwm = 99;  // 光线很强时，全暗
                    }
                    else
                    {
                        pwm = (light2 - 10) * 99 / (800 - 10);
                    }
                    
                    LIGHT(pwm);
                }
                else  // 无人存在
                {
                    light_power = 0;  // 关闭灯光
                    LED_OFF();
                    LIGHT(99);  // 灯光关闭，输出全暗亮度
                }
                // 在自动模式下，更新状态到云平台但不接受云平台控制
                online_control = 0;
                break;
                
            case MODE_MANUAL:  // 手动模式：用户通过按键控制
                // 根据灯光电源状态，决定输出亮度
                if (light_power)
                {
                    // 使用按键设定的pwm_value
                    LIGHT(pwm_value);
                }
                else
                {
                    LIGHT(99);  // 灯光关闭，输出全暗亮度
                }
                // 在手动模式下，更新状态到云平台但不接受云平台控制
                online_control = 0;
                break;
                
            case MODE_ONLINE:  // 在线模式：通过云平台控制
                // 在线模式下，接受云平台控制
                online_control = 1;
                
                // 根据云平台下发的灯光状态控制
                if (light_power)
                {
                    // 在线模式下，使用固定亮度值或根据云平台指令设置亮度
                    // 这里简单使用50%亮度
                    LIGHT(50);
                }
                else
                {
                    LIGHT(99);  // 灯光关闭，输出全暗亮度
                }
                break;
                
            default:
                break;
        }
        
        // 复位按键控制标志
        if (keyflag == 1)
        {
            keyflag = 0;
        }
        
        // 同步灯光状态到云平台
        currentDataPoint.valuelight = light_power;
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

 
