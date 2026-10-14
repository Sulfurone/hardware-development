#include "uart_calc.h"


#define STEP_SIZE 36
static const uint8_t table_cfg[STEP_SIZE] =
{
    0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x11,0x21,0x22,0x44,0x25,0x49,0x4a,0x52,0x92,
    0x53,0x55,0xaa,0x6b,0xad,0xb5,0xb6,0xd6,0xb7,0xbb,0xdd,0xed,0xee,0xbf,0xdf,0xff,
    0xf7,0xfb,0xfd,0xfe
};
static const uint16_t table_sp[STEP_SIZE] =
{
    529,715,835,1001,1252,1430,1670,2147,2224,2503,3000,3335,3575,3753,40003,4286,
    4378,5002,5715,6003,6254,6432,6667,7001,7147,7503,7861,8004,8333,8464,8572,8751,
    9004,9170,9288,10001
};
void uart_init(uint32_t baud)
{

	eUSCI_UART_Config  My_Uart_Config;
	float smclk;
	
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
	
	My_Uart_Config.parity = EUSCI_A_UART_NO_PARITY;
	My_Uart_Config.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
	My_Uart_Config.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
	My_Uart_Config.uartMode = EUSCI_A_UART_MODE;
	My_Uart_Config.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
	smclk = CS_getSMCLK();
	uart_baud_calc(&My_Uart_Config,smclk,baud);
	UART_initModule(EUSCI_A0_BASE,&My_Uart_Config);
	UART_enableModule(EUSCI_A0_BASE);
}
void number_b(unsigned char *str ,long number)
{
	char i = 0, j = 0;
	unsigned char temp = 0;
	if(number < 10)
		*(str+i) = number+'0';
	else
	{
		while(1)
		{
			*(str+i) = number%10+'0';
			number = number/10;
			i++;
			
			if(number < 10)
			{
				*(str+i) = number+'0';
				break;
			}
		}
	
	}

for(j = i;j>i/2; j--)
		{
			temp = *(str+j);
			*(str+j) = *(str+i-j);
			*(str+i-j) = temp;
		}




}
void UART_transmitBuff(unsigned char *transmitData)
{
	while(*transmitData != '\0')
	{
		UART_transmitData(EUSCI_A0_BASE,*transmitData);
		transmitData++;

	}

}
void uart_baud_calc(eUSCI_UART_Config* uart_cfg, float clk_val, float baud)
{
    float N;
    uint8_t i=0;
    uint16_t n;

    if((!uart_cfg) || (baud == 0.0f))
        return;
    N = clk_val/ baud;
    if(N > 16)
    {
        //Using oversampling
        uart_cfg->overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
        uart_cfg->clockPrescalar = (uint32_t)(N/16);
        uart_cfg->firstModReg = (uint32_t)(16 * (N/16 - uart_cfg->clockPrescalar));
        uart_cfg->secondModReg = 1;
    }
    else
    {
        //Low freq
        uart_cfg->overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
        uart_cfg->clockPrescalar = (uint32_t)(N);
    }
    n = (uint16_t)((N - (uint32_t)N) * 10000);
    while(n > table_sp[i]) i++;
    uart_cfg->secondModReg = table_cfg[i];
}
