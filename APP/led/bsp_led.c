/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  tigergatlin
  * @version V1.0
  * @date    2017-11-18
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  */
  
#include "bsp_led.h"   

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOB��GPIOF������ʱ��*/
		//RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);

		/*ѡ��Ҫ���Ƶ�GPIOB����*/															   
		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIOB0*/
		//GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

		/* �ر�����led��	*/
		//GPIO_SetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9);
 GPIO_SetBits(GPIOA,GPIO_Pin_8);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 ????, ????
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //???? ,IO????50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ??? 
}
/*********************************************END OF FILE**********************/
