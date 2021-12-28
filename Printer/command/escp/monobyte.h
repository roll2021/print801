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
#ifndef MONOBYTE_H
#define MONOBYTE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* extern functions *---------------------------------------------------------*/	 
extern uint8_t	ReadInBuffer(void);
extern	void DecInBufferGetPoint(void);
	 
/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/

/* Funtion Declare -----------------------------------------------------------*/
void Command_09(void);
void Command_0A(void);
void Command_0C(void);
void Command_0D(void);
void Command_15(void);
void Command_16(void);
void Command_18(void);
void MonoByte_Command(uint8_t CommandCode);
void Command_1004(void);
void Command_1005n(void);
void Command_1006n(void);
void Command_1014fnmt(void);
void RealTimeCommand(void);
void AutomaticStatusBack(void);

#ifdef __cplusplus
}
#endif

#endif /* MONOBYTE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
