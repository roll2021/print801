/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-23
  * @brief   并口驱动相关的程序.
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
#ifndef PARALLEL_H
#define PARALLEL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#define PARALLEL_STB_GPIO_PORT_SOURCE				GPIO_PortSourceGPIOA
#define PARALLEL_STB_GPIO_PIN_SOURCE				GPIO_PinSource12
#define PARALLEL_STB_EXTI_LINE							EXTI_Line12
#define PARALLEL_STB_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOA
#define PARALLEL_STB_GPIO_PORT							GPIOA
#define PARALLEL_STB_GPIO_PIN								GPIO_Pin_12
#define PARALLEL_STB_INT_IRQn								EXTI15_10_IRQn

#define PARALLEL_READ_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOA
#define PARALLEL_READ_GPIO_PORT							GPIOA
#define PARALLEL_READ_GPIO_PIN							GPIO_Pin_11

#define PARALLEL_ACK_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC
#define PARALLEL_ACK_GPIO_PORT							GPIOC
#define PARALLEL_ACK_GPIO_PIN								GPIO_Pin_12

#define PARALLEL_BUSY_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC
#define PARALLEL_BUSY_GPIO_PORT							GPIOC
#define PARALLEL_BUSY_GPIO_PIN							GPIO_Pin_11

#define PARALLEL_PE_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOC
#define PARALLEL_PE_GPIO_PORT								GPIOC
#define PARALLEL_PE_GPIO_PIN								GPIO_Pin_10

#define PARALLEL_ERR_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOC//2016.06.19
#define PARALLEL_ERR_GPIO_PORT								GPIOC
#define PARALLEL_ERR_GPIO_PIN								  GPIO_Pin_8

#define PARALLEL_DATA_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOB
#define PARALLEL_DATA_GPIO_PORT							GPIOB
#define PARALLEL_DATA0_GPIO_PIN							GPIO_Pin_8
#define PARALLEL_DATA1_GPIO_PIN							GPIO_Pin_9
#define PARALLEL_DATA2_GPIO_PIN							GPIO_Pin_10
#define PARALLEL_DATA3_GPIO_PIN							GPIO_Pin_11
#define PARALLEL_DATA4_GPIO_PIN							GPIO_Pin_12
#define PARALLEL_DATA5_GPIO_PIN							GPIO_Pin_13
#define PARALLEL_DATA6_GPIO_PIN							GPIO_Pin_14
#define PARALLEL_DATA7_GPIO_PIN							GPIO_Pin_15

#define	SetParallelReadData()		GPIO_SetBits(PARALLEL_READ_GPIO_PORT, PARALLEL_READ_GPIO_PIN)
#define	ClearParallelReadData()	GPIO_ResetBits(PARALLEL_READ_GPIO_PORT, PARALLEL_READ_GPIO_PIN)
#define	ReadParallelData()			(GPIO_ReadInputData(PARALLEL_DATA_GPIO_PORT) >> 8)

#define	SetParallelAck()				GPIO_SetBits(PARALLEL_ACK_GPIO_PORT, PARALLEL_ACK_GPIO_PIN)
#define	ClearParallelAck()			GPIO_ResetBits(PARALLEL_ACK_GPIO_PORT, PARALLEL_ACK_GPIO_PIN)

#define	SetParallelBusy()				GPIO_ResetBits(PARALLEL_BUSY_GPIO_PORT, PARALLEL_BUSY_GPIO_PIN)
#define	ClearParallelBusy()			GPIO_SetBits(PARALLEL_BUSY_GPIO_PORT, PARALLEL_BUSY_GPIO_PIN)

#define	SetParallelPE()					GPIO_SetBits(PARALLEL_PE_GPIO_PORT, PARALLEL_PE_GPIO_PIN)
#define	ClearParallelPE()				GPIO_ResetBits(PARALLEL_PE_GPIO_PORT, PARALLEL_PE_GPIO_PIN)

#define	LPT_PE_Out_PaperEnd()		SetParallelPE()
#define	LPT_PE_Out_PaperFull()	ClearParallelPE()

/* Funtion Declare -----------------------------------------------------------*/
void	InitParallelDataPort(void);
void	InitParallelCtrlPort(void);
void	LPT_ACK_Out(void);
void	LPT_PE_Out(uint8_t State);
void	LPT_ERR_Out(uint8_t State);
uint8_t	GetParallelData(void);
uint8_t	Read_LPT(void);

#ifdef __cplusplus
}
#endif

#endif /* PARALLEL_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
