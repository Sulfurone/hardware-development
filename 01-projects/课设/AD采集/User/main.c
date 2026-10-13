#include "stm32f10x.h"
#include "ads1256.h"
#include "measurement.h"
#include "can_protocol.h"

int main(void)
{
    GPIO_InitTypeDef gpio;
    uint8_t adc_ok;

    SystemCoreClockUpdate();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio);
    GPIO_SetBits(GPIOC, GPIO_Pin_13); /* Blue Pill LED is active low. */

    CAN_ProtocolInit();
    adc_ok = ADS1256_Init(0U, ADS1256_PGA_1);
    if (!adc_ok) {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13); /* Solid LED: ADC wiring/init fault. */
    }
    Measurement_Init(ADS1256_DATA_RATE_SPS, ADS1256_GetPgaValue());
    Measurement_SetHysteresisUv(2000U);

    while (1) {
        CAN_ProtocolPoll();
        if (adc_ok && CAN_ProtocolIsRunning() && ADS1256_DataReady()) {
            if (Measurement_Push(ADS1256_ReadContinuous())) {
                CAN_ProtocolPublish(Measurement_Get());
                GPIO_WriteBit(GPIOC, GPIO_Pin_13,
                              (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13));
            }
        }
    }
}
