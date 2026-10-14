#include "ming.h"

#include "main.h"


void mingInit(void)
{

			GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);//ledµÆ
			GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);//ledµÆ
			delay_us(800);
			GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);//ledµÆ
			delay_us(800);
}

