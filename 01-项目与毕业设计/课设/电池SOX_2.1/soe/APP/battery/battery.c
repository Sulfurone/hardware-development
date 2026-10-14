#include "battery.h"
#include "SysTick.h"

/********************* 内部变量 *********************/
static float g_voutv = 0.0f;
static float g_vouti = 0.0f;
static float g_bat_v = 0.0f;
static float g_curr  = 0.0f;

static float g_remain_Wh = BAT_ENERGY_WH;
static float g_soe = 100.0f;

static u8 g_battery_inited = 0;

/********************* 限幅函数 *********************/
static float LimitFloat(float val, float min, float max)
{
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

static float BATTERY_VoltageToSOE(float vbat)
{
    float soe;

    soe = 100.0f * (vbat - BAT_V_MIN) / (BAT_V_MAX - BAT_V_MIN);

    return LimitFloat(soe, 0.0f, 100.0f);
}

/********************* ADC初始化 *********************/
// PA0=VOUTV(ADC1_CH0), PA1=VOUTI(ADC1_CH1)
void BATTERY_ADC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_InitTypeDef   ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // STM32F1 ADC时钟建议不要超过14MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

/********************* 获取ADC原始值 *********************/
u16 BATTERY_GetAdcValue(u8 ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);

    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    return ADC_GetConversionValue(ADC1);
}

/********************* 多次采样取平均，减少抖动 *********************/
u16 BATTERY_GetAdcAverage(u8 ch, u8 times)
{
    u8 i;
    u32 sum = 0;

    for (i = 0; i < times; i++)
    {
        sum += BATTERY_GetAdcValue(ch);
    }

    return (u16)(sum / times);
}

/********************* 读取VOUTV引脚电压 *********************/
static float BATTERY_ReadVOUTV_PinVoltage(void)
{
    u16 adc_val = BATTERY_GetAdcAverage(VOUTV_ADC_CH, 10);
    return (float)adc_val / 4095.0f * ADC_VREF;
}

/********************* 读取VOUTI引脚电压 *********************/
static float BATTERY_ReadVOUTI_PinVoltage(void)
{
    u16 adc_val = BATTERY_GetAdcAverage(VOUTI_ADC_CH, 10);
    return (float)adc_val / 4095.0f * ADC_VREF;
}

/********************* 电池数据更新（库仑计积分） *********************/
// dt_s：两次更新之间的时间，单位秒
// 例如主函数500ms刷新一次，就传 0.5f
void BATTERY_Update(float dt_s)
{
    float init_soe;
    float delta_Wh;

    g_voutv = BATTERY_ReadVOUTV_PinVoltage();
    g_vouti = BATTERY_ReadVOUTI_PinVoltage();

    g_bat_v = VOUTV_K * g_voutv + VOUTV_B;

    g_curr = (g_vouti - VOUTI_ZERO_V) * VOUTI_SENS;

    if (g_battery_inited == 0)
    {
        init_soe = BATTERY_VoltageToSOE(g_bat_v);
        g_remain_Wh = BAT_ENERGY_WH * init_soe / 100.0f;
        g_battery_inited = 1;
    }

    delta_Wh = g_bat_v * g_curr * dt_s / 3600.0f;
    g_remain_Wh += delta_Wh;
    g_remain_Wh = LimitFloat(g_remain_Wh, 0.0f, BAT_ENERGY_WH);

    g_soe = g_remain_Wh / BAT_ENERGY_WH * 100.0f;
    g_soe = LimitFloat(g_soe, 0.0f, 100.0f);

    if (g_bat_v >= BAT_V_MAX && g_curr > 0.0f)
    {
        g_remain_Wh = BAT_ENERGY_WH;
        g_soe = 100.0f;
    }

    if (g_bat_v <= BAT_V_MIN && g_curr < 0.0f)
    {
        g_remain_Wh = 0.0f;
        g_soe = 0.0f;
    }
}

/********************* 获取缓存数据 *********************/
float BATTERY_GetVOUTV_V(void)
{
    return g_voutv;
}

float BATTERY_GetVOUTI_V(void)
{
    return g_vouti;
}

float BATTERY_GetVoltage(void)
{
    return g_bat_v;
}

float BATTERY_GetCurrent(void)
{
    return g_curr;
}

float BATTERY_GetSOE(void)
{
    return g_soe;
}
