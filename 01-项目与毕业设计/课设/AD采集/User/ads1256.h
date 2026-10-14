#ifndef __ADS1256_H
#define __ADS1256_H

#include "stm32f10x.h"

/* Board wiring: SPI1 PA5/PA6/PA7, CS=PA4, DRDY=PB0, RESET=PB1. */
#define ADS1256_DATA_RATE_SPS       30000UL
#define ADS1256_REFERENCE_UV        2500000UL

typedef enum
{
    ADS1256_PGA_1  = 0,
    ADS1256_PGA_2  = 1,
    ADS1256_PGA_4  = 2,
    ADS1256_PGA_8  = 3,
    ADS1256_PGA_16 = 4,
    ADS1256_PGA_32 = 5,
    ADS1256_PGA_64 = 6
} ADS1256_Pga;

uint8_t ADS1256_Init(uint8_t channel, ADS1256_Pga pga);
uint8_t ADS1256_SetChannel(uint8_t channel);
uint8_t ADS1256_DataReady(void);
int32_t ADS1256_ReadContinuous(void);
uint8_t ADS1256_GetPgaValue(void);

#endif
