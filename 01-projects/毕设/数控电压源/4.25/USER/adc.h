#ifndef __AD_H
#define __AD_H

#define AD_FILTER_LEN 10

void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);
void TIM4_Init(uint16_t arr, uint16_t psc);
uint16_t FilterADValue(uint16_t *buffer, uint8_t *index, uint16_t new_value);
extern uint16_t ad0_buffer[], ad1_buffer[], ad2_buffer[], ad3_buffer[];
extern uint8_t ad0_index, ad1_index, ad2_index, ad3_index;
#endif
