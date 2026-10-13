#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include "stm32f10x.h"   

#define FLASH_USER_START_ADDR   ((uint32_t)0x08006400)
#define CALIBRATION_V			15.00001f	//V
#define CALIBRATION_I			2.00001f	//A

typedef struct  
{
	float viRatio;
	float voRatio;
	float ioZero;
	float ioRatio;
}_uCalibration_Type;
extern _uCalibration_Type	calib;


void Read_CalibData(void);
void Save_CalibData(void);
void CalibrationOutput(void);


#endif
















