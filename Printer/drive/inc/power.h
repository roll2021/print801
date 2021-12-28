/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   电源检测控制相关的程序.
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
#ifndef POWER_H
#define POWER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#ifndef	VREF
	#define	VREF							(3.3)				//AD参考电压
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE			(4095)			//AD转换最大值
#endif

#define	POWER_ADC												ADC1						//输入电压ADC 
#define	POWER_ADC_PORT									GPIOC						//输入电压ADC端口
#define	POWER_ADC_PIN										GPIO_Pin_5			//输入电压ADC管脚
#define	POWER_ADC_CHANNEL								ADC_Channel_15	//输入电压ADC通道
#define POWER_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

#define POWER_CTRL_GPIO_PORT						GPIOE
#define POWER_CTRL_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define POWER_CTRL_GPIO_PIN							GPIO_Pin_7

#define	HeatPowerOn()		GPIO_SetBits(POWER_CTRL_GPIO_PORT, POWER_CTRL_GPIO_PIN)
#define	HeatPowerOff()	GPIO_ResetBits(POWER_CTRL_GPIO_PORT, POWER_CTRL_GPIO_PIN)
//#define	HeatPowerOn()
//#define	HeatPowerOff()
//2017.08.16
#define PHY_RESET_GPIO_PORT						  GPIOD
#define PHY_RESET_RCC_APB2Periph_GPIO	  RCC_APB2Periph_GPIOD
#define PHY_RESET_GPIO_PIN							GPIO_Pin_14

#define	PHYResetLow()		GPIO_ResetBits(PHY_RESET_GPIO_PORT, PHY_RESET_GPIO_PIN)
#define	PHYResetHigh()	GPIO_SetBits(PHY_RESET_GPIO_PORT, PHY_RESET_GPIO_PIN)

/* 外部电源输入分压值，从0到3.3V，实际使用时根据使用电源电压不同在某值附近浮动 */
#define	POWER_CHANGE_RATIO	(0.15)		//电源电压变化百分率10%
#define	POWER_STD						(24.0)		//供电电源标准值，单位V
#define	POWER_MIN						(POWER_STD * (1.0 - POWER_CHANGE_RATIO))	//供电电源最小值，单位V
#define	POWER_MAX						(POWER_STD * (1.0 + POWER_CHANGE_RATIO))	//供电电源最大值，单位V

#define	POWER_DIVIDE_RATIO	(10.0 / (10.0 + 100.0))	//分压电阻值确定的供电分压比

//正常电源,低电压,高电压及告警关闭电压对应的转换值
#define POWER_VALUE_STD			((uint16_t)(POWER_STD * POWER_DIVIDE_RATIO * ADC_MAX_VALUE / VREF))
#define POWER_VALUE_LOW			((uint16_t)(POWER_VALUE_STD * 0.85))	//低压报警时对应的转换值
#define POWER_VALUE_HIGH		((uint16_t)(POWER_VALUE_STD * 1.15))	//高压报警时对应的转换值
#define POWER_VALUE_CLOSE		((uint16_t)(POWER_VALUE_STD * 1.2))		//高压关闭打印时对应的转换值

#define POWER_COEFFICIENT		(0.1)		//定义电压补偿系数（电压与加热时间的转换斜率）

/* Funtion Declare -----------------------------------------------------------*/
void	InitPowerCtrlPort(void);
void	InitPowerInPort(void);

#ifdef __cplusplus
}
#endif

#endif /* POWER_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
