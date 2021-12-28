/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   机芯温度检测相关的程序.
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
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

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

#define	TEMPERATURE_ADC												ADC1							//输入电压ADC 
#define	TEMPERATURE_ADC_PORT									GPIOC							//输入电压ADC端口
#define	TEMPERATURE_ADC_PIN										GPIO_Pin_4				//输入电压ADC管脚
#define	TEMPERATURE_ADC_CHANNEL								ADC_Channel_14		//输入电压ADC通道
#define TEMPERATURE_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

/********* 定义温度传感器报警的上下限温度对应的转换值 **********/
/******** 温度检测公式  *******************/
/*AD转换值AD=ADC_MAX_VALUE*(RT/(RT+RH)，
*	 RT为对应温度的电阻值,RH为串联分压电阻*/
#define	RH						(8.2)			//串联分压电阻 KΩ

/* 注意热敏电阻的参数，以下为R25=30KΩ ,B=3950K的电阻，
* Rx=R25*EAP^(B*(1/(273+Tx)-1/(273+25)),EAP=2.71828
* 以下为当前机芯温敏电阻值在不同温度下的阻值，从-20到100℃ 
*/
#define	RTH_20				(269)
#define	RTH_15				(208)
#define	RTH_10				(178)
#define	RTH_5					(124)
#define	RTH0					(100)
#define	RTH5					(78)
#define	RTH10					(60)
#define	RTH15					(47.1)
#define	RTH20					(37.5)
#define	RTH25					(30.0)
#define	RTH30					(24.2)
#define	RTH35					(19.6)
#define	RTH40					(15.9)
#define	RTH45					(13.1)
#define	RTH50					(10.8)
#define	RTH55					(8.91)
#define	RTH60					(7.41)
#define	RTH65					(6.2)
#define	RTH70					(5.21)
#define	RTH75					(4.4)
#define	RTH80					(3.74)
#define	RTH85					(3.18)
#define	RTH90					(2.72)
#define	RTH95					(2.33)
#define	RTH100				(2.00)

/* 温度报警下限值 60℃ (FOR LTPFx47机芯)，温度报警上限值 85℃ */
#define TEMPERATURE_WARN_ADC_LOW		((uint16_t)(ADC_MAX_VALUE * RTH60 / (RH + RTH60)))
#define TEMPERATURE_WARN_ADC_HIGH		((uint16_t)(ADC_MAX_VALUE * RTH85 / (RH + RTH85)))

/* 温度错误下限值 -20℃ (FOR LTPFx47机芯)，温度错误上限值 100℃ */
#define TEMPERATURE_ERROR_ADC_LOW		((uint16_t)(ADC_MAX_VALUE * RTH_20 / (RH + RTH_20)))
#define TEMPERATURE_ERROR_ADC_HIGH	((uint16_t)(ADC_MAX_VALUE * RTH100 / (RH + RTH100)))

#define TEMPERATURE_TABLE_VALUE_LOW						(-20)	//定义温度值表第一个对应的温度值
#define TEMPERATURE_TABLE_VALUE_TOTAL_NUMBER	(25)	//定义温度值表中数量

/* Funtion Declare -----------------------------------------------------------*/
void	InitTemperaturePort(void);

#ifdef __cplusplus
}
#endif

#endif /* TEMPERATURE_H文件定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
