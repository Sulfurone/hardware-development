#ifndef PCAP_H
#define PCAP_H

#include "stm32f10x.h"


#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define __3nop()   __nop();__nop();__nop();	


#define SPI_Enable() 	  GPIO_ResetBits(GPIOA,GPIO_Pin_4);
#define SPI_Disable() 	  GPIO_SetBits(GPIOA,GPIO_Pin_4);

#define SPI_SCLK_High     GPIO_SetBits(GPIOA,GPIO_Pin_5);			 //SPI SCLK WIRE
#define SPI_SCLK_Low      GPIO_ResetBits(GPIOA,GPIO_Pin_5);

#define SPI_MOSI_High     GPIO_SetBits(GPIOA,GPIO_Pin_7);			 //SPI MOSI  MCU OUT
#define SPI_MOSI_Low      GPIO_ResetBits(GPIOA,GPIO_Pin_7);


u8 SPI_Read_Bit(void);
void SPI_Send_Bit(u8 bbit);
u32 spi_read(void);


void write_date_8(u8 date);
void write_date_16(u16 date);
void write_date_32(u32 date);
void cap01_init(void);
void write_firmware(void);
void spi_test(void);
void pcap01_init(void);

#endif
