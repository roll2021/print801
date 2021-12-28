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
#ifndef MAINTAIN_H
#define MAINTAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
     
// #if !defined (STM32F10X_MD) &&  !defined (STM32F10X_HD)
// //	#define STM32F10X_MD			//中密度器件
//  	#define STM32F10X_HD			//高密度器件
// #endif
//      
// #ifdef STM32F10X_MD
//   #define PAGE_SIZE							(0x400)
// 	#define MIN_FLASH_SIZE				(0x10000) /* 64K */
//   #define MAX_FLASH_SIZE				(0x20000) /* 128K */
// #elif defined STM32F10X_HD
//   #define PAGE_SIZE							(0x800)
// 	#define MIN_FLASH_SIZE				(0x40000) /* 256K */
//   #define MAX_FLASH_SIZE				(0x80000) /* 512K */
// #elif defined STM32F10X_CL
//   #define PAGE_SIZE							(0x800)
// 	#define MIN_FLASH_SIZE				(0x40000) /* 256K */
//   #define MAX_FLASH_SIZE				(0x80000) /* 512K */
// #endif

// #ifdef WITHBOOT
// 	#define ADDR 	(NVIC_VectTab_FLASH -PAGE_SIZE) 	//应用程序前一页
// #else
// 	#define ADDR 	(0x8040000 - PAGE_SIZE * 2 )						//最后一页
// #endif

#define MAINTAIN_WRITE_ROUNTIN_MINUTE		(60)

/* Funtion Declare -----------------------------------------------------------*/
uint8_t	GetMaintainParaStoreIndex(void);		//得到当前有效最先存储序列号
void	InitMaintainParameter(void);
void    ReadFlashParameter(void);
void	WriteFlashParameter(uint8_t Flag);
void	CheckMaintainRoutin(void);

#ifdef __cplusplus
}
#endif

#endif /* MAINTAIN_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
