/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-15
  * @brief   AD转换相关的程序.
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
#ifndef DRAWER_H
#define DRAWER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
//定义引脚
#define	MBOX_OUT1_PORT 								GPIOD					//钱箱输出控制端口
#define MBOX_OUT1_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOD
#define	MBOX_CTRL1			 							GPIO_Pin_0
	 
#define	MBOX_OUT2_PORT 								GPIOC					//钱箱输出控制端口
#define MBOX_OUT2_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOC
#define	MBOX_CTRL2			  						GPIO_Pin_12	 

#define	MBOX_IN_PORT 		 							GPIOC					//钱箱输入控制端口
#define MBOX_IN_RCC_APB2Periph_GPIO	  RCC_APB2Periph_GPIOC
#define	MBOX_STATUS										GPIO_Pin_9
	 
//信号控制
#define	MBOX_CTRL1_SET								GPIO_SetBits(MBOX_OUT1_PORT, MBOX_CTRL1)
#define	MBOX_CTRL1_CLR								GPIO_ResetBits(MBOX_OUT1_PORT, MBOX_CTRL1)
#define	MBOX_CTRL2_SET								GPIO_SetBits(MBOX_OUT2_PORT, MBOX_CTRL2)
#define	MBOX_CTRL2_CLR								GPIO_ResetBits(MBOX_OUT2_PORT, MBOX_CTRL2)
#define	READ_MONEYBOX_STATUS					GPIO_ReadInputDataBit(MBOX_IN_PORT, MBOX_STATUS)

	 
/* Funtion Declare -----------------------------------------------------------*/
	 
void	InitDrawerPort(void);
	 
	 
	 
#ifdef __cplusplus
}
#endif

#endif /* AD_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
