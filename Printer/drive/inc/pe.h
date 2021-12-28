/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   缺纸检测相关的程序.
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
#ifndef PE_H
#define PE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#ifndef	VREF
	#define	VREF						(3.3)				//AD参考电压
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE		(4095)			//AD转换最大值
#endif

#define	PE_ADC											ADC1								//纸尽ADC 
#define	PE_ADC_PORT									GPIOB								//纸尽ADC端口
#define	PE_ADC_PIN									GPIO_Pin_0					//纸尽ADC管脚
#define	PE_ADC_CHANNEL							ADC_Channel_8				//纸尽ADC通道
#define PE_ADC_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOB
	 
//压白纸时,输出电压0.15V;不放纸压下胶辊时,输出2.9V;全空白,输出3.23V
//高电平时为缺纸，低电平为有纸
/********* 定义测纸传感器三种电压对应的转换值 **********/
#define PE_ADC_VALUE_LOW					(ADC_MAX_VALUE * 0.6 / VREF)
#define PE_ADC_VALUE_MIDDLE				(ADC_MAX_VALUE * 1.6 / VREF)
#define PE_ADC_VALUE_HIGH					(ADC_MAX_VALUE * 2.0 / VREF)
#define	PE_ADC_VALUE_ONLY_PLATEN	(ADC_MAX_VALUE * 2.8 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitPaperEndPort(void);

#ifdef __cplusplus
}
#endif

#endif /* PE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
