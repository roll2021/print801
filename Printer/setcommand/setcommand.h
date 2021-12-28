/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-28
  * @brief   设置参数指令相关的程序.
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
#ifndef SETCOMMAND_H
#define SETCOMMAND_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

extern	void USART_SendStr(USART_TypeDef * USARTx, uint8_t *Str);

/* Macro Definition ----------------------------------------------------------*/
/* Funtion Declare -----------------------------------------------------------*/
void ReadSetByte(uint8_t *Parameter, uint8_t MaxValue);
void SetCommandFun(void);
void SetCommand_1F12(void);
	 
void SetCommand_1F1F(void);
void SetCommand_1F18(void);
void SetCommand_1F28(void);
void SetCommand_1F39(void);
void SetCommand_1F13(void);
void SetCommand_1F14(void);
void SetCommand_1F2F(void);
void SetCommand_1F15(void);
void SetCommand_1F2E(void);
void SetCommand_1F1A(void);
void SetCommand_1F19(void);
void SetCommand_1F44(void);
void SetCommand_1F2C(void);
void SetCommand_1F43(void);
void SetCommand_1F27(void);
void SetCommand_1F3B(void);
void SetCommand_1F30(void);
void SetCommand_1F3E(void);
void SetCommand_1F17(void);
void SetCommand_1F55(void);
void SetCommand_1F42(void);
void SetCommand_1F16(void);
void SetCommand_1F34(void);
void SetCommand_1F49(void);
void SetCommand_1F4A(void);
void ReceiverString(uint8_t * Str, uint8_t Length);

void US_Command(void);


#ifdef __cplusplus
}
#endif

#endif /* SETCOMMAND_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
