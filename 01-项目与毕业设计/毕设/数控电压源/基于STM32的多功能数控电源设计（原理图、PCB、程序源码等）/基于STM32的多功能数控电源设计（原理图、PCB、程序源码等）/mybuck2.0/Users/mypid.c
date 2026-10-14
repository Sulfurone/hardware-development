#include "mypid.h"
/* Private variables ---------------------------------------------------------*/
uint32_t CTMin;
uint32_t CTMax;
static int32_t Kp;
static int32_t Ki;
static int32_t Int_term_Buck;
/**
* @brief  This function initializes or resets the PI parameters when switching to another mode
* @param  None
* @retval None
*/
void Reset_PI(void)
{
  /* Reset integral terms for PI */
  Int_term_Buck = 0;
  /* Reset Counters Min and Max */
  CTMax = 0;
  CTMin = 0;
  /* Set Proportional and Integral constant terms*/
	switch(menuflag)
	{
		case 2:
		{
			Ki = 30;
			Kp = 15;
		}break;
		case 3:
		{
			Ki = -40;
			Kp = 20;
		}break;
	}	
}
/**
* @brief  This function calculates new duty order with PI.
* @param  None
* @retval New duty order
*/
int32_t PI_Buck(uint32_t RealVol,uint32_t SetVol,int32_t dec2hex(int32_t temp))
{ 
  /* Compute PI for Buck Mode */
  /* Every time the PI order sets extreme values then CTMax or CTMin are managed */
  int32_t seterr, pid_out;
  int32_t error;
	
  error = ((int32_t ) RealVol - (int32_t) SetVol);
	error = dec2hex(error);
	
  seterr = (-Kp * error) / 200;

  Int_term_Buck = Int_term_Buck + ((-Ki * error) / 200);

  if (Int_term_Buck > SAT_LIMIT)
  {
    Int_term_Buck = SAT_LIMIT;
  }
  if (Int_term_Buck < -(SAT_LIMIT))
  {
    Int_term_Buck = -(SAT_LIMIT);
  }
  pid_out = seterr + Int_term_Buck;
  pid_out += BUCK_PWM_PERIOD / 2;

  if (pid_out >= MAX_DUTY_A)
  {
    pid_out = MAX_DUTY_A;
    CTMax++;
  }
  else
  {
    if (CTMax != 0)
    {
      CTMax--;
    }
  }
  if (pid_out <= MIN_DUTY_A)
  {
    pid_out = MIN_DUTY_A;
    CTMin++;
  }
  else
  {
    if (CTMin != 0)
    {
      CTMin--;
    }
  }
  return  pid_out;
}
