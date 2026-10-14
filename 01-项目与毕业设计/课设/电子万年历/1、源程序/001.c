#include<reg51.h>
#include"DS18B20.h"
uchar a,b,miao,shi,fen,ri,yue,nian,week,flag,key1n,temp,temp_threshold=30,temp_alarm=0,chime_hour=0xff;
uchar lunar_month,lunar_day,lunar_leap;
uchar refresh_count;
bit refresh_flag;
void save_temp_threshold();
void refresh_display();
uchar code solar_month_days[]={31,28,31,30,31,30,31,31,30,31,30,31};
uint code lunar_info[]={
	0x92E0,0xC960,0xD954,0xD4A0,0xDA50,0x7552,0x56A0,0xABB7,
	0x25D0,0x92D0,0xCAB5,0xA950,0xB4A0,0xBAA4,0xAD50,0x55D9,
	0x4BA0,0xA5B0,0x5176,0x52B0,0xA930,0x7954,0x6AA0,0xAD50,
	0x5B52,0x4B60,0xA6E6,0xA4E0,0xD260,0xEA65,0xD530,0x5AA0,
	0x76A3,0x96D0,0x4AFB,0x4AD0,0xA4D0,0xD0B6,0xD250,0xD520,
	0xDD45,0xB5A0,0x56D0,0x55B2,0x49B0,0xA577,0xA4B0,0xAA50,
	0xB255,0x6D20,0xADA0,0x4B63,0x9370,0x49F8,0x4970,0x64B0,
	0x68A6,0xEA50,0x6B20,0xA6C4,0xAAE0,0x92E0,0xD2E3,0xC960,
	0xD557,0xD4A0,0xDA50,0x5D55,0x56A0,0xA6D0,0x55D4,0x52D0,
	0xA9B8,0xA950,0xB4A0,0xB6A6,0xAD50,0x55A0,0xABA4,0xA5B0,
	0x52B0,0xB273,0x6930,0x7337,0x6AA0,0xAD50,0x4B55,0x4B60,
	0xA570,0x54E4,0xD260,0xE968,0xD520,0xDAA0,0x6AA6,0x56D0,
	0x4AE0,0xA9D4,0xA4D0,0xD150,0xF252
};
uchar code lunar_leap_big[]={
	0x00,0x00,0x04,0x00,0x00,0x20,0x09,0x09,0x00,0x00,0x00,0x40,0x40
};
uchar code week_text[]={"MONTUEWEDTHUFRISATSUN"};
//flag���ڶ�ȡͷ�ļ��е��¶�ֵ������ʾ�¶�ֵ
#define yh 0x80 //LCD��һ�еĳ�ʼλ��,��ΪLCD1602�ַ���ַ��λD7�㶨Ϊ1��100000000=80��
#define er 0x80+0x40 //LCD�ڶ��г�ʼλ�ã���Ϊ�ڶ��е�һ���ַ�λ�õ�ַ��0x40��
//Һ��������C51֮����������Ӷ��壨��ʾ�����߽�C51��P0�ڣ�
sbit rs=P2^0;
sbit en=P2^2;
sbit rw=P2^1; //���Ӳ����rw�ӵأ��Ͳ���д���ͺ����rw=0��
sbit led=P2^6; //LCD���⿪��
//DS1302ʱ��оƬ��C51֮����������Ӷ���
sbit IO=P1^1;
sbit SCLK=P1^0;
sbit RST=P1^2;
sbit CLO=P1^4;
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
sbit fan=P3^0;


sbit key1=P1^5;  //���ü�
sbit key2=P1^6;  //�Ӽ�
sbit key3=P1^7;  //����
sbit buzzer=P1^3;//��������ͨ��������9012�������˿ڵ͵�ƽ��

void delay(uint xms)
{
	uint x,y;
	for(x=xms;x>0;x--)
		for(y=110;y>0;y--);
}
/********Һ��д��ָ�����д�����ݺ������Ժ�ɵ���**************/

/*����������У�Һ��д���йغ�������DS1302�ĺ����е��ã�����Һ������Ҫ����ǰ��*/

void write_1602com(uchar com)//****Һ��д��ָ���****
{
	rs=0;//����/ָ��ѡ����Ϊָ��
	rw=0; //��дѡ����Ϊд
	P0=com;//��������
	delay(1);
	en=1;//����ʹ�ܶˣ�Ϊ������Ч���½�����׼��
	delay(1);
	en=0;//en�ɸ߱�ͣ������½��أ�Һ��ִ������
}
void write_1602dat(uchar dat)//***Һ��д�����ݺ���****
{
	rs=1;//����/ָ��ѡ����Ϊ����
	rw=0; //��дѡ����Ϊд
	P0=dat;//��������
	delay(1);
	en=1; //en�øߵ�ƽ��Ϊ�����½�����׼��
	delay(1);
	en=0; //en�ɸ߱�ͣ������½��أ�Һ��ִ������
}
void lcd_init()
{
	delay(15);
	write_1602com(0x38);
	write_1602com(0x0c);
	write_1602com(0x06);
	write_1602com(0x01);
#if 0
	uchar j;
    write_1602com(0x0f|0x08);
	for(a=0;a<17;a++)
		write_1602dat(tab3[a]);
	j=17;
	while(j--)
	{
		write_1602com(0x1c);//ѭ������
		delay(700);
	} 
	write_1602com(0x01);
	delay(10);
	write_1602com(0x38);//����Һ������ģʽ����˼��16*2����ʾ��5*7����8λ����
	write_1602com(0x0c);//����ʾ����ʾ���
	write_1602com(0x06);//�������ƶ�������Զ�����
	write_1602com(0x01);//����ʾ
	/***����������ʾhello welcome dianzizhong****/
	write_1602com(yh+1);//������ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
	for(a=0;a<14;a++)
	{
		write_1602dat(tab1[a]);//��Һ����д������ʾ�Ĺ̶����Ų���
		//delay(3);
	}
	write_1602com(yh+15);
	write_1602dat(' ');
	write_1602com(er+0);
	write_1602dat(0x24);
	write_1602com(er+2);//ʱ����ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ	
	for(a=0;a<8;a++)
	{
		write_1602dat(tab2[a]);//д��ʾʱ��̶����ţ�����ð��
		//delay(3);
	}
}
/***************DS1302�й��Ӻ���********************/
#endif
}

void write_byte(uchar dat)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		SCLK=0;
		IO=dat&0x01;
		SCLK=1;
		dat=dat>>1;
	}
}
uchar read_byte()
{
	uchar i,dat;
	dat=0;
	IO=1;
	for(i=0;i<8;i++)
	{
		SCLK=0;
		if(IO)
			dat|=(1<<i);
		SCLK=1;
	}
	return(dat);
}
//----------------------------------------
void write_1302(uchar add,uchar dat)//��1302оƬд������ָ��д���ַ������
{
	RST=0;
	SCLK=0;
	RST=1;
	write_byte(add);
	write_byte(dat);
	SCLK=0;
	RST=0;
}
uchar read_1302(uchar add)//��1302�����ݺ�����ָ����ȡ������Դ��ַ
{
	uchar temp;
	RST=0;
	SCLK=0;
	RST=1;
	write_byte(add);
	temp=read_byte();
	SCLK=0;
	RST=0;
	return(temp);
}
uchar BCD_Decimal(uchar bcd)//BCD��תʮ���ƺ���������BCD������ʮ����
{
 	uchar Decimal;
	bcd&=0x7f;
  	Decimal=bcd>>4;
 	return(Decimal=Decimal*10+(bcd&=0x0F));
}

uint solar_day_offset(uchar year,uchar month,uchar day)
{
	uchar i;
	uint days;
	days=(uint)year*365+(year+3)/4+day-1;
	for(i=1;i<month;i++)
	{
		days+=solar_month_days[i-1];
		if(i==2&&year%4==0)
			days++;
	}
	return(days);
}

uint lunar_year_days(uchar index)
{
	uchar i;
	uint info,days;
	info=lunar_info[index];
	days=348;
	for(i=0;i<12;i++)
	{
		if(info&(0x8000>>i))
			days++;
	}
	if(info&0x000f)
	{
		if(lunar_leap_big[index/8]&(1<<(index%8)))
			days+=30;
		else
			days+=29;
	}
	return(days);
}

uchar lunar_month_length(uchar index,uchar month,uchar is_leap)
{
	if(is_leap)
	{
		if(lunar_leap_big[index/8]&(1<<(index%8)))
			return(30);
		return(29);
	}
	if(lunar_info[index]&(0x8000>>(month-1)))
		return(30);
	return(29);
}

void update_calendar()
{
	uchar index,month,leap_month,is_leap;
	uint offset,days;
	if(yue==0||yue>12||ri==0||ri>31)
	{
		week=0;
		lunar_month=0;
		lunar_day=0;
		lunar_leap=0;
		return;
	}
	offset=solar_day_offset(nian,yue,ri);
	week=(offset+6)%7;
	if(week==0)
		week=7;
	offset+=319;
	index=0;
	while(index<100)
	{
		days=lunar_year_days(index);
		if(offset<days)
			break;
		offset-=days;
		index++;
	}
	leap_month=lunar_info[index]&0x000f;
	month=1;
	is_leap=0;
	while(month<=12)
	{
		days=lunar_month_length(index,month,is_leap);
		if(offset<days)
			break;
		offset-=days;
		if(leap_month==month&&is_leap==0)
			is_leap=1;
		else
		{
			is_leap=0;
			month++;
		}
	}
	lunar_month=month;
	lunar_day=offset+1;
	lunar_leap=is_leap;
}
//--------------------------------------
//------------------------------------
//�¶���ʾ�Ӻ���
void write_temp(uchar add,uchar dat)//��LCDд�¶�����,��ָ����ʾλ��
{
	uchar gw,sw,bw;
	if(dat>=0&&dat<=128)
	{
		gw=dat%10;//ȡ�ø�λ����
		sw=dat%100/10;//ȡ��ʮλ����
		bw=dat/100	;//ȡ�ð�λ����	
	}
	else
	{
		dat=256-dat;
		gw=dat%10;//ȡ�ø�λ����
		sw=dat%100/10;//ȡ��ʮλ����
		bw=-3;		 //0x30-3��ʾΪ����
	}
	write_1602com(er+add);//er��ͷ�ļ��涨��ֵ0x80+0x40
	write_1602dat(0x30+bw);//����+30�õ������ֵ�LCD1602��ʾ��
	write_1602dat(0x30+sw);//����+30�õ������ֵ�LCD1602��ʾ��
	write_1602dat(0x30+gw);//����+30�õ������ֵ�LCD1602��ʾ��
    write_1602dat(0xdf);//��ʾ�¶ȵ�СԲȦ���ţ�0xdf��Һ�����ַ���ĸ÷��ŵ�ַ��
  	write_1602dat(0x43);	//��ʾ"C"���ţ�0x43��Һ�����ַ������дC�ĵ�ַ��		
}
//------------------------------------
//ʱ������ʾ�Ӻ���
void write_sfm(uchar add,uchar dat)//��LCDдʱ����,����ʾλ�üӡ���ʾ���ݣ���������
{
	uchar gw,sw;
	gw=dat%10;//ȡ�ø�λ����
	sw=dat/10;//ȡ��ʮλ����
	write_1602com(er+add);//er��ͷ�ļ��涨��ֵ0x80+0x40
	write_1602dat(0x30+sw);//����+30�õ������ֵ�LCD1602��ʾ��
	write_1602dat(0x30+gw);//����+30�õ������ֵ�LCD1602��ʾ��				
}
//-------------------------------------
//��������ʾ�Ӻ���
void write_nyr(uchar add,uchar dat)//��LCDд�����գ�����ʾλ�ü�������ʾ���ݣ���������
{
	uchar gw,sw;
	gw=dat%10;//ȡ�ø�λ����
	sw=dat/10;//ȡ��ʮλ����
	write_1602com(yh+add);//�趨��ʾλ��Ϊ��һ��λ��+add
	write_1602dat(0x30+sw);//����+30�õ������ֵ�LCD1602��ʾ��
	write_1602dat(0x30+gw);//����+30�õ������ֵ�LCD1602��ʾ��	
}
//-------------------------------------------
//****************����ɨ���йغ���**********************
void write_week_fast(uchar day)
{
	uchar i;
	write_1602com(yh+0x0c);
	if(day<1||day>7)
	{
		write_1602dat(' ');
		write_1602dat(' ');
		write_1602dat(' ');
		return;
	}
	for(i=0;i<3;i++)
		write_1602dat(week_text[(day-1)*3+i]);
}

#define write_week write_week_fast

void write_two_chars(uchar dat)
{
	write_1602dat(0x30+dat/10);
	write_1602dat(0x30+dat%10);
}

void display_solar_date()
{
	write_1602com(yh);
	write_1602dat(' ');
	write_1602dat('2');
	write_1602dat('0');
	write_two_chars(nian);
	write_1602dat('-');
	write_two_chars(yue);
	write_1602dat('-');
	write_two_chars(ri);
	write_1602dat(' ');
	write_week(week);
	write_1602com(yh+15);
	write_1602dat(' ');
}

void display_lunar_date()
{
	write_1602com(yh);
	write_1602dat('L');
	write_1602dat(lunar_leap?'E':'U');
	write_1602dat(lunar_leap?'P':'N');
	write_1602dat(' ');
	write_two_chars(lunar_month);
	write_1602dat('-');
	write_two_chars(lunar_day);
	write_1602dat(' ');
	write_1602dat(' ');
	write_1602dat(' ');
	write_week(week);
	write_1602com(yh+15);
	write_1602dat(' ');
}

void refresh_display()
{
	flag=ReadTemperature();
	miao=BCD_Decimal(read_1302(0x81));
	fen=BCD_Decimal(read_1302(0x83));
	shi=BCD_Decimal(read_1302(0x85));
	ri=BCD_Decimal(read_1302(0x87));
	yue=BCD_Decimal(read_1302(0x89));
	nian=BCD_Decimal(read_1302(0x8d));
	update_calendar();
	write_1602com(er);
	write_1602dat('$');
	write_1602dat(' ');
	write_two_chars(shi);
	write_1602dat(':');
	write_two_chars(fen);
	write_1602dat(':');
	write_two_chars(miao);
	write_1602dat(' ');
	write_temp(11,flag);
	if(miao%10<5)
		display_solar_date();
	else
		display_lunar_date();
	if(fen==0&&miao==0&&shi<22&&shi>6&&chime_hour!=shi)
	{
		buzzer=0;
		delay(20);
		buzzer=1;
		chime_hour=shi;
	}
	if(flag<=temp_threshold||flag>128)
	{
		fan=0;
		temp_alarm=0;
	}
	else
	{
		fan=1;
		if(temp_alarm==0)
			temp_alarm=1;
		if(CLO==0)
			temp_alarm=2;
		if(temp_alarm==1U)
		{
			buzzer=0;
			delay(20);
			buzzer=1;
		}
	}
}

void keyscan()
{
	if(refresh_flag)
	{
		refresh_flag=0;
		refresh_display();
	}
	if(key1==0)//---------------key1Ϊ���ܼ������ü���--------------------
	{
		delay(9);//��ʱ������������
		if(key1==0)//��ʱ���ٴ�ȷ�ϰ�������
		{
    		buzzer=0;//����������һ��
    		delay(20);
    		buzzer=1;
			while(!key1);
			key1n++;
			switch(key1n)
			{		
				case 1: TR0=0;
						display_solar_date();
						write_1602com(er+0x09);
					   	write_1602com(0x0f);
					   	temp=(miao)/10*16+(miao)%10;
					   	write_1302(0x8e,0x00);
					   	write_1302(0x80,0x80|temp);
					    write_1302(0x8e,0x80);
					   	break;
				case 2: write_1602com(er+6);
						break;
				case 3: write_1602com(er+3);
						break;
				case 4: write_1602com(yh+0x0e);
						break;
				case 5: write_1602com(yh+0x0a);
						break;
				case 6: write_1602com(yh+0x07);
						break;
				case 7: write_1602com(yh+0x04);
						break;
				case 8:	write_1602com(er+9);
				        write_1602dat(0x54);
						write_1602dat(0x48);
						write_1602com(er+9);
						break;
				case 9: write_1602com(er+10);
			        	write_1602dat(0x20);
						write_1602com(0x0c);
						TR0=1;
			         	temp=(miao)/10*16+(miao)%10;
					   	write_1302(0x8e,0x00);
					   	write_1302(0x80,0x00|temp);
					   	write_1302(0x8e,0x80);
						key1n=0;
			            break;																		
			}
		} 
	}
	//------------------------------�Ӽ�key2----------------------------		
	if(key1n!=0)//��key1�������¡��ٰ����¼�����Ч�����������������㣩
	{
		if(key2==0)  //�ϵ���
		{
			delay(10);
			if(key2==0)
			{
    			buzzer=0;//����������һ��
    			delay(20);
    			buzzer=1;
				while(!key2);
				switch(key1n)
				{
					case 1:	miao++;//���ü�����1�Σ�����
							if(miao==60)
								miao=0;//�볬��59���ټ�1���͹���
							write_sfm(0x08,miao);//��LCD����ȷλ����ʾ"��"�趨�õ�����
							temp=(miao)/10*16+(miao)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00); //����д����ֹд���� 
						   	write_1302(0x80,temp); //��DS1302��д��Ĵ���80Hд��������������BCD��
						   	write_1302(0x8e,0x80); //��д����
							write_1602com(er+0x09);//��Ϊ����Һ����ģʽ��д�����ݺ󣬹���Զ����ƣ�����Ҫָ������
							//write_1602com(0x0b);
							break;
					case 2:	fen++;
							if(fen==60)
								fen=0;
							write_sfm(0x05,fen);//��LCD����ȷλ����ʾ"��"�趨�õķ�����
							temp=(fen)/10*16+(fen)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд���� 
						   	write_1302(0x82,temp);//��DS1302��д�ּĴ���82Hд�������ķ�����BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(er+6);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ����������д��ԭ����λ��
							break;
					case 3:	shi++;
							if(shi==24)
								shi=0;
							write_sfm(2,shi);//��LCD����ȷ��λ����ʾ"��"�趨�õ�Сʱ����
							temp=(shi)/10*16+(shi)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд���� 
						   	write_1302(0x84,temp);//��DS1302��дСʱ�Ĵ���84Hд��������Сʱ����BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(er+3);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 4:	week++;
							if(week==8)
								week=1;
				            write_1602com(yh+0x0C);//ָ��'��'�����������ʾλ��
							write_week(week);//ָ����������ʾ����
				            temp=(week)/10*16+(week)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд���� 
						   	write_1302(0x8a,temp);//��DS1302��д�ܼĴ���8aHд��������������BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(yh+0x0e);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 5:	ri++;
							switch(yue)
							{
				            	case 1:
								case 3:
								case 5:
								case 7:
								case 8:
								case 10:
								case 12:
										if(ri>31) 
											ri=1;
						        		break;
				                case 2: 
										if(nian%4==0||nian%400==0)
						  	       		{ 
								     		if(ri>29) 
												ri=1;
								    	}	
						           		else					  	
						         		{					    
								    		if(ri>28) 
												ri=1;
						           		}
										break;
				               case 4:
							   case 6:
							   case 9:
							   case 11:
								    	if(ri>30) 
											ri=1;
							         	break;
				            }		 
							write_nyr(9,ri);//��LCD����ȷ��λ����ʾ"��"�趨�õ���������
							temp=(ri)/10*16+(ri)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд����
						   	write_1302(0x86,temp);//��DS1302��д���ڼĴ���86Hд����������������BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(yh+10);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 6:	yue++;
							if(yue==13)
								yue=1;
							write_nyr(6,yue);//��LCD����ȷ��λ����ʾ"��"�趨�õ��·�����
							temp=(yue)/10*16+(yue)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд����
						   	write_1302(0x88,temp);//��DS1302��д�·ݼĴ���88Hд���������·�����BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(yh+7);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 7:	nian++;
						 	if(nian==100)
								nian=0;
							write_nyr(3,nian);//��LCD����ȷ��λ����ʾ"��"�趨�õ��������
				            temp=(nian)/10*16+(nian)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд����
						   	write_1302(0x8c,temp);//��DS1302��д��ݼĴ���8cHд���������������BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(yh+4);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 8:	temp_threshold++;
					        if(temp_threshold>99)
							   temp_threshold=1;
							save_temp_threshold();
							write_1602com(er+9);
							write_1602dat(0x54);
							write_1602dat(0x48);
							write_temp(11,temp_threshold);
							write_1602com(er+9);
							break;
				}
			}
		}
		//------------------����key3�����书�ܲ���'�Ӽ�'ע��---------------
		if(key3==0)
		{
			delay(10);//����ʱ��������
			if(key3==0)
			{
	    		buzzer=0;//����������һ��
	    		delay(20);
			    buzzer=1;
				while(!key3);
				switch(key1n)
				{
					case 1:	miao--;
							if(miao==255)
								miao=59;//�����ݼ���-1ʱ�Զ����59
							write_sfm(0x08,miao);//��LCD����ȷλ����ʾ�ı���µ�����
				            temp=(miao)/10*16+(miao)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00); //����д����ֹд���� 
						   	write_1302(0x80,temp); //��DS1302��д��Ĵ���80Hд��������������BCD��
						   	write_1302(0x8e,0x80); //��д����
							write_1602com(er+0x09);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ����������д��ԭ����λ��
							//write_1602com(0x0b);
							break;
					case 2:	fen--;
							if(fen==255)
								fen=59;
							write_sfm(5,fen);
							temp=(fen)/10*16+(fen)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд���� 
						   	write_1302(0x82,temp);//��DS1302��д�ּĴ���82Hд�������ķ�����BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(er+6);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ����������д��ԭ����λ��
							break;
			
					case 3:	shi--;
						   	if(shi==255)
								shi=23;
							write_sfm(2,shi);
							temp=(shi)/10*16+(shi)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд���� 
						   	write_1302(0x84,temp);//��DS1302��дСʱ�Ĵ���84Hд��������Сʱ����BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(er+3);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 4:	week--;
							if(week==0)
								week=7;						 
			            	write_1602com(yh+0x0C);//ָ��'��'�����������ʾλ��
							write_week(week);//ָ����������ʾ����
						   	temp=(week)/10*16+(week)%10;//ʮ����ת����DS1302Ҫ���DCB��
					   		write_1302(0x8e,0x00);//����д����ֹд���� 
					   		write_1302(0x8a,temp);//��DS1302��д�ܼĴ���8aHд��������������BCD��
					   		write_1302(0x8e,0x80);//��д����
						   	write_1602com(yh+0x0e);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 5:	ri--;
							switch(yue)
							{ 
			                	case 1:
								case 3:
								case 5:
								case 7:
								case 8:
								case 10:
								case 12: 
							 			if(ri==0) 
											ri=31;
					         			break;
			                  	case 2: if(nian%4==0||nian%400==0)
					  	       			{ 
							    			if(ri==0) 
												ri=29;
							    		}			
					           			else	
						        		{
							        		if(ri==0) 
												ri=28;
					         			}
										break;
			                 	case 4:
								case 6:
								case 9:
								case 11:
							      		if(ri==0) 
											ri=30;
						         		break;
			             	}
							write_nyr(9,ri);
							temp=(ri)/10*16+(ri)%10;//ʮ����ת����DS1302Ҫ���DCB��
					   		write_1302(0x8e,0x00);//����д����ֹд����
					   		write_1302(0x86,temp);//��DS1302��д���ڼĴ���86Hд����������������BCD��
					   		write_1302(0x8e,0x80);//��д����
							write_1602com(yh+10);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 6:	yue--;
							if(yue==0)
								yue=12;
							write_nyr(6,yue);
							temp=(yue)/10*16+(yue)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд����
						   	write_1302(0x88,temp);//��DS1302��д�·ݼĴ���88Hд���������·�����BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(yh+7);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;	
					case 7:	nian--;
					 		if(nian==255)
								nian=99;
							write_nyr(3,nian);
			         		temp=(nian)/10*16+(nian)%10;//ʮ����ת����DS1302Ҫ���DCB��
						   	write_1302(0x8e,0x00);//����д����ֹд����
						   	write_1302(0x8c,temp);//��DS1302��д��ݼĴ���8cHд���������������BCD��
						   	write_1302(0x8e,0x80);//��д����
							write_1602com(yh+4);//��Ϊ����Һ����ģʽ��д�����ݺ�ָ���Զ���һ��������Ҫ����λ
							break;
					case 8:	temp_threshold--;
					        	if(temp_threshold==0)
						   		temp_threshold=99;
							save_temp_threshold();
							write_1602com(er+9);
							write_1602dat(0x54);
							write_1602dat(0x48);
							write_temp(11,temp_threshold);
							write_1602com(er+9);
							break;			
				}
			}
		}
	}
}
//��ʱ��0��ʼ������
void init() //��ʱ�������������ú���
{
	TMOD=0x11; //ָ����ʱ/�������Ĺ�����ʽΪ1
	TH0=0;  //��ʱ��T0�ĸ���λ=0
	TL0=0;  //��ʱ��T0�ĵ���λ=0
	EA=1;  //ϵͳ�����п��ŵ��ж�
	ET0=1; //����T0�ж�
	TR0=1; //�����жϣ�������ʱ��
}
//*******************������**************************
//***************************************************
void save_temp_threshold()
{
	write_1302(0x8e,0x00);
	write_1302(0xc2,temp_threshold);
	write_1302(0x8e,0x80);
}

void rtc_init()
{
	uchar marker;
	marker=read_1302(0xc1);
	write_1302(0x8e,0x00);
	if(marker!=0xa5||
	   (read_1302(0x8d)==0x05&&read_1302(0x89)==0x05&&read_1302(0x87)==0x05))
	{
		write_1302(0x82,0x22);
		write_1302(0x84,0x10);
		write_1302(0x86,0x10);
		write_1302(0x88,0x07);
		write_1302(0x8a,0x05);
		write_1302(0x8c,0x26);
		write_1302(0x80,0x00);
		write_1302(0xc0,0xa5);
		temp_threshold=30;
		write_1302(0xc2,temp_threshold);
	}
	else
	{
		temp_threshold=read_1302(0xc3);
		if(temp_threshold==0||temp_threshold>99)
		{
			temp_threshold=30;
			write_1302(0xc2,temp_threshold);
		}
	}
	write_1302(0x8e,0x80);
}

void main()
{	
	fan=0;
	lcd_init();      //����Һ������ʼ���Ӻ���
	rtc_init();
	init();          //���ö�ʱ�������������Ӻ���
	led=0;           //��LCD�ı����Դ
    buzzer=0;//����������һ��
    delay(80);
    buzzer=1; 
	refresh_flag=1;
	while(1)  //����ѭ���������䣺
	{		
    	keyscan();      //���ü���ɨ���Ӻ���
    }
}
/*************ͨ����ʱ�ж�ʵ�ֶ��Ƕ�������ʾ����******************/
void t0() interrupt 1
{	
	if(++refresh_count==14)
	{
		refresh_count=0;
		refresh_flag=1;
	}
#if 0
   	//Init_DS18B20();//�¶ȴ�����DS18b2��ʼ���Ӻ�������ͷ�ļ���
   	flag=ReadTemperature();//��18b2ͷ�ļ����з��صĺ�������͵�����FLAG�У�������ʾ
  	//��ȡ��ʱ�����������߸����ݣ�DS1302�Ķ��Ĵ�����д�Ĵ�����һ������
	miao=BCD_Decimal(read_1302(0x81));
	fen=BCD_Decimal(read_1302(0x83));
	shi=BCD_Decimal(read_1302(0x85));
	ri=BCD_Decimal(read_1302(0x87));
	yue=BCD_Decimal(read_1302(0x89));
	nian=BCD_Decimal(read_1302(0x8d));
	week=BCD_Decimal(read_1302(0x8b));
	//��ʾ�¶ȡ��롢ʱ�������ݣ�
	write_temp(11,flag);//��ʾ�¶ȣ��ӵڶ��е�11���ַ���ʼ��ʾ
	write_sfm(8,miao);//�룬�ӵڶ��е�8���ֺ�ʼ��ʾ������ʱ������ʾ�Ӻ�����
	write_sfm(5,fen);//�֣��ӵڶ��е�5���ַ���ʼ��ʾ
	write_sfm(2,shi);//Сʱ���ӵڶ��е�2���ַ���ʼ��ʾ
	//��ʾ�ա��¡������ݣ�
	write_nyr(9,ri);//���ڣ��ӵڶ��е�9���ַ���ʼ��ʾ
	write_nyr(6,yue);//�·ݣ��ӵڶ��е�6���ַ���ʼ��ʾ
	write_nyr(3,nian);//�꣬�ӵڶ��е�3���ַ���ʼ��ʾ
	update_calendar();
	if(miao%10<5)
		display_solar_date();
	else
		display_lunar_date();
	if(fen==0&&miao==0&&shi<22&&shi>6&&chime_hour!=shi)
	{
		buzzer=0;
		delay(20);
		buzzer=1;
		chime_hour=shi;
	}
	if(flag<=temp_threshold||flag>128)
	{
		fan=0;
		temp_alarm=0;
	}
	else
	{
		fan=1;
		if(temp_alarm==0)
			temp_alarm=1;
		if(CLO==0)
			temp_alarm=2;
		if(temp_alarm==1)
		{
			buzzer=0;
			delay(20);
			buzzer=1;
		}
	}
}

#endif
}


