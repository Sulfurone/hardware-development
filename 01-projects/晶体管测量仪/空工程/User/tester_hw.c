#include "tester_hw.h"
#include "app_config.h"
#include "bsp_delay.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} GpioPin;

typedef struct {
    uint8_t adc_channel;
    GpioPin r680;
    GpioPin r470k;
} TestPinHw;

static const TestPinHw g_testPins[3] = {
    { TP1_ADC_CH, {GPIOB, GPIO_Pin_0},  {GPIOB, GPIO_Pin_11} },
    { TP2_ADC_CH, {GPIOB, GPIO_Pin_1},  {GPIOA, GPIO_Pin_8}  },
    { TP3_ADC_CH, {GPIOB, GPIO_Pin_10}, {GPIOA, GPIO_Pin_9}  }
};

static void Pin_Output(GpioPin p, BitAction level)
{
    GPIO_InitTypeDef gpio;

    gpio.GPIO_Pin = p.pin;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(p.port, &gpio);
    GPIO_WriteBit(p.port, p.pin, level);
}

static void Pin_Float(GpioPin p)
{
    GPIO_InitTypeDef gpio;

    gpio.GPIO_Pin = p.pin;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(p.port, &gpio);
}

static void ADC_InitLocal(void)
{
    ADC_InitTypeDef adc;
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    ADC_DeInit(ADC1);
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &adc);

    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)) {
    }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)) {
    }
}

static uint16_t ADC_ReadRaw(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {
    }
    return ADC_GetConversionValue(ADC1);
}

static uint16_t ADC_ReadMv(uint8_t channel)
{
    uint32_t raw;

    raw = ADC_ReadRaw(channel);
    return (uint16_t)((raw * APP_VCC_MV + 2047UL) / 4095UL);
}

void TesterHW_TpSet(uint8_t tp, TpMode mode)
{
    const TestPinHw *pin;

    if (tp >= 3) {
        return;
    }

    pin = &g_testPins[tp];
    Pin_Float(pin->r680);
    Pin_Float(pin->r470k);

    switch (mode) {
    case TP_LOW_680:
        Pin_Output(pin->r680, Bit_RESET);
        break;
    case TP_HIGH_680:
        Pin_Output(pin->r680, Bit_SET);
        break;
    case TP_LOW_470K:
        Pin_Output(pin->r470k, Bit_RESET);
        break;
    case TP_HIGH_470K:
        Pin_Output(pin->r470k, Bit_SET);
        break;
    default:
        break;
    }
}

void TesterHW_TpAllFloat(void)
{
    uint8_t i;

    for (i = 0; i < 3; i++) {
        TesterHW_TpSet(i, TP_FLOAT);
    }
}

void TesterHW_TpDischargeAll(uint16_t ms)
{
    uint8_t i;

    for (i = 0; i < 3; i++) {
        TesterHW_TpSet(i, TP_LOW_680);
    }
    Delay_Ms(ms);
    TesterHW_TpAllFloat();
}

uint16_t TesterHW_TpReadMv(uint8_t tp)
{
    if (tp >= 3) {
        return 0;
    }
    return ADC_ReadMv(g_testPins[tp].adc_channel);
}

uint16_t TesterHW_TpReadMvAvg(uint8_t tp)
{
    uint32_t sum = 0;
    uint8_t i;

    for (i = 0; i < 8; i++) {
        sum += TesterHW_TpReadMv(tp);
    }
    return (uint16_t)(sum / 8);
}

uint16_t TesterHW_BatteryMv(void)
{
    uint32_t mv;

    mv = ADC_ReadMv(VBAT_ADC_CH);
    mv = mv * (BAT_DIV_TOP_KOHM + BAT_DIV_BOTTOM_KOHM) / BAT_DIV_BOTTOM_KOHM;
    return (uint16_t)mv;
}

uint16_t TesterHW_RefMv(void)
{
    return ADC_ReadMv(VREF_ADC_CH);
}

uint8_t TesterHW_ButtonPressed(void)
{
    return GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET;
}

void TesterHW_Led(uint8_t on)
{
    if (on) {
        GPIO_ResetBits(LED_PORT, LED_PIN);
    } else {
        GPIO_SetBits(LED_PORT, LED_PIN);
    }
}

void TesterHW_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    gpio.GPIO_Pin = POWER_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(POWER_PORT, &gpio);
    GPIO_SetBits(POWER_PORT, POWER_PIN);

    gpio.GPIO_Pin = BUTTON_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUTTON_PORT, &gpio);

    gpio.GPIO_Pin = LED_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &gpio);
    TesterHW_Led(0);

    ADC_InitLocal();
    TesterHW_TpAllFloat();
}
