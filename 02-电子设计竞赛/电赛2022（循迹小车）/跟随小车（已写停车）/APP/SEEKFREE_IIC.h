/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2016,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		SEEKFREE_IIC.h
 * @brief      		模拟IIC函数库
 * @company	   		成都逐飞科技有限公司
 * @author     		Go For It(1325536866)
 * @version    		v1.0
 * @Software 		IAR 7.7 or MDK 5.17
 * @Target core		MK60DN512VLL10
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2016-02-25
 * @note	
					模拟IIC接线定义
					------------------------------------ 
						SDA                 C17
						SCL                 C16
					------------------------------------ 
 ********************************************************************************************************************/



#ifndef _SEEKFREE_IIC_h
#define _SEEKFREE_IIC_h



#include "gpio.h"

typedef unsigned short  uint16;
typedef unsigned char  uint8;
typedef  short         int16;
#define SCL_PORT   GPIO_PORT_P2                           //定义SCL引脚
#define SCL_PIN    GPIO_PIN5                          //定义SDA引脚

#define SDA_PORT   GPIO_PORT_P3                           //定义SCL引脚
#define SDA_PIN    GPIO_PIN0                       //定义SDA引脚





void  IIC_start(void);
void  IIC_stop(void);
void  IIC_ack_main(unsigned char ack_main);
void  send_ch(unsigned char c);
unsigned char read_ch(void);
void  simiic_write_reg(unsigned char dev_add, unsigned char reg, unsigned char dat);
unsigned char simiic_read_reg(unsigned char dev_add, unsigned char reg);
void  IIC_init(void);
unsigned char read_ch1(void);
void I2C_SendACK(unsigned char ack_dat);








#endif

