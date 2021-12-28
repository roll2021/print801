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
#ifndef FS_H
#define FS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* extern functions *---------------------------------------------------------*/	 
/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/
#define		MAX_DOWNLOAD_NVFLASH_BITMAP_NUM				(0x08)

/* Funtion Declare -----------------------------------------------------------*/
void Command_1C21(void);
void Command_1C26(void);
void Command_1C2D(void);
void Command_1C2E(void);
void Command_1C32(void);
void Command_1C3F(void);
void Command_1C43(void);
void Command_1C50(void);
void Command_1C53(void);
void Command_1C55(void);
void Command_1C56(void);
void Command_1C57(void);
void Command_1C58(void);
void Command_1C67(void);
void Command_1C70(void);
void Command_1C71(void);
void FS_Command(void);

#ifdef __cplusplus
}
#endif

#endif /* FS_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
