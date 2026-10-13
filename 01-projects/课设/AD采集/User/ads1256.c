#include "ads1256.h"

#define ADS_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define ADS_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define ADS_RST_LOW()      GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define ADS_RST_HIGH()     GPIO_SetBits(GPIOB, GPIO_Pin_1)

#define ADS_CMD_RDATAC     0x03U
#define ADS_CMD_SDATAC     0x0FU
#define ADS_CMD_RREG       0x10U
#define ADS_CMD_WREG       0x50U
#define ADS_CMD_SELFCAL    0xF0U
#define ADS_CMD_SYNC       0xFCU
#define ADS_CMD_RESET      0xFEU
#define ADS_CMD_WAKEUP     0xFFU

#define ADS_REG_STATUS     0x00U
#define ADS_DRATE_30000    0xF0U

static uint8_t s_pga = ADS1256_PGA_1;

static void delay_cycles(volatile uint32_t count)
{
    while (count-- != 0U) {
        __NOP();
    }
}

static uint8_t spi_rw(uint8_t value)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) { }
    SPI_I2S_SendData(SPI1, value);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) { }
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

static uint8_t wait_drdy(uint32_t timeout)
{
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) != Bit_RESET) {
        if (timeout-- == 0U) {
            return 0U;
        }
    }
    return 1U;
}

static void command(uint8_t cmd)
{
    ADS_CS_LOW();
    (void)spi_rw(cmd);
    ADS_CS_HIGH();
}

static uint8_t read_register(uint8_t reg)
{
    uint8_t value;
    ADS_CS_LOW();
    (void)spi_rw((uint8_t)(ADS_CMD_RREG | reg));
    (void)spi_rw(0U);
    delay_cycles(800U); /* t6: at least 50 ADS1256 clock periods (~6.5 us). */
    value = spi_rw(0xFFU);
    ADS_CS_HIGH();
    return value;
}

static void write_configuration(uint8_t channel, ADS1256_Pga pga)
{
    ADS_CS_LOW();
    (void)spi_rw((uint8_t)(ADS_CMD_WREG | ADS_REG_STATUS));
    (void)spi_rw(3U);                 /* STATUS through DRATE: four bytes. */
    (void)spi_rw(0x04U);             /* MSB first, auto-calibration on. */
    (void)spi_rw((uint8_t)((channel << 4) | 0x08U)); /* AINx - AINCOM. */
    (void)spi_rw((uint8_t)pga);       /* CLKOUT off, sensor detect off. */
    (void)spi_rw(ADS_DRATE_30000);
    ADS_CS_HIGH();
}

uint8_t ADS1256_Init(uint8_t channel, ADS1256_Pga pga)
{
    GPIO_InitTypeDef gpio;
    SPI_InitTypeDef spi;
    uint8_t status;

    if ((channel > 7U) || ((uint8_t)pga > (uint8_t)ADS1256_PGA_64)) {
        return 0U;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_6;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    ADS_CS_HIGH();
    ADS_RST_LOW();
    delay_cycles(SystemCoreClock / 5000U); /* > 4 us. */
    ADS_RST_HIGH();
    delay_cycles(SystemCoreClock / 500U);  /* oscillator start margin. */

    SPI_I2S_DeInit(SPI1);
    SPI_StructInit(&spi);
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_2Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; /* 1.125 MHz @72 MHz. */
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &spi);
    SPI_Cmd(SPI1, ENABLE);

    command(ADS_CMD_RESET);
    delay_cycles(SystemCoreClock / 1000U);
    if (!wait_drdy(2000000U)) {
        return 0U;
    }

    command(ADS_CMD_SDATAC);
    delay_cycles(100U);
    status = read_register(ADS_REG_STATUS);
    if ((status >> 4) != 0x03U) {     /* ADS1256 chip ID is 3. */
        return 0U;
    }

    s_pga = (uint8_t)pga;
    write_configuration(channel, pga);
    if (!wait_drdy(3000000U)) {
        return 0U;
    }
    command(ADS_CMD_SELFCAL);
    if (!wait_drdy(3000000U)) {
        return 0U;
    }
    command(ADS_CMD_RDATAC);
    return 1U;
}

uint8_t ADS1256_SetChannel(uint8_t channel)
{
    if (channel > 7U) {
        return 0U;
    }
    command(ADS_CMD_SDATAC);
    ADS_CS_LOW();
    (void)spi_rw((uint8_t)(ADS_CMD_WREG | 0x01U));
    (void)spi_rw(0U);
    (void)spi_rw((uint8_t)((channel << 4) | 0x08U));
    ADS_CS_HIGH();
    command(ADS_CMD_SYNC);
    command(ADS_CMD_WAKEUP);
    if (!wait_drdy(1000000U)) {
        return 0U;
    }
    command(ADS_CMD_RDATAC);
    return 1U;
}

uint8_t ADS1256_DataReady(void)
{
    return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_RESET) ? 1U : 0U;
}

int32_t ADS1256_ReadContinuous(void)
{
    uint32_t raw;
    ADS_CS_LOW();
    raw  = ((uint32_t)spi_rw(0xFFU) << 16);
    raw |= ((uint32_t)spi_rw(0xFFU) << 8);
    raw |= (uint32_t)spi_rw(0xFFU);
    ADS_CS_HIGH();
    if ((raw & 0x00800000UL) != 0U) {
        raw |= 0xFF000000UL;
    }
    return (int32_t)raw;
}

uint8_t ADS1256_GetPgaValue(void)
{
    return (uint8_t)(1U << s_pga);
}
