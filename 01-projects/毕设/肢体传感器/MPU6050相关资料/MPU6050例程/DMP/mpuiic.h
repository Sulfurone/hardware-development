#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "stm32f10x.h"
#include "stdio.h"

#ifndef __SYS_H //如果没有引用正点原子的sys.h则
    #define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
    #define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
    #define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

    //IO口地址映射
    #define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
    #define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
    #define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
    #define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
    #define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
    #define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
    #define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

    #define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
    #define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
    #define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
    #define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
    #define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
    #define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
    #define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
    
    //IO口操作,只对单一的IO口!
    //确保n的值小于16!
    #define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
    #define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

    #define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
    #define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

    #define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
    #define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

    #define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
    #define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

    #define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
    #define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

    #define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
    #define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

    #define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
    #define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#endif
#define GPIO_PIN(x) x*4

//IO方向设置
#define MPU_SDA_IN()  {GPIOA->CRL&=(~(0X0000000F<<GPIO_PIN(1)));\
                        GPIOA->CRL|=8<<GPIO_PIN(1);}
#define MPU_SDA_OUT() {GPIOA->CRL&=(~(0X0000000F<<GPIO_PIN(1)));\
                        GPIOA->CRL|=3<<GPIO_PIN(1);}
//一个F就是4位,如果你的GPIO设置大于等于GPIO_Pin_8,你需要把CRL改为CRH
//IO操作函数
#define MPU_IIC_SCL    PAout(0) 		//SCL
#define MPU_IIC_SDA    PAout(1) 		//SDA
#define MPU_READ_SDA   PAin(1) 		//输入SDA

uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);//IIC连续写
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf); //IIC连续读 

#endif
