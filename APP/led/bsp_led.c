/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  tigergatlin
  * @version V1.0
  * @date    2017-11-18
  * @brief   led应用函数接口
  ******************************************************************************
  */
  
#include "bsp_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIOB和GPIOF的外设时钟*/
		//RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);

		/*选择要控制的GPIOB引脚*/															   
		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		//GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

		/* 关闭所有led灯	*/
		//GPIO_SetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9);
 GPIO_SetBits(GPIOA,GPIO_Pin_8);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 ????, ????
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //???? ,IO????50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ??? 
}
/*********************************************END OF FILE**********************/
