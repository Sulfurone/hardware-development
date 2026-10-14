#include "delay.h"
#include "timer.h"
#include "led.h"
#include "string.h"
#include "gizwits_product.h"
#include "stm32f10x.h"
#include "Light.h"
#include "KEY.h"
#include "OLED.h"
#include "BODY_HW.h"  // 人体感应模块头文件
#include "gizwits_protocol.h"
#include "gizwits_product.h"
volatile uint16_t pwm_value = 50;     // PWM值，控制灯光亮度，值越小越亮，初始值50
volatile uint8_t pwm_dir = 1;         // PWM方向控制: 1递增,0递减
volatile uint8_t light_mode = 0;      // 灯光模式: 0=自动, 1=手动, 2=在线
volatile uint8_t light_power = 1;     // 灯光电源状态: 0=关, 1=开

// 控制标志位
volatile uint8_t key_control = 0;     // 按键控制标志: 0=非按键控制, 1=按键控制  
volatile uint8_t auto_control = 0;    // 自动控制标志: 0=非自动控制, 1=自动控制  
volatile uint8_t online_control = 0;  // 在线控制标志: 0=非在线控制, 1=在线控制
volatile uint16_t auto_pwm = 50;      // 自动模式下计算的PWM值，用于在OLED上显示

// 控制计时器，用于自动重置控制标志
volatile uint16_t key_control_timer = 0;      // 按键控制计时器
volatile uint16_t online_control_timer = 0;   // 在线控制计时器
#define KEY_CONTROL_TIMEOUT 50  // 50个定时器周期后重置按键控制标志（约5秒，因为Timer2是100ms触发一次）
#define ONLINE_CONTROL_TIMEOUT 300  // 300个定时器周期后重置在线控制标志（约30秒）

// 在线模式参数
volatile uint8_t online_mode = 0;     // 在线模式下的灯光模式
volatile uint8_t online_power = 1;    // 在线模式下的电源状态
volatile uint8_t online_brightness = 50; // 在线模式下的亮度值(0-100)

extern u16 light;  // 外部声明的光照强度变量
/**
  * @brief  定时器3初始化函数
  * @param  arr: 自动重装载值
  * @param  psc: 预分频系数
  * @retval 无
  * @note   用于机智云平台通信的1ms定时器
  */
void TIM3_Int_Init(u16 arr, u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    
    // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    
    // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 开启定时器时钟,内部时钟CK_INT=72M
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // 自动重装载寄存器的值，累加TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = arr;
    
    // 时钟预分频数
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    
    // 时钟分频因子 ，没用到不用管
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    // 初始化定时器
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 清除计数器中断标志位
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    
    // 开启计数器中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // 使能计数器
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  定时器3中断处理函数
  * @param  无
  * @retval 无
  * @note   用于机智云平台的通信定时
  */
void TIM3_IRQHandler(void)   //TIM3中断，1ms中断
{
		if ( TIM_GetITStatus( TIM3, TIM_IT_Update) != RESET ) 
		{	
				TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);  	// 清除中断标志位
				gizTimerMs();  // 调用机智云平台的定时处理函数
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
	// 使能TIM2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// 配置TIM2基本参数
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_Period = 999;       // ARR值:999,计数器计到999时产生更新事件
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;   // 预分频:72MHz/7200=10kHz,10kHz/1000=10Hz,即100ms触发一次
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频因子
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	// 启用TIM2更新中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	// 配置NVIC中断控制器
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // TIM2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // 子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能中断通道
	NVIC_Init(&NVIC_InitStructure);

	// 启动TIM2
	TIM_Cmd(TIM2, ENABLE);
    
    // 初始化按键
    KEY_Init();
    
    // 设置默认的灯光模式和电源状态
    light_mode = 0;     // 0=自动模式
    light_power = 1;    // 1=灯光开启
    
    // 设置初始控制标志
    key_control = 0;    // 0=非按键控制
    auto_control = 1;   // 1=自动控制
    online_control = 0; // 0=非在线控制
    
    // 初始化为中等亮度
    pwm_value = 50;     // 设置初始PWM值为50
}

/**
  * @brief  按键处理函数,用于处理各种按键功能
  * @param  无
  * @retval 无
  */
void KEY_Process(void)
{
    // 扫描按键状态
    uint8_t key = KEY_Scan();
    
    if (key != KEY_NONE)  // 如果有按键按下
    {
        key_control_timer = 0;  // 重置按键控制计时器
        
        switch (key)
        {
            case KEY_MODE:  // 模式切换按键: 自动 -> 手动 -> 在线 -> 自动
                light_mode = (light_mode + 1) % 3;  // 循环切换三种模式
                // 设置控制标志
                key_control = 1;      // 短暂激活按键控制，让用户看到切换效果
                
                // 在切换到自动模式时，根据当前光照强度调整灯光亮度
                if (light_mode == 0) {  // 自动模式
                   
                    if (light < 10) {  // 环境光照很弱
                        pwm_value = 0;  // 设置为最亮
                    } else if (light > 800) {  // 环境光照很强
                        pwm_value = 99;  // 设置为最暗或关闭
                    } else {  // 环境光照适中，线性调节
                        // 线性映射：光照从10-800映射到PWM值0-99
                        pwm_value = (light - 10) * 99 / (800 - 10);
                    }
                    
                    // 保存自动计算的PWM值
                    auto_pwm = pwm_value;
                    
                    Motor_SetSpeed(pwm_value);  // 设置PWM输出
                }
                else if (light_mode == 2) {  // 在线模式
                    // 进入在线模式
                    online_control = 1;  // 激活在线控制标志
                    online_control_timer = 0;  // 重置在线控制计时器
                    
                    // 确保灯光开启
                    light_power = 1;
                    
                    // 使用当前亮度值
                    Motor_SetSpeed(pwm_value);
                    
                    // 设置LED指示灯
                    if (pwm_value < 50)  // PWM值小于50，灯光较亮
                        LED_ON();  // 打开LED指示灯
                    else
                        LED_OFF();  // 关闭LED指示灯
                    
                    printf("按键切换到在线模式\r\n");  // 添加调试输出
                }
                break;
                
            case KEY_BRIGHT_UP:  // 增加亮度按键 (减小PWM值)
                if ((light_mode == 1) && light_power)  // 仅在手动模式且灯光开启时有效
                {
                    if (pwm_value > 10)
                        pwm_value -= 10;  // 减小PWM值，增加亮度
                    else
                        pwm_value = 0;    // 最小为0(最亮)
                    
                    // 设置控制标志
                    key_control = 1;      // 激活按键控制
                    
                    // 直接调用Motor_SetSpeed设置亮度
                    Motor_SetSpeed(pwm_value);
                }
                break;
                
            case KEY_BRIGHT_DOWN:  // 降低亮度按键 (增加PWM值)
                if (light_mode == 1)  // 仅在手动模式时有效
                {
                    if (pwm_value < 89)
                        pwm_value += 10;  // 增加PWM值，降低亮度
                    else
                        pwm_value = 99;   // 最大为99(最暗)
                    
                    // 设置控制标志
                    key_control = 1;      // 激活按键控制
                    
                    // 直接调用Motor_SetSpeed设置亮度
                    Motor_SetSpeed(pwm_value);
                }
                break;
                
            case KEY_POWER:  // 电源开关按键
                light_power = !light_power;  // 切换电源状态
                
                // 设置控制标志
                key_control = 1;      // 激活按键控制
                
                // 根据电源状态控制灯光
                if (light_power)  // 如果开灯
                {
                    Motor_SetSpeed(pwm_value);  // 使用当前亮度值
                }
                else  // 如果关灯
                {
                    Motor_SetSpeed(99);  // 关灯，设置为最暗
                }
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
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)  // 检查是否发生TIM2更新中断
    {
        uint16_t pwm;  // 临时PWM值
        uint8_t human_present = BODY_HW_GetData();  // 获取人体存在状态
        
        // 按键控制超时处理
        if (key_control)  // 如果按键控制标志激活
        {
            key_control_timer++;  // 增加按键控制计时器
            if (key_control_timer >= KEY_CONTROL_TIMEOUT)  // 如果超时
            {
                key_control = 0;      // 重置按键控制标志
                key_control_timer = 0; // 重置计时器
            }
        }
        
        // 在线控制超时处理 - 只有在非按键控制时才处理超时
        if (online_control && !key_control)  // 如果在线控制标志激活且非按键控制
        {
            online_control_timer++;  // 增加在线控制计时器
            if (online_control_timer >= ONLINE_CONTROL_TIMEOUT)  // 如果超时
            {
                // 如果当前是在线模式但在线控制超时，则自动切换回自动模式
                if (light_mode == 2)
                {
                    light_mode = 0;  // 切换到自动模式
                }
                
                online_control = 0;      // 重置在线控制标志
                online_control_timer = 0; // 重置计时器
            }
        }
        
        // 在线模式处理 - 确保在线模式不被其他逻辑覆盖
        if (light_mode == 2)  // 如果当前是在线模式
        {
            // 重置控制标志
            key_control = 0;  // 禁用按键控制
            key_control_timer = 0;  // 重置按键控制计时器
            
            // 确保在线控制标志激活
            if (!online_control)
            {
                online_control = 1;  // 激活在线控制
                online_control_timer = 0;  // 重置在线控制计时器
                printf("进入在线模式\r\n");  // 添加调试输出
            }
            
            // 在线模式下，禁止自动控制
            auto_control = 0;  // 禁用自动控制
        }
        else
        {
            // 默认使用自动控制，除非有其他控制方式
            auto_control = (!key_control && !online_control) ? 1 : 0;
        }
        
        // 根据人体存在情况自动控制灯光，仅在自动模式且自动控制激活时生效
        if (auto_control && (light_mode == 0))  // 如果自动控制激活且当前是自动模式
        {
            if (!human_present)  // 如果没有人
            {
                Motor_SetSpeed(99);  // 关灯
                LED_OFF();  // 关闭LED指示灯
                light_power = 0;  // 设置电源状态为关
                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除中断标志位
                return;  // 提前返回
            }
            else  // 如果有人
            {
                light_power = 1;  // 开灯
            }
        }
        
        // 根据灯光电源状态，决定输出亮度
        if (light_power)  // 如果灯光开启
        {
            // 自动模式时，不管是按键还是自动控制都根据光照强度调节
            if (light_mode == 0)  // 自动模式
            {
                // 根据光照强度调节亮度
                // 环境光照越强，灯光越弱（PWM值越大）
                // 环境光照越弱，灯光越强（PWM值越小）
                
                if (light < 10)  // 环境光照很弱
                {
                    pwm = 0;  // 设置为最亮
                }
                else if (light > 800)  // 环境光照很强
                {
                    pwm = 99;  // 设置为最暗或关闭
                }
                else  // 环境光照适中，线性调节
                {
                    // 线性映射：光照从10-800映射到PWM值0-99
                    pwm = (light - 10) * 99 / (800 - 10);
                }
                
                // 保存自动计算的PWM值
                auto_pwm = pwm;
                
                // 直接使用PWM控制灯光亮度
                Motor_SetSpeed(pwm);
                
                // 根据计算出的PWM值控制LED指示灯
                if (pwm < 50) {  // PWM值小于50，灯光较亮
                    LED_ON();  // 打开LED指示灯
                } else {
                    LED_OFF();  // 关闭LED指示灯
                }
            }
            else if (light_mode == 1)  // 手动模式
            {
                // 使用按键设定的pwm_value
                Motor_SetSpeed(pwm_value);
                
                // 设置LED指示灯
                if (pwm_value < 50)  // PWM值小于50，灯光较亮
                    LED_ON();  // 打开LED指示灯
                else
                    LED_OFF();  // 关闭LED指示灯
            }
            else if (light_mode == 2)  // 在线模式
            {
                // 使用机智云平台设定的pwm_value
                Motor_SetSpeed(pwm_value);
                
                // 设置LED指示灯
                if (pwm_value < 50)  // PWM值小于50，灯光较亮
                    LED_ON();  // 打开LED指示灯
                else
                    LED_OFF();  // 关闭LED指示灯
            }
        }
        else  // 灯光关闭状态
        {
            Motor_SetSpeed(99);  // 设置为最暗
            LED_OFF();  // 关闭LED指示灯
        }
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除中断标志位
    }
}

 
