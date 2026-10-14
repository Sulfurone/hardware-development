//#include "MLX90614.h"

// unsigned char gh_crc8( unsigned char * p_buffer, unsigned short buf_size )
//{
//    unsigned char crc = 0,i;
//    if(buf_size <= 0)
//    {
//        return crc;
//    }
//    while( buf_size-- )
//    {
//        for (  i = 0x80; i != 0; i /= 2 )
//        {
//            if ( (crc & 0x80) != 0)
//            {
//                crc *= 2;
//                crc ^= 0x07; // 多项式：X8 + X2 + X + 1
//            }
//            else
//            {
//                crc *= 2;
//            }
//			if ( (*p_buffer & i) != 0 )
//            {
//                crc ^= 0x07;
//            }
//        }
//        p_buffer++;
//    }
//    return crc;
//}
//			


//unsigned short Read_MLX90614_Data(unsigned char adress,unsigned char  commond)
//{
//	unsigned char low;
//	unsigned char high;
//	unsigned char PEC=0;
//	
//	unsigned char xu[6] = {0,0x8b,0xb9,0x01,0x07,0x00};
//	
//	
//	unsigned short data;
//	do
//	{
//	IIC_start();
//	send_ch((adress<<1)|0);
//	send_ch(commond);
//	IIC_start();
//	send_ch((adress<<1)|1);
//	low = read_ch1();
//	high = read_ch1();
//	PEC = read_ch();
//	IIC_stop();
//	data = (high<<8) | low;
//		
//	xu[0] = 	adress;
//	xu[1] = 	commond;
//	xu[2] = 	adress+1;		
//	xu[3] = 	low;	
//	xu[4] = 	high;		
//		
//	}while(PEC != gh_crc8(xu,5) );
//	
//	return data;
//}


//void maopao1(float a[],int n)//冒泡排序
//{
//    int i,j;
//    int temp;
//    for(i=0;i<n-1;i++)//n-1
//    {
//        for(j=0;j<n-i-1;j++)//n-i-1
//        {
//            if(a[j]>a[j+1])//大于就交换
//            {
//                temp = a[j];
//                a[j] = a[j+1];
//                a[j+1] = temp;
//            }

//        }
//    }
//}

//float MLX90614_Get_Temperature()
//{

//	float temp;
//	int i = 0;
//	float data_adc1[MLX90614_COLLECT_COUNT];         //ADC要用的，采集到的原值
//	float data_sum1 = 0;
//	
//	float av_value = 0;
//	for(i = 0;i < MLX90614_COLLECT_COUNT;i++)	//采集数据LMT70_COLLECT_COUNT次
//	{
//		temp = (Read_MLX90614_Data(MLX90614_ADRESS,RAM_OBJECT_TEMPERATURE)*0.02f-273.15f);
//		data_adc1[i] = temp;
//	
//	
//	}
//	
//	
//	

//	maopao1(data_adc1,MLX90614_COLLECT_COUNT);
//	for(i=0;i<MLX90614_COLLECT_COUNT-1;i++) //去除最大最小值
//	{
//		data_adc1[i] = data_adc1[i+1];
//	}	 

//	data_sum1 = 0;
//	for(i = 0;i < (MLX90614_COLLECT_COUNT-2);i++)	//均值滤波
//	{
//		data_sum1 = data_sum1 + data_adc1[i];
// 
//	}
//	av_value = data_sum1 / (MLX90614_COLLECT_COUNT-2);
//	
//	return av_value;




//}
