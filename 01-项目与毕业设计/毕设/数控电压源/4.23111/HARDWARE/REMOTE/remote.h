#ifndef __RED_H 
#define __RED_H 
#include "sys.h"   
#define RDATA1 PBin(8)
#define RDATA2 PBin(9)//红外数据输入引脚
#define REMOTE_ID 0   //选用遥控器识别码为0      
extern u8 RmtCnt; 
void Remote_Init(void); 
u8 Remote_Scan(void);     


#endif 



