#include "led.h"
#include "adc.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "AD9854.h"
#include "stm32f10x.h"
#include "pcap.h"
#include "stdio.h"
#include "bsp_spi.h"
unsigned long cap_status = 0,cap_reszult0 = 0 ,cap_temp = 0,res_ref=0;
float cap_val = 0.0f;
float  final_cap=0.0f ,last_cap=0.0f;
float cha,cha_sum,cha_average,dianrong_cap;
unsigned char cap[7] = {'4','3','5','.','2','1','\n'};
unsigned int cap_bw,cap_sw,cap_gw,cap_int,cap_xs1,cap_xs2;
unsigned char wufuzai_flag=0,youfuzai_flag=0;
unsigned char jieguo_flag=0,jiance_flag=0;
unsigned char kailu_flag=0,dianzu_flag=0,dianrong_flag=0;
unsigned char fuzaijiance_flag=0;
float longth = 0.0f;
int i,n;
char txt[30];
void capval_to_capstring(void);//把cap_val转换到数组unsigned char cap[7]中
 void gpio_init();
 int main(void)
 {	 
		u8 x=0;
		u16 adcx;
		u8 key;
		float temp;
		u8 lcd_id[12];			//存放LCD ID字符串
		delay_init();	    	 //延时函数初始化	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		uart_init(115200);	 	//串口初始化为115200
		LED_Init();			     //LED端口初始化
		LCD_Init();
		Adc_Init();
		KEY_Init();
	  delay_ms(100);
	  SPI_Configuration();
	  pcap01_init();
	 
		AD9854_Init();
	  AD9854_SetSine_double(10000,4095);
		AD9854_InitRFSK();//初始化FSK扫频
//		AD9854_SetSine_double(35000000,4095);
//		uart_init(115200);
	//	LCD_ShowString(0,240,200,16,16,"ADC VOL:0.000V");
//		printf("\n RES1(Cap)= %.0f / Cap_ratio= %f", 5.2, 6.3);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			while(1) 
		{		 
			
			key=KEY_Scan(1);		
			
			SPI_Disable();
	    __3nop();
      write_date_8(0x48);   //addr8 Status  不理解为什么要写入0x48
      __3nop();
      cap_status = spi_read();//返回结果应该为0x100000或0x900000
		  SPI_Disable();
	    __3nop();
      write_date_8(0x41);   //addr0 Res0  0-7.9999  0b01xxxxxx  不理解为什么要写入0x41
      __3nop();
		  cap_reszult0 = spi_read();
			SPI_Disable();
	    __3nop();
      write_date_8(0x4E);   //addr0 Res0  0-7.9999  0b01xxxxxx  不理解为什么要写入0x41
      __3nop();
		  res_ref = spi_read();
		
			cap_temp = 0x1fffff;
			cap_val = (double)cap_reszult0/(double)cap_temp;
			cap_val = cap_val*7;//cap_val采集到的电容值
//			cap_val = cap_val*103;
			capval_to_capstring();
//			adcx=Get_Adc_Average(ADC_Channel_1,10);
//			adcx=filter();
			LCD_ShowString(0, 300, 60, 16, 16,cap);
//			LCD_ShowxNum(0,240,final_cap,4,24,0);
//			LCD_ShowxNum(0,264,longth,4,24,0);
//				LCD_ShowxNum(0,264,adcx,4,24,0);		
			Show_Chinese(72,0,4,24,1,0xFFFF,0);  //显示电赛
			Show_Chinese(96,0,5,24,1,0xFFFF,0);
			
			Show_Chinese(0,48,31,24,1,0xFFFF,0);  //显示工作状态 :
			Show_Chinese(24,48,32,24,1,0xFFFF,0);			
			Show_Chinese(48,48,6,24,1,0xFFFF,0);  
			Show_Chinese(72,48,7,24,1,0xFFFF,0);

			Show_Chinese(96,48,14,24,1,0xFFFF,0);
			
		if(jiance_flag==1)
		{
				Show_Chinese(120,48,15,24,1,0xFFFF,0);
				Show_Chinese(144,48,16,24,1,0xFFFF,0);	
				Show_Chinese(168,48,17,24,1,0xFFFF,0);
				Show_Chinese(192,48,18,24,1,0xFFFF,0);		
		}
		else if(jieguo_flag == 1)
		{
				Show_Chinese(122,48,19,24,1,0xFFFF,0);
				Show_Chinese(144,48,20,24,1,0xFFFF,0);	
				Show_Chinese(168,48,21,24,1,0xFFFF,0);
				Show_Chinese(192,48,22,24,1,0xFFFF,0);
		}
		Show_Chinese(0,96,35,24,1,0xFFFF,0);  //显示电缆长度
		Show_Chinese(24,96,36,24,1,0xFFFF,0)	;	
		Show_Chinese(48,96,8,24,1,0xFFFF,0);  
		Show_Chinese(72,96,9,24,1,0xFFFF,0);
		Show_Chinese(96,96,14,24,1,0xFFFF,0);
		if(jieguo_flag ==1)
		{
			  sprintf (txt,"%.2f",longth);	
			  
			  LCD_ShowString(120,96,200,24,24,txt);
			  LCD_ShowString(210,96,200,24,24,"CM");
//		    LCD_ShowxNum(120,96,longth,4,24,0);     	//显示longth
		
		}

		
		Show_Chinese(0,144,33,24,1,0xFFFF,0);  //显示负载类型
		Show_Chinese(24,144,34,24,1,0xFFFF,0);
		Show_Chinese(48,144,10,24,1,0xFFFF,0);  //显示负载类型
		Show_Chinese(72,144,11,24,1,0xFFFF,0);		
		Show_Chinese(96,144,14,24,1,0xFFFF,0);
		if(kailu_flag==1)
		{
			Show_Chinese(120,144,23,24,1,0xFFFF,0);
		  Show_Chinese(144,144,24,24,1,0xFFFF,0);	
		}
		else if(dianzu_flag==1)
		{
			Show_Chinese(120,144,25,24,1,0xFFFF,0);
		  Show_Chinese(144,144,26,24,1,0xFFFF,0);
			LCD_ShowxNum(120,192,21,4,24,0);			
			Show_Chinese(168,192,29,24,1,0xFFFF,0); //oumu
		}
    else if(dianrong_flag==1)
		{
			Show_Chinese(120,144,27,24,1,0xFFFF,0);
			Show_Chinese(144,144,28,24,1,0xFFFF,0);
			
			LCD_ShowxNum(120,192,dianrong_cap,3,24,0); 	//显示电压值的小数部分
			LCD_ShowString(168,192,200,24,24,"pF");
		}

		Show_Chinese(0,192,33,24,1,0xFFFF,0);  //显示参数
		Show_Chinese(24,192,34,24,1,0xFFFF,0);		
		Show_Chinese(48,192,12,24,1,0xFFFF,0);  
		Show_Chinese(72,192,13,24,1,0xFFFF,0);
		Show_Chinese(96,192,14,24,1,0xFFFF,0);

 				if(key==KEY0_PRES)	 //长度检测
			{
				kailu_flag =0;
				jieguo_flag =0;
				if(final_cap<0.75)	 wufuzai_flag=0;
				if(final_cap>0.75&&wufuzai_flag==0&&youfuzai_flag==0)   youfuzai_flag=1;				
			}
			if(key==KEY1_PRES)  //负载检测
			{
				fuzaijiance_flag = 1;
			}
	
	if(youfuzai_flag==1)
	{
		jiance_flag=1;
//		if(final_cap<10) longth = 42.5 * final_cap - 25;
//		if(final_cap<20&&final_cap>10) longth = 42.5 * final_cap - 20;
//		if(final_cap<30&&final_cap>20) longth = 42.5 * final_cap - 32;
//		if(final_cap<35&&final_cap>30) longth = 42.5 * final_cap - 25;
//		if(final_cap<40&&final_cap>35) longth = 42.5 * final_cap - 10;
//		if(final_cap<45&&final_cap>40) longth = 42.5 * final_cap - 20;
//		if(final_cap<50&&final_cap>45) longth = 42.5 * final_cap - 20;
		if(final_cap>=10)	longth = 44.1657*final_cap - 59.6713;
		if(final_cap>3.3&&final_cap<10)	longth = 44.787*final_cap - 45.6713;
		if(final_cap<3.3&&final_cap>0.7)	longth = 41.9448*final_cap - 26.8753;
		
		
		if(longth >2000.0) longth =2000;
		if(longth <0) longth =0;
		
		i++;
		if(i>50) 
		{
			  i=0;	
			  youfuzai_flag =0;
				jieguo_flag =1;
				jiance_flag=0;
//			  kailu_flag=1;
			  last_cap = final_cap ;
		}
	}
	
	if(fuzaijiance_flag==1)
	{
		jiance_flag=1;
		cha = final_cap-last_cap  ;
		for(n=0;n<10;n++)
		{
		    cha_sum +=cha;
		
		}
		cha_average  = cha_sum/10;
		if(cha_average >1&&final_cap <56) 
		{
					dianrong_flag =1;
			    dianrong_cap = cha *46.6-1.128;
		}
	  else if(final_cap>=56)
		{
			dianzu_flag =1;
		
		}
		i++;
		if(i>50) 
		{
			  i=0;	
			  fuzaijiance_flag =0;
				jieguo_flag =1;
				jiance_flag=0;
		}
	}
	
		
		} 
		
}
void capval_to_capstring(void)
{
			cap_int=(unsigned int)cap_val;

			cap_bw=cap_int/100;
			cap_sw=cap_int/10%10;
			cap_gw=cap_int%10;

			cap_val=cap_val-cap_int;
			cap_val=cap_val*100;
			cap_int=(unsigned int)cap_val;

			cap_xs1 = cap_int/10;
			cap_xs2 = cap_int%10;


			cap[0] =cap_bw + '0';
			cap[1] =cap_sw + '0';
			cap[2] = cap_gw + '0';
			cap[4] = cap_xs1 + '0';
			cap[5] = cap_xs2 + '0';
	final_cap = cap_bw*100+cap_sw*10+cap_gw*1+cap_xs1*0.1+cap_xs2*0.01;
		
}
