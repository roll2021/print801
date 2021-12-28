/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   纸将尽缺纸检测相关的程序.
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
#ifndef PNE_H
#define PNE_H

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

#define	PNE_ADC												ADC1								//纸将尽ADC
#define	PNE_ADC_PORT									GPIOC								//纸将尽ADC端口
#define	PNE_ADC_PIN										GPIO_Pin_2					//纸将尽ADC管脚
#define	PNE_ADC_CHANNEL								ADC_Channel_12			//纸将尽ADC通道
#define PNE_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

/********* 定义纸将尽传感器三种电压对应的转换值 **********/
/*使用光电传感器,发光LED正极串接470欧到VCC，接收三极管集电极串接100K接到VCC3.3V.*/
/*发光二极管限流电阻470欧,接收三极管发射极接地.*/
/*信号输出为集电极*/
/*纸将尽时电压值接近3V,纸充足时电压值接近0V*/
#define PNE_ADC_VALUE_LOW				(ADC_MAX_VALUE * 1.0 / VREF)
#define PNE_ADC_VALUE_MIDDLE		(ADC_MAX_VALUE * 1.6 / VREF)
#define PNE_ADC_VALUE_HIGH			(ADC_MAX_VALUE * 2.0 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitPaperNearEndPort(void);

#ifdef __cplusplus
}
#endif

#endif /* PNE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
