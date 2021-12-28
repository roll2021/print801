/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   蜂鸣器控制相关的程序.
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
#ifndef BEEP_H
#define BEEP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"extgvar.h"

/* Macro Definition ----------------------------------------------------------*/
#define BEEP_GPIO_PORT						GPIOE
#define BEEP_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define BEEP_GPIO_PIN							GPIO_Pin_12

#define	BeepOn()									GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
#define	BeepOff()									GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN)

/* Funtion Declare -----------------------------------------------------------*/
void InitBeepPort(void);
void DriveBuzzer(uint8_t Status);

#ifdef __cplusplus
}
#endif

#endif /* BEEP_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
