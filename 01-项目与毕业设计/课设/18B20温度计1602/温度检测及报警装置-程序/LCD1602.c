#include <REGX52.H>
sbit keyu=P3^4;//�Ӱ���
sbit keyl=P3^5;//������
sbit keyh=P3^6;//���ڳ߶�
sbit keym=P3^7; //ת��ģʽ
sbit RS=P2^5;//����/����ѡ��ˣ�H/L��
sbit RW=P2^6;//��/д��(H/L)
sbit EN=P2^7;//ʹ��
sbit DQ=P2^0;  //DS18B20�����
sbit relay=P2^1;//继电器接口
bit f=1;  //�����¶ȱ�־λ
unsigned char  m=1,y=0,o=0;//�Ĵ���
unsigned int sdata;//��������¶�
unsigned char xiaoshu1;	//���С�����һλ�¶���ֵ
unsigned char xiaoshu2;	 //���С����ڶ�λ�¶���ֵ
unsigned char code huany[]=" Welcome to use "; //��ӭʹ��
unsigned char code tt[]  =" ter thermometer"; //�¶ȼ�
unsigned char code duqu[]="   loading...   "; //��ȡ
unsigned char code tab1[]=" T:      C   M  ";	//���
unsigned char code tab2[]="U:        L:    "; 
unsigned char code shuzu[]="0123456789+- .*";	//����,,10�Ӻţ�����11���ţ�����12�ո񣬣�����13�㣬������14*
unsigned char u=40;//����
char l=15,p;
			  //����
void delay_50us(unsigned int t)	 //��ʱ����
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

///lcd
void write_com(unsigned com)//д����
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
void write_data(unsigned char dat)//д����
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
void init(void)//��ʼ��
{
     delay_50us(5);
	 write_com(0x38);
	 delay_50us(1);
	 write_com(0x38);
	 delay_50us(1);
	 write_com(0x38);
	 write_com(0x38);
	 write_com(0x08); //ֻ����ʾ
	 write_com(0x01); //����
	 write_com(0x06);//��ַ��1����д�����ݵ�ʱ��������
	 write_com(0x0C); //����ʾ������ʾ���
}  
void delay_18B20(unsigned int i)	//18b20ȫ����ʱ
{
  while(i--);
}
///////////////////////////////////18b20//////////////////////////////////////////////////
//��ʼ������������
  Init_DS18B20(void)
  {
  unsigned char x=0;
  DQ=1;//DQ��λ
  delay_18B20(10);//������ʱ
  DQ=0;//��Ƭ����DQ����
  delay_18B20(80);//��ȷ��ʱ����480us
  DQ=1;//��������
  delay_18B20(20);
  x=DQ;//������ʱ�����x=0���ʼ���ɹ�x=1���ʼ��ʧ��
  delay_18B20(30);
  return x;
}
//��һ���ֽ�
ReadOneChar(void)
{
   unsigned char i=0;
   unsigned char dat=0;
   for(i=8;i>0;i--)
   {
      DQ=0;//�������ź�
      dat>>=1;
      DQ=1;//�������ź�
      if(DQ)
      dat|=0x80;
      delay_18B20(4);
   }
   return(dat);
}
//дһ���ֽ�
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
   WriteOneChar(0xCC);//����������кŵĲ���
   WriteOneChar(0x44);//�����¶�ת��
   delay_18B20(100);
   Init_DS18B20();
   WriteOneChar(0xCC);//����������кŵĲ���
   WriteOneChar(0xBE);//��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ���ǰ�������� �¶�                
   L=ReadOneChar();
   H=ReadOneChar();
   if(H>0xf8)      	//��λǰ��λΪ1ʱ�¶��Ǹ�
   {
		L=~L+1;
		H=~H;    
		f=0;      					//��ȡ�¶�Ϊ��ʱfg=0
		sdata = L/16+H*16;      	//��������
	    xiaoshu1 = (L&0x0f)*10/16; 		//С����һλ
    	xiaoshu2 = (L&0x0f)*100/16%10;	//С���ڶ�λ
	}
//����������ֵ��16��ʵ���¶�ֵ
//Ϊ�˵õ�2λС��λ���ȳ�100���ٳ�16�������������ݳ��ȣ�
	else
   	{
	    f=1;
        sdata = L/16+H*16;           	//��������
    	xiaoshu1 = (L&0x0f)*10/16; 		//С����һλ
    	xiaoshu2 = (L&0x0f)*100/16%10;	//С���ڶ�λ
	}
}
///////////////////18b20����/////////////
void display0()//��ʾwelcome   to use the  
{
       unsigned char i,w,h=0x8f,i1,w1,h1=0xcf,w2,i2,h2,g=2;
       init();
	   for(w=0;w<16;w++)  //��ʾ��ӭ
	   {
	       write_com(h--);//��һ���Լ�����
	       for(i=0;i<16;i++)
	       {
	          write_data(huany[i]);
			  delay_50us(15);
	       }
	   }
	   for(w1=0;w1<16;w1++)//�ڶ����Լ�����
	   {
	       write_com(h1--);
	       for(i1=0;i1<16;i1++)
	       {
	          write_data(tt[i1]);
			  delay_50us(15);
	       }
	   }
       delay_50us(5000);//��ʾloading
	   init(); 
	   write_com(0x80);
	   for(w2=0;w2<16;w2++)//������ʾ
	   {	    
           write_data(duqu[w2]);
		   delay_50us(200);	       
	   }
	   while(g--)//С����ѭ������
	   {   
	       write_com(0x8a);
	       for(i2=0;i2<3;i2++)
	       {
		      write_data(duqu[i2]);
			    
		   }
		   write_com(0x8a);
	       for(h2=10;h2<14;h2++)
		   {  
		      delay_50us(300);
		      write_data(duqu[h2]);
			  delay_50us(300);
		   }
	   }
      
}
void display1()	//��ʾ���
{
       unsigned char p,q;
       init();//ȫ������
       write_com(0x80);
       for(q=0;q<16;q++)//��ʾTMHL
       {
          write_data(tab1[q]);
       }  
       write_com(0xc1);
       for(p=0;p<16;p++)  
       write_data(tab2[p]);
}
void display2()//��ʾ�������¶�	,�߶�
{
   //�߶�
 
      write_com(0x8e);
      write_data(shuzu[m/10]);//ʮλ
      write_com(0x8f);
      write_data(shuzu[m%10]);//��λ 
   if(o==1)		   //����ѡ�б�־*�ſɲ���
   {
     write_com(0xc0);
     write_data(shuzu[14]);
     write_com(0xca);
     write_data(shuzu[12]);
   }
   else if(o==2)	//����ѡ�б�־*�ſɲ���
   {
      write_com(0xca);
      write_data(shuzu[14]);
      write_com(0xc0);
      write_data(shuzu[12]);
   }
   else if(o==0)	 //����*�Ų��ɲ���
   {
      write_com(0xca);
      write_data(shuzu[12]);
      write_com(0xc0);
      write_data(shuzu[12]);
   }
   if(u/100==0)	 //����100���¶�
   {
      write_com(0xc3);
      write_data(shuzu[12]);//�ո�
      write_com(0xc4);
      write_data(shuzu[u/10]);//ʮλ
      write_com(0xc5);
      write_data(shuzu[u%10]);//��λ
   }
   else if (u/100==1)//����100���¶�
   {
      write_com(0xc3);
      write_data(shuzu[u/100]);//��λ
      write_com(0xc4);
      write_data(shuzu[(u-100)/10]);//ʮλ
      write_com(0xc5);
      write_data(shuzu[u%10]);//��λ
   }
 //�����¶�
   if (l<0)//���޵���0���¶���ʾ
   {
      p=-l;
      write_com(0xcD);
      write_data(shuzu[11]);//����
      write_com(0xcE);
      write_data(shuzu[p/10]);
      write_com(0xcF);
      write_data(shuzu[p%10]);
   }
   else if(l>=0)//���޸���0���¶���ʾ
   {  
      write_com(0xcD);
      write_data(shuzu[12]);	 
      write_com(0xcE);
      write_data(shuzu[l/10]);
      write_com(0xcF);
      write_data(shuzu[l%10]);
   } 
}  
void fuhao()	//�¶������ӳ���
{
  	  if(f==1) //�¶�Ϊ��
	 {
	    write_com(0x83);
        write_data(shuzu[12]);
	 }
	 else if(f==0) //�¶�Ϊ������ʾ����
	 {
	    write_com(0x83);
        write_data(shuzu[11]);
	 }
}
void display(unsigned int date)	 //��ʾ�¶ȣ�
{
      if(date/100==0) //100����
      {
         write_com(0x84);
         write_data(shuzu[date/10]);//ʮλ
         write_com(0x85);
         write_data(shuzu[date%10]);//��λ
         write_com(0x86);
         write_data(shuzu[13]);//С����
         write_com(0x87);
         write_data(shuzu[xiaoshu1]);//С�����һλ
         write_com(0x88);
         write_data(shuzu[xiaoshu2]); //С������λ
      }
      else if(date/100==1)	  //100����
      {
         write_com(0x83);
         write_data(shuzu[date/100]);//��λ
         write_com(0x84);
         write_data(shuzu[(date-100)/10]);//ʮλ
         write_com(0x85);
         write_data(shuzu[date%10]);//��λ
         write_com(0x86);
         write_data(shuzu[13]);//��
         write_com(0x87);
         write_data(shuzu[xiaoshu1]);//С�����һλ
         write_com(0x88);
         write_data(shuzu[xiaoshu2]);//С������λ
      }
}

void key()		//�����Ӽ����

{ 
 //�߶�
   if(keym==0)//�ж�
   {
     delay_50us(100);//��ʱ
     if(keym==0)
     {	
       y++;//ת���Ӽ�����ֵ
	   if(y>=3)y=0;
	   switch (y)
	   {
	   case 0:m=1;break;
	   case 1:m=5 ;break;
	   case 2:m=10 ;break;
	   }
	   while(keym==0);//����
     }
   }
 ///��
   if(keyh==0)//������ת������
   {
     delay_50us(100);
     if(keyh==0)
     {
       o++;
   	   if(o>=3)o=0;//1Ϊ����ѡ�У�2Ϊ����ѡ�У�0Ϊ����
  	   while(keyh==0);
     }
   }
/////����	  
   if(keyu==0&o==1)//���޼Ӱ���
   {
     delay_50us(100);
     if(keyu==0&o==1)
     {
        u=u+m; while(keyu==0);
     }
   }
   if(keyl==0&o==1) //���޼�����

   {
      delay_50us(100);
      if(keyl==0&o==1)
      {
        u=u-m; while(keyl==0);
      }
   }
//// ����
   if(keyu==0&o==2)//���޼Ӱ���
   {
      delay_50us(100);
      if(keyu==0&o==2)
      {
         l=l+m;while(keyu==0);
      }
   }
   if(keyl==0&o==2)//���޼�����
   {
      delay_50us(100);
      if(keyl==0&o==2)
      {
        l=l-m;while(keyl==0);
      }
   } 
}

 void BJ()//温度超限继电器控制
 {
  	if(f==1)
	 {
	        if(sdata>=u)
	        {
	            display(sdata); relay=0;
	        }
	        else if(l>0&&sdata<=l)
	        {
	           display(sdata); relay=0;
	        }
	        else { display(sdata); relay=1; }
	 }
	 else if(f==0)
	 {
		    if(l>=0)
	        {
	            display(sdata); relay=0;
	        }
	        else if(l<0)
	        {
	            if(sdata>=-l)
				{
					 display(sdata); relay=0;
				}
				else if(sdata<-l)
				{
				     display(sdata); relay=1;
				}
	        }
	        else { display(sdata); relay=1; }
	 }
 }
void main()
{
  relay=1;
  display0(); //欢迎界面
  display1();//���
  while(1)
  {
     display2(); //�����ޣ��߶�
   	 ReadTemperature();	//���³���
	 fuhao();//������ʾ����
	 key();	//����
	 BJ();	//��������
  }
 
}
/*Ԫ�����б�
	����				����
   LCD1602                1
   ������                 1
   USBĸ��                1	 
   89c52				  1
   9��103����             1
   ����10K                1
   ����1k   			  1
   ����4.7K               2
   ��λ��10k			  1
   ������10uf			  1
   ����30pf	              2
   11.0592����            1
   ������pnp8550		  1
   �ĽŰ���               5
   ����					  8
   ����ĸ��20p			  1
   �ȶ�������18B20		  1
   ����40��               1
   ��ͭ��15X20            1
   ���ſ���               1
 */