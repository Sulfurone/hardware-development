#ifndef __KEY_H
#define __KEY_H

void Key_Init(void);
void Key_Get(void);
uint8_t Key_Scan(void);

extern uint16_t input_integer;
extern uint8_t input_decimal, inputting, inputting_decimal, decimal_pos;

#endif

