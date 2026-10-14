#ifndef _4X4_H
#define _4X4_H
#include  "main.h"
 
#define a1_PORT  GPIO_PORT_P8
#define a1_PIN   GPIO_PIN5
#define a2_PORT  GPIO_PORT_P9
#define a2_PIN   GPIO_PIN0
#define a3_PORT  GPIO_PORT_P8
#define a3_PIN   GPIO_PIN4
#define a4_PORT  GPIO_PORT_P8
#define a4_PIN   GPIO_PIN2

#define l1_PORT  GPIO_PORT_P8
#define l1_PIN   GPIO_PIN6
#define l2_PORT  GPIO_PORT_P8
#define l2_PIN   GPIO_PIN7
#define l3_PORT  GPIO_PORT_P9
#define l3_PIN   GPIO_PIN1
#define l4_PORT  GPIO_PORT_P8
#define l4_PIN   GPIO_PIN3


#define A1  GPIO_getInputPinValue(a1_PORT,a1_PIN) 
#define A2  GPIO_getInputPinValue(a2_PORT,a2_PIN) 
#define A3  GPIO_getInputPinValue(a3_PORT,a3_PIN) 
#define A4  GPIO_getInputPinValue(a4_PORT,a4_PIN) 

#define L1  GPIO_getInputPinValue(l1_PORT,l1_PIN) 
#define L2  GPIO_getInputPinValue(l2_PORT,l2_PIN) 
#define L3  GPIO_getInputPinValue(l3_PORT,l3_PIN) 
#define L4  GPIO_getInputPinValue(l4_PORT,l4_PIN) 

extern int AA;
extern int LL;

void aInit(void);
void lInit(void);
u8 KEY_Scan(void);
void shu(void);

#endif
