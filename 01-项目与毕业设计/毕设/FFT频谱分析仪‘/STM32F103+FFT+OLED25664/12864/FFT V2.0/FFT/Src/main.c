#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32_dsp.h"
#include "table_fft.h"
#include "math.h"
#include "oled.h"
#include "config.h"
#include "bg.h"

#define NPT 256 // FFT采样点数
#define PI2 6.28318530717959 // 2*PI常量
#define Fs 9984 // 采样率，分辨率39Hz

uint32_t adc_buf[NPT] = {0}; // ADC采样缓冲区
long lBufInArray[NPT];       // FFT输入缓冲
long lBufOutArray[NPT/2];    // FFT输出缓冲
long lBufMagArray[NPT/2];    // FFT幅值缓冲

uint8_t prt = 10; // 显示比例，控制柱状高度
#define SHOW_NUM 4 // 显示模式数量
uint8_t display_num = 1; // 当前显示模式
uint8_t auto_display_flag = 0; // 自动切换显示标志

// 函数声明
void SystemClock_Config(void);
void Error_Handler(void);
void Creat_Single(void);
void GetPowerMag(void);
void display1(void);
void display2(void);
void display3(void);
void display4(void);
void Key_Scan(void);

int main(void)
{
    uint16_t i = 0;
    HAL_Init(); // HAL库初始化
    SystemClock_Config(); // 配置系统时钟
    MX_GPIO_Init(); // 初始化GPIO
    MX_DMA_Init(); // 初始化DMA
    MX_USART1_UART_Init(); // 初始化串口1
    MX_ADC1_Init(); // 初始化ADC1
    MX_TIM3_Init(); // 初始化定时器3

    printf("uart test! \r\n");
    // OLED初始化及显示开机画面
    GUI_Initialize();
    GUI_SetColor(1,0);
    GUI_LoadPic(0,0,(uint8_t *)&gImage_bg,128,64);
    GUI_Exec();
    HAL_Delay(3000);
    // 启动ADC DMA采集
    HAL_ADC_Start_DMA(&hadc1, adc_buf, NPT);
    // 启动定时器，定时触发ADC
    HAL_TIM_Base_Start(&htim3);

    while (1)
    {
        Key_Scan(); // 按键扫描，切换显示模式
    }
}

// 系统时钟配置
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) Error_Handler();
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

// 生成测试信号（用于FFT算法验证）
// 生成一个叠加了三种不同频率正弦波的信号，模拟实际采样数据
void Creat_Single(void)
{
    u16 i = 0;
    float fx=0.0;
    for(i=0; i<NPT; i++)
    {
        // 叠加三种频率的正弦波，幅值不同
        fx = 2048+2048*sin(PI2 * i * 200.0 / Fs)+
             3100*sin(PI2 * i * 502.0 / Fs)+
             1300*sin(PI2 * i * 990.0 / Fs);
        // 转换为16位有符号数后左移16位，适配FFT库输入格式
        lBufInArray[i] = ((signed short)fx) << 16;
    }
}

// 计算FFT输出的幅值（模值）
// lBufOutArray为FFT输出的复数，低16位实部，高16位虚部
// 结果存入lBufMagArray
void GetPowerMag(void)
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
    for(i=0; i<NPT/2; i++)
    {
        // 取出实部和虚部
        lX  = (lBufOutArray[i] << 16) >> 16; // 低16位
        lY  = (lBufOutArray[i] >> 16);       // 高16位
        // 转换为浮点数，缩放
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        // 计算模值
        Mag = sqrt(X * X + Y * Y)*1.0/ NPT;
        // 直流分量和交流分量缩放不同
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}

// 柱状显示（32根柱，间隔采样）
// 带坐标轴、刻度、峰值显示
void display1(void)
{
    uint16_t i = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t max_index = 0; // 峰值频率索引
    unsigned long max_value = 0; // 峰值幅值
    char str[32];
    GUI_ClearSCR(); // 清屏
    // 画Y轴
    GUI_LineWith(0, 0, 0, 63, 1, 1);
    // 画X轴
    GUI_LineWith(0, 63, 127, 63, 1, 1);
    // 画Y轴刻度和数值
    for(int k=0; k<=64; k+=16) {
        GUI_LineWith(0, k, 3, k, 1, 1); // 刻度线
        sprintf(str, "%d", (63-k)*prt); // 显示幅值
        GUI_PutString(5, k-8 < 0 ? 0 : k-8, str);
    }
    // 画X轴刻度和频率
    for(int k=0; k<=128; k+=32) {
        GUI_LineWith(k, 63, k, 60, 1, 1); // 刻度线
        sprintf(str, "%d", (k*NPT/128)*Fs/NPT); // 显示频率
        GUI_PutString(k, 54, str);
    }
    // 查找峰值
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    // 显示峰值信息
    sprintf(str, "Peak: %dHz %lu", (int)(max_index*Fs/NPT), max_value);
    GUI_PutString(40, 0, str);
    // 画频谱柱状
    for(i = 0; i < 32; i++)
    {
        x = (i<<2);
        y = 63-(lBufMagArray[x+1]/prt)-2;
        if(y>63) y = 63;
        GUI_LineWith(x, y, x, 63, 3, 1);
    }
    GUI_Exec();
}

// 单柱状显示（128根细柱）
void display2(void)
{
    uint16_t i = 0;
    uint8_t y = 0;
    uint16_t max_index = 0;
    unsigned long max_value = 0;
    char str[32];
    GUI_ClearSCR();
    // 画Y轴
    GUI_LineWith(0, 0, 0, 63, 1, 1);
    // 画X轴
    GUI_LineWith(0, 63, 127, 63, 1, 1);
    for(int k=0; k<=64; k+=16) {
        GUI_LineWith(0, k, 3, k, 1, 1);
        sprintf(str, "%d", (63-k)*prt);
        GUI_PutString(5, k-8 < 0 ? 0 : k-8, str);
    }
    for(int k=0; k<=128; k+=32) {
        GUI_LineWith(k, 63, k, 60, 1, 1);
        sprintf(str, "%d", (k*NPT/128)*Fs/NPT);
        GUI_PutString(k, 54, str);
    }
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    sprintf(str, "Peak: %dHz %lu", (int)(max_index*Fs/NPT), max_value);
    GUI_PutString(40, 0, str);
    for(i = 1; i < 128; i++)
    {
        y = 63-(lBufMagArray[i]/prt)-2;
        if(y>63) y = 63;
        GUI_RLine(i, y, 63, 1);
    }
    GUI_Exec();
}

// 柱状显示（中间对称）
void display3(void)
{
    uint16_t i = 0;
    uint8_t y = 0;
    uint16_t max_index = 0;
    unsigned long max_value = 0;
    char str[32];
    GUI_ClearSCR();
    // 画Y轴
    GUI_LineWith(0, 0, 0, 63, 1, 1);
    // 画X轴
    GUI_LineWith(0, 63, 127, 63, 1, 1);
    for(int k=0; k<=64; k+=16) {
        GUI_LineWith(0, k, 3, k, 1, 1);
        sprintf(str, "%d", (63-k)*prt);
        GUI_PutString(5, k-8 < 0 ? 0 : k-8, str);
    }
    for(int k=0; k<=128; k+=32) {
        GUI_LineWith(k, 63, k, 60, 1, 1);
        sprintf(str, "%d", (k*NPT/128)*Fs/NPT);
        GUI_PutString(k, 54, str);
    }
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    sprintf(str, "Peak: %dHz %lu", (int)(max_index*Fs/NPT), max_value);
    GUI_PutString(40, 0, str);
    for(i = 0; i < 127; i++)
    {
        y = 31-(lBufMagArray[i+1]/prt)-2;
        if(y>31) y = 31;
        GUI_RLine(i, 32, y, 1);
        GUI_RLine(i, 32, 63-y, 1);
    }
    GUI_Exec();
}

// 单柱状显示（中间对称，32根）
void display4(void)
{
    uint16_t i = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t max_index = 0;
    unsigned long max_value = 0;
    char str[32];
    GUI_ClearSCR();
    // 画Y轴
    GUI_LineWith(0, 0, 0, 63, 1, 1);
    // 画X轴
    GUI_LineWith(0, 63, 127, 63, 1, 1);
    for(int k=0; k<=64; k+=16) {
        GUI_LineWith(0, k, 3, k, 1, 1);
        sprintf(str, "%d", (63-k)*prt);
        GUI_PutString(5, k-8 < 0 ? 0 : k-8, str);
    }
    for(int k=0; k<=128; k+=32) {
        GUI_LineWith(k, 63, k, 60, 1, 1);
        sprintf(str, "%d", (k*NPT/128)*Fs/NPT);
        GUI_PutString(k, 54, str);
    }
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    sprintf(str, "Peak: %dHz %lu", (int)(max_index*Fs/NPT), max_value);
    GUI_PutString(40, 0, str);
    for(i = 0; i < 32; i++)
    {
        x = (i<<2);
        y = 31-(lBufMagArray[x+1]/prt)-2;
        if(y>31) y = 31;
        GUI_LineWith(x, y, x, 32, 3, 1);
        GUI_LineWith(x, 63-y, x, 32, 3, 1);
    }
    GUI_Exec();
}

// ADC DMA传输完成回调（每采集NPT个点自动触发）
// 进行FFT变换、幅值计算、自动/手动切换显示
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    uint16_t i = 0;
    static uint16_t num = 0;
    HAL_ADC_Stop_DMA(&hadc1); // 停止DMA，防止数据冲突
    // 填充FFT输入数组，去除直流偏置
    for(i=0;i<NPT;i++)
        lBufInArray[i] = ((signed short)(adc_buf[i]-2048)) << 16;
    // 进行256点FFT
    cr4_fft_256_stm32(lBufOutArray, lBufInArray, NPT);
    GetPowerMag(); // 计算幅值
    // 自动切换显示模式
    if(auto_display_flag == 1)
    {
        if(num>300)
        {
            num = 0;
            display_num ++;
            if(display_num>SHOW_NUM) display_num = 1;
        }
    }
    num++;
    // 根据当前显示模式显示
    switch(display_num)
    {
        case 1:
            display1();
            break;
        case 2:
            display2();
            break;
        case 3:
            display3();
            break;
        case 4:
            display4();
            break;
        default:
            display3();
            break;
    }
    HAL_ADC_Start_DMA(&hadc1, adc_buf, NPT); // 重新启动DMA
}

#define K1 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)
#define K2 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)
#define K3 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)

// 按键扫描，切换显示模式和显示方式
// K1：自动/手动切换，K2：手动下一个，K3：手动上一个
void Key_Scan(void)
{
    static uint8_t mode_num = 0;
    if(K1 == RESET)
    {
        HAL_Delay(10);
        if(K1 == RESET)
        {
            while(!K1);
            mode_num=!mode_num;
            if(mode_num == 1) // 自动显示
            {
                auto_display_flag = 1;
                GUI_PutString(0,0,"Auto");
                GUI_Exec();
            }
            else // 手动显示
            {
                auto_display_flag = 0;
                GUI_PutString(0,0,"Manual");
                GUI_Exec();
            }
        }
    }
    if(K2 == RESET)
    {
        HAL_Delay(10);
        if(K2 == RESET)
        {
            while(!K2);
            if(mode_num == 0) // 手动模式下切换下一个显示
            {
                display_num ++;
                if(display_num > SHOW_NUM) display_num = 1;
            }
        }
    }
    if(K3 == RESET)
    {
        HAL_Delay(10);
        if(K3 == RESET)
        {
            while(!K3);
            if(mode_num == 0) // 手动模式下切换上一个显示
            {
                if(display_num == 1) display_num = SHOW_NUM+1;
                display_num --;
            }
        }
    }
}

// 错误处理函数
void Error_Handler(void)
{
    while(1) {}
}
