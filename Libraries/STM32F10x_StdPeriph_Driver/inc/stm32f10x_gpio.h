/**
  ******************************************************************************
  * @file    stm32f10x_gpio.h
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    11-March-2011
  * @brief   This file contains all the functions prototypes for the GPIO 
  *          firmware library.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_GPIO_H
#define __STM32F10x_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32F10x_StdPeriph_Driver
  * @{
  */

/** @addtogroup GPIO
  * @{
  */

/** @defgroup GPIO_Exported_Types
  * @{
  */
#if defined (STM32F10X_403A) || defined (STM32F10X_407)
#define IS_GPIO_ALL_PERIPH(PERIPH) (((PERIPH) == GPIOA) || \
                                    ((PERIPH) == GPIOB) || \
                                    ((PERIPH) == GPIOC) || \
                                    ((PERIPH) == GPIOD) || \
                                    ((PERIPH) == GPIOE))
#else
#define IS_GPIO_ALL_PERIPH(PERIPH) (((PERIPH) == GPIOA) || \
                                    ((PERIPH) == GPIOB) || \
                                    ((PERIPH) == GPIOC) || \
                                    ((PERIPH) == GPIOD) || \
                                    ((PERIPH) == GPIOE) || \
                                    ((PERIPH) == GPIOF) || \
                                    ((PERIPH) == GPIOG))
#endif
                                     
/** 
  * @brief  Output Maximum frequency selection  
  */

typedef enum
{ 
  GPIO_Speed_10MHz = 1,
  GPIO_Speed_2MHz, 
  GPIO_Speed_50MHz = 2
}GPIOSpeed_TypeDef;
#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_Speed_10MHz) || ((SPEED) == GPIO_Speed_2MHz) || \
                              ((SPEED) == GPIO_Speed_50MHz))

/** 
  * @brief  Configuration Mode enumeration  
  */

typedef enum
{ GPIO_Mode_AIN = 0x0,
  GPIO_Mode_IN_FLOATING = 0x04,
  GPIO_Mode_IPD = 0x28,
  GPIO_Mode_IPU = 0x48,
  GPIO_Mode_Out_OD = 0x14,
  GPIO_Mode_Out_PP = 0x10,
  GPIO_Mode_AF_OD = 0x1C,
  GPIO_Mode_AF_PP = 0x18
}GPIOMode_TypeDef;

#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_AIN) || ((MODE) == GPIO_Mode_IN_FLOATING) || \
                            ((MODE) == GPIO_Mode_IPD) || ((MODE) == GPIO_Mode_IPU) || \
                            ((MODE) == GPIO_Mode_Out_OD) || ((MODE) == GPIO_Mode_Out_PP) || \
                            ((MODE) == GPIO_Mode_AF_OD) || ((MODE) == GPIO_Mode_AF_PP))

/** 
  * @brief  GPIO Init structure definition  
  */

typedef struct
{
  uint16_t GPIO_Pin;             /*!< Specifies the GPIO pins to be configured.
                                      This parameter can be any value of @ref GPIO_pins_define */

  GPIOSpeed_TypeDef GPIO_Speed;  /*!< Specifies the speed for the selected pins.
                                      This parameter can be a value of @ref GPIOSpeed_TypeDef */

  GPIOMode_TypeDef GPIO_Mode;    /*!< Specifies the operating mode for the selected pins.
                                      This parameter can be a value of @ref GPIOMode_TypeDef */
}GPIO_InitTypeDef;


/** 
  * @brief  Bit_SET and Bit_RESET enumeration  
  */

typedef enum
{ Bit_RESET = 0,
  Bit_SET
}BitAction;

#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || ((ACTION) == Bit_SET))

/**
  * @}
  */

/** @defgroup GPIO_Exported_Constants
  * @{
  */

/** @defgroup GPIO_pins_define 
  * @{
  */

#define GPIO_Pin_0                 ((uint16_t)0x0001)  /*!< Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /*!< Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /*!< Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /*!< Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /*!< Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /*!< Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /*!< Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /*!< Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /*!< Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /*!< Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /*!< Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /*!< Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /*!< Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /*!< Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /*!< Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /*!< Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /*!< All pins selected */

#define IS_GPIO_PIN(PIN) ((((PIN) & (uint16_t)0x00) == 0x00) && ((PIN) != (uint16_t)0x00))

#define IS_GET_GPIO_PIN(PIN) (((PIN) == GPIO_Pin_0) || \
                              ((PIN) == GPIO_Pin_1) || \
                              ((PIN) == GPIO_Pin_2) || \
                              ((PIN) == GPIO_Pin_3) || \
                              ((PIN) == GPIO_Pin_4) || \
                              ((PIN) == GPIO_Pin_5) || \
                              ((PIN) == GPIO_Pin_6) || \
                              ((PIN) == GPIO_Pin_7) || \
                              ((PIN) == GPIO_Pin_8) || \
                              ((PIN) == GPIO_Pin_9) || \
                              ((PIN) == GPIO_Pin_10) || \
                              ((PIN) == GPIO_Pin_11) || \
                              ((PIN) == GPIO_Pin_12) || \
                              ((PIN) == GPIO_Pin_13) || \
                              ((PIN) == GPIO_Pin_14) || \
                              ((PIN) == GPIO_Pin_15))

/**
  * @}
  */

/** @defgroup GPIO_Remap_define 
  * @{
  */

#if defined (STM32F10X_403A) || defined (STM32F10X_407)
/** @defgroup GPIO_Remap_define 
  * @{
  */
#define GPIO_Remap01_SPI1           ((uint32_t)0x00000001)  /*!< SPI1 Alternate Function mapping 01 */
#define GPIO_Remap10_SPI1           ((uint32_t)0x80000000)  /*!< SPI1 Alternate Function mapping 10*/
#define GPIO_Remap_I2C1             ((uint32_t)0x00000002)  /*!< I2C1 Alternate Function mapping */
#define GPIO_Remap_USART1           ((uint32_t)0x00000004)  /*!< USART1 Alternate Function mapping */
#define GPIO_Remap_USART2           ((uint32_t)0x00000008)  /*!< USART2 Alternate Function mapping */
#define GPIO_PartialRemap_USART3    ((uint32_t)0x00000010)  /*!< USART3 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART3       ((uint32_t)0x00000030)  /*!< USART3 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM1      ((uint32_t)0x00000040)  /*!< TIM1 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM1         ((uint32_t)0x000000C0)  /*!< TIM1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM2     ((uint32_t)0x00000100)  /*!< TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2     ((uint32_t)0x00000200)  /*!< TIM2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM2         ((uint32_t)0x00000300)  /*!< TIM2 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM3      ((uint32_t)0x00000800)  /*!< TIM3 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM3         ((uint32_t)0x00000C00)  /*!< TIM3 Full Alternate Function mapping */
#define GPIO_Remap_TIM4             ((uint32_t)0x00001000)  /*!< TIM4 Alternate Function mapping */
#define GPIO_Remap1_CAN1            ((uint32_t)0x00004000)  /*!< CAN1 Alternate Function mapping */
#define GPIO_Remap2_CAN1            ((uint32_t)0x00006000)  /*!< CAN1 Alternate Function mapping */
#define GPIO_Remap_PD01             ((uint32_t)0x00008000)  /*!< PD01 Alternate Function mapping */
#define GPIO_Remap_TIM5CH4_LSI      ((uint32_t)0x00010000)  /*!< LSI connected to TIM5 Channel4 input capture for calibration */
#define GPIO_Remap_ADC1_EXTRGINJ    ((uint32_t)0x00020000)  /*!< ADC1 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC1_EXTRGREG    ((uint32_t)0x00040000)  /*!< ADC1 External Trigger Regular Conversion remapping */
#define GPIO_Remap_ADC2_EXTRGINJ    ((uint32_t)0x00080000)  /*!< ADC2 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC2_EXTRGREG    ((uint32_t)0x00100000)  /*!< ADC2 External Trigger Regular Conversion remapping */
#define GPIO_Remap_SWJ_NoJNTRST     ((uint32_t)0x01000000)  /*!< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST */
#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x02000000)  /*!< JTAG-DP Disabled and SW-DP Enabled */
#define GPIO_Remap_SWJ_AllDisable   ((uint32_t)0x04000000)  /*!< Full SWJ Disabled (JTAG-DP + SW-DP) */
#define GPIO_Remap01_COMP           ((uint32_t)0x44000000)  /*!< COMP1/2 Alternate Function mapping 01: COMP1/2_OUT connect to PA6/7*/
#define GPIO_Remap10_COMP           ((uint32_t)0x48000000)  /*!< COMP1/2 Alternate Function mapping 10: COMP1/2_OUT connect to PA11/12*/
#define GPIO_Remap_EXT_FLASH        ((uint32_t)0x40200000)  /*!< EXT_FLASH Alternate Function mapping*/
#define GPIO_Remap_ETH              ((uint32_t)0x00200000)  /*!< ETH Alternate Function mapping*/
#endif

#if defined (STM32F10X_403A) || defined (STM32F10X_407)
#define IS_GPIO_REMAP(REMAP)        (((REMAP) == GPIO_Remap01_SPI1)         || ((REMAP) == GPIO_Remap_I2C1)           || \
                                     ((REMAP) == GPIO_Remap_USART1)         || ((REMAP) == GPIO_Remap_USART2)         || \
                                     ((REMAP) == GPIO_PartialRemap_USART3)  || ((REMAP) == GPIO_FullRemap_USART3)     || \
                                     ((REMAP) == GPIO_PartialRemap_TIM1)    || ((REMAP) == GPIO_FullRemap_TIM1)       || \
                                     ((REMAP) == GPIO_PartialRemap1_TIM2)   || ((REMAP) == GPIO_PartialRemap2_TIM2)   || \
                                     ((REMAP) == GPIO_FullRemap_TIM2)       || ((REMAP) == GPIO_PartialRemap_TIM3)    || \
                                     ((REMAP) == GPIO_FullRemap_TIM3)       || ((REMAP) == GPIO_Remap_TIM4)           || \
                                     ((REMAP) == GPIO_Remap1_CAN1)          || ((REMAP) == GPIO_Remap2_CAN1)          || \
                                     ((REMAP) == GPIO_Remap_PD01)          || ((REMAP) == GPIO_Remap_TIM5CH4_LSI)    || \
                                     ((REMAP) == GPIO_Remap_ADC1_EXTRGINJ)  ||((REMAP) == GPIO_Remap_ADC1_EXTRGREG)   || \
                                     ((REMAP) == GPIO_Remap_ADC2_EXTRGINJ)  ||((REMAP) == GPIO_Remap_ADC2_EXTRGREG)   || \
                                     ((REMAP) == GPIO_Remap_SWJ_NoJNTRST)   || ((REMAP) == GPIO_Remap_SWJ_JTAGDisable)|| \
                                     ((REMAP) == GPIO_Remap_SWJ_AllDisable) || ((REMAP) == GPIO_Remap10_SPI1)         || \
                                     ((REMAP) == GPIO_Remap01_COMP)         || ((REMAP) == GPIO_Remap10_COMP)         || \
                                     ((REMAP) == GPIO_Remap_EXT_FLASH)      || ((REMAP) == GPIO_Remap_ETH))
                              
/**
  * @}
  */ 
#endif

#if defined (STM32F10X_403A) || defined (STM32F10X_407)
#define AFIO_MAP3     0x00
#define AFIO_MAP4     0x01
#define AFIO_MAP5     0x02
#define AFIO_MAP6     0x03
#define AFIO_MAP7     0x04
#define AFIO_MAP8     0x05

#define BITS0         0x00
#define BITS1         0x01
#define BITS2         0x02
#define BITS3         0x03
#define BITS4         0x04
#define BITS5         0x05
#define BITS6         0x06
#define BITS7         0x07

#define OFFSET_MASK0  0xFFFFFFF0
#define OFFSET_MASK1  0xFFFFFF0F
#define OFFSET_MASK2  0xFFFFF0FF
#define OFFSET_MASK3  0xFFFF0FFF
#define OFFSET_MASK4  0xFFF0FFFF
#define OFFSET_MASK5  0xFF0FFFFF
#define OFFSET_MASK6  0xF0FFFFFF
#define OFFSET_MASK7  0x0FFFFFFF

/** @defgroup GPIO_Remap_define 
  * @{ 
  */

#define AFIO_MAP3_TIM9_0010         ((uint32_t)0x80000002)  /*!< TIM9 Alternate Function mapping  */
#define AFIO_MAP3_TIM10_0010        ((uint32_t)0x80000012)  /*!< TIM10 Alternate Function mapping  */
#define AFIO_MAP3_TIM11_0010        ((uint32_t)0x80000022)  /*!< TIM11 Alternate Function mapping  */

#define AFIO_MAP4_TIM1_0001         ((uint32_t)0x80000081)  /*!< TIM1 Alternate Function mapping */
#define AFIO_MAP4_TIM1_0011         ((uint32_t)0x80000083)  /*!< TIM1 Alternate Function mapping */
#define AFIO_MAP4_TIM2_0001         ((uint32_t)0x80000091)  /*!< TIM2 Alternate Function mapping 0001*/
#define AFIO_MAP4_TIM2_0010         ((uint32_t)0x80000092)  /*!< TIM2 Alternate Function mapping 0010*/
#define AFIO_MAP4_TIM2_0011         ((uint32_t)0x80000093)  /*!< TIM2 Alternate Function mapping 0011*/
#define AFIO_MAP4_TIM3_0010         ((uint32_t)0x800000A2)  /*!< TIM3 Alternate Function mapping 0010*/
#define AFIO_MAP4_TIM3_0011         ((uint32_t)0x800000A3)  /*!< TIM3 Alternate Function mapping 0011*/
#define AFIO_MAP4_TIM4_0001         ((uint32_t)0x800000B1)  /*!< TIM5 Alternate Function mapping 0001: CH1/CH2*/
#define AFIO_MAP4_TIM5_0001         ((uint32_t)0x800000C1)  /*!< TIM5 Alternate Function mapping 0001: CH1/CH2*/
#define AFIO_MAP4_TIM5_1000         ((uint32_t)0x800000C8)  /*!< TIM5 Alternate Function mapping 1000: CH4 */
#define AFIO_MAP4_TIM5_1001         ((uint32_t)0x800000C9)  /*!< TIM5 Alternate Function mapping 1001: CH1/CH2+CH4 */

#define AFIO_MAP5_USART5_0001       ((uint32_t)0x80000101)  /*!< USART5 Alternate Function mapping 0001*/
#define AFIO_MAP5_I2C1_0001         ((uint32_t)0x80000111)  /*!< I2C1 Alternate Function mapping 0001*/
#define AFIO_MAP5_I2C1_0011         ((uint32_t)0x80000113)  /*!< I2C1 Alternate Function mapping 0011*/
#define AFIO_MAP5_I2C2_0001         ((uint32_t)0x80000121)  /*!< I2C2 Alternate Function mapping 0001*/
#define AFIO_MAP5_I2C2_0010         ((uint32_t)0x80000122)  /*!< I2C2 Alternate Function mapping 0010*/
#define AFIO_MAP5_I2C2_0011         ((uint32_t)0x80000123)  /*!< I2C2 Alternate Function mapping 0011*/
#define AFIO_MAP5_I2C3_0001         ((uint32_t)0x80000131)  /*!< I2C3 Alternate Function mapping 0001*/
#define AFIO_MAP5_SPI1_0001         ((uint32_t)0x80000141)  /*!< SPI1 Alternate Function mapping 0001*/
#define AFIO_MAP5_SPI1_0010         ((uint32_t)0x80000142)  /*!< SPI1 Alternate Function mapping 0010*/
#define AFIO_MAP5_SPI1_0011         ((uint32_t)0x80000143)  /*!< SPI1 Alternate Function mapping 0011*/
#define AFIO_MAP5_SPI2_0001         ((uint32_t)0x80000151)  /*!< SPI2 Alternate Function mapping 0001*/
#define AFIO_MAP5_SPI2_0010         ((uint32_t)0x80000152)  /*!< SPI2 Alternate Function mapping 0010*/
#define AFIO_MAP5_SPI3_0001         ((uint32_t)0x80000161)  /*!< SPI3 Alternate Function mapping 0001*/
#define AFIO_MAP5_SPI3_0010         ((uint32_t)0x80000162)  /*!< SPI3 Alternate Function mapping 0010*/
#define AFIO_MAP5_SPI3_0011         ((uint32_t)0x80000163)  /*!< SPI3 Alternate Function mapping 0011*/
#define AFIO_MAP5_SPI4_0001         ((uint32_t)0x80000171)  /*!< SPI4 Alternate Function mapping 0001*/
#define AFIO_MAP5_SPI4_0010         ((uint32_t)0x80000172)  /*!< SPI4 Alternate Function mapping 0010*/
#define AFIO_MAP5_SPI4_0011         ((uint32_t)0x80000173)  /*!< SPI4 Alternate Function mapping 0011*/

#define AFIO_MAP6_CAN1_0010         ((uint32_t)0x80000182)  /*!< CAN1 Alternate Function mapping 0010*/
#define AFIO_MAP6_CAN1_0011         ((uint32_t)0x80000183)  /*!< CAN1 Alternate Function mapping 0011*/
#define AFIO_MAP6_CAN2_0001         ((uint32_t)0x80000191)  /*!< CAN2 Alternate Function mapping */
#define AFIO_MAP6_SDIO_0100         ((uint32_t)0x800001A4)  /*!< SDIO Alternate Function mapping 100 */
#define AFIO_MAP6_SDIO_0101         ((uint32_t)0x800001A5)  /*!< SDIO Alternate Function mapping 101 */
#define AFIO_MAP6_SDIO_0110         ((uint32_t)0x800001A6)  /*!< SDIO Alternate Function mapping 110 */
#define AFIO_MAP6_SDIO_0111         ((uint32_t)0x800001A7)  /*!< SDIO Alternate Function mapping 111 */
#define AFIO_MAP6_SDIO2_0001        ((uint32_t)0x800001B1)  /*!< SDIO2 Alternate Function mapping 0001 */
#define AFIO_MAP6_SDIO2_0010        ((uint32_t)0x800001B2)  /*!< SDIO2 Alternate Function mapping 0010 */
#define AFIO_MAP6_SDIO2_0011        ((uint32_t)0x800001B3)  /*!< SDIO2 Alternate Function mapping 0011 */
#define AFIO_MAP6_USART1_0001       ((uint32_t)0x800001C1)  /*!< USART1 Alternate Function mapping */
#define AFIO_MAP6_USART2_0001       ((uint32_t)0x800001D1)  /*!< USART2 Alternate Function mapping */
#define AFIO_MAP6_USART3_0001       ((uint32_t)0x800001E1)  /*!< USART3 Alternate Function mapping 0001*/
#define AFIO_MAP6_USART3_0011       ((uint32_t)0x800001E3)  /*!< USART3 Alternate Function mapping 0011*/
#define AFIO_MAP6_UART4_0001        ((uint32_t)0x800001F1)  /*!< UART4 Alternate Function mapping */

#define AFIO_MAP7_SPIF_1000         ((uint32_t)0x80000208)  /*!< EXT_FLASH Alternate Function mapping */ 
#define AFIO_MAP7_SPIF_1001         ((uint32_t)0x80000209)  /*!< EXT_FLASH Alternate Function enable */ 
#define AFIO_MAP7_ADC1_0001         ((uint32_t)0x80000211)  /*!< ADC1 External Trigger Injected Conversion remapping */
#define AFIO_MAP7_ADC1_0010         ((uint32_t)0x80000212)  /*!< ADC1 External Trigger Regular Conversion remapping */
#define AFIO_MAP7_ADC1_0011         ((uint32_t)0x80000213)  /*!< ADC1 External Trigger Regular & Injected Conversion remapping */
#define AFIO_MAP7_ADC2_0001         ((uint32_t)0x80000221)  /*!< ADC2 External Trigger Injected Conversion remapping */
#define AFIO_MAP7_ADC2_0010         ((uint32_t)0x80000222)  /*!< ADC2 External Trigger Regular Conversion remapping */
#define AFIO_MAP7_ADC2_0011         ((uint32_t)0x80000223)  /*!< ADC2 External Trigger Regular & Injected Conversion remapping */
#define AFIO_MAP7_SWJTAG_0001       ((uint32_t)0x80000241)  /*!< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST */
#define AFIO_MAP7_SWJTAG_0010       ((uint32_t)0x80000242)  /*!< JTAG-DP Disabled and SW-DP Enabled */
#define AFIO_MAP7_SWJTAG_0100       ((uint32_t)0x80000244)  /*!< Full SWJ Disabled (JTAG-DP + SW-DP) */
#define AFIO_MAP7_PD01_0001         ((uint32_t)0x80000251)  /*!< PD01 Alternate Function mapping */  
#define AFIO_MAP7_XMC_0001          ((uint32_t)0x80000261)  /*!< XMC Alternate Function mapping 0001 */
#define AFIO_MAP7_XMC_0010          ((uint32_t)0x80000262)  /*!< XMC Alternate Function mapping 0001 */
#define AFIO_MAP7_XMC_1001          ((uint32_t)0x80000269)  /*!< XMC Alternate Function mapping 1001 */
#define AFIO_MAP7_XMC_1010          ((uint32_t)0x8000026A)  /*!< XMC Alternate Function mapping 1001 */

#define AFIO_MAP8_ETH_0001          ((uint32_t)0x800002C1)  /*!< ETH Alternate Function mapping 0001*/
#define AFIO_MAP8_ETH_0100          ((uint32_t)0x800002C4)  /*!< ETH Alternate Function mapping 0100*/
#define AFIO_MAP8_ETH_0101          ((uint32_t)0x800002C5)  /*!< ETH Alternate Function mapping 0101*/
#define AFIO_MAP8_ETH_1000          ((uint32_t)0x800002C8)  /*!< ETH Alternate Function mapping 1000*/
#define AFIO_MAP8_ETH_1001          ((uint32_t)0x800002C9)  /*!< ETH Alternate Function mapping 1001*/
#define AFIO_MAP8_ETH_1100          ((uint32_t)0x800002CC)  /*!< ETH Alternate Function mapping 1100*/
#define AFIO_MAP8_ETH_1101          ((uint32_t)0x800002CD)  /*!< ETH Alternate Function mapping 1101*/
#define AFIO_MAP8_USART6_0001       ((uint32_t)0x800002D1)  /*!< USART6 Alternate Function mapping */
#define AFIO_MAP8_UART7_0001        ((uint32_t)0x800002E1)  /*!< UART7 Alternate Function mapping */
#define AFIO_MAP8_UART8_0001        ((uint32_t)0x800002F1)  /*!< UART8 Alternate Function mapping */

#define AFIO_MAP8_TIM1_BK1_00       ((uint32_t)0x80000280)  /*!< TIM1 BK1 input selection 00/01*/
#define AFIO_MAP8_TIM1_BK1_10       ((uint32_t)0x80000282)  /*!< TIM1 BK1 input selection 10   */
#define AFIO_MAP8_TIM1_BK1_11       ((uint32_t)0x80000283)  /*!< TIM1 BK1 input selection 11   */
#define AFIO_MAP8_TIM1_CH1_00       ((uint32_t)0x80000290)  /*!< TIM1 CH1 input selection 00/01*/
#define AFIO_MAP8_TIM1_CH1_10       ((uint32_t)0x80000298)  /*!< TIM1 CH1 input selection 10   */
#define AFIO_MAP8_TIM1_CH1_11       ((uint32_t)0x8000029C)  /*!< TIM1 CH1 input selection 11   */
#define AFIO_MAP8_TIM2_CH4_00       ((uint32_t)0x800002A0)  /*!< TIM2 CH4 input selection 00/01*/
#define AFIO_MAP8_TIM2_CH4_10       ((uint32_t)0x800002A2)  /*!< TIM2 CH4 input selection 10   */
#define AFIO_MAP8_TIM2_CH4_11       ((uint32_t)0x800002A3)  /*!< TIM2 CH4 input selection 11   */
#define AFIO_MAP8_TIM3_CH1_00       ((uint32_t)0x800002B0)  /*!< TIM3 CH1 input selection 00/01*/
#define AFIO_MAP8_TIM3_CH1_10       ((uint32_t)0x800002B8)  /*!< TIM3 CH1 input selection 10   */
#define AFIO_MAP8_TIM3_CH1_11       ((uint32_t)0x800002BC)  /*!< TIM3 CH1 input selection 11   */
#endif

#define IS_GREMAP(REMAP)            ((REMAP) > 0x80000000)

#define IS_GPIO_GREMAP(REMAP)        (((REMAP) == AFIO_MAP3_TIM9_0010)   || ((REMAP) == AFIO_MAP3_TIM10_0010)  || \
                                     ((REMAP) == AFIO_MAP3_TIM11_0010)  || ((REMAP) == AFIO_MAP4_TIM1_0001)   || \
                                     ((REMAP) == AFIO_MAP4_TIM2_0001)   || ((REMAP) == AFIO_MAP6_CAN1_0010)   || \
                                     ((REMAP) == AFIO_MAP4_TIM2_0010)   || ((REMAP) == AFIO_MAP6_CAN2_0001)   || \
                                     ((REMAP) == AFIO_MAP4_TIM2_0011)   || ((REMAP) == AFIO_MAP6_SDIO_0100)   || \
                                     ((REMAP) == AFIO_MAP4_TIM3_0010)   || ((REMAP) == AFIO_MAP6_SDIO_0101)   || \
                                     ((REMAP) == AFIO_MAP4_TIM3_0011)   || ((REMAP) == AFIO_MAP6_SDIO_0110)   || \
                                     ((REMAP) == AFIO_MAP4_TIM5_0001)   || ((REMAP) == AFIO_MAP6_SDIO_0111)   || \
                                     ((REMAP) == AFIO_MAP4_TIM5_1000)   || ((REMAP) == AFIO_MAP6_USART1_0001) || \
                                     ((REMAP) == AFIO_MAP4_TIM5_1001)   || ((REMAP) == AFIO_MAP6_USART3_0001) || \
                                     ((REMAP) == AFIO_MAP5_I2C1_0001)   || ((REMAP) == AFIO_MAP6_UART4_0001)  || \
                                     ((REMAP) == AFIO_MAP5_I2C1_0011)   || ((REMAP) == AFIO_MAP7_SPIF_1000)   || \
                                     ((REMAP) == AFIO_MAP5_I2C2_0001)   || ((REMAP) == AFIO_MAP7_SPIF_1001)   || \
                                     ((REMAP) == AFIO_MAP5_I2C2_0010)   || ((REMAP) == AFIO_MAP7_ADC1_0001)   || \
                                     ((REMAP) == AFIO_MAP5_I2C2_0011)   || ((REMAP) == AFIO_MAP7_ADC1_0010)   || \
                                     ((REMAP) == AFIO_MAP5_SPI1_0001)   || ((REMAP) == AFIO_MAP7_ADC2_0001)   || \
                                     ((REMAP) == AFIO_MAP5_SPI2_0001)   || ((REMAP) == AFIO_MAP7_ADC2_0010)   || \
                                     ((REMAP) == AFIO_MAP7_SWJTAG_0010) || ((REMAP) == AFIO_MAP7_SWJTAG_0001) || \
                                     ((REMAP) == AFIO_MAP7_SWJTAG_0100) || ((REMAP) == AFIO_MAP7_PD01_0001)   || \
                                     ((REMAP) == AFIO_MAP8_TIM1_BK1_00) || ((REMAP) == AFIO_MAP8_TIM1_BK1_10) || \
                                     ((REMAP) == AFIO_MAP8_TIM1_BK1_11) || ((REMAP) == AFIO_MAP8_TIM1_CH1_00) || \
                                     ((REMAP) == AFIO_MAP8_TIM1_CH1_10) || ((REMAP) == AFIO_MAP8_TIM1_CH1_11) || \
                                     ((REMAP) == AFIO_MAP8_TIM2_CH4_00) || ((REMAP) == AFIO_MAP8_TIM2_CH4_10) || \
                                     ((REMAP) == AFIO_MAP8_TIM2_CH4_11) || ((REMAP) == AFIO_MAP8_TIM3_CH1_00) || \
                                     ((REMAP) == AFIO_MAP8_TIM3_CH1_10) || ((REMAP) == AFIO_MAP8_TIM3_CH1_11) || \
                                     ((REMAP) == AFIO_MAP4_TIM1_0011)   || ((REMAP) == AFIO_MAP4_TIM4_0001)   || \
                                     ((REMAP) == AFIO_MAP4_TIM2_1001)   || ((REMAP) == AFIO_MAP4_TIM2_1010)   || \
                                     ((REMAP) == AFIO_MAP4_TIM2_1011)   || ((REMAP) == AFIO_MAP4_TIM2_1101)   || \
                                     ((REMAP) == AFIO_MAP4_TIM2_1110)   || ((REMAP) == AFIO_MAP4_TIM2_1111)   || \
                                     ((REMAP) == AFIO_MAP5_USART5_0001) || ((REMAP) == AFIO_MAP5_I2C3_0001)   || \
                                     ((REMAP) == AFIO_MAP5_SPI1_0010)   || ((REMAP) == AFIO_MAP5_SPI1_0011)   || \
                                     ((REMAP) == AFIO_MAP5_SPI2_0010)   || ((REMAP) == AFIO_MAP5_SPI3_0001)   || \
                                     ((REMAP) == AFIO_MAP5_SPI3_0010)   || ((REMAP) == AFIO_MAP5_SPI3_0011)   || \
                                     ((REMAP) == AFIO_MAP5_SPI4_0001)   || ((REMAP) == AFIO_MAP5_SPI4_0010)   || \
                                     ((REMAP) == AFIO_MAP5_SPI4_0011)   || ((REMAP) == AFIO_MAP6_CAN1_0011)   || \
                                     ((REMAP) == AFIO_MAP6_SDIO2_0001)  || ((REMAP) == AFIO_MAP6_SDIO2_0010)  || \
                                     ((REMAP) == AFIO_MAP6_SDIO2_0011)  || ((REMAP) == AFIO_MAP6_USART2_0001) || \
                                     ((REMAP) == AFIO_MAP6_USART3_0011) || ((REMAP) == AFIO_MAP7_ADC1_0011)   || \
                                     ((REMAP) == AFIO_MAP7_ADC2_0011)   || ((REMAP) == AFIO_MAP7_XMC_0001)    || \
                                     ((REMAP) == AFIO_MAP7_XMC_0010)    || ((REMAP) == AFIO_MAP7_XMC_1001)    || \
                                     ((REMAP) == AFIO_MAP7_XMC_1010)    || ((REMAP) == AFIO_MAP8_ETH_0001)    || \
                                     ((REMAP) == AFIO_MAP8_ETH_0100)    || ((REMAP) == AFIO_MAP8_ETH_0101)    || \
                                     ((REMAP) == AFIO_MAP8_ETH_1000)    || ((REMAP) == AFIO_MAP8_ETH_1001)    || \
                                     ((REMAP) == AFIO_MAP8_ETH_1100)    || ((REMAP) == AFIO_MAP8_ETH_1101)    || \
                                     ((REMAP) == AFIO_MAP8_USART6_0001) || ((REMAP) == AFIO_MAP8_UART7_0001)  || \
                                     ((REMAP) == AFIO_MAP8_UART8_0001))
                              
/**
  * @}
  */ 

/** @defgroup GPIO_Port_Sources 
  * @{
  */

#define GPIO_PortSourceGPIOA       ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB       ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC       ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD       ((uint8_t)0x03)
#define GPIO_PortSourceGPIOE       ((uint8_t)0x04)
#define GPIO_PortSourceGPIOF       ((uint8_t)0x05)
#define GPIO_PortSourceGPIOG       ((uint8_t)0x06)
#define IS_GPIO_EVENTOUT_PORT_SOURCE(PORTSOURCE) (((PORTSOURCE) == GPIO_PortSourceGPIOA) || \
                                                  ((PORTSOURCE) == GPIO_PortSourceGPIOB) || \
                                                  ((PORTSOURCE) == GPIO_PortSourceGPIOC) || \
                                                  ((PORTSOURCE) == GPIO_PortSourceGPIOD) || \
                                                  ((PORTSOURCE) == GPIO_PortSourceGPIOE))

#define IS_GPIO_EXTI_PORT_SOURCE(PORTSOURCE) (((PORTSOURCE) == GPIO_PortSourceGPIOA) || \
                                              ((PORTSOURCE) == GPIO_PortSourceGPIOB) || \
                                              ((PORTSOURCE) == GPIO_PortSourceGPIOC) || \
                                              ((PORTSOURCE) == GPIO_PortSourceGPIOD) || \
                                              ((PORTSOURCE) == GPIO_PortSourceGPIOE) || \
                                              ((PORTSOURCE) == GPIO_PortSourceGPIOF) || \
                                              ((PORTSOURCE) == GPIO_PortSourceGPIOG))

/**
  * @}
  */

/** @defgroup GPIO_Pin_sources 
  * @{
  */

#define GPIO_PinSource0            ((uint8_t)0x00)
#define GPIO_PinSource1            ((uint8_t)0x01)
#define GPIO_PinSource2            ((uint8_t)0x02)
#define GPIO_PinSource3            ((uint8_t)0x03)
#define GPIO_PinSource4            ((uint8_t)0x04)
#define GPIO_PinSource5            ((uint8_t)0x05)
#define GPIO_PinSource6            ((uint8_t)0x06)
#define GPIO_PinSource7            ((uint8_t)0x07)
#define GPIO_PinSource8            ((uint8_t)0x08)
#define GPIO_PinSource9            ((uint8_t)0x09)
#define GPIO_PinSource10           ((uint8_t)0x0A)
#define GPIO_PinSource11           ((uint8_t)0x0B)
#define GPIO_PinSource12           ((uint8_t)0x0C)
#define GPIO_PinSource13           ((uint8_t)0x0D)
#define GPIO_PinSource14           ((uint8_t)0x0E)
#define GPIO_PinSource15           ((uint8_t)0x0F)

#define IS_GPIO_PIN_SOURCE(PINSOURCE) (((PINSOURCE) == GPIO_PinSource0) || \
                                       ((PINSOURCE) == GPIO_PinSource1) || \
                                       ((PINSOURCE) == GPIO_PinSource2) || \
                                       ((PINSOURCE) == GPIO_PinSource3) || \
                                       ((PINSOURCE) == GPIO_PinSource4) || \
                                       ((PINSOURCE) == GPIO_PinSource5) || \
                                       ((PINSOURCE) == GPIO_PinSource6) || \
                                       ((PINSOURCE) == GPIO_PinSource7) || \
                                       ((PINSOURCE) == GPIO_PinSource8) || \
                                       ((PINSOURCE) == GPIO_PinSource9) || \
                                       ((PINSOURCE) == GPIO_PinSource10) || \
                                       ((PINSOURCE) == GPIO_PinSource11) || \
                                       ((PINSOURCE) == GPIO_PinSource12) || \
                                       ((PINSOURCE) == GPIO_PinSource13) || \
                                       ((PINSOURCE) == GPIO_PinSource14) || \
                                       ((PINSOURCE) == GPIO_PinSource15))

/**
  * @}
  */

/** @defgroup Ethernet_Media_Interface 
  * @{
  */ 
#define GPIO_ETH_MediaInterface_MII    ((u32)0x00000000) 
#define GPIO_ETH_MediaInterface_RMII   ((u32)0x00000001)                                       

#define IS_GPIO_ETH_MEDIA_INTERFACE(INTERFACE) (((INTERFACE) == GPIO_ETH_MediaInterface_MII) || \
                                                ((INTERFACE) == GPIO_ETH_MediaInterface_RMII))

/**
  * @}
  */                                                
/**
  * @}
  */

/** @defgroup GPIO_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup GPIO_Exported_Functions
  * @{
  */

void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_AFIODeInit(void);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_EventOutputCmd(FunctionalState NewState);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_ETH_MediaInterfaceConfig(uint32_t GPIO_ETH_MediaInterface);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_GPIO_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
