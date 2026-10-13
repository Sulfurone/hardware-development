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
#define PI2 6.28318530717959 // 2*PI常数
#define Fs 9900 // 采样率，分辨率约19.5Hz (原采样率的一半)

uint32_t adc_buf[NPT] = {0}; // ADC采样数据缓存
long lBufInArray[NPT];       // FFT输入缓冲
long lBufOutArray[NPT/2];    // FFT输出缓冲
long lBufMagArray[NPT/2];    // FFT幅值缓冲

uint8_t prt = 10; // 显示缩放比例，波形高度
#define SHOW_NUM 4 // 显示模式数量
uint8_t display_num = 1; // 当前显示模式
#define X_SCALE 25 // X轴显示刻度间隔

// 系统时钟配置
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
    HAL_Init(); // HAL初始化
    SystemClock_Config(); // 配置系统时钟
    MX_GPIO_Init(); // 初始化GPIO
    MX_DMA_Init(); // 初始化DMA
    MX_USART1_UART_Init(); // 初始化USART1
    MX_ADC1_Init(); // 初始化ADC1
    MX_TIM3_Init(); // 初始化TIM3

    printf("uart test! \r\n");
    // OLED初始化
    GUI_Initialize();
    GUI_SetColor(1,0);
    // 启动ADC DMA采集
    HAL_ADC_Start_DMA(&hadc1, adc_buf, NPT);
    // 启动TIM3，TIM3作为ADC的触发源
    HAL_TIM_Base_Start(&htim3);

    while (1)
    {
        Key_Scan(); // 扫描按键，切换显示模式
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

// 生成单频信号，用于FFT算法验证
// 输入参数：无
// 输出参数：无
void Creat_Single(void)
{
    u16 i = 0;
    float fx=0.0;
    for(i=0; i<NPT; i++)
    {
        // 生成不同频率的信号，频率不同，幅值不同
        fx = 2048+2048*sin(PI2 * i * 200.0 / Fs)+
             3100*sin(PI2 * i * 502.0 / Fs)+
             1300*sin(PI2 * i * 990.0 / Fs);
        // 转换为16位定点数格式，16位整数，16位小数
        lBufInArray[i] = ((signed short)fx) << 16;
    }
}

// 获取FFT输出数据的幅值，模值
// lBufOutArray为FFT输出缓冲，16位实部，16位虚部
// 输出参数：lBufMagArray
void GetPowerMag(void)
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
    for(i=0; i<NPT/2; i++)
    {
        // 取实部和虚部
        lX  = (lBufOutArray[i] << 16) >> 16; // 取低16位
        lY  = (lBufOutArray[i] >> 16);       // 取高16位
        // 转换为浮点数
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        // 计算模值
        Mag = sqrt(X * X + Y * Y)*1.0/ NPT;
        // 直接转换为不同范围的值
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}

// 条状显示（32个柱状图）
// 坐标轴、刻度、幅值显示
void display1(void)
{
    uint16_t i = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t max_index = 0; // 最大值频率索引
    unsigned long max_value = 0; // 最大值幅值
    char str[32];
    GUI_ClearSCR(); // 清屏
    // 画Y轴
    GUI_LineWith(0, 8, 0, 52, 1, 1);
    // 画X轴
    GUI_LineWith(0, 52, 127, 52, 1, 1);
    // 画Y轴刻度和幅值
    for(int k=8; k<=52; k+=11) {
        GUI_LineWith(0, k, 3, k, 1, 1); // 刻度线
        sprintf(str, "%d", (52-k)*prt); // 显示幅值
        GUI_PutString(5, k-8 < 8 ? 8 : k-8, str);
    }
    // 画X轴刻度和频率
    for(int k=0; k<=128; k+=X_SCALE) {
        GUI_LineWith(k, 52, k, 55, 1, 1); // 刻度线向下延伸，便于观察
        int freq = k*Fs/256; // 已经是正确的公式
        // 直接转换为kHz整数
        int freq_k = freq / 1000; 
        
        // 调整标记位置
        if (k == 0) {
            // 0k放在原点右侧一点，避免超出显示区域
            sprintf(str, "0k");
            GUI_PutString(2, 56, str);
        } else if (freq_k >= 0) { // 只显示大于0的频率值
            sprintf(str, "%dk", freq_k+1);
            // 其他刻度居中显示
            int text_width = 12; // 估计文本宽度
            GUI_PutString(k-(text_width/2), 56, str);
        }
    }
    // 查找最值
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    // 显示最值信息
    int peak_freq = (int)(max_index*Fs/NPT); // 恢复原始峰值频率计算
   
        sprintf(str, "Peak: %dHz %lu", peak_freq, max_value);
    GUI_PutString(40, 0, str);
    // 画频谱柱状
    for(i = 0; i < 32; i++)
    {
        x = (i<<2);
        y = 52-(lBufMagArray[x+1]/prt)-2;
        if(y<8) y = 8;
        if(y>52) y = 52;
        GUI_LineWith(x, y, x, 52, 3, 1);
    }
    GUI_Exec();
}

// 线状显示（128点细线）
void display2(void)
{
    uint16_t i = 0;
    uint8_t y = 0;
    uint16_t max_index = 0;
    unsigned long max_value = 0;
    char str[32];
    GUI_ClearSCR();
    // 画Y轴
    GUI_LineWith(0, 8, 0, 52, 1, 1);
    // 画X轴
    GUI_LineWith(0, 52, 127, 52, 1, 1);
    for(int k=8; k<=52; k+=11) {
        GUI_LineWith(0, k, 3, k, 1, 1);
        sprintf(str, "%d", (52-k)*prt);
        GUI_PutString(5, k-8 < 8 ? 8 : k-8, str);
    }
    for(int k=0; k<=128; k+=X_SCALE) {
        GUI_LineWith(k, 52, k, 55, 1, 1);
        int freq = k*Fs/256; // 修改为统一的公式
        // 直接转换为kHz整数
        int freq_k = freq / 1000;
        
        // 调整标记位置
        if (k == 0) {
            // 0k放在原点右侧一点，避免超出显示区域
            sprintf(str, "0k");
            GUI_PutString(2, 56, str);
        } else if (freq_k >= 0) { // 只显示大于0的频率值
            sprintf(str, "%dk", freq_k+1);
            // 其他刻度居中显示
            int text_width = 12; // 估计文本宽度
            GUI_PutString(k-(text_width/2), 56, str);
        }
    }
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    int peak_freq = (int)(max_index*Fs/NPT); // 恢复原始峰值频率计算

        sprintf(str, "Peak: %dHz %lu", peak_freq, max_value);
    GUI_PutString(40, 0, str);
    for(i = 1; i < 128; i++)
    {
        y = 52-(lBufMagArray[i]/prt)-2;
        if(y<8) y = 8;
        if(y>52) y = 52;
        GUI_RLine(i, y, 52, 1);
    }
    GUI_Exec();
}

// 条状显示（中线对称）
void display3(void)
{
    uint16_t i = 0;
    uint8_t y = 0;
    uint16_t max_index = 0;
    unsigned long max_value = 0;
    char str[32];
    GUI_ClearSCR();
    // 画Y轴
    GUI_LineWith(0, 8, 0, 52, 1, 1);
    // 画X轴
    GUI_LineWith(0, 52, 127, 52, 1, 1);
    for(int k=8; k<=52; k+=11) {
        GUI_LineWith(0, k, 3, k, 1, 1);
        sprintf(str, "%d", (52-k)*prt);
        GUI_PutString(5, k-8 < 8 ? 8 : k-8, str);
    }
    for(int k=0; k<=128; k+=X_SCALE) {
        GUI_LineWith(k, 52, k, 55, 1, 1);
        int freq = k*Fs/256; // 修改为统一的公式
        // 直接转换为kHz整数
        int freq_k = freq / 1000;
        
        // 调整标记位置
        if (k == 0) {
            // 0k放在原点右侧一点，避免超出显示区域
            sprintf(str, "0k");
            GUI_PutString(2, 56, str);
        } else if (freq_k > 0) { // 只显示大于0的频率值
            sprintf(str, "%dk", freq_k);
            // 其他刻度居中显示
            int text_width = 12; // 估计文本宽度
            GUI_PutString(k-(text_width/2), 56, str);
        }
    }
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    int peak_freq = (int)(max_index*Fs/NPT); // 恢复原始峰值频率计算
    if(peak_freq >= 1000)
        sprintf(str, "Peak: %dkHz %lu", peak_freq/1000, max_value);
    else
        sprintf(str, "Peak: %dHz %lu", peak_freq, max_value);
    GUI_PutString(40, 0, str);
    for(i = 0; i < 127; i++)
    {
        y = 30-(lBufMagArray[i+1]/prt)-2;
        if(y<8) y = 8;
        if(y>30) y = 30;
        GUI_RLine(i, 30, y, 1);
        GUI_RLine(i, 30, 52+30-y, 1);
    }
    GUI_Exec();
}

// 条形显示（中线对称，32个）
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
    GUI_LineWith(0, 8, 0, 52, 1, 1);
    // 画X轴
    GUI_LineWith(0, 52, 127, 52, 1, 1);
    for(int k=8; k<=52; k+=11) {
        GUI_LineWith(0, k, 3, k, 1, 1);
        sprintf(str, "%d", (52-k)*prt);
        GUI_PutString(5, k-8 < 8 ? 8 : k-8, str);
    }
    for(int k=0; k<=128; k+=X_SCALE) {
        GUI_LineWith(k, 52, k, 55, 1, 1);
        int freq = k*Fs/256; // 修改为统一的公式
        // 直接转换为kHz整数
        int freq_k = freq / 1000;
        
        // 调整标记位置
        if (k == 0) {
            // 0k放在原点右侧一点，避免超出显示区域
            sprintf(str, "0k");
            GUI_PutString(2, 56, str);
        } else if (freq_k > 0) { // 只显示大于0的频率值
            sprintf(str, "%dk", freq_k);
            // 其他刻度居中显示
            int text_width = 12; // 估计文本宽度
            GUI_PutString(k-(text_width/2), 56, str);
        }
    }
    for(i=1; i<NPT/2; i++) {
        if(lBufMagArray[i] > max_value) {
            max_value = lBufMagArray[i];
            max_index = i;
        }
    }
    int peak_freq = (int)(max_index*Fs/NPT); // 恢复原始峰值频率计算

        sprintf(str, "Peak: %dHz %lu", peak_freq*2, max_value);
    GUI_PutString(40, 0, str);
    for(i = 0; i < 32; i++)
    {
        x = (i<<2);
        y = 30-(lBufMagArray[x+1]/prt)-2;
        if(y<8) y = 8;
        if(y>30) y = 30;
        GUI_LineWith(x, y, x, 30, 3, 1);
        GUI_LineWith(x, 52+30-y, x, 30, 3, 1);
    }
    GUI_Exec();
}

// ADC DMA采集完成回调函数，每次采集NPT个数据，自动切换显示
// 输入参数：hadc，ADC句柄
// 输出参数：无
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    uint16_t i = 0;
    HAL_ADC_Stop_DMA(&hadc1); // 停止DMA采集，防止数据溢出
    // 去掉直流偏移
    for(i=0;i<NPT;i++)
        lBufInArray[i] = ((signed short)(adc_buf[i]-2048)) << 16;
    // 进行256点FFT
    cr4_fft_256_stm32(lBufOutArray, lBufInArray, NPT);
    GetPowerMag(); // 获取幅值
    
    // 显示当前显示模式
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
    HAL_ADC_Start_DMA(&hadc1, adc_buf, NPT); // 重新启动DMA采集
}

#define K1 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)
#define K2 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)
#define K3 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)

// 扫描按键，切换显示模式，显示模式切换方式
// K1不再使用，K2手动切换一次，K3手动切换一次
void Key_Scan(void)
{
    if(K2 == RESET)
    {
        HAL_Delay(10);
        if(K2 == RESET)
        {
            while(!K2);
            display_num++;
            if(display_num > SHOW_NUM) display_num = 1;
        }
    }
    if(K3 == RESET)
    {
        HAL_Delay(10);
        if(K3 == RESET)
        {
            while(!K3);
            if(display_num == 1) display_num = SHOW_NUM+1;
            display_num--;
        }
    }
}

// 错误处理函数
void Error_Handler(void)
{
    while(1) {}
}
