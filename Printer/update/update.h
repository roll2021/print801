/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-30
  * @brief   相关的程序.
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
#ifndef	UPDATE_H
#define	UPDATE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
//#include	<absacc.h>
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/

/* Funtion Declare -----------------------------------------------------------*/
void	WriteToBackupReg(uint16_t FirstBackupData);
uint8_t	CheckBackupReg(uint16_t FirstBackupData);
void	CheckBootLegality(void);
void System_Upgrade(uint8_t Interface);
void	Erase_Flash_Inside(void);
void	Update(void);
void ConfigOptionByte(void);
#ifdef __cplusplus
}
#endif

#endif /* UPDATE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
