/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-29
  * @brief   外部SRAM相关的程序.
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
#ifndef SRAM_H
#define SRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
/* 数据线管脚 */
#define FSMC_D0_GPIO_PIN							GPIO_Pin_14
#define FSMC_D1_GPIO_PIN							GPIO_Pin_15
#define FSMC_D2_GPIO_PIN							GPIO_Pin_0
#define FSMC_D3_GPIO_PIN							GPIO_Pin_1

#define FSMC_D4_GPIO_PIN							GPIO_Pin_7
#define FSMC_D5_GPIO_PIN							GPIO_Pin_8
#define FSMC_D6_GPIO_PIN							GPIO_Pin_9
#define FSMC_D7_GPIO_PIN							GPIO_Pin_10
#define FSMC_D8_GPIO_PIN							GPIO_Pin_11
#define FSMC_D9_GPIO_PIN							GPIO_Pin_12
#define FSMC_D10_GPIO_PIN							GPIO_Pin_13
#define FSMC_D11_GPIO_PIN							GPIO_Pin_14
#define FSMC_D12_GPIO_PIN							GPIO_Pin_15

#define FSMC_D13_GPIO_PIN							GPIO_Pin_8
#define FSMC_D14_GPIO_PIN							GPIO_Pin_9
#define FSMC_D15_GPIO_PIN							GPIO_Pin_10

/* 地址线管脚 */
#define FSMC_A0_GPIO_PIN							GPIO_Pin_14
#define FSMC_A1_GPIO_PIN							GPIO_Pin_15
#define FSMC_A2_GPIO_PIN							GPIO_Pin_0
#define FSMC_A3_GPIO_PIN							GPIO_Pin_1

#define FSMC_A4_GPIO_PIN							GPIO_Pin_7
#define FSMC_A5_GPIO_PIN							GPIO_Pin_8
#define FSMC_A6_GPIO_PIN							GPIO_Pin_9
#define FSMC_A7_GPIO_PIN							GPIO_Pin_10
#define FSMC_A8_GPIO_PIN							GPIO_Pin_11
#define FSMC_A9_GPIO_PIN							GPIO_Pin_12
#define FSMC_A10_GPIO_PIN							GPIO_Pin_13
#define FSMC_A11_GPIO_PIN							GPIO_Pin_14
#define FSMC_A12_GPIO_PIN							GPIO_Pin_15

#define FSMC_A13_GPIO_PIN							GPIO_Pin_8
#define FSMC_A14_GPIO_PIN							GPIO_Pin_9
#define FSMC_A15_GPIO_PIN							GPIO_Pin_10
#define FSMC_A16_GPIO_PIN							GPIO_Pin_11
#define FSMC_A17_GPIO_PIN							GPIO_Pin_12
#define FSMC_A18_GPIO_PIN							GPIO_Pin_13

/* 合并定义数据线管脚 */
#define FSMC_D0_TO_D3_GPIO_PORT								GPIOD
#define FSMC_D0_TO_D3_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOD

#define FSMC_D4_TO_D12_GPIO_PORT							GPIOE
#define FSMC_D4_TO_D12_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE

#define FSMC_D13_TO_D15_GPIO_PORT							GPIOD
#define FSMC_D13_TO_D15_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOD

/* 合并定义地址线管脚 */
#define FSMC_A0_TO_A3_GPIO_PORT								GPIOD
#define FSMC_A0_TO_A3_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOD

#define FSMC_A4_TO_A12_GPIO_PORT							GPIOE
#define FSMC_A4_TO_A12_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE

#define FSMC_A13_TO_A18_GPIO_PORT							GPIOD
#define FSMC_A13_TO_A18_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOD

/* 其他控制线管脚 */
#define FSMC_NE1_GPIO_PORT								GPIOD
#define FSMC_NE1_GPIO_PIN									GPIO_Pin_7
#define FSMC_NE1_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOD

#define FSMC_NADV_GPIO_PORT								GPIOB
#define FSMC_NADV_GPIO_PIN								GPIO_Pin_7
#define FSMC_NADV_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOB

#define FSMC_NOE_GPIO_PORT								GPIOD
#define FSMC_NOE_GPIO_PIN									GPIO_Pin_4
#define FSMC_NOE_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOD

#define FSMC_NWE_GPIO_PORT								GPIOD
#define FSMC_NWE_GPIO_PIN									GPIO_Pin_5
#define FSMC_NWE_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOD

#define FSMC_NBL1_GPIO_PORT								GPIOE
#define FSMC_NBL1_GPIO_PIN								GPIO_Pin_1
#define FSMC_NBL1_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOE

#define FSMC_NBL0_GPIO_PORT								GPIOE
#define FSMC_NBL0_GPIO_PIN								GPIO_Pin_0
#define FSMC_NBL0_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOE

#define	EXT_SRAM_BASE											(0x60000000)

#define	EXT_SRAM_BYTE_LEN_64K							(0x10000)		//64KB
#define	EXT_SRAM_BYTE_LEN_128K						(0x20000)		//128KB
#define	EXT_SRAM_BYTE_LEN_256K						(0x40000)		//256KB
#define	EXT_SRAM_BYTE_LEN_512K						(0x80000)		//512KB
#define	EXT_SRAM_BYTE_LEN_1024K						(0x100000)	//1MB

/* Private function prototypes -----------------------------------------------*/
void	InitExtSRAMPort(void);
void	InitExtSRAM(void);
uint32_t	TestReadWriteExtSRAM(void);
uint32_t	GetExtSRAMLen(void);

extern uint32_t ExtSRamSize;

#ifdef __cplusplus
}
#endif

#endif /* SRAM_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
