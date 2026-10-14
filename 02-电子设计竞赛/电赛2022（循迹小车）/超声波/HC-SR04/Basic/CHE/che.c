#include "che.h"
#include  "main.h"




Timer_A_UpDownModeConfig upDownConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock SOurce
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz
        TIMER_PERIOD,                           // 127 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value

};
Timer_A_CompareModeConfig compareConfig_PWM1 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        0                                 // 32 Duty Cycle
};
Timer_A_CompareModeConfig compareConfig_PWM2 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_2,          // Use CCR2
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        0                                 // 96 Duty Cycle
};
Timer_A_CompareModeConfig compareConfig_PWM3 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_3,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        0                                // 32 Duty Cycle
};
Timer_A_CompareModeConfig compareConfig_PWM4 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_4,          // Use CCR2
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        0                                // 96 Duty Cycle
};
void cheInin(void)
{
    MAP_WDT_A_holdTimer();
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_Timer_A_configureUpDownMode(TIMER_A1_BASE, &upDownConfig);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UPDOWN_MODE);
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM1);
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM2);
	  MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM3);
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM4);
}

void cheout(void)
{
        compareConfig_PWM1.compareValue = pwm1;
				compareConfig_PWM2.compareValue = pwm2;
				compareConfig_PWM3.compareValue = pwm3;
				compareConfig_PWM4.compareValue = pwm4;
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM1);
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM2);
	  MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM3);
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM4);
}
