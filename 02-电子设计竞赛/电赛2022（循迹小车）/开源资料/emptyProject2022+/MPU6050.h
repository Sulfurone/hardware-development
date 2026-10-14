#ifndef __MPU6050_H
#define __MPU6050_H
#include "driverlib.h"
#define uint unsigned int   
#define u8 unsigned char 

extern void  InitMPU6050(void);												
extern void  Delay2us(void);
extern void  I2C_Start(void);
extern void  I2C_Stop(void);
extern uint8_t   I2C_RecvACK(void);
extern void  I2C_SendByte(u8 Dat);
extern u8	  I2C_RecvByte(void);
extern void  I2C_ReadPage(void);
extern void  I2C_WritePage(void);
extern u8	  Single_ReadI2C(u8 REG_Address);					
extern void  Single_WriteI2C(u8 REG_Address,u8 REG_data);
extern void  Read_MPU6050(u8 *buf);

#endif  

