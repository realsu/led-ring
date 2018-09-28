/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  tigergatlin
  * @version V1.0
  * @date    2017-11-18
  * @brief   main函数
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_spi.h"
#include "bsp_led.h" 
#include "delay.h" 
#include "WS2812B.h" 
#include "WS2811.h"
#include "sys.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	  int i;
	  /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  delay_ms(500);

	  //WS_Init();
	  LED_SPI_LowLevel_Init();
	
  	while(1)
		{  
			  WS_SetAll();
        LED_SPI_Update(WsDat,LED_Num);

				delay_ms(20);
			
			/*
				WS_SetAll_1();
        LED_SPI_Update(WsDat1,LED_Num);
			
				delay_ms(200);
*/
//			ColorToColor(0x004400,0x000000);
//			ColorToColor(0x000000,0x000044);
//			for(i=0;i<96;i++)
//			{
//				WS_SetAll();
//				delay_ms(500);
//			}
						
		}	  
}

/*********************************************END OF FILE**********************/

