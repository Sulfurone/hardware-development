#ifndef _BSP_SPI_H
#define _BSP_SPI_H

#include"stm32f10x.h"
/* 这里只宏定义了端口 引脚 模块，相关时钟请进入
void SPI_Configuration(void)使能*/

#define PCAP_PORT				  GPIOA
#define PCAP_SSN_PIN			GPIO_Pin_4
#define PCAP_SCK_PIN			GPIO_Pin_5
#define PCAP_MISO_PIN			GPIO_Pin_6
#define PCAP_MOSI_PIN			GPIO_Pin_7
#define PCAP_SPI	        SPI1

void SPI_Configuration(void);



//u8 SPIx_WritereadWORD(u16 TxData);
//u8 SPIx_WritereadDWORD(u32 TxData);
//uint8_t SPI_Flash_ReadSR(void);
//uint8_t SPI_Flash_WriteSR(uint16_t);
//void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);
//void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);   
//void SPI_Flash_Erase_Chip(void);   
//void SPI1_WriteByte(u8 TxData);
//void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);

#endif

