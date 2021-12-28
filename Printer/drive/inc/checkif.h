/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   通信接口检测相关的程序.
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
  * <h2><center>&copy; COPYRIGHT 2016 </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CHECKIF_H
#define CHECKIF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"extgvar.h"

/* Private typedef -----------------------------------------------------------*/
/******* 定义接口类型 *************/

/******* 定义接口类型结构 *************/

/* Macro Definition ----------------------------------------------------------*/
/* 检测输出信号端口 */
//#define INTERFACE_CHECK_OUT_GPIO_PORT							GPIOA
//#define INTERFACE_CHECK_OUT_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOA
//#define INTERFACE_CHECK_OUT_GPIO_PIN							GPIO_Pin_8

/* 检测输入信号端口 */
#define INTERFACE_CHECK_IN_GPIO_PORT							GPIOA
#define INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOA
#define INTERFACE_CHECK_IN0_GPIO_PIN							GPIO_Pin_8
#define INTERFACE_CHECK_IN1_GPIO_PIN							GPIO_Pin_10

#define CHECK_NET_GPIO_PORT							GPIOA
#define CHECK_NET_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOA
#define CHECK_NET_GPIO_PIN							GPIO_Pin_2


/* Private function prototypes -----------------------------------------------*/
void	InitCheckInterfacePort(void);
uint8_t		GetInterfaceType(void);
void	CheckInterface(void);

#ifdef __cplusplus
}
#endif

#endif /* CHECKIF_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
