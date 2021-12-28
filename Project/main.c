/**
  ******************************************************************************
  * @file    Templates/main.c 
  * @author  STMicroelectronics
  * @version V1.1.2
  * @date    2019-01-04
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS ST PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMicroelectronicsTEK SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "at32_board.h"
/** @addtogroup STM32F10x_StdPeriph_Templates
  * @{
  */

/** @addtogroup Template
  * @{
  */

/* Private define ------------------------------------------------------------*/
#define DELAY          100
#define FAST           1
#define SLOW           4

/* Extern variables ---------------------------------------------------------*/
extern GPIO_TypeDef *BUTTON_GPIO_PORT[BUTTON_NUM];
extern uint16_t BUTTON_GPIO_PIN[BUTTON_NUM];
extern uint32_t BUTTON_GPIO_RCC_CLK [BUTTON_NUM];

/* Private variables ---------------------------------------------------------*/
uint16_t BUTTON_EXTI_LINE[BUTTON_NUM] = {EXTI_Line0, EXTI_Line13};
uint16_t BUTTON_EXTI_IRQ[BUTTON_NUM] = {EXTI0_IRQn, EXTI15_10_IRQn};
uint8_t BUTTON_EXTI_SOURCE_PORT[BUTTON_NUM] = {GPIO_PortSourceGPIOA, GPIO_PortSourceGPIOC};
uint8_t BUTTON_EXTI_SOURCE_PIN[BUTTON_NUM] = {GPIO_PinSource0, GPIO_PinSource13};

BUTTON_Type gButtonType = BUTTON_WAKEUP;
uint8_t gSpeed = FAST;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Config Clock Out Function.
  * @param  None
  * @retval None
  */
void MCO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*PA8Êä³öPLL/4*/
  RCC_MCOConfig(RCC_MCO_PLLCLK_Div4, RCC_MCOPRE_1);
}

/**
  * @brief  Configure Button EXTI   
  * @param  Button: Specifies the Button to be configured.
  * @retval None
  */
void BUTTON_EXTI_Init(BUTTON_Type button)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  GPIO_EXTILineConfig(BUTTON_EXTI_SOURCE_PORT[button], BUTTON_EXTI_SOURCE_PIN[button]);
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[button];
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = BUTTON_EXTI_IRQ[button];
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Button EXTI0 Interrupt Handler
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  Delay_ms(5);
  EXTI_ClearITPendingBit(BUTTON_EXTI_LINE[gButtonType]);
  if ((GPIO_ReadInputData(BUTTON_GPIO_PORT[gButtonType]) & BUTTON_GPIO_PIN[gButtonType]) == \
      BUTTON_GPIO_PIN[gButtonType])
  {
    if(gSpeed == SLOW)
      gSpeed = FAST;
    else
      gSpeed = SLOW;
  }
}

/**
  * @brief  Button EXTI13 Interrupt Handler
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  Delay_ms(5);
  EXTI_ClearITPendingBit(BUTTON_EXTI_LINE[gButtonType]);
  if ((GPIO_ReadInputData(BUTTON_GPIO_PORT[gButtonType]) & BUTTON_GPIO_PIN[gButtonType]) == \
      BUTTON_GPIO_PIN[gButtonType])
  {
    if(gSpeed == SLOW)
      gSpeed = FAST;
    else
      gSpeed = SLOW;
  }
}

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  gButtonType = BUTTON_WAKEUP;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  AT32_Board_Init();

  MCO_Config();

  BUTTON_EXTI_Init(gButtonType);

  for(;;)
  {
    AT32_LEDn_Toggle(LED2);
    Delay_ms(gSpeed * DELAY);
    AT32_LEDn_Toggle(LED3);
    Delay_ms(gSpeed * DELAY);
    AT32_LEDn_Toggle(LED4);
    Delay_ms(gSpeed * DELAY);
  }
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */



