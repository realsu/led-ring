/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  tigergatlin
  * @version V1.0
  * @date    2017-11-18
  * @brief   ws2812����
  ******************************************************************************
  */

#include "WS2812B.h"
#include "sys.h"
/* �Դ� */                //R  G  B

//unsigned long WsDat[nWs]={0x0000EE,0x0000CC,0x0000AA,0x000088,0x000066,0x000044};
//unsigned long WsDat[nWs]={0x0000EE,0x0000CC,0x0000AA};

unsigned long WsDat[nWs]={0x00AAAA,0x000000,0x00AAAA,0x00AAAA,0x00AAAA,0x00AAAA};

/*
unsigned long WsDat[nWs]={0x000000,0x000000,0x000000,0xAAAA00,
													 0x000000,0x888800,0x000000,0x999900,
													 0xAAAA00,0xBBBB00,0x000000,0x000000,
													 0x000000,0xFFFF00,0x000000,0x000000,
};


unsigned long WsDat[nWs]={0x00FFFF,0x00EEEE,0x00DDDD,0x00CCCC,
													0x00BBBB,0x00AAAA,0x009999,0x008888,
                          0x007777,0x006666,0x005555,0x004444,                        
                          0x003333,0x002222,0x001111,0x000505,
	                        0x000505,0x001111,0x002222,0x003333,
                          0x004444,0x005555,0x006666,0x007777,
													0x008888,0x009999,0x00AAAA,0x00BBBB,
													0x00CCCC,0x00DDDD,0x00EEEE,0x00FFFF,	
};
*/

/**************************************************************************************
* IO��ʼ������ֲʱ���޸ģ�
**************************************************************************************/
void WS_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
	
	//�˿�ʱ�ӣ�ʹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );	 

	// �˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				// PIN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// �����趨������ʼ�� 
}

/**************************
* �ڲ���ʱ
***************************/
void delay2us()
{
	unsigned char i;
	i=3;
	while(i)
	{
		i--;
	}
	
}
void delay05us()
{
	unsigned char i;
	i=1;
	while(i)
	{
		i--;
	}
}

/***************************
* ����һ����
****************************/
void TX0()  	{ PAout(7) = 1; delay05us(); PAout(7) = 0; delay2us(); } // ����0
void TX1()  	{ PAout(7) = 1; delay2us();  PAout(7) = 0; delay05us(); } // ����1
void WS_Reset() { PAout(7) = 0; delay_us(60);PAout(7) = 1; PAout(7) = 0;   }

/**************************************************************************************
* ����һ�ֽ�
**************************************************************************************/
void WS_Set1(unsigned long dat)
{
	unsigned char i;
	
	for(i=0; i<24; i++)
	{
		if(0x800000 == (dat & 0x800000) )	
			TX1();
		else						
			TX0();
		dat<<=1;							//����һλ
	}
}

/**************************************************************************************
* ���������ֽ�
**************************************************************************************/
/*
void WS_SetAll_1()
{
	//static uint8_t k;
	unsigned char j;
	unsigned long temp;
	
	for(j=0; j<nWs; j++)
	{
		WS_Set1(WsDat1[j]);  // j / 0
	}
	WS_Reset();					
}
*/

void WS_SetAll()
{
	//static uint8_t k;
	unsigned char j;
	unsigned long temp;


//	for(j=0; j<nWs; j++)
//	{
//		WS_Set1(WsDat[j]);  // j / 0
//	}
//	WS_Reset();									//�̶���ʾ

	    
	{
		temp=WsDat[0];
		
		for(j=0;j<LED_Num-1;j++)
		{
			WsDat[j]=WsDat[j+1];
		}
		WsDat[LED_Num-1]=temp;
  }													//��ˮ��
	
}
/********************************************
* �����ֵ
********************************************/
unsigned char abs0(int num)
{
	if(num>0) return num;
	
	num = -num;
	return (unsigned char) num;
}

/***********************************************************************************
* ��ɫ�����㷨
* ��� <= 2
************************************************************************************/
u32 ColorToColor(unsigned long color0, unsigned long color1)
{
	unsigned char Red0, Green0, Blue0;  // ��ʼ��ԭɫ
	unsigned char Red1, Green1, Blue1;  // �����ԭɫ
	int			  RedMinus, GreenMinus, BlueMinus;	// ��ɫ�color1 - color0��
	unsigned char NStep; 							// ��Ҫ����
	float		  RedStep, GreenStep, BlueStep;		// ��ɫ����ֵ
	unsigned long color;							// ���ɫ
	unsigned char i,j;
	static uint32_t WsDatTemp;
	// �� �� �� ��ԭɫ�ֽ�
	Red0   = color0>>16;
	Green0 = color0>>8;
	Blue0  = color0;
	
	Red1   = color1>>16;
	Green1 = color1>>8;
	Blue1  = color1;
	
	// ������Ҫ���ٲ���ȡ��ֵ�����ֵ��
	RedMinus   = (Red1 - Red0); 
	GreenMinus = (Green1 - Green0); 
	BlueMinus  = (Blue1 - Blue0);
	
	NStep = ( abs0(RedMinus) > abs0(GreenMinus) ) ? abs0(RedMinus):abs0(GreenMinus);
	NStep = ( NStep > abs0(BlueMinus) ) ? NStep:abs0(BlueMinus);
	
	// �������ɫ����ֵ
	RedStep   = (float)RedMinus   / NStep;
	GreenStep = (float)GreenMinus / NStep;
	BlueStep  = (float)BlueMinus  / NStep;

	// ���俪ʼ
	for(i=0; i<NStep; i++)
	{
		Red1   = Red0   + (int)(RedStep   * i);
		Green1 = Green0 + (int)(GreenStep * i);
		Blue1  = Blue0  + (int)(BlueStep  * i);
		
		color  = Green1<<16 | Red1<<8 | Blue1; 	// �ϳ�  �̺���
		WsDatTemp = color;
		for(j=0; j<nWs; j++)
	{
		WS_Set1(WsDatTemp);  // j / 0
	}
	  //WS_Reset();
		//delay_ms(5);						// �����ٶ�
	}
	// �������
	
	return color;
}





