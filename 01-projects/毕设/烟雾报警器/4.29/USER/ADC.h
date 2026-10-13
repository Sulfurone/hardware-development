#ifndef __ADC_H
#define __ADC_H

void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);
uint16_t AD_GetValue_Filtered(uint8_t ADC_Channel);

#endif
