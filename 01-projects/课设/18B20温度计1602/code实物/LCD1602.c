#include <REGX52.H>
sbit keyu=P3^0;
sbit keyl=P3^1;
sbit keyh=P3^2;
sbit keym=P3^3;
sbit RS=P2^6;
sbit RW=P2^5;
sbit EN=P2^7;
sbit DQ=P3^7;
sbit buzzer=P2^1;
bit f=1;
unsigned char m=1,y=0,o=0;
unsigned int sdata;
unsigned char xiaoshu1;
unsigned char xiaoshu2;
unsigned char code tab1[]=" T:      C   M  ";
unsigned char code tab2[]="U:        L:    ";
unsigned char code shuzu[]="0123456789+- .*";
unsigned char u=40;
char l=15,p;

void delay_50us(unsigned int t)
{
 unsigned int j,k;
 for (;t>0;t--)
 {
  for(j=10;j>0;j--)
  {
   for(k=1;k>0;k--);
  }
 }
}

void write_com(unsigned com)
{
   EN=0;
   RS=0;
   RW=0;
   P0=com;
   delay_50us(1);
   EN=1;
   delay_50us(5);
   EN=0;
}

void write_data(unsigned char dat)
{
   EN=0;
   RS=1;
   RW=0;
   P0=dat;
   delay_50us(1);
   EN=1;
   delay_50us(5);
   EN=0;
}

void init(void)
{
     delay_50us(5);
	 write_com(0x38);
	 delay_50us(1);
	 write_com(0x38);
	 delay_50us(1);
	 write_com(0x38);
	 write_com(0x38);
	 write_com(0x08);
	 write_com(0x01);
	 write_com(0x06);
	 write_com(0x0C);
}

void delay_18B20(unsigned int i)
{
  while(i--);
}

Init_DS18B20(void)
{
  unsigned char x=0;
  DQ=1;
  delay_18B20(10);
  DQ=0;
  delay_18B20(80);
  DQ=1;
  delay_18B20(20);
  x=DQ;
  delay_18B20(30);
  return x;
}

ReadOneChar(void)
{
   unsigned char i=0;
   unsigned char dat=0;
   for(i=8;i>0;i--)
   {
      DQ=0;
      dat>>=1;
      DQ=1;
      if(DQ)
      dat|=0x80;
      delay_18B20(4);
   }
   return(dat);
}

WriteOneChar(unsigned char dat)
{
   unsigned char i=0;
   for(i=8;i>0;i--)
   {
      DQ=0;
      DQ=dat&0x01;
      delay_18B20(5);
      DQ=1;
      dat>>=1;
   }
   return(dat);
}

void ReadTemperature(void)
{
   unsigned char L=0;
   unsigned char H=0;
   Init_DS18B20();
   WriteOneChar(0xCC);
   WriteOneChar(0x44);
   delay_18B20(100);
   Init_DS18B20();
   WriteOneChar(0xCC);
   WriteOneChar(0xBE);
   L=ReadOneChar();
   H=ReadOneChar();
   if(H>0xf8)
   {
		L=~L+1;
		H=~H;
		f=0;
		sdata = L/16+H*16;
	    xiaoshu1 = (L&0x0f)*10/16;
    	xiaoshu2 = (L&0x0f)*100/16%10;
	}
	else
   	{
	    f=1;
        sdata = L/16+H*16;
    	xiaoshu1 = (L&0x0f)*10/16;
    	xiaoshu2 = (L&0x0f)*100/16%10;
	}
}

void display1()
{
       unsigned char p,q;
       init();
       write_com(0x80);
       for(q=0;q<16;q++)
       {
          write_data(tab1[q]);
       }
       write_com(0xc1);
       for(p=0;p<16;p++)
       write_data(tab2[p]);
}

void display2()
{
      write_com(0x8e);
      write_data(shuzu[m/10]);
      write_com(0x8f);
      write_data(shuzu[m%10]);
   if(o==1)
   {
     write_com(0xc0);
     write_data(shuzu[14]);
     write_com(0xca);
     write_data(shuzu[12]);
   }
   else if(o==2)
   {
      write_com(0xca);
      write_data(shuzu[14]);
      write_com(0xc0);
      write_data(shuzu[12]);
   }
   else if(o==0)
   {
      write_com(0xca);
      write_data(shuzu[12]);
      write_com(0xc0);
      write_data(shuzu[12]);
   }
   if(u/100==0)
   {
      write_com(0xc3);
      write_data(shuzu[12]);
      write_com(0xc4);
      write_data(shuzu[u/10]);
      write_com(0xc5);
      write_data(shuzu[u%10]);
   }
   else if (u/100==1)
   {
      write_com(0xc3);
      write_data(shuzu[u/100]);
      write_com(0xc4);
      write_data(shuzu[(u-100)/10]);
      write_com(0xc5);
      write_data(shuzu[u%10]);
   }
   if (l<0)
   {
      p=-l;
      write_com(0xcD);
      write_data(shuzu[11]);
      write_com(0xcE);
      write_data(shuzu[p/10]);
      write_com(0xcF);
      write_data(shuzu[p%10]);
   }
   else if(l>=0)
   {
      write_com(0xcD);
      write_data(shuzu[12]);
      write_com(0xcE);
      write_data(shuzu[l/10]);
      write_com(0xcF);
      write_data(shuzu[l%10]);
   }
}

void fuhao()
{
  	  if(f==1)
	 {
	    write_com(0x83);
        write_data(shuzu[12]);
	 }
	 else if(f==0)
	 {
	    write_com(0x83);
        write_data(shuzu[11]);
	 }
}

void display(unsigned int date)
{
      if(date/100==0)
      {
         write_com(0x84);
         write_data(shuzu[date/10]);
         write_com(0x85);
         write_data(shuzu[date%10]);
         write_com(0x86);
         write_data(shuzu[13]);
         write_com(0x87);
         write_data(shuzu[xiaoshu1]);
         write_com(0x88);
         write_data(shuzu[xiaoshu2]);
      }
      else if(date/100==1)
      {
         write_com(0x83);
         write_data(shuzu[date/100]);
         write_com(0x84);
         write_data(shuzu[(date-100)/10]);
         write_com(0x85);
         write_data(shuzu[date%10]);
         write_com(0x86);
         write_data(shuzu[13]);
         write_com(0x87);
         write_data(shuzu[xiaoshu1]);
         write_com(0x88);
         write_data(shuzu[xiaoshu2]);
      }
}

void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=123;j>0;j--);
}

void key()
{
   P3=0xff;
   if(keym==0)
   {
     delay_ms(10);
     if(keym==0)
     {
       y++;
	   if(y>=3)y=0;
	   switch (y)
	   {
	   case 0:m=1;break;
	   case 1:m=5 ;break;
	   case 2:m=10 ;break;
	   }
	   while(keym==0);
     }
   }
   if(keyh==0)
   {
     delay_ms(10);
     if(keyh==0)
     {
       o++;
   	   if(o>=3)o=0;
  	   while(keyh==0);
     }
   }
   if(keyu==0&&o==1)
   {
     delay_ms(10);
     if(keyu==0&&o==1)
     {
        u=u+m; while(keyu==0);
     }
   }
   if(keyl==0&&o==1)
   {
      delay_ms(10);
      if(keyl==0&&o==1)
      {
        u=u-m; while(keyl==0);
      }
   }
   if(keyu==0&&o==2)
   {
      delay_ms(10);
      if(keyu==0&&o==2)
      {
         l=l+m;while(keyu==0);
      }
   }
   if(keyl==0&&o==2)
   {
      delay_ms(10);
      if(keyl==0&&o==2)
      {
        l=l-m;while(keyl==0);
      }
   }
}

void BJ()
{
  	if(f==1)
	 {
	        if(sdata>=u)
	        {
	            display(sdata); buzzer=0;
	        }
	        else if(l>0&&sdata<=l)
	        {
	           display(sdata); buzzer=0;
	        }
	        else { display(sdata); buzzer=1; }
	 }
	 else if(f==0)
	 {
		    if(l>=0)
	        {
	            display(sdata); buzzer=0;
	        }
	        else if(l<0)
	        {
	            if(sdata>=-l)
				{
					 display(sdata); buzzer=0;
				}
				else if(sdata<-l)
				{
				     display(sdata); buzzer=1;
				}
	        }
	        else { display(sdata); buzzer=1; }
	 }
}

void main()
{
  buzzer=1;
  display1();
  while(1)
  {
     display2();
   	 ReadTemperature();
	 fuhao();
	 key();
	 BJ();
  }
}