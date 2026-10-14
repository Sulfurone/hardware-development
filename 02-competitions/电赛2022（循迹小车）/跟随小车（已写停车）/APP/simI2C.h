#ifndef __SIMI2C_H__
#define __SIMI2C_H__

//Author: Cloud Studio

#include <driverlib.h>

#define SIMI2C_SDA_PORT       GPIO_PORT_P1
#define SIMI2C_SDA_PIN        GPIO_PIN6

#define SIMI2C_SCL_PORT       GPIO_PORT_P1
#define SIMI2C_SCL_PIN        GPIO_PIN7

/*
Example 1, one-time read 4 byte from PCF8591:
    unsigned char dat[4];
    simI2C_init();
    simI2C_SendAddress(0x48, 0, 0);     //Send Write Cmd, Device address is 0x48
    dat[0] = 0x00;
    simI2C_WriteData(dat, 1);           //Write 1 Byte 0x00 to Device
    simI2C_SendAddress(0x48, 0, 1);     //Send Read Cmd, Device address is 0x48
    simI2C_ReadData(dat, 4, 1);         //Read 4 Byte data from Device, last receive send a NACK for no more data get
    simI2C_Stop();                      //Stop
Example 2, multi-times reads from PCF8591:
    unsigned char dat[4];
    simI2C_init();
    simI2C_SendAddress(0x48, 0, 0);
    dat[0] = 0x00;
    simI2C_WriteData(dat, 1);
    simI2C_SendAddress(0x48, 0, 1);
    simI2C_ReadData(dat, 2, 0);         //Read 2 Bytes at 1st time
    simI2C_ReadData(dat, 2, 1);         //Read 2 Bytes at 2nd time and send a NACK
    simI2C_Stop();
*/


void simI2C_init(void);
void simI2C_Start(void);
void simI2C_Stop(void);
int simI2C_SendAddress(uint16_t addr, uint8_t addr10mode, uint8_t rw);
int simI2C_WriteData(uint8_t* data, uint32_t count);
int simI2C_ReadData(uint8_t* data, uint32_t count, uint8_t lastNACK);

#endif
