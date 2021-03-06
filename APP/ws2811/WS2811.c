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

		//DMA数据传输的配置
		//通过STM参考手册可以查到SPI的TX使用的DMA1_CH3通道
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
		//前两句就可以看出，我们将SPI配置成了主模式下的单线发送模式
		//采用了16微数据帧格式，时钟引脚悬空未用，片选引脚未用。

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
//处理SPI模拟RGB的0和1，写到缓存数组中
// zero = 0x7000;  //111000000000000,代表ws2812灯珠的24bit的0
// one = 0x7F00;   //111111100000000,代表ws2812灯珠的24bit的1
void LED_SPI_WriteByte(uint16_t Data)
{
    PixelBuffer[PixelPointer] = Data;
    PixelPointer++;

}

//按R,G,B三色分开发送
void LED_SPI_SendBits(uint8_t bits)
{
		//SPI的MOSI引脚，利用MO功能，通过发送一个字节或者半字的数据，来模拟LED控制信号的0或1
    int zero = 0x7000;  //111000000000000,代表ws2812灯珠的24bit的0
    int one = 0x7F00;   //111111100000000,代表ws2812灯珠的24bit的1
    int i = 0x00;
    for (i = 0x80; i >= 0x01; i >>= 1)
    {
				//0x80 = 1000 0000
				//(bits & i)按位与，之后从8bits 转成 16bits 的zero和one
				//RGB的每1bit需要2byte(16b)写到缓存数组中
        LED_SPI_WriteByte((bits & i) ? one : zero);
    }
}
//每个灯珠如何发送颜色数组
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
	  uint8_t Red, Green, Blue;  // 三原色
	// 绿 红 蓝 三原色分解color，0XAA11BB，r:AA(170),g:11(17),b:BB(187)
	//
	  Red   = color>>16; //得到8bits AA
	  Green = color>>8;	//得到8bits 11
	  Blue  = color;		//得到8bits BB
    LED_SPI_SendBits(Green);
    LED_SPI_SendBits(Red);
    LED_SPI_SendBits(Blue);
}

//LED灯珠更新函数
//参数：LED颜色数组，LED灯珠总数
void LED_SPI_Update(unsigned long buffer[], uint32_t length)
{
    uint8_t i = 0;
//    uint8_t m = 0;

    if(DMA_GetCurrDataCounter(DMA1_Channel3) == 0)	//判断当前传输是否完成
    {
				//按灯珠每个Pixel进行发送
				for(i = 0; i < length; i++)
        {
					//逐个发送
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


