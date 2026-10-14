#include "AD9833.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "math.h"

// 引脚定义
#define AD9833_FSYNC_PORT    GPIOB
#define AD9833_FSYNC_PIN     GPIO_Pin_15

#define AD9833_SCK_PORT      GPIOB
#define AD9833_SCK_PIN       GPIO_Pin_14

#define AD9833_SDATA_PORT    GPIOB
#define AD9833_SDATA_PIN     GPIO_Pin_13

// 宏定义
#define FSYNC_HIGH()        GPIO_SetBits(AD9833_FSYNC_PORT, AD9833_FSYNC_PIN)
#define FSYNC_LOW()         GPIO_ResetBits(AD9833_FSYNC_PORT, AD9833_FSYNC_PIN)

#define SCK_HIGH()          GPIO_SetBits(AD9833_SCK_PORT, AD9833_SCK_PIN)
#define SCK_LOW()           GPIO_ResetBits(AD9833_SCK_PORT, AD9833_SCK_PIN)

#define SDATA_HIGH()        GPIO_SetBits(AD9833_SDATA_PORT, AD9833_SDATA_PIN)
#define SDATA_LOW()         GPIO_ResetBits(AD9833_SDATA_PORT, AD9833_SDATA_PIN)

// 内部函数声明
static void AD9833_GPIO_Init(void);
static void AD9833_Delay(uint16_t us);
static void AD9833_Write(uint16_t data);

// 当前配置
static double current_frequency = 0;
static uint16_t current_phase = 0;
static uint8_t current_waveform = SIN_WAVE;
static uint8_t output_enabled = 0;

/**
  * @brief  初始化AD9833
  * @param  None
  * @retval None
  */
void AD9833_Init(void)
{
    // 初始化GPIO
    AD9833_GPIO_Init();
    
    // 复位AD9833
    AD9833_Reset();
    
    // 默认设置
    AD9833_SetFrequency(1000.0, FREQ_REG0);  // 默认1kHz
    AD9833_SetPhase(0, 0);                  // 默认0相位
    AD9833_SetWaveform(SIN_WAVE);           // 默认正弦波
    AD9833_EnableOutput(1);                 // 使能输出
}

/**
  * @brief  复位AD9833
  * @param  None
  * @retval None
  */
void AD9833_Reset(void)
{
    AD9833_Write(0x0100);  // RESET=1
    AD9833_Delay(10);
    AD9833_Write(0x2000);  // 退出复位状态
    AD9833_Delay(10);
}

/**
  * @brief  设置输出频率
  * @param  frequency: 输出频率(Hz)
  * @param  freq_reg: 频率寄存器选择(0或1)
  * @retval None
  */
void AD9833_SetFrequency(double frequency, uint8_t freq_reg)
{
    uint32_t freq_word;
    uint16_t freq_lsb, freq_msb;
    
    // 限制频率范围(0-12.5MHz)
    if(frequency < 0) frequency = 0;
    if(frequency > 12500000) frequency = 12500000;
    
    current_frequency = frequency;
    
    // 计算频率字(28位)
    freq_word = (uint32_t)((frequency * 268435456.0) / 25000000.0);
    
    // 分离高低14位
    freq_lsb = freq_word & 0x3FFF;
    freq_msb = (freq_word >> 14) & 0x3FFF;
    
    // 设置频率寄存器地址位
    if(freq_reg == FREQ_REG0) {
        freq_lsb |= 0x4000;  // FREQ0寄存器低14位
        freq_msb |= 0x4000;  // FREQ0寄存器高14位
    } else {
        freq_lsb |= 0x8000;  // FREQ1寄存器低14位
        freq_msb |= 0x8000;  // FREQ1寄存器高14位
    }
    
    // 写入频率寄存器
    AD9833_Write(0x2100);  // B28=1, 允许28位写入
    AD9833_Write(freq_lsb);
    AD9833_Write(freq_msb);
    
    // 更新控制寄存器
    uint16_t ctrl_reg = 0x2000;  // 基本控制字
    if(freq_reg) ctrl_reg |= 0x0800;  // FSELECT=1
    
    switch(current_waveform) {
        case TRI_WAVE: ctrl_reg |= 0x0002; break;  // MODE=1
        case SQU_WAVE: ctrl_reg |= 0x0028; break;  // OPBITEN=1, DIV2=1
        case SIN_WAVE: break;  // 默认正弦波
    }
    
    if(!output_enabled) {
        ctrl_reg |= 0x0040;  // SLEEP12=1, DAC掉电
    }
    
    AD9833_Write(ctrl_reg);
}

/**
  * @brief  设置输出相位
  * @param  phase: 相位值(0-4095对应0-2π)
  * @param  phase_reg: 相位寄存器选择(0或1)
  * @retval None
  */
void AD9833_SetPhase(uint16_t phase, uint8_t phase_reg)
{
    uint16_t phase_word;
    
    // 限制相位范围
    phase = phase & 0x0FFF;
    current_phase = phase;
    
    // 设置相位寄存器地址位
    phase_word = phase;
    if(phase_reg == 0) {
        phase_word |= 0xC000;  // PHASE0寄存器
    } else {
        phase_word |= 0xE000;  // PHASE1寄存器
    }
    
    // 写入相位寄存器
    AD9833_Write(phase_word);
}

/**
  * @brief  设置输出波形类型
  * @param  wave_type: 波形类型(SIN_WAVE, TRI_WAVE, SQU_WAVE)
  * @retval None
  */
void AD9833_SetWaveform(uint8_t wave_type)
{
    uint16_t ctrl_reg = 0x2000;  // 基本控制字
    
    current_waveform = wave_type;
    
    // 设置波形模式位
    switch(wave_type) {
        case TRI_WAVE: 
            ctrl_reg |= 0x0002;  // MODE=1
            break;
        case SQU_WAVE: 
            ctrl_reg |= 0x0028;  // OPBITEN=1, DIV2=1
            break;
        case SIN_WAVE: 
            // 默认设置
            break;
    }
    
    if(!output_enabled) {
        ctrl_reg |= 0x0040;  // SLEEP12=1, DAC掉电
    }
    
    AD9833_Write(ctrl_reg);
}

/**
  * @brief  设置AD9833休眠模式
  * @param  sleep_mode: 休眠模式
  *         0 - 不休眠
  *         1 - DAC掉电
  *         2 - 内部时钟禁用
  *         3 - DAC掉电且内部时钟禁用
  * @retval None
  */
void AD9833_Sleep(uint8_t sleep_mode)
{
    uint16_t ctrl_reg = 0x2000;  // 基本控制字
    
    // 设置波形模式位
    switch(current_waveform) {
        case TRI_WAVE: ctrl_reg |= 0x0002; break;  // MODE=1
        case SQU_WAVE: ctrl_reg |= 0x0028; break;  // OPBITEN=1, DIV2=1
        case SIN_WAVE: break;  // 默认正弦波
    }
    
    // 设置休眠位
    switch(sleep_mode) {
        case 0: break;  // 不休眠
        case 1: ctrl_reg |= 0x0040; break;  // SLEEP12=1, DAC掉电
        case 2: ctrl_reg |= 0x0020; break;  // SLEEP1=1, 内部时钟禁用
        case 3: ctrl_reg |= 0x0060; break;  // SLEEP12=1, SLEEP1=1
    }
    
    AD9833_Write(ctrl_reg);
}

/**
  * @brief  启用/禁用输出
  * @param  enable: 1-启用, 0-禁用
  * @retval None
  */
void AD9833_EnableOutput(uint8_t enable)
{
    output_enabled = enable;
    
    if(enable) {
        // 重新应用当前波形设置以唤醒芯片
        AD9833_SetWaveform(current_waveform);
    } else {
        // 进入休眠模式(DAC掉电)
        AD9833_Sleep(1);
    }
}

/**
  * @brief  初始化AD9833 GPIO
  * @param  None
  * @retval None
  */
static void AD9833_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置FSYNC, SCK, SDATA为推挽输出
    GPIO_InitStructure.GPIO_Pin = AD9833_FSYNC_PIN | AD9833_SCK_PIN | AD9833_SDATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始状态
    FSYNC_HIGH();
    SCK_HIGH();
    SDATA_HIGH();
}

/**
  * @brief  简单延时函数
  * @param  us: 微秒数(近似)
  * @retval None
  */
static void AD9833_Delay(uint16_t us)
{
    uint32_t ticks = us * (SystemCoreClock / 1000000) / 5;
    while(ticks--);
}

/**
  * @brief  向AD9833写入16位数据
  * @param  data: 要写入的16位数据
  * @retval None
  */
static void AD9833_Write(uint16_t data)
{
    uint8_t i;
    
    // 确保SCK初始为高
    SCK_HIGH();
    FSYNC_HIGH();
    AD9833_Delay(1);
    
    // 开始传输(FSYNC拉低)
    FSYNC_LOW();
    AD9833_Delay(1);
    
    // 发送16位数据(MSB first)
    for(i = 0; i < 16; i++) {
        if(data & 0x8000) {
            SDATA_HIGH();
        } else {
            SDATA_LOW();
        }
        
        AD9833_Delay(1);
        SCK_LOW();      // 数据在SCK下降沿被采样
        AD9833_Delay(1);
        SCK_HIGH();
        
        data <<= 1;     // 移位准备下一位
    }
    
    // 结束传输(FSYNC拉高)
    AD9833_Delay(1);
    FSYNC_HIGH();
}