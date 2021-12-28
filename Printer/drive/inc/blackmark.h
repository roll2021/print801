/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   黑标检测相关的程序.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2016</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLACKMARK_H
#define BLACKMARK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
//#define	BLACKMARK_ADC_USE_PE_ADC		//黑标与纸尽传感器是否独立

#ifndef	VREF
	#define	VREF						(3.3)				//AD参考电压
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE		(4095)			//AD转换最大值
#endif

#define	BLACKMARK_ADC												ADC1						//黑标ADC
#define	BLACKMARK_ADC_PORT									GPIOC						//黑标ADC端口
#define	BLACKMARK_ADC_PIN										GPIO_Pin_1			//黑标ADC管脚
#define	BLACKMARK_ADC_CHANNEL								ADC_Channel_11	//黑标ADC通道
#define BLACKMARK_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

/********* 定义黑标传感器三种电压对应的转换值 **********/
/* 在黑标位置时BM检测到高电平，测量为2V左右 */
/* 不在黑标位置时检测到低电平，接近0.3V */
#define BLACKMARK_ADC_VALUE_LOW			(ADC_MAX_VALUE * 1.0 / VREF)
#define BLACKMARK_ADC_VALUE_MIDDLE	(ADC_MAX_VALUE * 1.6 / VREF)
#define BLACKMARK_ADC_VALUE_HIGH		(ADC_MAX_VALUE * 2.0 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitBlackMarkPort(void);

#ifdef __cplusplus
}
#endif

#endif /* BLACKMARK_H */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
