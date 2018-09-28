#include "WS2811.h"
#include "sys.h"
uint16_t BufferSize=1024;
uint16_t PixelBuffer[1024] = {0};
uint16_t PixelPointer = 0;

void LED_SPI_LowLevel_Init(void)
{
    uint16_t i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

		//DMA���ݴ��������
		//ͨ��STM�ο��ֲ���Բ鵽SPI��TXʹ�õ�DMA1_CH3ͨ��
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (SPI1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)PixelBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure); /* DMA1 CH3 = MEM -> DR */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

		/*
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
		*/
		//ǰ����Ϳ��Կ��������ǽ�SPI���ó�����ģʽ�µĵ��߷���ģʽ
		//������16΢����֡��ʽ��ʱ����������δ�ã�Ƭѡ����δ�á�

    SPI_I2S_DeInit(SPI1);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; /* 48MHz / 8 = 6MHz */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    SPI_Cmd(SPI1, ENABLE);

    for (i = 0; i < BufferSize; i++)
    {
        PixelBuffer[i] = 0xAAAA;
    }

    PixelPointer = 0;

}
//����SPIģ��RGB��0��1��д������������
// zero = 0x7000;  //111000000000000,����ws2812�����24bit��0
// one = 0x7F00;   //111111100000000,����ws2812�����24bit��1
void LED_SPI_WriteByte(uint16_t Data)
{
    PixelBuffer[PixelPointer] = Data;
    PixelPointer++;

}

//��R,G,B��ɫ�ֿ�����
void LED_SPI_SendBits(uint8_t bits)
{
		//SPI��MOSI���ţ�����MO���ܣ�ͨ������һ���ֽڻ��߰��ֵ����ݣ���ģ��LED�����źŵ�0��1
    int zero = 0x7000;  //111000000000000,����ws2812�����24bit��0
    int one = 0x7F00;   //111111100000000,����ws2812�����24bit��1
    int i = 0x00;
    for (i = 0x80; i >= 0x01; i >>= 1)
    {
				//0x80 = 1000 0000
				//(bits & i)��λ�룬֮���8bit ת�� 16bit ��zero��one
				//RGB��ÿ1bit��Ҫ2byte(16b)д������������
        LED_SPI_WriteByte((bits & i) ? one : zero);
    }
}
//ÿ��������η�����ɫ����
void LED_SPI_SendPixel(uint32_t color)
{
    /*
     r7,r6,r5,r4,r3,r2,r1,r0,g7,g6,g5,g4,g3,g2,g1,g0,b7,b6,b5,b4,b3,b2,b1,b0
     \_____________________________________________________________________/
                               |      _________________...
                               |     /   __________________...
                               |    /   /   ___________________...
                               |   /   /   /
                              RGB,RGB,RGB,RGB,...,STOP
    */

    /*
    	BUG Fix : Actual is GRB,datasheet is something wrong.
    */
	  uint8_t Red, Green, Blue;  // ��ԭɫ
	// �� �� �� ��ԭɫ�ֽ�color��0XAA11BB��r:AA(170),g:11(17),b:BB(187)
	//
	  Red   = color>>16; //�õ�AA
	  Green = color>>8;	//�õ�11
	  Blue  = color;		//�õ�BB
    LED_SPI_SendBits(Green);
    LED_SPI_SendBits(Red);
    LED_SPI_SendBits(Blue);
}

//LED������º���
//������LED��ɫ���飬LED��������
void LED_SPI_Update(unsigned long buffer[], uint32_t length)
{
    uint8_t i = 0;
//    uint8_t m = 0;

    if(DMA_GetCurrDataCounter(DMA1_Channel3) == 0)	//�жϵ�ǰ�����Ƿ����
    {
				//������ÿ��Pixel���з���
				for(i = 0; i < length; i++)
        {
					//�������
            LED_SPI_SendPixel(buffer[i]);
        }

				/*
        if(length < 16)
        {
            for(i = 16 - length; i < length; i++)
            {
                for(m = 0; m < 3; m++)
                {
                    LED_SPI_SendBits(0x00);
                }
            }
        }
				
        for (i = 0; i < 20; i++)   
        {
            LED_SPI_WriteByte(0x00);
        }
				*/
        PixelPointer = 0;

        DMA_Cmd(DMA1_Channel3, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC3);
        DMA_SetCurrDataCounter(DMA1_Channel3, 24*length+20);
				//DMA_SetCurrDataCounter(DMA1_Channel3, length);
        DMA_Cmd(DMA1_Channel3, ENABLE);
				//delay_ms(1000);
    }
}

/*
void LED_SPI_Update(unsigned long buffer[], uint32_t length)
{
    uint8_t i = 0;
    uint8_t m = 0;
    if(DMA_GetCurrDataCounter(DMA1_Channel3) == 0)
    {

        for (i = 0; i < length; i++)
        {
            LED_SPI_SendPixel(buffer[i]);
        }

        if(length < 16)
        {
            for(i = 16 - length; i < length; i++)
            {
                for(m = 0; m < 3; m++)
                {
                    LED_SPI_SendBits(0x00);
                }
            }
        }

        for (i = 0; i < 20; i++)   
        {
            LED_SPI_WriteByte(0x00);
        }

        PixelPointer = 0;

        DMA_Cmd(DMA1_Channel3, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC3);
        DMA_SetCurrDataCounter(DMA1_Channel3, 24*length+20);
        DMA_Cmd(DMA1_Channel3, ENABLE);
    }
}
*/


