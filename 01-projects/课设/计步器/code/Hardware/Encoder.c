#include "Encoder.h"

// 编码器计数变量
static volatile uint32_t encoder1_count = 0;
static volatile uint32_t encoder1_speed = 0;
static volatile uint32_t encoder1_last_count = 0;

/**
 * @brief  编码器初始化
 * @param  无
 * @retval 无
 */
void Encoder_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 只使用左编码器，开启GPIOA、AFIO和TIM4时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置编码器1引脚（左轮 PA7）
    GPIO_InitStructure.GPIO_Pin = ENCODER1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ENCODER1_PORT, &GPIO_InitStructure);
    
    // 配置编码器1外部中断线（左轮 PA7）
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  // 仅上升沿触发，适合方波脉冲计数
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 配置TIM4用于速度计算
    TIM_TimeBaseStructure.TIM_Period = 999;  // 100ms中断 (1000 / 10kHz = 100ms)
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;  // 10kHz计数频率 (72MHz / 7200 = 10kHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 配置TIM4中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启用TIM4更新中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    // 启动TIM4
    TIM_Cmd(TIM4, ENABLE);
    
    // 初始化计数
    encoder1_count = 0;
    encoder1_speed = 0;
    encoder1_last_count = 0;
}

/**
 * @brief  获取编码器1计数值
 * @param  无
 * @retval 编码器计数值
 */
uint32_t Encoder1_GetCount(void) {
    return encoder1_count;
}

/**
 * @brief  获取电机1速度（cm/s）
 * @param  无
 * @retval 电机速度（cm/s）
 */
float Encoder1_GetSpeed(void) {
    return (float)encoder1_speed / 10.0f;
}

/**
 * @brief  获取编码器累计里程（cm）
 * @param  无
 * @retval 累计里程（cm）
 */
float Encoder1_GetDistanceCm(void) {
    return ((float)encoder1_count / ENCODER_PPR) * (ENCODER_CIRCUMFERENCE / 10.0f);
}

/**
 * @brief  外部中断9-5处理函数
 * @param  无
 * @retval 无
 */
void EXTI9_5_IRQHandler(void) {
    // 检查编码器1中断（左轮 PA7 - EXTI_Line7）
    if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
        encoder1_count++;
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}

/**
 * @brief  TIM4中断处理函数
 * @param  无
 * @retval 无
 */
void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        // 计算左轮速度
        uint32_t count_diff = encoder1_count - encoder1_last_count;
        // 速度 = (脉冲差 / PPR) * 周长 * 1000 / 采样时间 / 10
        encoder1_speed = (uint32_t)(((float)count_diff / ENCODER_PPR) * ENCODER_CIRCUMFERENCE * 1000.0f / ENCODER_SAMPLE_TIME * 10.0f);
        encoder1_last_count = encoder1_count;

        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
