#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f10x.h"

#define APP_VCC_MV                  3300UL

#define OLED_I2C_ADDR               0x3CU
#define OLED_WIDTH                  128U
#define OLED_PAGES                  8U
#define OLED_SCL_PORT               GPIOB
#define OLED_SCL_PIN                GPIO_Pin_6
#define OLED_SDA_PORT               GPIOB
#define OLED_SDA_PIN                GPIO_Pin_7

#define POWER_PORT                  GPIOB
#define POWER_PIN                   GPIO_Pin_13
#define BUTTON_PORT                 GPIOB
#define BUTTON_PIN                  GPIO_Pin_12
#define LED_PORT                    GPIOC
#define LED_PIN                     GPIO_Pin_13

#define TP1_ADC_CH                  ADC_Channel_0
#define TP2_ADC_CH                  ADC_Channel_1
#define TP3_ADC_CH                  ADC_Channel_2
#define VBAT_ADC_CH                 ADC_Channel_3
#define VREF_ADC_CH                 ADC_Channel_4

#define DIODE_MIN_MV                180U
#define DIODE_MAX_MV                1800U
#define RES_LOW_OHM_SUM             1360UL
#define RES_HIGH_OHM_SUM            940000UL
#define CAP_CHARGE_R_OHM            470000UL

#define BAT_DIV_TOP_KOHM            680UL
#define BAT_DIV_BOTTOM_KOHM         220UL

#endif
