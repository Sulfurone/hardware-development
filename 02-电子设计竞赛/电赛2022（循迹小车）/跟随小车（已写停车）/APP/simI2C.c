#include "simI2C.h"

//Author: Cloud Studio

#define simI2C_SDA_0() \
    GPIO_setAsOutputPin(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN);\
    GPIO_setOutputLowOnPin(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN)
    //HWREG16(P1 + OFS_LIB_PADIR) |= SIMI2C_SDA_PIN
    //GPIO_setAsPeripheralModuleFunctionOutputPin(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN, GPIO_SECONDARY_MODULE_FUNCTION)
#define simI2C_SDA_1() \
    GPIO_setAsInputPin(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN)
    //HWREG16(P1 + OFS_LIB_PADIR) &= ~SIMI2C_SDA_PIN
    //GPIO_setAsInputPinWithPullUpResistor(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN)
#define simI2C_SCL_0() \
    GPIO_setAsOutputPin(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN);\
    GPIO_setOutputLowOnPin(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN)
    //HWREG16(P1 + OFS_LIB_PADIR) |= SIMI2C_SCL_PIN
    //GPIO_setAsPeripheralModuleFunctionOutputPin(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN, GPIO_SECONDARY_MODULE_FUNCTION)
#define simI2C_SCL_1() \
    GPIO_setAsInputPin(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN)
    //HWREG16(P1 + OFS_LIB_PADIR) &= ~SIMI2C_SCL_PIN
    //GPIO_setAsInputPinWithPullUpResistor(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN)
#define simI2C_SDA_read() \
    GPIO_getInputPinValue(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN)

void simI2C_delay(uint32_t count)
{
    volatile uint32_t i;
    while(count--)
        for(i=0; i<5; i++);
}

void simI2C_init(void)
{
    //GPIO_setAsPeripheralModuleFunctionOutputPin(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN, GPIO_SECONDARY_MODULE_FUNCTION);
    //GPIO_setAsPeripheralModuleFunctionOutputPin(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN, GPIO_SECONDARY_MODULE_FUNCTION);
    //GPIO_setAsInputPinWithPullUpResistor(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN);
    //GPIO_setAsInputPinWithPullUpResistor(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN);
    GPIO_setAsInputPin(SIMI2C_SDA_PORT, SIMI2C_SDA_PIN);
    GPIO_setAsInputPin(SIMI2C_SCL_PORT, SIMI2C_SCL_PIN);
}

void simI2C_Start(void)
{
    simI2C_SDA_1();
    simI2C_SCL_1();
    simI2C_delay(10);
    simI2C_SDA_0();
    simI2C_delay(10);
    simI2C_SCL_0();
}

void simI2C_Stop(void)
{
    simI2C_SDA_0();
    simI2C_SCL_0();
    simI2C_delay(10);
    simI2C_SCL_1();
    simI2C_delay(10);
    simI2C_SDA_1();
}

uint8_t simI2C_getAck(void)
{
    uint8_t ack;
    
    simI2C_SDA_1();
    simI2C_delay(10);
    simI2C_SCL_1();
    simI2C_delay(10);
    if(simI2C_SDA_read() != GPIO_INPUT_PIN_LOW)
        ack = 0;
    else
        ack = 1;
    simI2C_SCL_0();
    simI2C_delay(10);
    return ack;
}

int simI2C_SendAddress(uint16_t addr, uint8_t addr10mode, uint8_t rw)
{
    uint8_t i;
    uint8_t ack;
    uint8_t tmp;
    
    simI2C_Start();
    simI2C_SCL_0();
    if(addr10mode)
    {
        tmp = 0xF0 + ((addr >> 7) & 0x06) + (rw & 0x01);
    }
    else
    {
        tmp = (addr << 1) + (rw & 0x01);
    }
    for(i=0; i<8; i++)
    {
        if(tmp & 0x80)
            simI2C_SDA_1();
        else
            simI2C_SDA_0();
        tmp <<= 1;
        simI2C_delay(10);
        simI2C_SCL_1();
        simI2C_delay(10);
        simI2C_SCL_0();
    }
    simI2C_delay(10);
    ack = simI2C_getAck();
    if(!ack)
    {
        simI2C_Stop();
        return -1;
    }
    if(addr10mode)
    {
        tmp = addr & 0xFF;
        for(i=0; i<8; i++)
        {
            if(tmp & 0x80)
                simI2C_SDA_1();
            else
                simI2C_SDA_0();
            tmp <<= 1;
            simI2C_delay(10);
            simI2C_SCL_1();
            simI2C_delay(10);
            simI2C_SCL_0();
        }
        ack = simI2C_getAck();
        if(!ack)
        {
            simI2C_Stop();
            return -1;
        }
    }
    return 0;
}

int simI2C_WriteData(uint8_t* data, uint32_t count)
{
    uint32_t wcount;
    uint8_t i;
    uint8_t ack;
    uint8_t tmp;
    
    simI2C_SCL_0();
    for(wcount=1; wcount<=count; wcount++)
    {
        tmp = *data++;
        for(i=0;i<8;i++)
        {
            if(tmp & 0x80)
                simI2C_SDA_1();
            else
                simI2C_SDA_0();
            tmp <<= 1;
            simI2C_delay(10);
            simI2C_SCL_1();
            simI2C_delay(10);
            simI2C_SCL_0();
        }
        simI2C_delay(10);
        ack = simI2C_getAck();
        if(!ack)
        {
            simI2C_Stop();
            return wcount;
        }
    }
    return wcount;
}

int simI2C_ReadData(uint8_t* data, uint32_t count, uint8_t lastNACK)
{
    uint32_t wcount;
    uint8_t i;
    uint8_t ack;
    uint8_t tmp;
    
    simI2C_SCL_0();
    simI2C_SDA_1();
    for(wcount=1; wcount<=count; wcount++)
    {
        tmp = 0;
        for(i=0;i<8;i++)
        {
            simI2C_delay(10);
            simI2C_SCL_1();
            simI2C_delay(10);
            if(simI2C_SDA_read() != GPIO_INPUT_PIN_LOW)
                tmp++;
            tmp <<= 1;
            simI2C_SCL_0();
        }
        simI2C_delay(10);
        if(lastNACK && wcount==count)
        {
            //last byte give NACK
            simI2C_SDA_1();
        }
        else
        {
            simI2C_SDA_0();
        }
        simI2C_SCL_1();
        simI2C_delay(10);
        simI2C_SCL_0();
        simI2C_SDA_1();
    }
    return wcount;
}
