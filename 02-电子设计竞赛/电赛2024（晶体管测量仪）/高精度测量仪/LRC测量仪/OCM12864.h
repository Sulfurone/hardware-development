#ifndef __OCM12864
#define __OCM12864


#include "driverlib/systemInit.h"
typedef unsigned char   uchar;
typedef unsigned int   uint;
/*
#define  lcddata_out      P3OUT
#define  lcddata_in       P3IN
#define  LCD_DOUT_EN      P3DIR=0XFF
#define  LCD_DIN_EN       P3DIR=0x00
#define  set_rs()         (P4DIR|=BIT3, P4OUT|=BIT3)	//都是输出
#define  clear_rs()       (P4DIR|=BIT3, P4OUT&=~BIT3) 
#define  set_rw()         (P4DIR|=BIT4, P4OUT|=BIT4)
#define  clear_rw()       (P4DIR|=BIT4, P4OUT&=~BIT4)
#define  set_en()         (P4DIR|=BIT5, P4OUT|=BIT5)
#define  clear_en()       (P4DIR|=BIT5, P4OUT&=~BIT5)
#define  set_cs()         (P4DIR|=BIT6, P4OUT|=BIT6)
#define  clear_cs()       (P4DIR|=BIT6, P4OUT&=~BIT6) 
#define  set_reset()      (P4DIR|=BIT7, P4OUT|=BIT7)
#define  clear_reset()    (P4DIR|=BIT7, P4OUT&=~BIT7) */
#define  LCD_DATA_PERIPH             SYSCTL_PERIPH_GPIOB
#define  LCD_DATA_PORT               GPIO_PORTB_BASE
#define  LCD_DATA_PINS				 0x000000FF
#define  LCD_DOUT_EN      			 GPIOPinTypeOut(LCD_DATA_PORT, LCD_DATA_PINS);
#define  LCD_DIN_EN       		 	 GPIOPinTypeIn(LCD_DATA_PORT, LCD_DATA_PINS);


#define  LCD_CTRL_PERIPH             SYSCTL_PERIPH_GPIOD
#define  LCD_CTRL_PORT               GPIO_PORTD_BASE
#define  LCD_CTRL_PINS				 (GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7)
#define  LCD_CTRL_EN       		 	 GPIOPinTypeOut(LCD_CTRL_PORT, 0x000000F8); //测试时序用	 ……………………
//见OCM12864P11
#define  LCD_RS_PIN               	 GPIO_PIN_3	  //指令数据选择位	
#define  LCD_RW_PIN               	 GPIO_PIN_4	  //读写操作选择 
#define  LCD_EN_PIN              	 GPIO_PIN_5	  //使能
#define  LCD_CS_PIN              	 GPIO_PIN_6	  //片选
#define  LCD_RESET_PIN               GPIO_PIN_7	  //复位

#define  set_rs()					GPIOPinWrite(LCD_CTRL_PORT, LCD_RS_PIN, 0xff); 
#define  clear_rs()       			GPIOPinWrite(LCD_CTRL_PORT, LCD_RS_PIN, 0x00); 
#define  set_rw()         			GPIOPinWrite(LCD_CTRL_PORT, LCD_RW_PIN, 0xff); 
#define  clear_rw()       			GPIOPinWrite(LCD_CTRL_PORT, LCD_RW_PIN, 0x00); 
#define  set_en()         			GPIOPinWrite(LCD_CTRL_PORT, LCD_EN_PIN, 0xff); 
#define  clear_en()      			GPIOPinWrite(LCD_CTRL_PORT, LCD_EN_PIN, 0x00); 
#define  set_cs()        			GPIOPinWrite(LCD_CTRL_PORT, LCD_CS_PIN, 0xff); 
#define  clear_cs()      			GPIOPinWrite(LCD_CTRL_PORT, LCD_CS_PIN, 0x00);  
#define  set_reset()      			GPIOPinWrite(LCD_CTRL_PORT, LCD_RESET_PIN, 0xff); 
#define  clear_reset()    			GPIOPinWrite(LCD_CTRL_PORT, LCD_RESET_PIN, 0x00); 


#define RC 2
#define RR 2 

////*****************////
////*****函数声明****////
////*****************////
void LCD_Delay(unsigned long v);
void check_busy(void);
void write_data(uchar dat);
void write_command(uchar command);
void reset(void);
void clear_screen(uchar m);
void lcd_initial(void);
void set_page_address(uchar page);
void set_colume_address(uchar colume);
void disp_word(uchar col,uchar pag,uchar ascii);
void disp_chn(uchar col,uchar pag,uchar ascii);
void disp_str(uchar col,uchar page,uchar *cn);
void point(uchar x,uchar y);
void disp_bmp( uchar const *puts );
void shuxian(uchar n);
void hengxian(uchar k);

void write(uchar a,uchar b);
void write_dat(uchar dat,uchar c,uchar b);
void write_add(uchar dat,uchar c);
//void write_int(uchar col,uchar pag,unsigned int);
#endif
