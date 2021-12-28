/**
  ******************************************************************************
  * @file    usb_sil.c 
  * @author  Artery Technology
  * @version V1.1.2
  * @date    2019-01-04
  * @brief   Simplified Interface Layer for Global Initialization and Endpoint
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, ARTERYTEK SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2018 ArteryTek</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the USB Device IP and the Endpoint 0.
  * @param  None
  * @retval 0.
  */
uint32_t USB_SIL_Init(void)
{
  /* USB interrupts initialization */
  /* clear pending interrupts */
  _SetINTSTS(0);
  wInterrupt_Mask = IMR_MSK;
  /* set interrupts mask */
  _SetCTRL(wInterrupt_Mask);
  return 0;
}

/**
  * @brief  Write a buffer of data to a selected endpoint.
  * @param  bEpAddr: The address of the non control endpoint.
  * @param  pBufferPointer: The pointer to the buffer of data to be written to the endpoint.
  * @param  wBufferSize: Number of data to be written (in bytes).
  * @retval 0.
  */
uint32_t USB_SIL_Write(uint8_t bEpAddr, uint8_t* pBufferPointer, uint32_t wBufferSize)
{
  /* Use the memory interface function to write to the selected endpoint */
  UserToPMABufferCopy(pBufferPointer, GetEPTxAddr(bEpAddr & 0x7F), wBufferSize);

  /* Update the data length in the control register */
  SetEPTxCount((bEpAddr & 0x7F), wBufferSize);
  
  return 0;
}

/**
  * @brief  read a buffer of data from the selected endpoint.
  * @param  bEpAddr: The address of the non control endpoint.
  * @param  pBufferPointer: The pointer to the buffer of data to be written to the endpoint.
  * @retval Number of data read.
  */
uint32_t USB_SIL_Read(uint8_t bEpAddr, uint8_t* pBufferPointer)
{
  uint32_t DataLength = 0;

  /* Get the number of received data on the selected Endpoint */
  DataLength = GetEPRxCount(bEpAddr & 0x7F);
  
  /* Use the memory interface function to write to the selected endpoint */
  PMAToUserBufferCopy(pBufferPointer, GetEPRxAddr(bEpAddr & 0x7F), DataLength);

  /* Return the number of received data */
  return DataLength;
}


