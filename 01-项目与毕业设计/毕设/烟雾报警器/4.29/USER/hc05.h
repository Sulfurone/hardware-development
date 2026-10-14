#ifndef __HC05_H
#define __HC05_H

void HC05_Init(void);
void HC05_EnterAT(void);
void HC05_ExitAT(void);
void HC05_SendString(char *Buf);
void HC05_GetData(char *Buf);

#endif

