#include "bsp_delay.h"

void Delay_Init(void)
{
    SystemCoreClockUpdate();
}

void Delay_Us(uint32_t us)
{
    volatile uint32_t cycles;

    cycles = (SystemCoreClock / 8000000UL) * us;
    if (cycles == 0) {
        cycles = us;
    }

    while (cycles--) {
        __NOP();
    }
}

void Delay_Ms(uint32_t ms)
{
    while (ms--) {
        Delay_Us(1000);
    }
}
