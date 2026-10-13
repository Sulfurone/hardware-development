#include "MPU6050.h"
#include "Delay.h"

#define I2C_SCL_PIN      GPIO_Pin_10
#define I2C_SDA_PIN      GPIO_Pin_11
#define I2C_GPIO_PORT    GPIOB
#define I2C_GPIO_RCC     RCC_APB2Periph_GPIOB

static void I2C_Delay(uint32_t us)
{
    Delay_us(us);
}

static void MPU6050_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(I2C_GPIO_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);

    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(10);
}

static void I2C_Start(void)
{
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(5);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(5);
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);
}

static void I2C_Stop(void)
{
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(5);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(5);
}

static void I2C_SendAck(uint8_t ack)
{
    if (ack)
    {
        GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    }
    else
    {
        GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    }

    I2C_Delay(5);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);
    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(5);
}

static uint8_t I2C_ReceiveAck(void)
{
    uint8_t ack;

    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
    I2C_Delay(5);
    GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);

    if (GPIO_ReadInputDataBit(I2C_GPIO_PORT, I2C_SDA_PIN))
    {
        ack = 1;
    }
    else
    {
        ack = 0;
    }

    GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
    I2C_Delay(5);
    return ack;
}

static void I2C_SendByte(uint8_t byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
        }
        else
        {
            GPIO_ResetBits(I2C_GPIO_PORT, I2C_SDA_PIN);
        }

        I2C_Delay(5);
        GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
        I2C_Delay(5);
        GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
        I2C_Delay(5);
        byte <<= 1;
    }

    I2C_ReceiveAck();
}

static uint8_t I2C_ReceiveByte(void)
{
    uint8_t i;
    uint8_t byte = 0;

    GPIO_SetBits(I2C_GPIO_PORT, I2C_SDA_PIN);

    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        GPIO_SetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
        I2C_Delay(5);

        if (GPIO_ReadInputDataBit(I2C_GPIO_PORT, I2C_SDA_PIN))
        {
            byte |= 0x01;
        }

        GPIO_ResetBits(I2C_GPIO_PORT, I2C_SCL_PIN);
        I2C_Delay(5);
    }

    return byte;
}

void MPU6050_Init(void)
{
    uint8_t who_am_i;

    MPU6050_I2C_Init();
    MPU6050_WriteByte(MPU6050_REG_PWR_MGMT_1, 0x00);
    Delay_ms(100);

    who_am_i = MPU6050_ReadByte(MPU6050_REG_WHO_AM_I);
    if (who_am_i != 0x68)
    {
        while (1)
        {
        }
    }

    MPU6050_WriteByte(MPU6050_REG_SMPLRT_DIV, 0x04);
    MPU6050_WriteByte(MPU6050_REG_CONFIG, 0x03);
    MPU6050_WriteByte(MPU6050_REG_ACCEL_CONFIG, ACCEL_FS_2G);
}

uint8_t MPU6050_ReadByte(uint8_t reg)
{
    uint8_t data;

    I2C_Start();
    I2C_SendByte(MPU6050_ADDR);
    I2C_SendByte(reg);

    I2C_Start();
    I2C_SendByte(MPU6050_ADDR | 0x01);
    data = I2C_ReceiveByte();
    I2C_SendAck(1);
    I2C_Stop();

    return data;
}

void MPU6050_WriteByte(uint8_t reg, uint8_t data)
{
    I2C_Start();
    I2C_SendByte(MPU6050_ADDR);
    I2C_SendByte(reg);
    I2C_SendByte(data);
    I2C_Stop();
}

void MPU6050_ReadData(MPU6050_Data *data)
{
    uint8_t buf[6];
    uint8_t i;

    I2C_Start();
    I2C_SendByte(MPU6050_ADDR);
    I2C_SendByte(MPU6050_REG_ACCEL_XOUT_H);

    I2C_Start();
    I2C_SendByte(MPU6050_ADDR | 0x01);

    for (i = 0; i < 5; i++)
    {
        buf[i] = I2C_ReceiveByte();
        I2C_SendAck(0);
    }

    buf[5] = I2C_ReceiveByte();
    I2C_SendAck(1);
    I2C_Stop();

    data->accel_x = (int16_t)((buf[0] << 8) | buf[1]);
    data->accel_y = (int16_t)((buf[2] << 8) | buf[3]);
    data->accel_z = (int16_t)((buf[4] << 8) | buf[5]);
}
