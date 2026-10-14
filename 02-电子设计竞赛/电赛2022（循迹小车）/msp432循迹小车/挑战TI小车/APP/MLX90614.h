#ifndef _MLX90614_h
#define _MLX90614_h
#include "SEEKFREE_IIC.h"
# define MLX90614_COLLECT_COUNT 10   //采集了几次

#define  MLX90614_ADRESS     					0X00
#define  RAM_AMBIENT_TEMPERATURE     	0X07  //环境温度
#define  RAM_OBJECT_TEMPERATURE     	0X07	//物体温度
#define  EEPROM_READ_ADRESS     			0X22





unsigned short Read_MLX90614_Data(unsigned char adress,unsigned char  commond);
 unsigned char gh_crc8( unsigned char * p_buffer, unsigned short buf_size );
 float MLX90614_Get_Temperature(void);
#endif

