/**
  ******************************************************************************
  * @file    stm32f10x_dma.h
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    11-March-2011
  * @brief   This file contains all the functions prototypes for the DMA firmware 
  *          library.
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
#ifndef __STM32F10x_DMA_H
#define __STM32F10x_DMA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32F10x_StdPeriph_Driver
  * @{
  */

/** @addtogroup DMA
  * @{
  */

/** @defgroup DMA_Exported_Types
  * @{
  */

/** 
  * @brief  DMA Init structure definition
  */

typedef struct
{
  uint32_t DMA_PeripheralBaseAddr; /*!< Specifies the peripheral base address for DMAy Channelx. */

  uint32_t DMA_MemoryBaseAddr;     /*!< Specifies the memory base address for DMAy Channelx. */

  uint32_t DMA_DIR;                /*!< Specifies if the peripheral is the source or destination.
                                        This parameter can be a value of @ref DMA_data_transfer_direction */

  uint32_t DMA_BufferSize;         /*!< Specifies the buffer size, in data unit, of the specified Channel. 
                                        The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                        or DMA_MemoryDataSize members depending in the transfer direction. */

  uint32_t DMA_PeripheralInc;      /*!< Specifies whether the Peripheral address register is incremented or not.
                                        This parameter can be a value of @ref DMA_peripheral_incremented_mode */

  uint32_t DMA_MemoryInc;          /*!< Specifies whether the memory address register is incremented or not.
                                        This parameter can be a value of @ref DMA_memory_incremented_mode */

  uint32_t DMA_PeripheralDataSize; /*!< Specifies the Peripheral data width.
                                        This parameter can be a value of @ref DMA_peripheral_data_size */

  uint32_t DMA_MemoryDataSize;     /*!< Specifies the Memory data width.
                                        This parameter can be a value of @ref DMA_memory_data_size */

  uint32_t DMA_Mode;               /*!< Specifies the operation mode of the DMAy Channelx.
                                        This parameter can be a value of @ref DMA_circular_normal_mode.
                                        @note: The circular buffer mode cannot be used if the memory-to-memory
                                              data transfer is configured on the selected Channel */

  uint32_t DMA_Priority;           /*!< Specifies the software priority for the DMAy Channelx.
                                        This parameter can be a value of @ref DMA_priority_level */

  uint32_t DMA_M2M;                /*!< Specifies if the DMAy Channelx will be used in memory-to-memory transfer.
                                        This parameter can be a value of @ref DMA_memory_to_memory */
}DMA_InitTypeDef;

/**
  * @}
  */

/** @defgroup DMA_Exported_Constants
  * @{
  */

#define IS_DMA_ALL_PERIPH(PERIPH) (((PERIPH) == DMA1_Channel1) || \
                                   ((PERIPH) == DMA1_Channel2) || \
                                   ((PERIPH) == DMA1_Channel3) || \
                                   ((PERIPH) == DMA1_Channel4) || \
                                   ((PERIPH) == DMA1_Channel5) || \
                                   ((PERIPH) == DMA1_Channel6) || \
                                   ((PERIPH) == DMA1_Channel7) || \
                                   ((PERIPH) == DMA2_Channel1) || \
                                   ((PERIPH) == DMA2_Channel2) || \
                                   ((PERIPH) == DMA2_Channel3) || \
                                   ((PERIPH) == DMA2_Channel4) || \
                                   ((PERIPH) == DMA2_Channel5))
								   
/** @defgroup DMA_flexible_channel
  * @{
  */
#define Flex_Channel1          ((uint8_t)0x01)
#define Flex_Channel2          ((uint8_t)0x02)
#define Flex_Channel3          ((uint8_t)0x03)
#define Flex_Channel4          ((uint8_t)0x04)
#define Flex_Channel5          ((uint8_t)0x05)
#define Flex_Channel6          ((uint8_t)0x06)
#define Flex_Channel7          ((uint8_t)0x07)

#define IS_DMA_ALL_CHANNELS(CHANNELS)     (((CHANNELS) == Flex_Channel1) || \
                                           ((CHANNELS) == Flex_Channel2) || \
                                           ((CHANNELS) == Flex_Channel3) || \
                                           ((CHANNELS) == Flex_Channel4) || \
                                           ((CHANNELS) == Flex_Channel5) || \
                                           ((CHANNELS) == Flex_Channel6) || \
                                           ((CHANNELS) == Flex_Channel7))

/** @defgroup DMA_hardware_id
  * @{
  */
#define DMA_FLEXIBLE_ADC1           ((uint8_t)0x01)
#define DMA_FLEXIBLE_ADC3           ((uint8_t)0x03)
#define DMA_FLEXIBLE_DAC1           ((uint8_t)0x05)
#define DMA_FLEXIBLE_DAC2           ((uint8_t)0x06)
#define DMA_FLEXIBLE_SPI1_RX        ((uint8_t)0x09)
#define DMA_FLEXIBLE_SPI1_TX        ((uint8_t)0x0A)
#define DMA_FLEXIBLE_SPI2_RX        ((uint8_t)0x0B)
#define DMA_FLEXIBLE_SPI2_TX        ((uint8_t)0x0C)
#define DMA_FLEXIBLE_SPI3_RX        ((uint8_t)0x0D)
#define DMA_FLEXIBLE_SPI3_TX        ((uint8_t)0x0E)
#define DMA_FLEXIBLE_SPI4_RX        ((uint8_t)0x0F)
#define DMA_FLEXIBLE_SPI4_TX        ((uint8_t)0x10)
#define DMA_FLEXIBLE_I2S2EXT_RX     ((uint8_t)0x11)
#define DMA_FLEXIBLE_I2S2EXT_TX     ((uint8_t)0x12)
#define DMA_FLEXIBLE_I2S3EXT_RX     ((uint8_t)0x13)
#define DMA_FLEXIBLE_I2S3EXT_TX     ((uint8_t)0x14)
#define DMA_FLEXIBLE_UART1_RX       ((uint8_t)0x19)
#define DMA_FLEXIBLE_UART1_TX       ((uint8_t)0x1A)
#define DMA_FLEXIBLE_UART2_RX       ((uint8_t)0x1B)
#define DMA_FLEXIBLE_UART2_TX       ((uint8_t)0x1C)
#define DMA_FLEXIBLE_UART3_RX       ((uint8_t)0x1D)
#define DMA_FLEXIBLE_UART3_TX       ((uint8_t)0x1E)
#define DMA_FLEXIBLE_UART4_RX       ((uint8_t)0x1F)
#define DMA_FLEXIBLE_UART4_TX       ((uint8_t)0x20)
#define DMA_FLEXIBLE_UART5_RX       ((uint8_t)0x21)
#define DMA_FLEXIBLE_UART5_TX       ((uint8_t)0x22)
#define DMA_FLEXIBLE_UART6_RX       ((uint8_t)0x23)
#define DMA_FLEXIBLE_UART6_TX       ((uint8_t)0x24)
#define DMA_FLEXIBLE_UART7_RX       ((uint8_t)0x25)
#define DMA_FLEXIBLE_UART7_TX       ((uint8_t)0x26)
#define DMA_FLEXIBLE_UART8_RX       ((uint8_t)0x27)
#define DMA_FLEXIBLE_UART8_TX       ((uint8_t)0x28)
#define DMA_FLEXIBLE_I2C1_RX        ((uint8_t)0x29)
#define DMA_FLEXIBLE_I2C1_TX        ((uint8_t)0x2A)
#define DMA_FLEXIBLE_I2C2_RX        ((uint8_t)0x2B)
#define DMA_FLEXIBLE_I2C2_TX        ((uint8_t)0x2C)
#define DMA_FLEXIBLE_I2C3_RX        ((uint8_t)0x2D)
#define DMA_FLEXIBLE_I2C3_TX        ((uint8_t)0x2E)
#define DMA_FLEXIBLE_SDIO1          ((uint8_t)0x31)
#define DMA_FLEXIBLE_SDIO2          ((uint8_t)0x32)
#define DMA_FLEXIBLE_TIM1_TRIG      ((uint8_t)0x35)
#define DMA_FLEXIBLE_TIM1_COM       ((uint8_t)0x36)
#define DMA_FLEXIBLE_TIM1_UP        ((uint8_t)0x37)
#define DMA_FLEXIBLE_TIM1_CH1       ((uint8_t)0x38)
#define DMA_FLEXIBLE_TIM1_CH2       ((uint8_t)0x39)
#define DMA_FLEXIBLE_TIM1_CH3       ((uint8_t)0x3A)
#define DMA_FLEXIBLE_TIM1_CH4       ((uint8_t)0x3B)
#define DMA_FLEXIBLE_TIM2_TRIG      ((uint8_t)0x3D)
#define DMA_FLEXIBLE_TIM2_UP        ((uint8_t)0x3F)
#define DMA_FLEXIBLE_TIM2_CH1       ((uint8_t)0x40)
#define DMA_FLEXIBLE_TIM2_CH2       ((uint8_t)0x41)
#define DMA_FLEXIBLE_TIM2_CH3       ((uint8_t)0x42)
#define DMA_FLEXIBLE_TIM2_CH4       ((uint8_t)0x43)
#define DMA_FLEXIBLE_TIM3_TRIG      ((uint8_t)0x45)
#define DMA_FLEXIBLE_TIM3_UP        ((uint8_t)0x47)
#define DMA_FLEXIBLE_TIM3_CH1       ((uint8_t)0x48)
#define DMA_FLEXIBLE_TIM3_CH2       ((uint8_t)0x49)
#define DMA_FLEXIBLE_TIM3_CH3       ((uint8_t)0x4A)
#define DMA_FLEXIBLE_TIM3_CH4       ((uint8_t)0x4B)
#define DMA_FLEXIBLE_TIM4_TRIG      ((uint8_t)0x4D)
#define DMA_FLEXIBLE_TIM4_UP        ((uint8_t)0x4F)
#define DMA_FLEXIBLE_TIM4_CH1       ((uint8_t)0x50)
#define DMA_FLEXIBLE_TIM4_CH2       ((uint8_t)0x51)
#define DMA_FLEXIBLE_TIM4_CH3       ((uint8_t)0x52)
#define DMA_FLEXIBLE_TIM4_CH4       ((uint8_t)0x53)
#define DMA_FLEXIBLE_TIM5_TRIG      ((uint8_t)0x55)
#define DMA_FLEXIBLE_TIM5_UP        ((uint8_t)0x57)
#define DMA_FLEXIBLE_TIM5_CH1       ((uint8_t)0x58)
#define DMA_FLEXIBLE_TIM5_CH2       ((uint8_t)0x59)
#define DMA_FLEXIBLE_TIM5_CH3       ((uint8_t)0x5A)
#define DMA_FLEXIBLE_TIM5_CH4       ((uint8_t)0x5B)
#define DMA_FLEXIBLE_TIM6_UP        ((uint8_t)0x5F)
#define DMA_FLEXIBLE_TIM7_UP        ((uint8_t)0x67)
#define DMA_FLEXIBLE_TIM8_TRIG      ((uint8_t)0x6D)
#define DMA_FLEXIBLE_TIM8_COM       ((uint8_t)0x6E)
#define DMA_FLEXIBLE_TIM8_UP        ((uint8_t)0x6F)
#define DMA_FLEXIBLE_TIM8_CH1       ((uint8_t)0x70)
#define DMA_FLEXIBLE_TIM8_CH2       ((uint8_t)0x71)
#define DMA_FLEXIBLE_TIM8_CH3       ((uint8_t)0x72)
#define DMA_FLEXIBLE_TIM8_CH4       ((uint8_t)0x73)

#define IS_DMA_ALL_HARDWARE_ID(HARDWARE_ID)     (((HARDWARE_ID) == DMA_FLEXIBLE_ADC1)     || ((HARDWARE_ID) == DMA_FLEXIBLE_ADC3)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_SPI1_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_SPI1_TX)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_SPI2_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_SPI2_TX)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_SPI3_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_SPI3_TX)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_SPI4_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_SPI4_TX)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_I2S2EXT_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_I2S2EXT_TX)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_I2S3EXT_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_I2S3EXT_TX)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART1_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART1_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART2_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART2_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART3_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART3_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART4_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART4_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART5_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART5_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART6_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART6_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART7_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART7_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_UART8_RX) || ((HARDWARE_ID) == DMA_FLEXIBLE_UART8_TX) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_I2C1_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_I2C1_TX)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_I2C2_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_I2C2_TX)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_I2C3_RX)  || ((HARDWARE_ID) == DMA_FLEXIBLE_I2C3_TX)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_SDIO1)    || ((HARDWARE_ID) == DMA_FLEXIBLE_SDIO2)    || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_TRIG)|| ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_COM) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_UP)  || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_CH1) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_CH2) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_CH3) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM1_CH4) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM2_TRIG)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM2_UP)  || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM2_CH1) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM2_CH2) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM2_CH3) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM2_CH4) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM3_TRIG)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM3_UP)  || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM3_CH1) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM3_CH2) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM3_CH3) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM3_CH4) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM4_TRIG)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM4_UP)  || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM4_CH1) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM4_CH2) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM4_CH3) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM4_CH4) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM5_TRIG)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM5_UP)  || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM5_CH1) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM5_CH2) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM5_CH3) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM5_CH4) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM6_UP)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM7_UP)  || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_TRIG)|| \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_COM) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_UP)  || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_CH1) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_CH2) || \
                                                 ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_CH3) || ((HARDWARE_ID) == DMA_FLEXIBLE_TIM8_CH4))

#define DMA_FLEX_FUNCTION_EN                ((uint32_t)0X1000000)								   

/** @defgroup DMA_data_transfer_direction 
  * @{
  */

#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_DIR_PeripheralSRC              ((uint32_t)0x00000000)
#define IS_DMA_DIR(DIR) (((DIR) == DMA_DIR_PeripheralDST) || \
                         ((DIR) == DMA_DIR_PeripheralSRC))
/**
  * @}
  */

/** @defgroup DMA_peripheral_incremented_mode 
  * @{
  */

#define DMA_PeripheralInc_Enable           ((uint32_t)0x00000040)
#define DMA_PeripheralInc_Disable          ((uint32_t)0x00000000)
#define IS_DMA_PERIPHERAL_INC_STATE(STATE) (((STATE) == DMA_PeripheralInc_Enable) || \
                                            ((STATE) == DMA_PeripheralInc_Disable))
/**
  * @}
  */

/** @defgroup DMA_memory_incremented_mode 
  * @{
  */

#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)
#define DMA_MemoryInc_Disable              ((uint32_t)0x00000000)
#define IS_DMA_MEMORY_INC_STATE(STATE) (((STATE) == DMA_MemoryInc_Enable) || \
                                        ((STATE) == DMA_MemoryInc_Disable))
/**
  * @}
  */

/** @defgroup DMA_peripheral_data_size 
  * @{
  */

#define DMA_PeripheralDataSize_Byte        ((uint32_t)0x00000000)
#define DMA_PeripheralDataSize_HalfWord    ((uint32_t)0x00000100)
#define DMA_PeripheralDataSize_Word        ((uint32_t)0x00000200)
#define IS_DMA_PERIPHERAL_DATA_SIZE(SIZE) (((SIZE) == DMA_PeripheralDataSize_Byte) || \
                                           ((SIZE) == DMA_PeripheralDataSize_HalfWord) || \
                                           ((SIZE) == DMA_PeripheralDataSize_Word))
/**
  * @}
  */

/** @defgroup DMA_memory_data_size 
  * @{
  */

#define DMA_MemoryDataSize_Byte            ((uint32_t)0x00000000)
#define DMA_MemoryDataSize_HalfWord        ((uint32_t)0x00000400)
#define DMA_MemoryDataSize_Word            ((uint32_t)0x00000800)
#define IS_DMA_MEMORY_DATA_SIZE(SIZE) (((SIZE) == DMA_MemoryDataSize_Byte) || \
                                       ((SIZE) == DMA_MemoryDataSize_HalfWord) || \
                                       ((SIZE) == DMA_MemoryDataSize_Word))
/**
  * @}
  */

/** @defgroup DMA_circular_normal_mode 
  * @{
  */

#define DMA_Mode_Circular                  ((uint32_t)0x00000020)
#define DMA_Mode_Normal                    ((uint32_t)0x00000000)
#define IS_DMA_MODE(MODE) (((MODE) == DMA_Mode_Circular) || ((MODE) == DMA_Mode_Normal))
/**
  * @}
  */

/** @defgroup DMA_priority_level 
  * @{
  */

#define DMA_Priority_VeryHigh              ((uint32_t)0x00003000)
#define DMA_Priority_High                  ((uint32_t)0x00002000)
#define DMA_Priority_Medium                ((uint32_t)0x00001000)
#define DMA_Priority_Low                   ((uint32_t)0x00000000)
#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_Priority_VeryHigh) || \
                                   ((PRIORITY) == DMA_Priority_High) || \
                                   ((PRIORITY) == DMA_Priority_Medium) || \
                                   ((PRIORITY) == DMA_Priority_Low))
/**
  * @}
  */

/** @defgroup DMA_memory_to_memory 
  * @{
  */

#define DMA_M2M_Enable                     ((uint32_t)0x00004000)
#define DMA_M2M_Disable                    ((uint32_t)0x00000000)
#define IS_DMA_M2M_STATE(STATE) (((STATE) == DMA_M2M_Enable) || ((STATE) == DMA_M2M_Disable))

/**
  * @}
  */

/** @defgroup DMA_interrupts_definition 
  * @{
  */

#define DMA_IT_TC                          ((uint32_t)0x00000002)
#define DMA_IT_HT                          ((uint32_t)0x00000004)
#define DMA_IT_TE                          ((uint32_t)0x00000008)
#define IS_DMA_CONFIG_IT(IT) ((((IT) & 0xFFFFFFF1) == 0x00) && ((IT) != 0x00))

#define DMA1_IT_GL1                        ((uint32_t)0x00000001)
#define DMA1_IT_TC1                        ((uint32_t)0x00000002)
#define DMA1_IT_HT1                        ((uint32_t)0x00000004)
#define DMA1_IT_TE1                        ((uint32_t)0x00000008)
#define DMA1_IT_GL2                        ((uint32_t)0x00000010)
#define DMA1_IT_TC2                        ((uint32_t)0x00000020)
#define DMA1_IT_HT2                        ((uint32_t)0x00000040)
#define DMA1_IT_TE2                        ((uint32_t)0x00000080)
#define DMA1_IT_GL3                        ((uint32_t)0x00000100)
#define DMA1_IT_TC3                        ((uint32_t)0x00000200)
#define DMA1_IT_HT3                        ((uint32_t)0x00000400)
#define DMA1_IT_TE3                        ((uint32_t)0x00000800)
#define DMA1_IT_GL4                        ((uint32_t)0x00001000)
#define DMA1_IT_TC4                        ((uint32_t)0x00002000)
#define DMA1_IT_HT4                        ((uint32_t)0x00004000)
#define DMA1_IT_TE4                        ((uint32_t)0x00008000)
#define DMA1_IT_GL5                        ((uint32_t)0x00010000)
#define DMA1_IT_TC5                        ((uint32_t)0x00020000)
#define DMA1_IT_HT5                        ((uint32_t)0x00040000)
#define DMA1_IT_TE5                        ((uint32_t)0x00080000)
#define DMA1_IT_GL6                        ((uint32_t)0x00100000)
#define DMA1_IT_TC6                        ((uint32_t)0x00200000)
#define DMA1_IT_HT6                        ((uint32_t)0x00400000)
#define DMA1_IT_TE6                        ((uint32_t)0x00800000)
#define DMA1_IT_GL7                        ((uint32_t)0x01000000)
#define DMA1_IT_TC7                        ((uint32_t)0x02000000)
#define DMA1_IT_HT7                        ((uint32_t)0x04000000)
#define DMA1_IT_TE7                        ((uint32_t)0x08000000)

#define DMA2_IT_GL1                        ((uint32_t)0x10000001)
#define DMA2_IT_TC1                        ((uint32_t)0x10000002)
#define DMA2_IT_HT1                        ((uint32_t)0x10000004)
#define DMA2_IT_TE1                        ((uint32_t)0x10000008)
#define DMA2_IT_GL2                        ((uint32_t)0x10000010)
#define DMA2_IT_TC2                        ((uint32_t)0x10000020)
#define DMA2_IT_HT2                        ((uint32_t)0x10000040)
#define DMA2_IT_TE2                        ((uint32_t)0x10000080)
#define DMA2_IT_GL3                        ((uint32_t)0x10000100)
#define DMA2_IT_TC3                        ((uint32_t)0x10000200)
#define DMA2_IT_HT3                        ((uint32_t)0x10000400)
#define DMA2_IT_TE3                        ((uint32_t)0x10000800)
#define DMA2_IT_GL4                        ((uint32_t)0x10001000)
#define DMA2_IT_TC4                        ((uint32_t)0x10002000)
#define DMA2_IT_HT4                        ((uint32_t)0x10004000)
#define DMA2_IT_TE4                        ((uint32_t)0x10008000)
#define DMA2_IT_GL5                        ((uint32_t)0x10010000)
#define DMA2_IT_TC5                        ((uint32_t)0x10020000)
#define DMA2_IT_HT5                        ((uint32_t)0x10040000)
#define DMA2_IT_TE5                        ((uint32_t)0x10080000)

#define IS_DMA_CLEAR_IT(IT) (((((IT) & 0xF0000000) == 0x00) || (((IT) & 0xEFF00000) == 0x00)) && ((IT) != 0x00))

#define IS_DMA_GET_IT(IT) (((IT) == DMA1_IT_GL1) || ((IT) == DMA1_IT_TC1) || \
                           ((IT) == DMA1_IT_HT1) || ((IT) == DMA1_IT_TE1) || \
                           ((IT) == DMA1_IT_GL2) || ((IT) == DMA1_IT_TC2) || \
                           ((IT) == DMA1_IT_HT2) || ((IT) == DMA1_IT_TE2) || \
                           ((IT) == DMA1_IT_GL3) || ((IT) == DMA1_IT_TC3) || \
                           ((IT) == DMA1_IT_HT3) || ((IT) == DMA1_IT_TE3) || \
                           ((IT) == DMA1_IT_GL4) || ((IT) == DMA1_IT_TC4) || \
                           ((IT) == DMA1_IT_HT4) || ((IT) == DMA1_IT_TE4) || \
                           ((IT) == DMA1_IT_GL5) || ((IT) == DMA1_IT_TC5) || \
                           ((IT) == DMA1_IT_HT5) || ((IT) == DMA1_IT_TE5) || \
                           ((IT) == DMA1_IT_GL6) || ((IT) == DMA1_IT_TC6) || \
                           ((IT) == DMA1_IT_HT6) || ((IT) == DMA1_IT_TE6) || \
                           ((IT) == DMA1_IT_GL7) || ((IT) == DMA1_IT_TC7) || \
                           ((IT) == DMA1_IT_HT7) || ((IT) == DMA1_IT_TE7) || \
                           ((IT) == DMA2_IT_GL1) || ((IT) == DMA2_IT_TC1) || \
                           ((IT) == DMA2_IT_HT1) || ((IT) == DMA2_IT_TE1) || \
                           ((IT) == DMA2_IT_GL2) || ((IT) == DMA2_IT_TC2) || \
                           ((IT) == DMA2_IT_HT2) || ((IT) == DMA2_IT_TE2) || \
                           ((IT) == DMA2_IT_GL3) || ((IT) == DMA2_IT_TC3) || \
                           ((IT) == DMA2_IT_HT3) || ((IT) == DMA2_IT_TE3) || \
                           ((IT) == DMA2_IT_GL4) || ((IT) == DMA2_IT_TC4) || \
                           ((IT) == DMA2_IT_HT4) || ((IT) == DMA2_IT_TE4) || \
                           ((IT) == DMA2_IT_GL5) || ((IT) == DMA2_IT_TC5) || \
                           ((IT) == DMA2_IT_HT5) || ((IT) == DMA2_IT_TE5))

/**
  * @}
  */

/** @defgroup DMA_flags_definition 
  * @{
  */
#define DMA1_FLAG_GL1                      ((uint32_t)0x00000001)
#define DMA1_FLAG_TC1                      ((uint32_t)0x00000002)
#define DMA1_FLAG_HT1                      ((uint32_t)0x00000004)
#define DMA1_FLAG_TE1                      ((uint32_t)0x00000008)
#define DMA1_FLAG_GL2                      ((uint32_t)0x00000010)
#define DMA1_FLAG_TC2                      ((uint32_t)0x00000020)
#define DMA1_FLAG_HT2                      ((uint32_t)0x00000040)
#define DMA1_FLAG_TE2                      ((uint32_t)0x00000080)
#define DMA1_FLAG_GL3                      ((uint32_t)0x00000100)
#define DMA1_FLAG_TC3                      ((uint32_t)0x00000200)
#define DMA1_FLAG_HT3                      ((uint32_t)0x00000400)
#define DMA1_FLAG_TE3                      ((uint32_t)0x00000800)
#define DMA1_FLAG_GL4                      ((uint32_t)0x00001000)
#define DMA1_FLAG_TC4                      ((uint32_t)0x00002000)
#define DMA1_FLAG_HT4                      ((uint32_t)0x00004000)
#define DMA1_FLAG_TE4                      ((uint32_t)0x00008000)
#define DMA1_FLAG_GL5                      ((uint32_t)0x00010000)
#define DMA1_FLAG_TC5                      ((uint32_t)0x00020000)
#define DMA1_FLAG_HT5                      ((uint32_t)0x00040000)
#define DMA1_FLAG_TE5                      ((uint32_t)0x00080000)
#define DMA1_FLAG_GL6                      ((uint32_t)0x00100000)
#define DMA1_FLAG_TC6                      ((uint32_t)0x00200000)
#define DMA1_FLAG_HT6                      ((uint32_t)0x00400000)
#define DMA1_FLAG_TE6                      ((uint32_t)0x00800000)
#define DMA1_FLAG_GL7                      ((uint32_t)0x01000000)
#define DMA1_FLAG_TC7                      ((uint32_t)0x02000000)
#define DMA1_FLAG_HT7                      ((uint32_t)0x04000000)
#define DMA1_FLAG_TE7                      ((uint32_t)0x08000000)

#define DMA2_FLAG_GL1                      ((uint32_t)0x10000001)
#define DMA2_FLAG_TC1                      ((uint32_t)0x10000002)
#define DMA2_FLAG_HT1                      ((uint32_t)0x10000004)
#define DMA2_FLAG_TE1                      ((uint32_t)0x10000008)
#define DMA2_FLAG_GL2                      ((uint32_t)0x10000010)
#define DMA2_FLAG_TC2                      ((uint32_t)0x10000020)
#define DMA2_FLAG_HT2                      ((uint32_t)0x10000040)
#define DMA2_FLAG_TE2                      ((uint32_t)0x10000080)
#define DMA2_FLAG_GL3                      ((uint32_t)0x10000100)
#define DMA2_FLAG_TC3                      ((uint32_t)0x10000200)
#define DMA2_FLAG_HT3                      ((uint32_t)0x10000400)
#define DMA2_FLAG_TE3                      ((uint32_t)0x10000800)
#define DMA2_FLAG_GL4                      ((uint32_t)0x10001000)
#define DMA2_FLAG_TC4                      ((uint32_t)0x10002000)
#define DMA2_FLAG_HT4                      ((uint32_t)0x10004000)
#define DMA2_FLAG_TE4                      ((uint32_t)0x10008000)
#define DMA2_FLAG_GL5                      ((uint32_t)0x10010000)
#define DMA2_FLAG_TC5                      ((uint32_t)0x10020000)
#define DMA2_FLAG_HT5                      ((uint32_t)0x10040000)
#define DMA2_FLAG_TE5                      ((uint32_t)0x10080000)

#define IS_DMA_CLEAR_FLAG(FLAG) (((((FLAG) & 0xF0000000) == 0x00) || (((FLAG) & 0xEFF00000) == 0x00)) && ((FLAG) != 0x00))

#define IS_DMA_GET_FLAG(FLAG) (((FLAG) == DMA1_FLAG_GL1) || ((FLAG) == DMA1_FLAG_TC1) || \
                               ((FLAG) == DMA1_FLAG_HT1) || ((FLAG) == DMA1_FLAG_TE1) || \
                               ((FLAG) == DMA1_FLAG_GL2) || ((FLAG) == DMA1_FLAG_TC2) || \
                               ((FLAG) == DMA1_FLAG_HT2) || ((FLAG) == DMA1_FLAG_TE2) || \
                               ((FLAG) == DMA1_FLAG_GL3) || ((FLAG) == DMA1_FLAG_TC3) || \
                               ((FLAG) == DMA1_FLAG_HT3) || ((FLAG) == DMA1_FLAG_TE3) || \
                               ((FLAG) == DMA1_FLAG_GL4) || ((FLAG) == DMA1_FLAG_TC4) || \
                               ((FLAG) == DMA1_FLAG_HT4) || ((FLAG) == DMA1_FLAG_TE4) || \
                               ((FLAG) == DMA1_FLAG_GL5) || ((FLAG) == DMA1_FLAG_TC5) || \
                               ((FLAG) == DMA1_FLAG_HT5) || ((FLAG) == DMA1_FLAG_TE5) || \
                               ((FLAG) == DMA1_FLAG_GL6) || ((FLAG) == DMA1_FLAG_TC6) || \
                               ((FLAG) == DMA1_FLAG_HT6) || ((FLAG) == DMA1_FLAG_TE6) || \
                               ((FLAG) == DMA1_FLAG_GL7) || ((FLAG) == DMA1_FLAG_TC7) || \
                               ((FLAG) == DMA1_FLAG_HT7) || ((FLAG) == DMA1_FLAG_TE7) || \
                               ((FLAG) == DMA2_FLAG_GL1) || ((FLAG) == DMA2_FLAG_TC1) || \
                               ((FLAG) == DMA2_FLAG_HT1) || ((FLAG) == DMA2_FLAG_TE1) || \
                               ((FLAG) == DMA2_FLAG_GL2) || ((FLAG) == DMA2_FLAG_TC2) || \
                               ((FLAG) == DMA2_FLAG_HT2) || ((FLAG) == DMA2_FLAG_TE2) || \
                               ((FLAG) == DMA2_FLAG_GL3) || ((FLAG) == DMA2_FLAG_TC3) || \
                               ((FLAG) == DMA2_FLAG_HT3) || ((FLAG) == DMA2_FLAG_TE3) || \
                               ((FLAG) == DMA2_FLAG_GL4) || ((FLAG) == DMA2_FLAG_TC4) || \
                               ((FLAG) == DMA2_FLAG_HT4) || ((FLAG) == DMA2_FLAG_TE4) || \
                               ((FLAG) == DMA2_FLAG_GL5) || ((FLAG) == DMA2_FLAG_TC5) || \
                               ((FLAG) == DMA2_FLAG_HT5) || ((FLAG) == DMA2_FLAG_TE5))
/**
  * @}
  */

/** @defgroup DMA_Buffer_Size 
  * @{
  */

#define IS_DMA_BUFFER_SIZE(SIZE) (((SIZE) >= 0x1) && ((SIZE) < 0x10000))

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup DMA_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup DMA_Exported_Functions
  * @{
  */

void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx);
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber); 
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
void DMA_ClearFlag(uint32_t DMAy_FLAG);
ITStatus DMA_GetITStatus(uint32_t DMAy_IT);
void DMA_ClearITPendingBit(uint32_t DMAy_IT);
void DMA_Flexible_Config(DMA_TypeDef *DMAx,uint8_t Flex_Channelx,uint8_t Hardware_ID);
#ifdef __cplusplus
}
#endif

#endif /*__STM32F10x_DMA_H */
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
