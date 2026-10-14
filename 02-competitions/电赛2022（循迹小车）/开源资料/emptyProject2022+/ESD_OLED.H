 #define byte  unsigned char
 #define word  unsigned int
 #define dword unsigned long
	
 void LCD_Init(void);
 void LCD_CLS(void);
 void LCD_P6x8Str(byte x,byte y,byte ch[]);
 void LCD_P8x16Str(byte x,byte y,byte ch[]);
 void LCD_P14x16Ch(byte x,byte y,byte n);
 void LCD_Fill(byte dat);
 void LCD_Set_Pos(unsigned char x, unsigned char y);
 void LCD_WrDat(unsigned char dat);
 void LCD_Write_a_char(byte x,byte y,byte ch);
 void LCD_Dis_Logo(void);
 void LCD_Dis_Main(void);
 void LCD_Add_Int(char x,char y,long num ,char n,char ack);
