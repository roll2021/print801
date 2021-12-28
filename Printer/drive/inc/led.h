/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   led相关的程序.
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
#ifndef LED_H
#define LED_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#define PE_LED_GPIO_PORT								GPIOB
#define PE_LED_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOB
#define PE_LED_GPIO_PIN									GPIO_Pin_9
#define ERROR_LED_GPIO_PORT							GPIOE
#define ERROR_LED_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE
#define ERROR_LED_GPIO_PIN							GPIO_Pin_0

#define LINE_LED_GPIO_PORT							GPIOC					
#define LINE_LED_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC
#define LINE_LED_GPIO_PIN							  GPIO_Pin_13
	 
#define	PELedOn()					GPIO_ResetBits(PE_LED_GPIO_PORT, PE_LED_GPIO_PIN)
#define	PELedOff()				GPIO_SetBits(PE_LED_GPIO_PORT, PE_LED_GPIO_PIN)
#define	PaperEndLedOn()		PELedOn()
#define	PaperEndLedOff()	PELedOff()
#define	ErrorLedOn()			GPIO_ResetBits(ERROR_LED_GPIO_PORT, ERROR_LED_GPIO_PIN)
#define	ErrorLedOff()			GPIO_SetBits(ERROR_LED_GPIO_PORT, ERROR_LED_GPIO_PIN)
	 
#define	LineLedOn()				GPIO_SetBits(LINE_LED_GPIO_PORT, LINE_LED_GPIO_PIN)
#define	LineLedOff()			GPIO_ResetBits(LINE_LED_GPIO_PORT, LINE_LED_GPIO_PIN)	 
/* Funtion Declare -----------------------------------------------------------*/
void	PELedPortInit(void);
void	ErrorLedPortInit(void);
void	TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void	TogglePELed(void);
void	ToggleErrorLed(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
