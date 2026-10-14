#ifndef __SR04_H
#define __SR04_H
#include "driverlib.h"
#define uint unsigned int
#define u8 unsigned char


extern void  SRDelay2us(void);
extern void  SRI2C_Start(void);
extern void  SRI2C_Stop(void);
extern uint8_t   SRI2C_RecvACK(void);
extern void  SRI2C_SendByte(u8 Dat);
extern u8	 SRI2C_RecvByte(void);
extern u8	 SRSingle_ReadI2C(u8 REG_Address);
extern void  InitSR04(void);
extern void SR04Read(u8 *buf);
extern void SR04Start(void);
extern unsigned long SR04Cal(u8 *buf);
#endif  

