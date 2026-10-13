#ifndef TESTER_HW_H
#define TESTER_HW_H

#include "tester_types.h"

void TesterHW_Init(void);
void TesterHW_Led(uint8_t on);
uint8_t TesterHW_ButtonPressed(void);

void TesterHW_TpSet(uint8_t tp, TpMode mode);
void TesterHW_TpAllFloat(void);
void TesterHW_TpDischargeAll(uint16_t ms);
uint16_t TesterHW_TpReadMv(uint8_t tp);
uint16_t TesterHW_TpReadMvAvg(uint8_t tp);

uint16_t TesterHW_BatteryMv(void);
uint16_t TesterHW_RefMv(void);

#endif
