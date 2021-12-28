/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the Application firmware functions.
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

/* Includes ------------------------------------------------------------------*/ 
#include "stm32f10x.h"
#include "usbd_prn_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usb_main.h"
#include "defconstant.h"


extern TypeDef_StructInterfaceType	g_tInterface;		//接口类型结构


__ALIGN_BEGIN uint8_t USB_Tx_Buffer[USB_TX_DATA_SIZE] __ALIGN_END ; 

APP_Tx_Struct USB_Tx={USB_TX_DATA_SIZE,0,0,0,0,USB_Tx_Buffer};

  
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
   
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_Functions
  * @{
  */ 

/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */


/*******************************************************************************
* Function Name  : GetToHostData(APP_Tx_Struct *Tx_Buf, uint16_t *Tx_length)
* Description    : 获取上传数据指针与长度
* Input          : Tx_length：希望发送数据长度地址
* Output         : Tx_length：发送数据长度
* Return         : 发送缓冲区地址
*******************************************************************************/
uint8_t *GetToHostData(APP_Tx_Struct *Tx_Buf, uint16_t *Tx_length)
{
	uint16_t Tx_ptr, PackageLen; 
	uint8_t  *p;
	
	PackageLen = *Tx_length;
	p=Tx_Buf->Buffer;
	
	if (Tx_Buf->State == 1)
	{
		if (Tx_Buf->length == 0) 
		{
			Tx_Buf->State = 0;
			*Tx_length =0;
		}
		else 
		{
			Tx_ptr = Tx_Buf->ptr_out;
			if (Tx_Buf->length > PackageLen)
			{
				*Tx_length = PackageLen;
			}
			else 
			{
				*Tx_length = Tx_Buf->length;
			}
			Tx_Buf->ptr_out += *Tx_length;
			Tx_Buf->length -= *Tx_length;    
			p += Tx_ptr;
		}
	}
	return	p;
}
/*******************************************************************************
* Function Name  : *StartToHostData(APP_Tx_Struct *Tx_Buf, uint16_t *Tx_length)
* Description    : 启动发送数据
* Input          : Tx_length：希望发送数据长度地址
* Output         : Tx_length：发送数据长度
* Return         : 发送缓冲区地址
*******************************************************************************/
uint8_t *StartToHostData(APP_Tx_Struct *Tx_Buf, uint16_t *Tx_length)
{
	uint8_t  *p;
	
	p =Tx_Buf->Buffer;
	if(Tx_Buf->State != 1)
	{
		if (Tx_Buf->ptr_out == Tx_Buf->size)
		{
			Tx_Buf->ptr_out = 0;   
		}
    
		if(Tx_Buf->ptr_out == Tx_Buf->ptr_in) 
		{
			Tx_Buf->State = 0;
			*Tx_length =0;
			if(g_tInterface.WIFI_Type)
			{
				//clear_busy = 1 ; 
			} 
		}
		else
		{
			//计算发送缓冲区字节数
			if(Tx_Buf->ptr_out > Tx_Buf->ptr_in) /* rollback */
			{ 
				Tx_Buf->length = Tx_Buf->size - Tx_Buf->ptr_out;
			}
			else 
			{
				Tx_Buf->length = Tx_Buf->ptr_in - Tx_Buf->ptr_out;
			}
			#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
			 Tx_Buf->length &= ~0x03;
			#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
				
			Tx_Buf->State = 1; 
			p = GetToHostData(Tx_Buf, Tx_length );
		}
	}
	else
		p = GetToHostData(Tx_Buf, Tx_length );
	return p;
}


/*******************************************************************************
* Function Name  : USB_Port_Init
* Description    : 初始化IO
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_Port_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(USB_RCC_APB2Periph_PORT, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;		   //2016.07.21
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
}	
	
/*******************************************************************************
* Function Name  : USB_Main
* Description    : 初始化USB
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_Main(void)
{

  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32fxxx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file
  */  


	USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
