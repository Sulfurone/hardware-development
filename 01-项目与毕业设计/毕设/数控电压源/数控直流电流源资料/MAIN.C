//include
#include<math.h>
#include<absacc.h>
#include<reg51.h>

#define uchar unsigned char
#define uint unsigned int
#define valve 10
//sbit dat = P3^5;
//sbit key = P3^2;
//sbit cls = P3^4;
//sbit clk = P3^3;
sbit dat = P1^4;
sbit key = P1^5;
sbit cls = P1^6;
sbit clk = P1^7;
sbit DQ  = P1^0;
sbit ad_busy =P3^3;

uchar A[4]={0,2,0,0};
uchar arrAD[4]={0,0,0,0};
uchar AIndex=0;

uchar xdata *da=0xA000;
uchar xdata *ad=0x9FF8;

uchar adin()
{
	uchar i=0;
	*ad=0;
	i=1;//for delay
	i=1;//for delay
	i=1;//for delay
	while(ad_busy==1)
	{;}
	return *ad;
}
void delay(int num)
{
	while(num--);
}

void delay50us(uint num)
{
	uint i,j;
	for(i=0;i<num;i++)
		for(j=0;j<6;j++)
			;
}
void senddata(uchar data1)
{
	uchar tempdata,i;
	tempdata=data1;
	cls=0;
	delay50us(1);
	for(i=0;i<8;i++)
	{
		dat=(tempdata & 0x80)>>7;
		clk=1;
		tempdata=tempdata<<1;
		delay(1);
		clk=0;
		delay(1);
	}
	dat=0;	
}

uchar keyscan()
{
	uchar i,tempdata=0;
	if(key==0)
	{
		senddata(0x15);
		dat=1;
		delay50us(1);
		for(i=0;i<8;i++)
		{
			clk=1;
			delay(1);
			tempdata=(tempdata<<1) | dat;	
			clk=0;
			delay(1);
		}
		dat=0;
		return tempdata;
	}
	else
		return 0xFF;
}
//一些列的延时函数,都是在晶振频率为12MHz的情况下使用的
void delay1ms(uint num)
{
	uint i;
	while((num--!=0)&&(keyscan()==0xFF))
	{
		for(i=0;i<72;i++)
		;
	}
}
void initkey()
{
	P1=0xFF;
	delay1ms(25);
	senddata(0xA4);
	cls=1;
}
//setup at startup
void Mission1()
{
	uchar key1=0;
	uchar tmpA[4]={0,0,0,0};
	uint tmp=0;
	A[0]=0;
	A[1]=2;
	A[2]=0;
	A[3]=0;
	AIndex=0;
	//send 0 to DA
	*da=10;
	while(key1!=0x05)
    {
		//show 1D  0200
		senddata(0xCF);
		senddata(0x01);//display "1" in the 8th
		senddata(0xCE);
		senddata(0x0D);//display "d" in the 7th
		senddata(0x85);
		senddata(0x0F);//display " " in the 6th
		senddata(0x84);
		senddata(0x0F);//display " " in the 5th
		senddata(0x83);
		senddata(A[3]);//display thousand in the 4th
		senddata(0x82);
		senddata(A[2]);//display hundred in the 3th
		senddata(0x81);
		senddata(A[1]);//display ten in the 2th
		senddata(0x80);
		senddata(A[0]);//display one in the 1th
		
		senddata(0x88);
		senddata(~(0x01<<AIndex));//flash
		key1=keyscan();
		if(key1!=0xFF)
		{
			while(keyscan()!=0xFF)
			;
			tmpA[0]=A[0];
			tmpA[1]=A[1];
			tmpA[2]=A[2];
			tmpA[3]=A[3];
 		}
		switch(key1)
		{
			case 1://add 1
				
				if(tmpA[AIndex]==9)
				{
					tmpA[AIndex]=0;
				}
				else
					tmpA[AIndex]=tmpA[AIndex]+1;
				tmp=tmpA[3]*1000+tmpA[2]*100+tmpA[1]*10+tmpA[0];
				if((tmp<=2000)&&(tmp>=20))
					A[AIndex]=tmpA[AIndex];
				break;
			case 2://minus 1
				if(tmpA[AIndex]==0)
				{
					tmpA[AIndex]=9;
				}
				else
					tmpA[AIndex]=tmpA[AIndex]-1;
				tmp=tmpA[3]*1000+tmpA[2]*100+tmpA[1]*10+tmpA[0];
				if((tmp<=2000)&&(tmp>=20))
					A[AIndex]=tmpA[AIndex];
				break;
			case 3://move left
				if(AIndex==3)
				{
					AIndex=0;
				}
				else
					AIndex=AIndex+1;
				break;
			case 4://move right
				if(AIndex==0)
				{
					AIndex=3;
				}
				else
					AIndex=AIndex-1;
				break;		
		}
	}
}

void Mission2()
{
	uchar key1=0;
	uchar cAD=0;
	uint iAD=0;
	uint iDA=0;
	uchar cDA=0;
	uchar cSendDA=0;
	senddata(0x88);
	senddata(0xFF);//stop flash
	*da=1;//send 10 to DA when the program start run.
	cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);
	while(key1!=0x05)
    {
		//get AD0 and show in the 8765 LED
		delay1ms(500);
		cAD=adin();
		//cAD=cDA;
		//iAD=(int)((int)cAD/255*2000);
		//去抖动
		/*iAD=arrAD[3]*1000+arrAD[2]*100+arrAD[1]*10+arrAD[0];
		if(abs(iAD-(int)cAD*10)>valve)
		{
			iAD=cAD*10;
			arrAD[3]=(uchar)(iAD/1000);
			iAD=iAD%1000;
			arrAD[2]=(uchar)(iAD/100);
			iAD=iAD%100;
			arrAD[1]=(uchar)(iAD/10);
			iAD=iAD%10;
			arrAD[0]=(uchar)iAD;
		}
		*/
		iAD=cAD*10;
		arrAD[3]=(uchar)(iAD/1000);
		iAD=iAD%1000;
		arrAD[2]=(uchar)(iAD/100);
		iAD=iAD%100;
		arrAD[1]=(uchar)(iAD/10);
		iAD=iAD%10;
		arrAD[0]=(uchar)iAD;
		//show AD in 8765 LED
		senddata(0x87);
		senddata(arrAD[3]);//display thousand in the 8th
		senddata(0x86);
		senddata(arrAD[2]);//display hundred in the 7th
		senddata(0x85);
		senddata(arrAD[1]);//display ten in the 6th
		senddata(0x84);
		senddata(arrAD[0]);//display one in the 5th
		senddata(0x83);
		//show setting in 4321 LED
		senddata(A[3]);//display thousand in the 4th
		senddata(0x82);
		senddata(A[2]);//display hundred in the 3th
		senddata(0x81);
		senddata(A[1]);//display ten in the 2th
		senddata(0x80);
		senddata(A[0]);//display one in the 1th
		if(cAD<(A[3]*100+A[2]*10+A[1]+(A[0]>=5?1:0)))
		{
			if(cSendDA<230)
				cSendDA=cSendDA+1;
		}
		else if(cAD>(A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0)))
		{
			if(cSendDA>1)
				cSendDA=cSendDA-1;
		}
		//send to DA
		iDA=A[3]*1000+A[2]*100+A[1]*10+A[0];
		//iDA=A[3]*100+A[2]*10+A[1];
		//cDA=iDA/2000*255;
		//cDA=A[3]*100+A[2]*10+A[1];
		*da=cSendDA;
		key1=keyscan();
		if(key1!=0xFF)
		{
			while(keyscan()!=0xFF)
			;
 		}
		switch(key1)
		{
			case 1://add 1mA
				if(iDA<=1999)
				{
					iDA=iDA+1;
					A[3]=(uchar)(iDA/1000);
					iDA=iDA%1000;
					A[2]=(uchar)(iDA/100);
					iDA=iDA%100;
					A[1]=(uchar)(iDA/10);
					iDA=iDA%10;
					A[0]=iDA;
				}
				cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);			
				break;
			case 2://minus 1mA
				if(iDA>=21)
				{
					iDA=iDA-1;
					A[3]=(uchar)(iDA/1000);
					iDA=iDA%1000;
					A[2]=(uchar)(iDA/100);
					iDA=iDA%100;
					A[1]=(uchar)(iDA/10);
					iDA=iDA%10;
					A[0]=iDA;
				}
				cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);
				break;
			case 3://add 100mA
				if(iDA<=1900)
				{
					iDA=iDA+100;
					A[3]=(uchar)(iDA/1000);
					iDA=iDA%1000;
					A[2]=(uchar)(iDA/100);
					iDA=iDA%100;
					A[1]=(uchar)(iDA/10);
					iDA=iDA%10;
					A[0]=iDA;
				}
				cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);
				break;
			case 4://minus 100mA
				if(iDA>=120)
				{
					iDA=iDA-100;
					A[3]=(uchar)(iDA/1000);
					iDA=iDA%1000;
					A[2]=(uchar)(iDA/100);
					iDA=iDA%100;
					A[1]=(uchar)(iDA/10);
					iDA=iDA%10;
					A[0]=iDA;
				}
				cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);
				break;
			case 6://add 10mA
				
				if(iDA<=1990)
				{
					iDA=iDA+10;
					A[3]=(uchar)(iDA/1000);
					iDA=iDA%1000;
					A[2]=(uchar)(iDA/100);
					iDA=iDA%100;
					A[1]=(uchar)(iDA/10);
					iDA=iDA%10;
					A[0]=iDA;
				}
				cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);
				break;
			case 7://minus 10mA
				if(iDA>=30)
				{
					iDA=iDA-10;
					A[3]=(uchar)(iDA/1000);
					iDA=iDA%1000;
					A[2]=(uchar)(iDA/100);
					iDA=iDA%100;
					A[1]=(uchar)(iDA/10);
					iDA=iDA%10;
					A[0]=iDA;
				}
				cSendDA=A[3]*100+A[2]*10+A[1]+((A[0]>=5)?1:0);
				break;	
		}
	}
	if(keyscan()!=0xFF)
	{
		while(keyscan()!=0xFF)
		;
	}
	
}
void main()
{
	initkey();
	while(1)
	{
		Mission1();
		Mission2();
	}
}


