#ifndef __UART_CALC_H__
#define __UART_CALC_H__

#include "uart.h"
#include "gpio.h"
#include "cs.h"
void uart_baud_calc(eUSCI_UART_Config* uart_cfg, float clk_val, float baud);
void uart_init(uint32_t baud);
void UART_transmitBuff(unsigned char *transmitData);
void number_b(unsigned char *str ,long number);
#endif
