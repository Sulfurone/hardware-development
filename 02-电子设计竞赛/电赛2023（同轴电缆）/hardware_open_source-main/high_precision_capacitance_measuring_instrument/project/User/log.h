#ifndef __LOG_H__
#define __LOG_H__


#include "usart.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define USART_DEBUG		&huart1

void Log(char *fmt,...);


#endif
