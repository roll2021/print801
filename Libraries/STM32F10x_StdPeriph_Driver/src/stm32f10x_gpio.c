/**
  ******************************************************************************
  * @file    stm32f10x_gpio.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    11-March-2011
  * @brief   This file provides all the GPIO firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/** @addtogroup STM32F10x_StdPeriph_Driver
  * @{
  */

/** @defgroup GPIO
  * @brief GPIO driver modules
  * @{
  */

/** @defgroup GPIO_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup GPIO_Private_Defines
  * @{
  */

/* ------------ RCC registers bit address in the alias region ----------------*/
#define AFIO_OFFSET                 (AFIO_BASE - PERIPH_BASE)

/* --- EVENTCR Register -----*/

/* Alias word address of EVOE bit */
#define EVCR_OFFSET                 (AFIO_OFFSET + 0x00)
#define EVOE_BitNumber              ((uint8_t)0x07)
#define EVCR_EVOE_BB                (PERIPH_BB_BASE + (EVCR_OFFSET * 32) + (EVOE_BitNumber * 4))


/* ---  MAPR Register ---*/
/* Alias word address of MII_RMII_SEL bit */
#define MAPR_OFFSET                 (AFIO_OFFSET + 0x04)
#define MII_RMII_SEL_BitNumber      ((u8)0x17)
#define MAPR_MII_RMII_SEL_BB        (PERIPH_BB_BASE + (MAPR_OFFSET * 32) + (MII_RMII_SEL_BitNumber * 4))


#define EVCR_PORTPINCONFIG_MASK     ((uint16_t)0xFF80)
#define LSB_MASK                    ((uint16_t)0xFFFF)
#define DBGAFR_POSITION_MASK        ((uint32_t)0x000F0000)
#define DBGAFR_SWJCFG_MASK          ((uint32_t)0xF0FFFFFF)
#define DBGAFR_LOCATION_MASK        ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK         ((uint32_t)0x00100000)
/**
  * @}
  */

/** @defgroup GPIO_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup GPIO_Private_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup GPIO_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup GPIO_Private_Functions
  * @{
  */

/**
  * @brief  Deinitializes the GPIOx peripheral registers to their default reset values.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @retval None
  */
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  if (GPIOx == GPIOA)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
  }
  else if (GPIOx == GPIOB)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);
  }
  else if (GPIOx == GPIOC)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
  }
  else if (GPIOx == GPIOD)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, DISABLE);
  }
  else if (GPIOx == GPIOE)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
  }
#if !defined (STM32F10X_403A) && !defined(STM32F10X_407)
  else if (GPIOx == GPIOF)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, DISABLE);
  }
  else
  {
    if (GPIOx == GPIOG)
    {
      RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOG, ENABLE);
      RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOG, DISABLE);
    }
  }
#endif
}

/**
  * @brief  Deinitializes the Alternate Functions (remap, event control
  *   and EXTI configuration) registers to their default reset values.
  * @param  None
  * @retval None
  */
void GPIO_AFIODeInit(void)
{
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, DISABLE);
}

/**
  * @brief  Initializes the GPIOx peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  uint32_t currentmode = 0x00, currentpin = 0x00, pinpos = 0x00, pos = 0x00;
  uint32_t tmpreg = 0x00, pinmask = 0x00;
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
  assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin));

  /*---------------------------- GPIO Mode Configuration -----------------------*/
  currentmode = ((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x0F);

  if ((((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x10)) != 0x00)
  {
    /* Check the parameters */
    assert_param(IS_GPIO_SPEED(GPIO_InitStruct->GPIO_Speed));
    /* Output mode */
    currentmode |= (uint32_t)GPIO_InitStruct->GPIO_Speed;
  }

  /*---------------------------- GPIO CRL Configuration ------------------------*/
  /* Configure the eight low port pins */
  if (((uint32_t)GPIO_InitStruct->GPIO_Pin & ((uint32_t)0x00FF)) != 0x00)
  {
    tmpreg = GPIOx->CRL;

    for (pinpos = 0x00; pinpos < 0x08; pinpos++)
    {
      pos = ((uint32_t)0x01) << pinpos;
      /* Get the port pins position */
      currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

      if (currentpin == pos)
      {
        pos = pinpos << 2;
        /* Clear the corresponding low control register bits */
        pinmask = ((uint32_t)0x0F) << pos;
        tmpreg &= ~pinmask;
        /* Write the mode configuration in the corresponding bits */
        tmpreg |= (currentmode << pos);

        /* Reset the corresponding ODR bit */
        if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPD)
        {
          GPIOx->BRR = (((uint32_t)0x01) << pinpos);
        }
        else
        {
          /* Set the corresponding ODR bit */
          if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPU)
          {
            GPIOx->BSRR = (((uint32_t)0x01) << pinpos);
          }
        }
      }
    }

    GPIOx->CRL = tmpreg;
  }

  /*---------------------------- GPIO CRH Configuration ------------------------*/
  /* Configure the eight high port pins */
  if (GPIO_InitStruct->GPIO_Pin > 0x00FF)
  {
    tmpreg = GPIOx->CRH;

    for (pinpos = 0x00; pinpos < 0x08; pinpos++)
    {
      pos = (((uint32_t)0x01) << (pinpos + 0x08));
      /* Get the port pins position */
      currentpin = ((GPIO_InitStruct->GPIO_Pin) & pos);

      if (currentpin == pos)
      {
        pos = pinpos << 2;
        /* Clear the corresponding high control register bits */
        pinmask = ((uint32_t)0x0F) << pos;
        tmpreg &= ~pinmask;
        /* Write the mode configuration in the corresponding bits */
        tmpreg |= (currentmode << pos);

        /* Reset the corresponding ODR bit */
        if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPD)
        {
          GPIOx->BRR = (((uint32_t)0x01) << (pinpos + 0x08));
        }

        /* Set the corresponding ODR bit */
        if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPU)
        {
          GPIOx->BSRR = (((uint32_t)0x01) << (pinpos + 0x08));
        }
      }
    }

    GPIOx->CRH = tmpreg;
  }
}

/**
  * @brief  Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;
  GPIO_InitStruct->GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN_FLOATING;
}

/**
  * @brief  Reads the specified input port pin.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin:  specifies the port bit to read.
  *   This parameter can be GPIO_Pin_x where x can be (0..15).
  * @retval The input port pin value.
  */
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint8_t bitstatus = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->IDR & GPIO_Pin) != (uint32_t)Bit_RESET)
  {
    bitstatus = (uint8_t)Bit_SET;
  }
  else
  {
    bitstatus = (uint8_t)Bit_RESET;
  }

  return bitstatus;
}

/**
  * @brief  Reads the specified GPIO input data port.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @retval GPIO input data port value.
  */
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  return ((uint16_t)GPIOx->IDR);
}

/**
  * @brief  Reads the specified output data port bit.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin:  specifies the port bit to read.
  *   This parameter can be GPIO_Pin_x where x can be (0..15).
  * @retval The output port pin value.
  */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint8_t bitstatus = 0x00;
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->ODR & GPIO_Pin) != (uint32_t)Bit_RESET)
  {
    bitstatus = (uint8_t)Bit_SET;
  }
  else
  {
    bitstatus = (uint8_t)Bit_RESET;
  }

  return bitstatus;
}

/**
  * @brief  Reads the specified GPIO output data port.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @retval GPIO output data port value.
  */
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  return ((uint16_t)GPIOx->ODR);
}

/**
  * @brief  Sets the selected data port bits.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->BSRR = GPIO_Pin;
}

/**
  * @brief  Clears the selected data port bits.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->BRR = GPIO_Pin;
}

/**
  * @brief  Sets or clears the selected data port bit.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to be written.
  *   This parameter can be one of GPIO_Pin_x where x can be (0..15).
  * @param  BitVal: specifies the value to be written to the selected bit.
  *   This parameter can be one of the BitAction enum values:
  *     @arg Bit_RESET: to clear the port pin
  *     @arg Bit_SET: to set the port pin
  * @retval None
  */
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_BIT_ACTION(BitVal));

  if (BitVal != Bit_RESET)
  {
    GPIOx->BSRR = GPIO_Pin;
  }
  else
  {
    GPIOx->BRR = GPIO_Pin;
  }
}

/**
  * @brief  Writes data to the specified GPIO data port.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  PortVal: specifies the value to be written to the port output data register.
  * @retval None
  */
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  GPIOx->ODR = PortVal;
}

/**
  * @brief  Locks GPIO Pins configuration registers.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to be written.
  *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint32_t tmp = 0x00010000;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  tmp |= GPIO_Pin;
  /* Set LCKK bit */
  GPIOx->LCKR = tmp;
  /* Reset LCKK bit */
  GPIOx->LCKR =  GPIO_Pin;
  /* Set LCKK bit */
  GPIOx->LCKR = tmp;
  /* Read LCKK bit*/
  tmp = GPIOx->LCKR;
  /* Read LCKK bit*/
  tmp = GPIOx->LCKR;
}

/**
  * @brief  Selects the GPIO pin used as Event output.
  * @param  GPIO_PortSource: selects the GPIO port to be used as source
  *   for Event output.
  *   This parameter can be GPIO_PortSourceGPIOx where x can be (A..E).
  * @param  GPIO_PinSource: specifies the pin for the Event output.
  *   This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @retval None
  */
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
  uint32_t tmpreg = 0x00;
  /* Check the parameters */
  assert_param(IS_GPIO_EVENTOUT_PORT_SOURCE(GPIO_PortSource));
  assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource));

  tmpreg = AFIO->EVCR;
  /* Clear the PORT[6:4] and PIN[3:0] bits */
  tmpreg &= EVCR_PORTPINCONFIG_MASK;
  tmpreg |= (uint32_t)GPIO_PortSource << 0x04;
  tmpreg |= GPIO_PinSource;
  AFIO->EVCR = tmpreg;
}

/**
  * @brief  Enables or disables the Event Output.
  * @param  NewState: new state of the Event output.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void GPIO_EventOutputCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) EVCR_EVOE_BB = (uint32_t)NewState;
}

/**
  * @brief  Changes the mapping of the specified pin.
  * @param  GPIO_Remap: selects the pin to remap.
  *   This parameter can be one of the following values:
  *     @arg GPIO_Remap01_SPI1           : SPI1 Alternate Function mapping01
  *     @arg GPIO_Remap10_SPI1           : SPI1 Alternate Function mapping10
  *     @arg GPIO_Remap_I2C1             : I2C1 Alternate Function mapping
  *     @arg GPIO_Remap_USART1           : USART1 Alternate Function mapping
  *     @arg GPIO_Remap_USART2           : USART2 Alternate Function mapping
  *     @arg GPIO_PartialRemap_USART3    : USART3 Partial Alternate Function mapping
  *     @arg GPIO_FullRemap_USART3       : USART3 Full Alternate Function mapping
  *     @arg GPIO_PartialRemap_TIM1      : TIM1 Partial Alternate Function mapping
  *     @arg GPIO_FullRemap_TIM1         : TIM1 Full Alternate Function mapping
  *     @arg GPIO_PartialRemap1_TIM2     : TIM2 Partial1 Alternate Function mapping
  *     @arg GPIO_PartialRemap2_TIM2     : TIM2 Partial2 Alternate Function mapping
  *     @arg GPIO_FullRemap_TIM2         : TIM2 Full Alternate Function mapping
  *     @arg GPIO_PartialRemap_TIM3      : TIM3 Partial Alternate Function mapping
  *     @arg GPIO_FullRemap_TIM3         : TIM3 Full Alternate Function mapping
  *     @arg GPIO_Remap_TIM4             : TIM4 Alternate Function mapping
  *     @arg GPIO_Remap1_CAN1            : CAN1 Alternate Function mapping
  *     @arg GPIO_Remap2_CAN1            : CAN1 Alternate Function mapping
  *     @arg GPIO_Remap_PD01            : PD01 Alternate Function mapping
  *     @arg GPIO_Remap_TIM5CH4_LSI      : LSI connected to TIM5 Channel4 input capture for calibration
  *     @arg GPIO_Remap_ADC1_EXTRGINJ    : ADC1 External Trigger Injected Conversion remapping
  *     @arg GPIO_Remap_ADC1_EXTRGREG    : ADC1 External Trigger Regular Conversion remapping
  *     @arg GPIO_Remap_ADC2_EXTRGINJ    : ADC2 External Trigger Injected Conversion remapping
  *     @arg GPIO_Remap_ADC2_EXTRGREG    : ADC2 External Trigger Regular Conversion remapping
  *     @arg GPIO_Remap_SWJ_NoJNTRST     : Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
  *     @arg GPIO_Remap_SWJ_JTAGDisable  : JTAG-DP Disabled and SW-DP Enabled
  *     @arg GPIO_Remap_SWJ_AllDisable   : Full SWJ Disabled (JTAG-DP + SW-DP)
  *     @arg GPIO_Remap_TIM15            : TIM15 Alternate Function mapping
  *     @arg GPIO_Remap_TIM9             : TIM9 Alternate Function mapping
  *     @arg GPIO_Remap_TIM10            : TIM10 Alternate Function mapping
  *     @arg GPIO_Remap_TIM11            : TIM11 Alternate Function mapping
  *     @arg GPIO_Remap_TIM13            : TIM13 Alternate Function mapping
  *     @arg GPIO_Remap_TIM14            : TIM14 Alternate Function mapping
  *     @arg GPIO_Remap_XMC_NADV         : XMC_NADV Alternate Function mapping
  *     @arg GPIO_Remap_SPI4             : SPI4 Alternate Function mapping
  *     @arg GPIO_Remap_I2C3             : I2C3 Alternate Function mapping
  *     @arg GPIO_Remap01_SDIO2          : SDIO2 Alternate Function mapping 01:CK/CMD Remaped None,D0~D3 Remaped to PA4~PA7.
  *     @arg GPIO_Remap10_SDIO2          : SDIO2 Alternate Function mapping 10:CK/CMD Remaped to PA2/PA3,D0~D3 Remaped None.
  *     @arg GPIO_Remap11_SDIO2          : SDIO2 Alternate Function mapping 11:CK/CMD Remaped to PA2/PA3,D0~D3 Remaped to PA4~PA7.
  *     @arg GPIO_Remap_EXT_FLASH        : EXT_FLASH Alternate Function mapping
  
  *     @arg AFIO_MAP3_TIM9_0010         : TIM9 Alternate Function mapping:
  *     @arg AFIO_MAP3_TIM10_0010        : TIM10 Alternate Function mapping:
  *     @arg AFIO_MAP3_TIM11_0010        : TIM11 Alternate Function mapping:
  *     @arg AFIO_MAP4_TIM1_0001         : TIM1 Alternate Function mapping 
  *     @arg AFIO_MAP4_TIM2_0001         : TIM2 Alternate Function mapping 0001
  *     @arg AFIO_MAP4_TIM2_0010         : TIM2 Alternate Function mapping 0010
  *     @arg AFIO_MAP4_TIM2_0011         : TIM2 Alternate Function mapping 0011
  *     @arg AFIO_MAP4_TIM3_0010         : TIM3 Alternate Function mapping 0010
  *     @arg AFIO_MAP4_TIM3_0011         : TIM3 Alternate Function mapping 0011
  *     @arg AFIO_MAP4_TIM5_0001         : TIM5 Alternate Function mapping 0001: CH1/CH2
  *     @arg AFIO_MAP4_TIM5_1000         : TIM5 Alternate Function mapping 1000: CH4 
  *     @arg AFIO_MAP4_TIM5_1001         : TIM5 Alternate Function mapping 1001: CH1/CH2+CH4 
  *     @arg AFIO_MAP5_I2C1_0001         : I2C1 Alternate Function mapping 0001
  *     @arg AFIO_MAP5_I2C1_0011         : I2C1 Alternate Function mapping 0011
  *     @arg AFIO_MAP5_I2C2_0001         : I2C2 Alternate Function mapping 0001
  *     @arg AFIO_MAP5_I2C2_0010         : I2C2 Alternate Function mapping 0010
  *     @arg AFIO_MAP5_I2C2_0011         : I2C2 Alternate Function mapping 0011
  *     @arg AFIO_MAP5_SPI1_0001         : SPI1 Alternate Function mapping 
  *     @arg AFIO_MAP5_SPI2_0001         : SPI2 Alternate Function mapping 
  *     @arg AFIO_MAP6_CAN1_0010         : CAN1 Alternate Function mapping 
  *     @arg AFIO_MAP6_CAN2_0001         : CAN2 Alternate Function mapping 
  *     @arg AFIO_MAP6_SDIO_0100         : SDIO Alternate Function mapping 100 
  *     @arg AFIO_MAP6_SDIO_0101         : SDIO Alternate Function mapping 101 
  *     @arg AFIO_MAP6_SDIO_0110         : SDIO Alternate Function mapping 110 
  *     @arg AFIO_MAP6_SDIO_0111         : SDIO Alternate Function mapping 111 
  *     @arg AFIO_MAP6_USART1_0001       : USART1 Alternate Function mapping 
  *     @arg AFIO_MAP6_USART3_0001       : USART3 Partial Alternate Function mapping 
  *     @arg AFIO_MAP6_UART4_0001        : UART4 Alternate Function mapping 
  *     @arg AFIO_MAP7_SPIF_1000         : EXT_FLASH Alternate Function mapping  
  *     @arg AFIO_MAP7_SPIF_1001         : EXT_FLASH Alternate Function enable  
  *     @arg AFIO_MAP7_ADC1_0001         : ADC1 External Trigger Injected Conversion remapping 
  *     @arg AFIO_MAP7_ADC1_0010         : ADC1 External Trigger Regular Conversion remapping 
  *     @arg AFIO_MAP7_ADC1_0011         : ADC1 External Trigger Regular & Injected Conversion remapping 
  *     @arg AFIO_MAP7_ADC2_0001         : ADC2 External Trigger Injected Conversion remapping 
  *     @arg AFIO_MAP7_ADC2_0010         : ADC2 External Trigger Regular Conversion remapping 
  *     @arg AFIO_MAP7_ADC2_0011         : ADC2 External Trigger Regular & Injected Conversion remapping 
  *     @arg AFIO_MAP7_SWJTAG_0001       : Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST 
  *     @arg AFIO_MAP7_SWJTAG_0010       : JTAG-DP Disabled and SW-DP Enabled 
  *     @arg AFIO_MAP7_SWJTAG_0100       : Full SWJ Disabled (JTAG-DP + SW-DP) 
  *     @arg AFIO_MAP7_PD01_0001         : PD01 Alternate Function mapping 
  * @param  NewState: new state of the port pin remapping.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState)
{
#if defined (STM32F10X_403A) || defined (STM32F10X_407)
  if(IS_GREMAP(GPIO_Remap))
  {
    uint32_t reg_ost = 0x00, bit_ost = 0x00, Remap_Addr, Remap_val;
    volatile uint32_t *AFIO_MAPx;

    /* Calculate the remap register value by parameter GPIO_Remap */
    Remap_Addr = GPIO_Remap & 0x7FFFFFFF;
    reg_ost = Remap_Addr >> 7;
    Remap_Addr &= 0x7f;
    bit_ost = Remap_Addr >> 4;
    Remap_Addr &= 0x0f;

    if(reg_ost != AFIO_MAP8)
      Remap_val = Remap_Addr << (bit_ost * 4);
    else
      Remap_val = Remap_Addr << (bit_ost/2 * 4);

    switch(reg_ost)
    {
      case AFIO_MAP3:
        AFIO_MAPx = &(AFIO->MAPR3);
        break;

      case AFIO_MAP4:
        AFIO_MAPx = &(AFIO->MAPR4);
        break;

      case AFIO_MAP5:
        AFIO_MAPx = &(AFIO->MAPR5);
        break;

      case AFIO_MAP6:
        AFIO_MAPx = &(AFIO->MAPR6);
        break;

      case AFIO_MAP7:
        AFIO_MAPx = &(AFIO->MAPR7);
        break;

      case AFIO_MAP8:
        AFIO_MAPx = &(AFIO->MAPR8);
        break;

      default:
        break;
    }

    if(bit_ost == BITS0)
    {
      if(reg_ost == AFIO_MAP8)
      {
        *AFIO_MAPx &= 0xFFFFFFFC;
      }else
      {
        *AFIO_MAPx &= OFFSET_MASK0;
      }
    }
    else if(bit_ost == BITS1)
    {
      if(reg_ost == AFIO_MAP8)
      {
        *AFIO_MAPx &= 0xFFFFFFF3;
      }else
      {
        *AFIO_MAPx &= OFFSET_MASK1;
      }
    }
    else if(bit_ost == BITS2)
    {
      if(reg_ost == AFIO_MAP8)
      {
        *AFIO_MAPx &= 0xFFFFFFCF;
      }else
      {
        *AFIO_MAPx &= OFFSET_MASK2;
      }
    }
    else if(bit_ost == BITS3)
    {
      if(reg_ost == AFIO_MAP8)
      {
        *AFIO_MAPx &= 0xFFFFFF3F;
      }else
      {
        *AFIO_MAPx &= OFFSET_MASK3;
      }
    }
    else if(bit_ost == BITS4)
    {
      *AFIO_MAPx &= OFFSET_MASK4;
    }
    else if(bit_ost == BITS5)
    {
      *AFIO_MAPx &= OFFSET_MASK5;
    }
    else if(bit_ost == BITS6)
    {
      *AFIO_MAPx &= OFFSET_MASK6;
    }
    else if(bit_ost == BITS7)
    {
      *AFIO_MAPx &= OFFSET_MASK7;
    }

    if(NewState == ENABLE)
    {
      *AFIO_MAPx |= Remap_val;
    }

  }
  else
#endif
  {
    /* Check the parameters */
    assert_param(IS_GPIO_REMAP(GPIO_Remap));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    switch(GPIO_Remap)
    {
    case GPIO_Remap_I2C1:
    case GPIO_Remap_USART1:
    case GPIO_Remap_USART2:
    case GPIO_Remap_TIM4:
    case GPIO_Remap_PD01:
    case GPIO_Remap_TIM5CH4_LSI:
    case GPIO_Remap_ADC1_EXTRGINJ:
    case GPIO_Remap_ADC1_EXTRGREG:
    case GPIO_Remap_ADC2_EXTRGINJ:
    case GPIO_Remap_ADC2_EXTRGREG:
      AFIO->MAPR &= ~GPIO_Remap;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap;
      }

      break;

    case GPIO_Remap01_SPI1:
      AFIO->MAPR &= 0x7FFFFFFE;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap01_SPI1;
      }

      break;

    case GPIO_Remap10_SPI1:
      AFIO->MAPR &= 0x7FFFFFFE;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap10_SPI1;
      }

      break;

    case GPIO_PartialRemap_USART3:
      AFIO->MAPR &= 0xFFFFFFCF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_PartialRemap_USART3;
      }

      break;

    case GPIO_FullRemap_USART3:
      AFIO->MAPR &= 0xFFFFFFCF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_FullRemap_USART3;
      }

      break;

    case GPIO_PartialRemap_TIM1:
      AFIO->MAPR &= 0xFFFFFF3F;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_PartialRemap_TIM1;
      }

      break;

    case GPIO_FullRemap_TIM1:
      AFIO->MAPR &= 0xFFFFFF3F;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_FullRemap_TIM1;
      }

      break;

    case GPIO_PartialRemap1_TIM2:
      AFIO->MAPR &= 0xFFFFFCFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_PartialRemap1_TIM2;
      }

      break;

    case GPIO_PartialRemap2_TIM2:
      AFIO->MAPR &= 0xFFFFFCFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_PartialRemap2_TIM2;
      }

      break;

    case GPIO_FullRemap_TIM2:
      AFIO->MAPR &= 0xFFFFFCFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_FullRemap_TIM2;
      }

      break;

    case GPIO_PartialRemap_TIM3:
      AFIO->MAPR &= 0xFFFFF3FF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_PartialRemap_TIM3;
      }

      break;

    case GPIO_FullRemap_TIM3:
      AFIO->MAPR &= 0xFFFFF3FF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_FullRemap_TIM3;
      }

      break;

    case GPIO_Remap1_CAN1:
      AFIO->MAPR &= 0xFFFF9FFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap1_CAN1;
      }

      break;

    case GPIO_Remap2_CAN1:
      AFIO->MAPR &= 0xFFFF9FFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap2_CAN1;
      }

      break;

    case GPIO_Remap_SWJ_NoJNTRST:
      AFIO->MAPR &= 0xF8FFFFFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap_SWJ_NoJNTRST;
      }

      break;

    case GPIO_Remap_SWJ_JTAGDisable:
      AFIO->MAPR &= 0xF8FFFFFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap_SWJ_JTAGDisable;
      }

      break;

    case GPIO_Remap_SWJ_AllDisable:
      AFIO->MAPR &= 0xF8FFFFFF;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap_SWJ_AllDisable;
      }

      break;
    case GPIO_Remap_ETH:
      AFIO->MAPR &= ~GPIO_Remap_ETH;

      if(NewState == ENABLE)
      {
        AFIO->MAPR |= GPIO_Remap_ETH;
      }

      break;
    default:
      break;
    }
  }
}

/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  GPIO_PortSource: selects the GPIO port to be used as source for EXTI lines.
  *   This parameter can be GPIO_PortSourceGPIOx where x can be (A..G).
  * @param  GPIO_PinSource: specifies the EXTI line to be configured.
  *   This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @retval None
  */
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
  uint32_t tmp = 0x00;
  /* Check the parameters */
  assert_param(IS_GPIO_EXTI_PORT_SOURCE(GPIO_PortSource));
  assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource));

  tmp = ((uint32_t)0x0F) << (0x04 * (GPIO_PinSource & (uint8_t)0x03));
  AFIO->EXTICR[GPIO_PinSource >> 0x02] &= ~tmp;
  AFIO->EXTICR[GPIO_PinSource >> 0x02] |= (((uint32_t)GPIO_PortSource) << (0x04 * (GPIO_PinSource & (uint8_t)0x03)));
}

/**
  * @brief  Selects the Ethernet media interface.
  * @note   This function applies only to STM32 Connectivity line devices.
  * @param  GPIO_ETH_MediaInterface: specifies the Media Interface mode.
  *   This parameter can be one of the following values:
  *     @arg GPIO_ETH_MediaInterface_MII: MII mode
  *     @arg GPIO_ETH_MediaInterface_RMII: RMII mode
  * @retval None
  */
void GPIO_ETH_MediaInterfaceConfig(uint32_t GPIO_ETH_MediaInterface)
{
  assert_param(IS_GPIO_ETH_MEDIA_INTERFACE(GPIO_ETH_MediaInterface));

  /* Configure MII_RMII selection bit */
  *(__IO uint32_t *) MAPR_MII_RMII_SEL_BB = GPIO_ETH_MediaInterface;
}

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
