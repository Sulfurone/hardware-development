#ifndef MYPID_H
#define MYPID_H
#include "mybsp.h"
/* Private constants ------------------------------------------------------------*/
#define BUCK_PWM_PERIOD ((uint16_t) HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR) /* PWM Fre */
#define MIN_DUTY_A ((int32_t)(BUCK_PWM_PERIOD * 5)/100) // % MinDuty for Buck
#define MAX_DUTY_A ((int32_t)(BUCK_PWM_PERIOD * 90)/100) // % MaxDuty for Buck
#define SAT_LIMIT ((int32_t)(BUCK_PWM_PERIOD * 50)/100) // PI Integral term saturation value
/****************************************************************************/
extern void Reset_PI(void);
extern int32_t PI_Buck(uint32_t RealVol,uint32_t SetVol,int32_t dec2hex(int32_t temp));
#endif


