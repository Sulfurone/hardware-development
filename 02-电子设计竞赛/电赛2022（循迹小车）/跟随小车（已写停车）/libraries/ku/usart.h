#ifndef _USART_H
#define _USART_H

#include "main.h"

/* ´®¿Ú0 */
void Usart0tInit(void);
void USART0PutChar(unsigned char ch);
void USART0PutString(unsigned char *str);

/* ´®¿Ú1*/
#define BUFSIZE 5
 extern u8   BUF[BUFSIZE];
void Usart1tInit(void);
void USART1PutChar(unsigned char ch);
void USART1PutString(unsigned char *str);

#endif




