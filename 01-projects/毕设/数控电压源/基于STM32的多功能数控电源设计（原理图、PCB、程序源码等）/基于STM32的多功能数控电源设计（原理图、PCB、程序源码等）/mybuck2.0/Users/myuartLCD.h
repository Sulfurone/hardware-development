#ifndef _MYUARTLCD_H_
#define _MYUARTLCD_H_
#include "mylightos.h"
#include "mybsp.h"
extern BOOLEAN Uartflag; 
extern uint8_t menuflag ;
extern uint8_t keyvalue ;
extern uint16_t Set_vol;
extern double Outvolvalue;
extern void GetADValue(PVOID pvParams);
extern void pidpoll(PVOID pvParams);
extern void OverCurProtect(PVOID pvParams);
extern void dipvol(PVOID pvParams);
extern void UartLCDPoll(void);
extern void KeyPoll(void);
extern void MenuPoll(void);
extern void RefreshDutyOnVolmenu(void);
extern void MainMenu(void);
#endif
