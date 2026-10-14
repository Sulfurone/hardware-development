#include "key.h"

uint8_t Is_Calibrate_Button_KeyDown(void)
{

    return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET;

}
