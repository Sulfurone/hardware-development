#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

#define MPU6050_ADDR                 0xD0
#define MPU6050_REG_SMPLRT_DIV       0x19
#define MPU6050_REG_CONFIG           0x1A
#define MPU6050_REG_ACCEL_CONFIG     0x1C
#define MPU6050_REG_PWR_MGMT_1       0x6B
#define MPU6050_REG_ACCEL_XOUT_H     0x3B
#define MPU6050_REG_WHO_AM_I         0x75

#define ACCEL_FS_2G                  0x00

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} MPU6050_Data;

void MPU6050_Init(void);
uint8_t MPU6050_ReadByte(uint8_t reg);
void MPU6050_WriteByte(uint8_t reg, uint8_t data);
void MPU6050_ReadData(MPU6050_Data *data);

#endif
