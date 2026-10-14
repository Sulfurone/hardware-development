#include "inc/hw_types.h"   
#include "inc/hw_memmap.h"   
#include "driverlib/sysctl.h"   
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systemInit.h"
#include "OCM12864.h"
#include "key.h"
#include "driverlib/adc.h"
typedef unsigned char   uchar;
typedef unsigned int   uint;
//  ¶¨ÒåLED
#define  LED_PERIPH             SYSCTL_PERIPH_GPIOD
#define  LED_PORT               GPIO_PORTD_BASE
#define  LED_PINS               GPIO_PIN_1 | GPIO_PIN_2
 #define date 1
  #define dat 2
#define com 0
unsigned int num;                        //ÔÝ´æ°´¼ü¶ÀÈÈÂëÊý¾Ý 
unsigned char key_value,flag1,flag2,flag3;                 //°´¼üÖµ1-16

uchar  table1[]={"1.ÐÅºÅ·¢ÉúÆ÷"};
uchar  table2[]={"2.µç×è¡¢µçÈÝ¡¢µç"};
uchar table3[]={"¸Ð²âÊÔÒÇ"};
uchar  table4[]={"ÆµÂÊ£º        HZ"};
uchar  table5[]={"·ùÖµ£º        V"};
uchar  table6[]={"1.µç×è: 1.1--1K2.1K-100K 3.100K-1M 4.1--5M"};
uchar table7[]={"2.µçÈÝ: 1.5-500pF 2.500p-50n3.50n-1uF 4.1u--15uF"};
uchar  table8[]={"3.µç¸Ð: 1.10-1000uH2.1m--200mH"};
uchar  table9[]={"²¨ÐÎ£º  ÏÒ²¨"};
uchar  table10[]={"3.×î½ü²âÊÔ"};
uchar  table11[]={"ÎÞ²âÊÔ½á¹û:"};
uchar  table12[]={"0123456789"};
uchar  table13[]={"Á¿³ÌÑ¡Ôñ:"};
uchar  table14[]={"²â  ÊÔ  ÒÇ"};
uchar a,b,flag,flag1,flag2,k,flag3,label,adr=1;
//float ,u1,u2,xx;
float rd;


#define  	FSYNC_10  GPIOPinWrite(RW_PORT,RW_PIN,0); //Ð´¿ØÖÆ
#define  	FSYNC_11  GPIOPinWrite(RW_PORT,RW_PIN,RW_PIN);
#define     SDATA_10  GPIOPinWrite(RS_PORT,RS_PIN,0); //Êý¾Ý/ÃüÁîÇÐ»»
#define     SDATA_11  GPIOPinWrite(RS_PORT,RS_PIN,RS_PIN);
#define     SCLK_10  GPIOPinWrite(RST_PORT,RST_PIN,0);  //¸´Î»   
#define     SCLK_11  GPIOPinWrite(RST_PORT,RST_PIN,RST_PIN);

#define ADCSequEnable ADCSequenceEnable
#define ADCSequDisable ADCSequenceDisable
#define ADCSequConfig ADCSequenceConfigure
#define ADCSequStepConfig ADCSequenceStepConfigure
#define ADCSequDataGet ADCSequenceDataGet
tBoolean ADC_EndFlag = false; // ¶¨ÒåADC×ª»»½áÊøµÄ±êÖ¾



/*¶¨ÒåÒº¾§µÄÊý¾Ý¶Ë¿Ú*/
#define DBS_PERIPH	SYSCTL_PERIPH_GPIOD
#define	DBS_PORT	GPIO_PORTD_BASE
#define	DBS_PINS	GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7

/*¶¨ÒåÒº¾§Êý¾ÝÃüÁîÑ¡Ôñ¶Ë */
#define	RS_PERIPH	SYSCTL_PERIPH_GPIOB
#define	RS_PORT		GPIO_PORTB_BASE
#define	RS_PIN		GPIO_PIN_0

/*¶¨ÒåÒº¾§¶ÁÐ´¿ØÖÆ¶Ë*/
#define	RW_PERIPH	SYSCTL_PERIPH_GPIOB
#define	RW_PORT		GPIO_PORTB_BASE
#define	RW_PIN		GPIO_PIN_1

/*Òº¾§Ê¹ÄÜ¶Ë*/
#define	E_PERIPH	SYSCTL_PERIPH_GPIOB
#define	E_PORT		GPIO_PORTB_BASE
#define	E_PIN		GPIO_PIN_2

/*Êý¾ÝÄ£Ê½Ñ¡Ôñ*/
#define	RST_PERIPH	SYSCTL_PERIPH_GPIOB
#define	RST_PORT	GPIO_PORTB_BASE
#define	RST_PIN		GPIO_PIN_3

/*Êý¾ÝÄ£Ê½Ñ¡Ôñ*/
#define	RD_PERIPH	SYSCTL_PERIPH_GPIOB
#define	RD_PORT	GPIO_PORTB_BASE
#define	RD_PIN		GPIO_PIN_4

#define FCLK 50
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

/**************************
·¢ËÍ¿ØÖÆ×Ö
**************************/
void Send_Con(uint Con_word)
{
uchar i;
FSYNC_10;
for(i=0;i<16;i++)
{
if((Con_word&0x8000)!=0) 
SDATA_11;
if((Con_word&0x8000)==0) 
SDATA_10; 
Con_word<<=1;
SCLK_10;
SCLK_11;
} 
FSYNC_11; 
}
/***********************************************************************************
³õÊ¼»¯º¯Êý£º
-------------------------------------------------------------------
¹¦ÄÜ£ºÈí¼þÎ»¿ØÖÆ£¬IOUTÕýÏÒ²¨£¬SIGNBITOUT·½²¨£¬Ð´FREQREG0£¬Ð´PHASE0
Send_Con(0x2038)   Ò»´ÎÐÔÐ´FREQREG0
Send_Con(0x0038)   µ¥¶À¸ÄÐ´FREQREG0µÄLSB
Send_Con(0x1038)   µ¥¶À¸ÄÐ´FREQREG0µÄMSB
-------------------------------------------------------------------
¹¦ÄÜ£ºÈí¼þÎ»¿ØÖÆ£¬IOUTÈý½Ç²¨Ð´FREQREG0£¬Ð´PHASE0
Send_Con(0x2002)   Ò»´ÎÐÔÐ´FREQREG0
Send_Con(0x0002)   µ¥¶À¸ÄÐ´FREQREG0µÄLSB
Send_Con(0x1008)   µ¥¶À¸ÄÐ´FREQREG0µÄMSB************************************************************************************/

/****************************
·¢ËÍÆµÂÊ×Ö
****************************/
void Send_Freq(uint Freq_word)
{
uchar i;
FSYNC_10;
for(i=0;i<16;i++)
{

if((Freq_word&0x8000)!=0) 
SDATA_11;
if((Freq_word&0x8000)==0)  
SDATA_10; 
Freq_word<<=1;
SCLK_10;
SCLK_11;
} 
FSYNC_11; 
}

/*****************************
·¢ËÍÏàÎ»×Ö
×¢£º¸ù¾ÝÇé¿ö¿ÉÉè¶¨Èë¿Ú²ÎÊý£¬ÉèÖÃÏàÎ»
*****************************/
void Send_Phase()
{
Send_Con(0xC000);   //ÉèÖÃ³õÏàÎ»Îª0
}


/*******************************************************
ÉèÖÃÆµÂÊÖµ   Freq_value(value)=Freq_data(data)*FCLK/2^28
2^28=268435456=0x10000000
******************************************************
void main(ulong Freq_value)
{
ulong value1,value2;
uint LSB_D,MSB_D;
SCLK_1=1;
FSYNC_1=1;
value1=Freq_value;
value2=Freq_value;
LSB_D=(value1/0x4000)+0x4000;  //Ð´FREQ0 REG µÄLSB
MSB_D=(value2%0x4000)+0x4000;   //Ð´FREQ0 REG µÄMSB
Send_Con(0x2038);    //DDS³õÊ¼»¯ÉèÖÃ£¬IOUTÕýÏÒ²¨£¬SIGNBITOUT·½²¨£¬Ò»´ÎÐÔÐ´FREQREG0
Send_Freq(LSB_D);
Send_Freq(MSB_D);
} */

void change(float b)
{
	uchar bai,shi,ge,p1,p2,p3;
	uint x;
	float a;
	 
/**********************************************************************
************************************µç×èÏÔÊ¾****************************
************************************************************************/	
	a=b;
	if(flag1==2)
	{	
		if(a<1000&&k==1)								//1--1000¦¸	Á¿³Ì	   
		{	
			if(rd<10)										   //0.000
			{
				x=a*1000;
				x=(int)a;
				ge=x/1000;
				p1=x%1000/100;
				p2=x%1000%100/10;
				p3=x%10;
				write(com,0x8a);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,p3+0x30);
									//¦¸ 
			}
			if(a<100&&a>10)										//00.00
			{
                x=a*100;
				x=(int)a;
				shi=x/1000;
				ge=x%1000/100;
				p1=x%1000%100/10;
				p2=x%10;
				write(com,0x8a);
				write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
									//¦¸
			}
			if(a>100)											  //000.0
			{
			 	x=a*10;
				x=(int)x;
				bai=x/1000;
				shi=x%1000/100;
				ge=x%1000%100/10;
				p1=x%10;
				write(com,0x8a);
				write(dat,bai+0x30);
				write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				
			}
										   
			write_dat(adr,a,1);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
		}


		else if(a>1000&&a<100000&&k==2)					 //1000--100k   
		{	
			if(a>1000&&a<10000)
			{													//0.000k
				x=(int)a;	
				ge=x/1000;
				p1=x%1000/100;
				p2=x%100/10;
				p3=x%10;
				write(com,0x8a);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,p3+0x30);
				write(dat,'K');
								//¦¸	
			}
			if(a>10000&&a<100000)											//00.00K
			{
				x=(int)a;
				shi=x/10000;
				ge=x%10000/1000;
				p1=x%10000%1000/100;
				p2=x%10000%1000%100/10;
				write(com,0x8a);
				write(dat,shi+0x30);	
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'K');
								//¦¸	
			}

			write_dat(adr,a,1);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
		}
	    else if(a>100000&&a<1000000&&k==3)			//100k--1m  	   
		{	  
			x=a/100;
			bai=x/1000;										//000.0k
			shi=x%1000/100;
			ge=x%1000%100/10;
			p1=x%10;
			write(com,0x8a);
			write(dat,bai+0x30);	
			write(dat,shi+0x30);
			write(dat,ge+0x30);
			write(dat,'.');
			write(dat,p1+0x30);
			write(dat,'K');
							//¦¸	

			write_dat(adr,a,1);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
						 
		}
		else if(a>1000000&&a<5000000&&k==4)				 //1m--5m    0.000m
		{
			a=a/1000;											//0.000m
			x=(int)a;
			ge=x/1000;
			p1=x%1000/100;
			p2=x%1000%100/10;
			p3=x%10;
			write(com,0x8a);
			write(dat,ge+0x30);
			write(dat,'.');
			write(dat,p1+0x30);
			write(dat,p2+0x30);
			write(dat,p3+0x30);
			write(dat,'M');	 
							//¦¸	

			write_dat(adr,a,1);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
		}
	}

/**********************************************************************
************************************µçÈÝÏÔÊ¾****************************
************************************************************************/
    if(flag2==2)
	{	 
		 if(a>5&&a<500&&k==1)					 //5-10p
		 {
		 	if(a>5&&a<10)
			{									   //0.00
				x=(uint)a;
				p1=(uint)(a-x)*10;
				p2=(uint)((a-x)*100)%10;
				write(com,0x8a);
			    write(dat,x+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'p');
			    write(dat,'F');
			}
		 	if(a>10&&a<100)						   //10-100p
			{									   //00.00
				x=(uint)a;
				shi=x/10;
				ge=x%10;
				x=(uint)(a-x)*100;
				p1=x/10;
				p2=x%10;
				write(com,0x8a);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'p');
			    write(dat,'F');	
			}
			if(a>100&&a<500)						//00.00p
			{
				x=(uint)a;
				bai=x/100;
				shi=x%100/10;
				ge=x%10;
				p1=(uint)(a-x)*10;
				write(com,0x8a);
			    write(dat,bai+0x30);
				write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,'p');
			    write(dat,'F');
			}
			write_dat(adr,a,2);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
		 }
		 else if(a>500&&a<50000&&k==2)
		 {
		 	if(a>500&&a<1000)
			{
				x=(uint)a;								   //00.00p
				bai=x/100;
				shi=x%100/10;
				ge=x%10;
				p1=(uint)(a-x)*10;
				write(com,0x8a);
				write(dat,bai+0x30);
				write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,'p');
			    write(dat,'F');	
			}
		 	 if(a>1000&&a<10000)
			 {
			 	x=(uint)a;
				ge=x/1000;
				p1=x%1000/100;
				p2=x%1000%100/10;
				p3=x%10;
				write(com,0x8a);				  //0.000n
			    write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,p3+0x30);
				write(dat,'n');
			    write(dat,'F');
			 }
			 if(a>10000&&a<50000)
			 {
			 	x=(uint)a;
				shi=x/10000;
				ge=x%10000/1000;
				p1=x%10000%1000/100;			  //00.00n
				p2=x%10000%1000%100/10;
				write(com,0x8a);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'n');
			    write(dat,'F');
			 }

			write_dat(adr,a,2);
			adr++;
		    if(adr==11)
				adr=1;
			write_add(200,adr);
		 }
		 else if(a>50000&&a<1000000&&k==3)
		 {
		 	if(a>50000&&a<100000)
			{
				x=(uint)a;
				shi=x/10000;
				ge=x%10000/1000;					//00.00n
				p1=x%10000%1000/100;
				p2=x%10000%1000%100/10;
				write(com,0x8a);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'n');
			    write(dat,'F');
			}
			if(a>100000&&a<1000000)
			{
				x=(uint)a;
				bai=x/100000;
				shi=x%100000/10000;
				ge=x%100000%10000/1000;
				p1=x%100000%10000%1000/100;
				write(com,0x8a);
				write(dat,bai+0x30);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,'n');
			    write(dat,'F');
			}

			write_dat(adr,a,2);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
		 }
	}
/**********************************************************************
************************************µç¸ÐÏÔÊ¾****************************
************************************************************************/
	if(flag3==2)
	{
		if(a>10&&a<1000&&k==1)
		{
			if(a>10&&a<100)
			{
				x=(uint)a;
				shi=x/10;
				ge=x%10;
				p1=(uint)(a-x)*10;
				p2=(uint)((a-x)*100)%10;
				write(com,0x8a);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'u');
			    write(dat,'H');
			}
			if(a>100&&a<1000)
			{
				x=(uint)a;
				bai=x/100;
				shi=x%100/10;
				ge=x%10;
				p1=(uint)(a-x)*10;
				write(com,0x8a);
				write(dat,bai+0x30);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,'u');
			    write(dat,'H');
			}

			write_dat(adr,a,3);
			adr++;
		    if(adr==9)
				adr=1;
			
		}
		else if(a>1000&&a<200000&&k==2)
		{
			 if(a>1000&&a<10000)
			 {
			 	x=(uint)a;
				ge=x/1000;
				p1=x%1000/100;
				p2=x%1000%100/10;
				p3=x%10;
				write(com,0x8a);				  //0.000m
			    write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,p3+0x30);
				write(dat,'m');
			    write(dat,'H');
			 }
			 if(a>10000&&a<100000)							 //00.00m
			 {
			 	x=(uint)a;
				shi=x/10000;
				ge=x%10000/1000;
				p1=x%10000%1000/100;
				p2=x%10000%1000%100/10;
				write(com,0x8a);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,p2+0x30);
				write(dat,'m');
			    write(dat,'H');
			 }
			 if(a>100000&&a<200000)
			 {												   //000.0m
			 	x=(uint)a;
				bai=x/100000;
				shi=x%100000/10000;
				ge=x%100000%10000/1000;
				p1=x%100000%10000%1000/100;
				write(com,0x8a);
				write(dat,bai+0x30);
			    write(dat,shi+0x30);
				write(dat,ge+0x30);
				write(dat,'.');
				write(dat,p1+0x30);
				write(dat,'m');
			    write(dat,'H');
			 }
			 

			write_dat(adr,a,3);
			adr++;
		    if(adr==9)
				adr=1;
			write_add(200,adr);
		}	
	}


} 


												
void adcInit(void)
{
		SysCtlLDOSet(SYSCTL_LDO_2_75V);    //ÅäÖÃPLLÇ°Ðè½«LDOµçÑ¹ÉèÖÃÎª2.75V                    
		        SysCtlClockSet(SYSCTL_USE_PLL |    // ÏµÍ³Ê±ÖÓÉèÖÃ£¬²ÉÓÃPLL                          
		        SYSCTL_OSC_MAIN |                  // Ö÷Õñµ´Æ÷          
		  SYSCTL_XTAL_6MHZ |                 // Íâ½Ó6MHz¾§Õñ         
		  SYSCTL_SYSDIV_4);        // ·ÖÆµ½á¹ûÎª50MHz
		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC); // Ê¹ÄÜADCÄ£¿é
		SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS); // ÉèÖÃADC²ÉÑùËÙÂÊ
		ADCSequDisable(ADC_BASE, 0); // ÅäÖÃÇ°ÏÈ½ûÖ¹²ÉÑùÐòÁÐ
		// ²ÉÑùÐòÁÐÅäÖÃ£ºADC»ùÖ·£¬²ÉÑùÐòÁÐ±àºÅ£¬´¥·¢ÊÂ¼þ£¬²ÉÑùÓÅÏÈ¼¶
		ADCSequConfig(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
		// ²ÉÑù²½½øÉèÖÃ£ºADC»ùÖ·£¬²ÉÑùÐòÁÐ±àºÅ£¬²½Öµ£¬Í¨µÀÉèÖÃ
		ADCSequStepConfig(ADC_BASE, 0, 0, ADC_CTL_CH0 |
		ADC_CTL_END |
		ADC_CTL_IE);
		ADCIntEnable(ADC_BASE, 0); // Ê¹ÄÜADCÖÐ¶Ï
		IntEnable(INT_ADC0); // Ê¹ÄÜADC²ÉÑùÐòÁÐÖÐ¶Ï
		IntMasterEnable( ); // Ê¹ÄÜ´¦ÀíÆ÷ÖÐ¶Ï
		ADCSequEnable(ADC_BASE, 0); // Ê¹ÄÜ²ÉÑùÐòÐ
}
// ADC²ÉÑù
unsigned long adcSample(void)
{
unsigned long ulValue;
ADCProcessorTrigger(ADC_BASE, 0); // ´¦ÀíÆ÷´¥·¢²ÉÑùÐòÁÐ
while (!ADC_EndFlag); // µÈ´ý²ÉÑù½áÊø
ADC_EndFlag = false; // Çå³ýADC²ÉÑù½áÊø±êÖ¾
ADCSequDataGet(ADC_BASE, 0, &ulValue); // ¶ÁÈ¡ADC×ª»»½á¹û
return(ulValue);
}

void scan()
{
	uchar i,j,k,flag,n;
	if(Key_Get()!=0xff)
		{
			//delay(20);		//Ïû¶¶
			switch(Key_Get())
			{
				case 0xe7:	//a1=0;	 
/***********************************Ö÷²Ëµ¥*******************************************/							
				         if(flag==0)										  //¹¦ÄÜÑ¡Ôñ
						{	
							j++;
							if(j==1)
							{	 write(com,0x80);
						         write(com,0x0d);
							}    
							 if(j==2)
							 {	
							 	write(com,0x90);
						        write(com,0x0d);
							}
							if(j==3)
							 {	
							 	write(com,0x98);
						        write(com,0x0d);
							}
							if(j==4)
							 {	
							 	j=0;
						        write(com,0x0c);
							}	
						}
						 if(flag==2)									//Æ÷¼þÑ¡Ôñ
						{ 
							n++;
							if(n==1)
							{	 
								write(com,0x90);
						        write(com,0x0d);
							}    
							 if(n==2)
							 {	
							 	write(com,0x88);
						        write(com,0x0d);
							}
							if(n==3)
							 {	
							 	write(com,0x98);
						        write(com,0x0d);
							}
							if(n==4)
							 {	
							 	n=0;
						        write(com,0x0c);
							}	  
						}
						 if(flag==3&&flag1!=2)							   //µç×èÁ¿³ÌÇÐ»»
						 {
						 	k++;
							if(k==1)
							{	 
								write(com,0x83);
						        write(com,0x0d);
							}    
							 if(k==2)
							 {	
							 	write(com,0x93);
						        write(com,0x0d);
							}
							if(k==3)
							 {	
							 	write(com,0x8b);
						        write(com,0x0d);
							}
							if(k==4)
							 {	
								write(com,0x9b);
						        write(com,0x0d);
						        
							}
							if(k==5)
							{
								k=0;
								write(com,0x0c);
							}
							flag1=1;
						 }
						 if(flag==4&&flag2!=2)							   //µçÈÝÁ¿³ÌÇÐ»»
						 {
						 	k++;
							if(k==1)
							{	 
								write(com,0x83);
						        write(com,0x0d);
							}    
							 if(k==2)
							 {	
							 	write(com,0x93);
						        write(com,0x0d);
							}
							if(k==3)
							 {	
							 	write(com,0x8b);
						        write(com,0x0d);
							}
							if(k==4)
							 {	
							 	write(com,0x9b);
						        write(com,0x0d);
							}
							if(k==5)
							{
								k=0;
								write(com,0x0c);
							}

							flag2=1;
						 }	
						 if(flag==5&&flag3!=2)							   //µç¸ÐÁ¿³ÌÇÐ»»
						 {
						 	k++;
							if(k==1)
							{	 
								write(com,0x83);
						        write(com,0x0d);
							}    
							 if(k==2)
							 {	
							 	write(com,0x93);
						        write(com,0x0d);
							}
							if(k==3)
							{
								k=0;
								write(com,0x0c);
							}
							flag3=1;
						 }		 
							break;						 
				case 0xeb://	a1=1;							  		 //È·¶¨¼ü
/******************************************È·¶¨µç×èÁ¿³Ì**************************************/

								if(flag1==1)
								{ 
							  	if(k==1)
							   {
							   	 
							   		 
								  write(com,0x80);
								  write(com,0x0c);
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);
								  write(date,':');
								  write(date,' ');					  
								  for(i=10;i<15;i++)						  //	1.1-1K
										write(date,table6[i]);
								 
								  	write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }   
							   if(k==2)
							   {   
							     
							   	  
								  write(com,0x80);
								  write(com,0x0c);
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);		
								  write(date,':');
								  write(date,' ');				
								  for(i=17;i<25;i++)						  			//2.1K-100K
										write(date,table6[i]);
									
									write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   if(k==3)
							   {   
							   	  
								  write(com,0x80);
								  write(com,0x0c);			
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);			
									write(date,':');
								  write(date,' ');
								  for(i=27;i<35;i++)						     //   3.100K--1M 
										write(date,table6[i]);
									
									write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   if(k==4)
							   {  
							   	  
								  write(com,0x80);
								  write(com,0x0c);
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);
									write(date,':');
								  write(date,' ');
								  for(i=37;i<43;i++)								//	 4.1--5M
										write(date,table6[i]);
												 	  
									write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);      
								   
							   }	 
							   flag1=2;  			   
							   change(RR);
							}
/******************************************È·¶¨µçÈÝÁ¿³Ì**************************************/
							if(flag2==1)
							{ 
							   if(k==1)
							   {
							   	 
								  write(com,0x80);
								  write(com,0x0c);
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);	
									write(date,':');
								  write(date,' ');					 
								  for(i=10;i<18;i++)						   //1.5-500p
									write(date,table7[i]);
								  write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   if(k==2)
							   {
							   	  						
								 write(com,0x80);
								 write(com,0x0c);
								 for(i=0;i<4;i++)	
								  	write(date,table13[i]);
									write(date,':');
								  write(date,' ');
								 for(i=20;i<28;i++)						   //2.500p-50n
									write(date,table7[i]);

								write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   if(k==3)
							   {
							   	  
								  write(com,0x80);
								  write(com,0x0c);
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);
									write(date,':');
								  write(date,' ');
								 for(i=30;i<38;i++)						   //3.50n-1U
									write(date,table7[i]);
									write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   if(k==4)
							   {
							   	 	 
								  write(com,0x80);
								  write(com,0x0c);
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);
									write(date,':');
								  write(date,' ');
								 for(i=40;i<49;i++)						   //3.1u-15U
									write(date,table7[i]);
									write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   flag2=2;	
							   
							    change(RC);
/**************************************************************************************
************************************ÕâÀïÐ´µçÈÝ²âÊÔ½á¹û************************************
************************************************************************************/
							}

/******************************************È·¶¨µç¸ÐÁ¿³Ì**************************************/
							if(flag3==1)
							{ 
							   if(k==1)
							   {
							   	  	 
								  write(com,0x80);
								  write(com,0x0c);	
								  for(i=0;i<4;i++)	
								  	write(date,table13[i]);				
									write(date,':');
								  write(date,' ');	 
								  for(i=10;i<19;i++)						   //1.10-1000u
									write(date,table8[i]);
								  write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   if(k==2)
							   {
							   	 	 						
								 write(com,0x80);
								 write(com,0x0c);
								 for(i=0;i<4;i++)	
								  	write(date,table13[i]);
									write(date,':');
								  write(date,' ');
								 for(i=21;i<30;i++)						   //2.1m--100m
									write(date,table8[i]);
									write(com,0x90);
								  for(i=2;i<12;i++)
								  	   write(date,table11[i]);
							   }
							   flag3=2;			  
							   

							  
/**************************************************************************************
************************************ÕâÀïÐ´µç¸Ð²âÊÔ½á¹û************************************
************************************************************************************/
							}

						   if(j==1)
						   {
						    
							 write(com,0x80);
							 write(com,0x0c);							 
							 for(i=2;i<14;i++)							  //ÐÅºÅ·¢ÉúÆ÷
								write(date,table1[i]);
							 
							 write(com,0x90);
							 for(i=0;i<12;i++)							   //ÕýÏÒ²¨
								write(date,table9[i]);
											 //Õý

							 write(com,0x88);
							 for(i=0;i<16;i++)							  //ÆµÂÊ
								write(date,table4[i]);

	//						 write(com,0x98);
	//						 for(i=0;i<16;i++)							   //·ùÖµ
	//							write(date,table5[i]);
	//						flag=1;
						   }
/**************************************²Ëµ¥¶þ***********************************************/						    
						   if(j==2&&n==0&&flag==0)								 
						   {
				 
							 write(com,0x80);
							 write(com,0x0c);
							 for(i=2;i<8;i++)						  //²âÁ¿ÒÇ
								write(date,table3[i]);
							 
							 write(com,0x90);
							 for(i=0;i<6;i++)						   //µç×è
								write(date,table6[i]);			 

							 write(com,0x88);
							 for(i=0;i<6;i++)					  //µçÈÝ
								write(date,table7[i]);

							 write(com,0x98);
							 for(i=0;i<6;i++)						//µç¸Ð
								write(date,table8[i]);
							flag=2;
						   }
/*********************************Á¿³Ì²Ëµ¥************************************************/
						  if(n!=0)
						   {   
							   if(n==1)
							   { 
						 n=0; 
							   	 
								 write(com,0x0c);
								 write(com,0x0c);
								 write(com,0x80);

								 for(i=0;i<4;i++)						   
									write(date,table13[i]);
								 write(com,0x90);							 //Á¿³ÌÑ¡Ôñ
								 for(i=4;i<9;i++)						   
									write(date,table13[i]);

								write(com,0x83);
								for(i=8;i<15;i++)							   //1.1-1k
								 	write(date,table6[i]);			  
													   //¦¸
								write(com,0x93);
								for(i=15;i<25;i++)						   //  2.1k-1k0K 
									write(date,table6[i]);
								write(com,0x8B);
								for(i=25;i<35;i++)						   //  3.10kK--1M 
									write(date,table6[i]);
								write(com,0x9B);
								for(i=35;i<43;i++)						   //  4.1--5M
									write(date,table6[i]);
							
								flag=3;
							   }
							   if(n==2)
							   { 
						 n=0; 
							   
								 write(com,0x0c);
								 write(com,0x0c);
								 write(com,0x80);
								 for(i=0;i<4;i++)						   
									write(date,table13[i]);
								 write(com,0x90);							 //Á¿³ÌÑ¡Ôñ
								 for(i=4;i<9;i++)						   
									write(date,table13[i]);

								write(com,0x83);
								 for(i=8;i<18;i++)						   //1.5-500p
									write(date,table7[i]);
								 write(com,0x93);
								 for(i=18;i<28;i++)						   //2.500p-50n
									write(date,table7[i]);
								 write(com,0x8b);
								 for(i=28;i<38;i++)						   //3.50n-1u
									write(date,table7[i]);
								 write(com,0x9b);
								  for(i=38;i<48;i++)						   //3.1u-15u
									write(date,table7[i]);
								 flag=4;
							   }
							   if(n==3)
							   {  
							 n=0; 
							  
								 write(com,0x0c);
								 write(com,0x0c);
								 write(com,0x80);
								 for(i=0;i<4;i++)						   
									write(date,table13[i]);
								 write(com,0x90);							 //Á¿³ÌÑ¡Ôñ
								 for(i=4;i<9;i++)						   
									write(date,table13[i]);

								write(com,0x83);
								 for(i=8;i<18;i++)						   //1.10-1000u
									write(date,table8[i]);
								 write(com,0x93);
								 for(i=19;i<29;i++)						   //2.1m--100m
									write(date,table8[i]);
								 flag=5;
							   }
							   }


			 }


}

}
//  Ö÷º¯Êý£¨³ÌÐòÈë¿Ú£©
int main(void)
{
         //»º´æÉÏÒ»´Î°´¼üÖµ
   uint LSB_D,MSB_D;
   uint value1,value2;
   unsigned long ulVal;

    jtagWait();                                             //  ·ÀÖ¹JTAGÊ§Ð§£¬ÖØÒª£¡
    clockInit();                                            //  Ê±ÖÓ³õÊ¼»¯£º¾§Õñ£¬6MHz

   lcd_initial();
   Key_Initial();                  //°´¼ü³õÊ¼»¯

   LCD_Delay(10);
   write_command(0xa6);


	SCLK_11;
	FSYNC_11;
	LSB_D=(value1/0x4000)+0x4000;  //Ð´FREQ0 REG µÄLSB
	MSB_D=(value2%0x4000)+0x4000;   //Ð´FREQ0 REG µÄMSB
	Send_Con(0x2038);    //DDS³õÊ¼»¯ÉèÖÃ£¬IOUTÕýÏÒ²¨£¬SIGNBITOUT·½²¨£¬Ò»´ÎÐÔÐ´FREQREG0
	Send_Freq(LSB_D);
	Send_Freq(MSB_D);   	 adcInit( ); 

  while(1)
  {
   // ADC³õÊ¼»¯

	ulVal = adcSample( ); // ADC²ÉÑù
	ulVal = (ulVal * 3000) / 1024; // ×ª»»³ÉµçÑ¹Öµ

    scan();     


  }
}


void ADC_Sequence_0_ISR(void)
{
unsigned long ulStatus;
ulStatus = ADCIntStatus(ADC_BASE, 0, true); // ¶ÁÈ¡ÖÐ¶Ï×´Ì¬
ADCIntClear(ADC_BASE, 0); // Çå³ýÖÐ¶Ï×´Ì¬£¬ÖØÒª
if (ulStatus != 0) // Èç¹ûÖÐ¶Ï×´Ì¬ÓÐÐ§
{
ADC_EndFlag = true; // ÖÃÎ»ADC²ÉÑù½áÊø±êÖ¾
}
}

