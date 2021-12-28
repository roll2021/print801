/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   工作模式检测相关的程序.
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
#ifndef MODE_H
#define MODE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#define		MODE_NORMAL										(0)
#define		MODE_SELFTEST_PRT							(1)
#define		MODE_HEX_PRT									(2)
#define		MODE_SET_PARAS								(3)
#define		MODE_NORMAL_UPDATE						(4)
#define		MODE_RESTORE_DEFAULT_PARAS		(5)
#define		MODE_PRINT_TEST								(6)
#define		MODE_ERASE_APP								(7)
#define		MODE_RELEASE_JTAG							(8)
#define		MODE_RESTORE_NET_PARAS				(9)
	 
#define		MODE_DEFAULT									(MODE_NORMAL)

extern void SysDelay1mS(__IO uint32_t DelayNumber);
/* Private function prototypes -----------------------------------------------*/
uint8_t	GetModeType(void);
void PrtTestMode(void);
void PutStrToInbuf( const uint8_t *Str );

#ifdef __cplusplus
}
#endif

#endif /* MODE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
