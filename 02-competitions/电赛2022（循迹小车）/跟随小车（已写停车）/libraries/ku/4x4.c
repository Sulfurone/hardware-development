#include "4x4.h"

#include "main.h"

int AA;
int LL;
int q,w;
u16 a[5];
u16 l[5];
void shu(void)
{
  a[1] = A1;
	a[2] = A2;
	a[3] = A3;
	a[4] = A4;
	l[1] = L1;
	l[2] = L2;
	l[3] = L3;
	l[4] = L4;
}

void aInit(void)
{
	//ÐÐ
GPIO_setAsInputPin(a1_PORT, a1_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(a1_PORT, a1_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a1_PORT, a1_PIN);//½ûÖ¹ÖÐ¶
	
GPIO_setAsInputPin(a2_PORT, a2_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(a2_PORT, a2_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a2_PORT, a2_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsInputPin(a3_PORT, a3_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(a3_PORT, a3_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a3_PORT, a3_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsInputPin(a4_PORT, a4_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(a4_PORT, a4_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a4_PORT, a4_PIN);//½ûÖ¹ÖÐ¶Ï
//ÁÐ
GPIO_setAsOutputPin(l1_PORT, l1_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(l1_PORT, l1_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l1_PORT, l1_PIN);//½ûÖ¹ÖÐ
	
GPIO_setAsOutputPin(l2_PORT, l2_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(l2_PORT, l2_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l2_PORT, l2_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsOutputPin(l3_PORT, l3_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(l3_PORT, l3_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l3_PORT, l3_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsOutputPin(l4_PORT, l4_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(l4_PORT, l4_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l4_PORT, l4_PIN);//½ûÖ¹ÖÐ¶Ï
}
void lInit(void)
{
	//ÐÐ
GPIO_setAsOutputPin(a1_PORT, a1_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(a1_PORT, a1_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a1_PORT, a1_PIN);//½ûÖ¹ÖÐ¶
	
GPIO_setAsOutputPin(a2_PORT, a2_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(a2_PORT, a2_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a2_PORT, a2_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsOutputPin(a3_PORT, a3_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(a3_PORT, a3_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a3_PORT, a3_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsOutputPin(a4_PORT, a4_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setOutputLowOnPin(a4_PORT, a4_PIN);//ÉÏÀ­
GPIO_disableInterrupt(a4_PORT, a4_PIN);//½ûÖ¹ÖÐ¶Ï
//ÁÐ
GPIO_setAsInputPin(l1_PORT, l1_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(l1_PORT, l1_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l1_PORT, l1_PIN);//½ûÖ¹ÖÐ
	
GPIO_setAsInputPin(l2_PORT, l2_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(l2_PORT, l2_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l2_PORT, l2_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsInputPin(l3_PORT, l3_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(l3_PORT, l3_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l3_PORT, l3_PIN);//½ûÖ¹ÖÐ¶Ï
	
GPIO_setAsInputPin(l4_PORT, l4_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
GPIO_setAsInputPinWithPullUpResistor(l4_PORT, l4_PIN);//ÉÏÀ­
GPIO_disableInterrupt(l4_PORT, l4_PIN);//½ûÖ¹ÖÐ¶Ï
}


u8 KEY_Scan(void)
{	 
	u8 check,temp;
	u16 keyvalue = 0;
	aInit();
//	keyvalue = A1+A2+A3+A4+L1+L2+L3+L4;
	if(A1+A2+A3+A4+L1+L2+L3+L4 != 4)
	{
	delay_ms(10);//Ïû¶¶ÑÓÊ±
	if(A1+A2+A3+A4+L1+L2+L3+L4 != 4)
	{
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);//ÉÏÀ­
		shu();
			for(w = 1;w<5;w++)
	 {
		temp=a[w];
		if(temp == 0)//¼ì²âÁÐ ÁÐÖµÎª i
		{
			AA=w;
		}
	 }
	  switch(AA)
			{
				case 1:keyvalue = 4;break;
				case 2:keyvalue = 3;break;
				case 3:keyvalue = 2;break;
				case 4:keyvalue = 1;break;
			}
			lInit();
			delay_ms(10);
			shu();
			for(q = 1;q<5;q++)
	 {
		check=l[q];
		if(check == 0)//¼ì²âÁÐ ÁÐÖµÎª i
		{
			LL=q;
		}
	 }
	 switch(LL)
			{
				case 1:keyvalue += 0;break;
				case 2:keyvalue += 4;break;
				case 3:keyvalue += 8;break;
				case 4:keyvalue += 12;break;
			}	
  }
 }
	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);//ÉÏÀ­
return keyvalue;
}






//while(a[w] == 0)//ËÉÊÖ¼ì²â
//			{
//				GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);//ÉÏÀ­
//			}
//			delay_ms(10);//Ïû¶¶ÑÓÊ±
//			GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);//ÉÏÀ­

























//readvalue = A1+A2+A3+A4+L1+L2+L3+L4;
//	readvalue &= 0x00ff;
//	if(readvalue != 0x000f)
//	{
//		delay_ms(8);//Ïû¶¶10ms
//		if(readvalue != 0x000f)
//		{
//			readvalue = A1+A2+A3+A4+L1+L2+L3+L4;//ÔÙ´Î¶ÁÈ¡×´Ì¬
//			readvalue &= 0x00ff;
//			switch(readvalue)
//			{
//				case (0x001f):keyvalue = 1;break;
//				case (0x002f):keyvalue = 2;break;
//				case (0x004f):keyvalue = 3;break;
//				case (0x008f):keyvalue = 4;break;
//			}
//			lInit();
//			delay_ms(8);
//			readvalue =A1+A2+A3+A4+L1+L2+L3+L4;//ÔÙ´Î¶ÁÈ¡×´Ì¬
//			readvalue |= 0x00f0;
//			readvalue &= 0x00ff;
//			switch(readvalue)
//			{
//				case (0x00f1):keyvalue += 0;break;
//				case (0x00f2):keyvalue += 4;break;
//				case (0x00f4):keyvalue += 8;break;
//				case (0x00f8):keyvalue += 12;break;
//			}
//			
//			while(readvalue != 0x00f0)//¼ì²â°´¼üËÉ¿ª
//			{
//				GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);//ÉÏÀ­
//				readvalue = A1+A2+A3+A4+L1+L2+L3+L4;//ÔÙ´Î¶ÁÈ¡×´Ì¬
//				readvalue |= 0x00f0;
//				readvalue &= 0x00ff;
//			}
//		}
//	}
//		GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);//ÉÏÀ­
//	return keyvalue;
























































//	for(i=0;i<4;i++){
//GPIO_setAsInputPin(ai_PORT, ai_PIN); //Ñ¡Ôñ¶Ë¿Ú¼°Òý½Å×÷ÎªÊäÈë¶Ë¿Ú
//GPIO_setAsInputPinWithPullUpResistor(ai_PORT, ai_PIN);//ÉÏÀ­
//GPIO_disableInterrupt(ai_PORT, ai_PIN);//½ûÖ¹ÖÐ¶Ï
//	}
