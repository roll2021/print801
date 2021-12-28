/**
  ******************************************************************************
  * @file    usbd_desc.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   header file for the usbd_desc.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USB_MAIN_H
#define __USB_MAIN_H

/* Includes ------------------------------------------------------------------*/
#include	"defconstant.h"

void USB_Port_Init(void);
void USB_Main(void);

#define USB_RCC_APB2Periph_PORT						RCC_APB2Periph_GPIOB
#define USB_DISCONNECT										GPIOB
#define USB_DISCONNECT_PIN								GPIO_Pin_15
#define RCC_APB2Periph_GPIO_DISCONNECT		RCC_APB2Periph_GPIOC  //2016.07.21


//usb返回缓存宏定义和相关函数
#define USB_TX_DATA_SIZE 256

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */


/* Private function prototypes -----------------------------------------------*/
uint8_t *GetToHostData(APP_Tx_Struct *Tx_Buf, uint16_t *Tx_length);




#endif /* 定义结束 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
