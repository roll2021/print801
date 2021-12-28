/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   胶辊（纸仓盖）检测相关的程序.
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
#ifndef PLATEN_H
#define PLATEN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
//定义检测方式,通过外部中断方式还是IO查询方式
//#define		PLATEN_CHECK_BY_INT

#ifdef	PLATEN_CHECK_BY_INT
	#define GPIO_PORT_PLATEN										GPIOB
	#define RCC_APB2Periph_GPIO_PLATEN					RCC_APB2Periph_GPIOB
	#define GPIO_PIN_PLATEN											GPIO_Pin_1
	#define EXTI_LINE_PLATEN										EXTI_Line1
	#define GPIO_PORT_SOURCE_PLATEN							GPIO_PortSourceGPIOB
	#define GPIO_PIN_SOURCE_PLATEN							GPIO_PinSource1
#else
	#define PLATEN_GPIO_PORT										GPIOB
	#define PLATEN_RCC_APB2Periph_GPIO					RCC_APB2Periph_GPIOB
	#define PLATEN_GPIO_PIN											GPIO_Pin_1
	#define	ReadPlatenStatus()									ReadCoverStatus()
#endif

/* 所用机芯压杆检测传感器为机械开关。机械开关一端接地，另一端常开，上拉到高电平。
*  机芯压杆压下，检测到电平变为高。机芯压杆打开，检测电平变为低。
*/
/* 定义机芯压杆开关时的测量到的值，与具体实现电路有关 */
#define		PLATEN_VALUE_CLOSE		(0)
#define		PLATEN_VALUE_OPEN			(1)

/* 定义返回的机芯压杆的状态，与具体电路及电平状态定义无关 */
#define		PLATEN_STATUS_CLOSE		(0)
#define		PLATEN_STATUS_OPEN		(1)

/* Funtion Declare -----------------------------------------------------------*/
#ifdef	PLATEN_CHECK_BY_INT
#else
	void	InitPlatenPort(void);
	uint8_t	ReadCoverStatus(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PLATEN_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
