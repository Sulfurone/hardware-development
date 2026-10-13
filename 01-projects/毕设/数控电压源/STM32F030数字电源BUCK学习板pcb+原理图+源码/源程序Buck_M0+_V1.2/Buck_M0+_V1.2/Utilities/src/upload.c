#if USE_USART_PRINTF
#include "upload.h"
#include "stdarg.h"	 
#include "string.h"	





/**********************************************************
**从buf里面得到第cx个逗号所在的位置
**返回值:0~0XFE,代表逗号所在位置的偏移.
**       0XFF,代表不存在第cx个逗号
*/
uint8_t My_Comma_Pos(uint8_t *buf, uint8_t cx)
{	 		    
	uint8_t *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z') { return 0XFF; }//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',') { cx--; }
		buf++;
	}
	return buf-p;	 
}

/*********************************************************
**m^n函数
**返回值:m^n次方.
*/
uint32_t My_Pow(uint8_t m, uint8_t n)
{
	uint32_t result=1;	 
	while(n--) { result *= m; }    
	return result;
}

/*********************************************************
**str转换为数字,以','或者'*'结束
**buf:数字存储区
**dx:小数点位数,返回给调用函数
**返回值:转换后的数值
*/
int My_Str2num(uint8_t *buf, uint8_t*dx)
{
	uint8_t *p=buf;
	uint32_t ires=0,fres=0;
	uint8_t ilen=0,flen=0,i;
	uint8_t mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-') { mask|=0X02; p++; }//是负数
		if(*p==','||(*p=='*')) { break; }//遇到结束了
		if(*p=='.') { mask|=0X01; p++; }//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01) { flen++; }
		else { ilen++; }
		p++;
	}
	if(mask&0X02) { buf++; }	//去掉负号
	for(i=0; i<ilen; i++)	//得到整数部分数据
	{  
		ires += My_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5) { flen=5; } //最多取5位小数
	*dx = flen;	 		//小数点位数
	for(i=0; i<flen; i++)	//得到小数部分数据
	{  
		fres += My_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res = ires * My_Pow(10,flen)+fres;
	if(mask&0X02) { res = -res; }		   
	return res;
}	  	

/*********************************************************
**分析CalibMsg信息
**msg:校准信息结构体
**buf:接收到的GPS数据缓冲区首地址
*/
void Calib_Analysis(_CalibMsgType *msg, uint8_t *buf)
{
	uint8_t *p1, dx;			 
	uint8_t posx;    
	p1 = (uint8_t*)strstr((const char *)buf,"$CALIB");
	posx = My_Comma_Pos(p1, 1);
	if(posx!=0XFF)
	{
		msg->iBuckOffset = My_Str2num(p1+posx, &dx);
	}
	posx = My_Comma_Pos(p1,2);
	if(posx!=0XFF)
	{
		msg->iBoostOffset = My_Str2num(p1+posx, &dx);
	}
}
#endif




































