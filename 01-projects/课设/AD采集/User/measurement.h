#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stm32f10x.h"

typedef struct
{
    uint32_t rms_uV;
    uint32_t low_frequency_mHz;
    uint32_t carrier_frequency_mHz;
    uint8_t valid;
    uint8_t clipped;
} MeasurementResult;

void Measurement_Init(uint32_t sample_rate, uint8_t pga);
void Measurement_SetScaleQ16(uint32_t scale_q16);
void Measurement_SetHysteresisUv(uint32_t hysteresis_uV);
uint8_t Measurement_Push(int32_t sample);
const MeasurementResult *Measurement_Get(void);

#endif
