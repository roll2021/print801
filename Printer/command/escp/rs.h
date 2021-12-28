/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   指令相关程序.
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
#ifndef RS_H
#define RS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"

/* extern functions *---------------------------------------------------------*/
#ifdef	DBG_SELF_DEF_CMD_ENABLE

void RS_Command(void);

/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/
/* Private	Funtion Declare --------------------------------------------------*/
void Command_1E01(void);
void Command_1E02(void);
void Command_1E03(void);
void Command_1E04(void);

#ifdef	DBG_025
void	Command_1E10(void);	//测试字库
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* RS_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
