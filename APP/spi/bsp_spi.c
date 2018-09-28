/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  tigergatlin
  * @version V1.0
  * @date    2017-11-18
  * @brief   SPI��ʼ���ӿ�
  ******************************************************************************
  */
  
#include "bsp_spi.h"


/*******************************************************************************
 * ��������VS1003_ReadByte
 * ����  ����SPI��ʽ��NRF24L01����һ���ֽ�
 * ����  ���� 
 * ���  ��-u8
 * ����  ����/�ⲿ����
 *******************************************************************************/	
void SPI1_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);

  /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
 
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}
/*******************************************************************************
 * ��������VS1003_ReadByte
 * ����  ����SPI��ʽ��NRF24L01����һ���ֽ�
 * ����  ���� 
 * ���  ��-u8
 * ����  ����/�ⲿ����
 *******************************************************************************/	
u8 SPI1_WriteByte(u8 byte)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}		// �ж�SPI1 ���ͻ������Ƿ��  
  SPI_I2S_SendData(SPI1, byte);											                // ����8λ����
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}	  // �ж��Ƿ���ջ������ǿ�
  return SPI_I2S_ReceiveData(SPI1);
}
/*******************************************************************************
 * ��������VS1003_ReadByte
 * ����  ����SPI��ʽ��NRF24L01����һ���ֽ�
 * ����  ���� 
 * ���  ��-u8
 * ����  ����/�ⲿ����
 *******************************************************************************/	
u8 SPI1_ReadByte(void)
{
   while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);  	 // �ж�SPI1 ���ͻ������Ƿ��  
   SPI_I2S_SendData(SPI1, 0);										                     // ����һ�����ֽ�	   
   while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	 // �ж��Ƿ���ջ������ǿ�
   return SPI_I2S_ReceiveData(SPI1);								                 // ���ؽ��յ�������
}  

/*********************************************END OF FILE**********************/
