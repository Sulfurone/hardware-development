#include "log.h"


void Log(char *fmt,...)
{
 
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//∏Ò ΩªØ
	va_end(ap);
	
	while(*pStr != NULL)
	{
        HAL_UART_Transmit (USART_DEBUG ,(uint8_t *)pStr++,1,HAL_MAX_DELAY );		
	}
 
}

