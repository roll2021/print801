/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-9
  * @brief   全局变量封装操作相关程序.
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
#ifndef GVAROP_H
#define GVAROP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"extgvar.h"	//全局外部变量声明

/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/
/* Funtion Declare -----------------------------------------------------------*/
uint8_t		GetCommandSystem(void);

#ifdef __cplusplus
}
#endif

#endif /* GVAROP_H 定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
