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
#ifndef GS_H
#define GS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<stdlib.h>
#include	"stm32f10x.h"

/* extern functions *---------------------------------------------------------*/
extern uint8_t	*VersionString;
extern uint8_t	*LanguageString[];
extern void ProcessBarCode_2D_1D28(void);
/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/
/* Funtion Declare -----------------------------------------------------------*/
void LongToStr(uint32_t num, uint8_t *Str);
void Command_1D21(void);
void Command_1D23(void);
void Command_1D24(void);
void Command_1D2841(void);
void Command_1D2844(void);
void Command_1D2845(void);
void Command_1D284B(void);
void Command_1D284C(void);
void Command_1D284E(void);
void Command_1D286B(void);
void Command_1D28(void);
void Command_1D2A(void);
void Command_1D2F(void);
void Command_1D3A(void);
void Command_1D42(void);
void Command_1D44(void);
void Command_1D45(void);
void Command_1D48(void);
void Command_1D49(void);
void Command_1D4C(void);
void Command_1D50(void);
void Command_1D51(void);
void Command_1D55(void);
void Command_1D56(void);
void Command_1D57(void);
void Command_1D5A(void);
void Command_1D5E(void);
void Command_1D61(void);
void Command_1D66(void);
void Command_1D6730(void);
void Command_1D6732(void);
void Command_1D67(void);
void Command_1D68(void);
void Command_1D6B(void);
void Command_1D72(void);
void Command_1D76(void);
void Command_1D77(void);
void GS_Command(void);

#ifdef __cplusplus
}
#endif

#endif /* FS_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
