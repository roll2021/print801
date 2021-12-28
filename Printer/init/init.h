/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-31
  * @brief   初始化相关的程序.
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
#ifndef INIT_H
#define INIT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"ad.h"
#include	"beep.h"
#include	"blackmark.h"
#include	"button.h"
#include	"checkif.h"
#include	"cutter.h"
#include	"feedpaper.h"
#include	"heat.h"
#include	"led.h"
#include	"pe.h"
#include	"platen.h"
#include	"pne.h"
#include	"power.h"
#include	"print.h"
#include	"temperature.h"
#include	"mode.h"
#include	"spiflash.h"
#include	"spiheat.h"
#include	"usart.h"
#include	"extgvar.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"timconfig.h"
#include	"parallel.h"
//#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"button.h"
#include	"extgvar.h"
#include	"maintain.h"
#include	"esc.h"
#include	"sram.h"
#include	"gvarop.h"
#include	"cpcl.h"
#include	"ctext.h"
#include	"cgraphic.h"
#include	"cbarcode1d.h"
#include	"cbarcode2d.h"
#include	"cprint.h"
#include	"drawer.h"

/* Macro Definition ----------------------------------------------------------*/

/* Funtion Declare -----------------------------------------------------------*/
void	Sys_Init(void);
void	InitParameter(uint8_t initflag);
void	InitNVIC(void);
void	InitErrorStructParameter(void);
void	InitKeyCtrlStructParameter(void);
uint8_t	GetFeedButtonCount(void);

/* Static Funtion Declare ----------------------------------------------------*/
static void	InitCommonGPIO(void);
static void	EXT_SRAM_Init(void);
static void	Test_Ext_Sram(void);
static void	InitStdPeriph(void);
static void	GetConfigData(void);
static void	InitBarCodeParameter(void);
static void	InitUserDefCodeParameter(void);
static void	InitPrintModeParameter(void);
static void	InitSystemCtrlStructParameter(void);
static void	InitRealTimeStructParameter(void);
static void	InitRealTimeStatusStructParameter(void);
static void	InitPrtCtrlDataParameter(void);
static void	InitTabStructParameter(void);
static void	InitPageModeStructParameter(void);
static void	InitSystemCtrlStructParameter(void);
static void	InitDrawerParameter(void);
static void	InitInbufParameter(void);
static void	InitInterfaceParameter(void);
static void	InitMacroStructParameter(void);
static void LEDPowerOn(void);
void Flash_Inside_ReadByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
FLASH_Status Flash_Inside_BufWrite(const u8* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void	InitSysTick(void);
void    RestoreDefaultParas(void);
//内部参数区地址

#define SystemFrequency 72000000
#ifdef __cplusplus
}
#endif

#endif /* INIT_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
