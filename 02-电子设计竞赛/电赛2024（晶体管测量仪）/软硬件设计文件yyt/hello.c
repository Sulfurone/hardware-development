/** 
*** hello.c
*** jks
*** 24-Feb-2012
**/

// used to competition

#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include <stdio.h>
#include "lm3s811.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h" 
#include "driverlib/uart.h"	 
#include "driverlib/interrupt.h"
#include "ili9320.h"

#define F_COLOR ClrGray
#define B_COLOR ClrBlack

volatile unsigned long g_frequent = 0;	// Hz
volatile unsigned long g_c = 0;	  // pF
volatile unsigned long g_r = 0;	  // ohme
char g_char[4][20]; // F R L C
volatile unsigned char g_frlc = 0;
volatile unsigned char g_number = 0;
tContext sContext;
volatile unsigned short g_rate = 200;

void uart_send(unsigned char* c);
void delay(volatile unsigned long i);
void num_to_char(char* c, unsigned long num);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// 
//
//*****************************************************************************
int
main(void)
{	

    /* clock*/
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_6MHZ);
				 
    /*LCD*/
    LCD_Setup();
	LCD_Clear(B_COLOR);

	//
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &LCD_ILI9320);


    GrContextForegroundSet(&sContext, F_COLOR);
    GrContextFontSet(&sContext, &g_sFontCm26);
    GrStringDrawCentered(&sContext, "INITIALIZING", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         ((GrContextDpyHeightGet(&sContext)) / 2),
                         0);

    //
    // Flush any cached drawing operations.
    //
    GrFlush(&sContext);

	/*Uart0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    IntMasterEnable(); // Interrupt masl enable
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); 

    uart_send("jks\n24-Feb-2012\n");

	/*Timer*/
    //
    // Enable the peripherals
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);	
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); 
	GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_4);
	GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PD4_CCP0);
	GPIOPinConfigure(GPIO_PD5_CCP2);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |
	               TIMER_CFG_A_CAP_COUNT);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR |
	               TIMER_CFG_A_CAP_TIME);
	TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
	TimerMatchSet(TIMER0_BASE, TIMER_A, 0x0001);

    TimerConfigure(TIMER2_BASE, TIMER_CFG_32_BIT_PER);
    TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet() / g_rate);
    //
    // Setup the interrupts for the timer timeouts.
    //
    IntEnable(INT_TIMER2A);
	IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER0_BASE, TIMER_CAPA_MATCH);
    //
    // Enable the timers.
    //
    TimerEnable(TIMER0_BASE, TIMER_A);
	TimerEnable(TIMER2_BASE, TIMER_A);

	// GPIOC											 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIO_PORTC_DIR_R |= 0x10;

	LCD_Clear(B_COLOR);
	g_char[0][0] = 'F';
	g_char[0][1] = ':';
	g_char[0][2] = ' ';
	g_char[0][13] = 'H';
	g_char[0][14] = 'z';
	g_char[0][15] = '\0';
	
	g_char[1][0] = 'R';
	g_char[1][1] = ':';
	g_char[1][2] = ' ';
	g_char[1][13] = 'o';
	g_char[1][14] = 'h';
	g_char[1][15] = 'm';
	g_char[1][16] = 'e';
	g_char[1][17] = '\0';
	
	g_char[2][0] = 'L';
	g_char[2][1] = ':';
	g_char[2][2] = ' ';
	g_char[2][13] = 'u';
	g_char[2][14] = 'H';
	g_char[2][15] = '\0';

    g_char[3][0] = 'C';
	g_char[3][1] = ':';
	g_char[3][2] = ' ';
	g_char[3][13] = 'p';
	g_char[3][14] = 'F';
	g_char[3][15] = '\0';

    GrStringDrawCentered(&sContext, "Device ID: RLC", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 230, 0);


    while(1)
    {
   	   LCD_SetTextColor(B_COLOR);
	   LCD_FullRect(0,10, 240, 40);
	   LCD_FullRect(0,60, 240, 40);
	   LCD_FullRect(0,110, 240, 40);
	   LCD_FullRect(0,160, 240, 40);

//	   printf("%d\n", g_frequent);
	   num_to_char(g_char[0] + 3, g_frequent);
	   GrStringDrawCentered(&sContext, g_char[0], -1,
                         GrContextDpyWidthGet(&sContext) / 2, 30, 0);

	   num_to_char(g_char[1] + 3, g_r);
	   GrStringDrawCentered(&sContext, g_char[1], -1,
                         GrContextDpyWidthGet(&sContext) / 2, 80, 0);

	   num_to_char(g_char[2] + 3, 0);
	   GrStringDrawCentered(&sContext, g_char[2], -1,
                         GrContextDpyWidthGet(&sContext) / 2, 130, 0);

	   num_to_char(g_char[3] + 3, g_c);
	   GrStringDrawCentered(&sContext, g_char[3], -1,
                         GrContextDpyWidthGet(&sContext) / 2, 180, 0);

	   delay(30000000);
    }
}

//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
void
Timer2IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	//
    // Update the interrupt status on the display.
    //
    IntMasterDisable();
	g_frequent = (65535 - TimerValueGet(TIMER0_BASE, TIMER_A)) * g_rate;
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xffff);
	IntMasterEnable();

	g_c = 8796921 / g_frequent / 103 * 100; // 82k
	g_r = 1534781 / g_frequent / 103 * 100;

	if (g_frequent < 1000000)
	{
	    g_rate = 50;
        TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet() / g_rate);
	}

	if (g_frequent < 100000)
	{
	    g_rate = 5;
        TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet() / g_rate);
	}

	if (g_frequent < 10000)
	{
	   g_rate = 1;
       TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet() / g_rate);
	}
}

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_MATCH);
	g_rate = 200;
    TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet() / g_rate);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xffff); 
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer1IntHandler(void)
{
}


void UARTIntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}

void uart_send(unsigned char* c)
{
   while (*c)
   {
      UARTCharPutNonBlocking(UART0_BASE, *c++);
   }
}

void delay(volatile unsigned long i)
{
   while (i--)
   {
   }
}

void num_to_char(char* c, unsigned long num)
{
   unsigned long x = 100000000;
   unsigned long y = 10000000;
   unsigned char i = 0;

   if (num == 0)
   {
      for (i=0; i<10; ++i)
	  {
	     *c++ = '-';
	  }

	  return;
   }

   for (i=0; i<10; ++i)
   {
	  if ((i!=2) && (i!=6))
	  {
         *c++ = (char)(num % x / y) + 0x30;
	     x /= 10;
	     y /= 10;
	  }
	  else
	  {
	     *c++ = ',';
	  }
	}
}

int fputc(int c, FILE* a)
{
   UARTCharPutNonBlocking(UART0_BASE, (unsigned char)c);

   return c;
}

// End of file

