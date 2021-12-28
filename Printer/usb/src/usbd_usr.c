/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file includes the user application layer
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
#include "usbd_usr.h"
#include "usbd_ioreq.h"
#include "usbd_cdc_prn.h"
#include "usbd_desc.h"

#include "extgvar.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USBD_USR 
* @brief    This file includes the user application layer
* @{
*/ 

/** @defgroup USBD_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Defines
* @{
*/ 
#define VCP_USBD_VID                    0x0483
#define VCP_USBD_PID                    0x5740

#define EPSON_USBD_VID                  0x04B8
#define EPSON_USBD_PID                  0x0202

/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Variables
* @{
*/ 

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  
  
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

/**
* @}
*/

/** @defgroup USBD_USR_Private_Constants
* @{
*/ 

/**
* @}
*/



/** @defgroup USBD_USR_Private_FunctionPrototypes
* @{
*/ 
void GetSerialNum(void);
void GetPrinterID(void);

/**
* @}
*/ 
extern uint8_t USB_cdc_Desc_size;
extern uint8_t *usbd_cdc_CfgDesc;
extern uint8_t *usbd_cdc_OtherCfgDesc;
extern uint8_t usbd_cdc_CfgDesc_VCP [USB_CDC_CONFIG_DESC_SIZ_VCP];
extern uint8_t usbd_cdc_OtherCfgDesc_VCP [USB_CDC_CONFIG_DESC_SIZ_VCP];

extern uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC];
extern uint8_t USBD_SERIALNUMBER_FS_STRING[11];
extern uint8_t PRINTER_ID_String[34+ 2*SETSTRINGLEN];

/** @defgroup USBD_USR_Private_Functions
* @{
*/ 

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{  
	uint8_t *pbuf;

	//获取序列号
	GetSerialNum();
	
	/*用FLASH中的打印机制造商、型号修正打印机ID */
	GetPrinterID();
	
	//按USB接口类型初始化设备描述符、接口描述符
	if(g_tSysConfig.USB_Class ==VCP_CLASS)
	{
		pbuf = (uint8_t *)USBD_DeviceDesc;
		pbuf[8] = LOBYTE(VCP_USBD_VID);
		pbuf[9] = HIBYTE(VCP_USBD_VID);
		pbuf[10] = LOBYTE(VCP_USBD_PID);
		pbuf[11] = HIBYTE(VCP_USBD_PID);           

		USB_cdc_Desc_size = USB_CDC_CONFIG_DESC_SIZ_VCP;
		usbd_cdc_CfgDesc =usbd_cdc_CfgDesc_VCP;
		usbd_cdc_OtherCfgDesc= usbd_cdc_OtherCfgDesc_VCP;
	}
	else
	{
		//如果厂商名前设置为“EPSON”，则设备描述符中使用EPSONPOS打印机的VID与PID
		if(ManufacturerType ==1)
		{
			pbuf = (uint8_t *)USBD_DeviceDesc;
			pbuf[8] = LOBYTE(EPSON_USBD_VID);
			pbuf[9] = HIBYTE(EPSON_USBD_VID);
			pbuf[10] = LOBYTE(EPSON_USBD_PID);
			pbuf[11] = HIBYTE(EPSON_USBD_PID);           
		}
	}
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{
}

/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConfigured (void)
{
 }

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConnected (void)
{
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceDisconnected (void)
{
}
/**
* @}
*/ 

/**
* @}
*/ 
/*******************************************************************************
* Function Name  : GetSerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void GetSerialNum(void)
{
  u32 Device_Serial0, Device_Serial1, Device_Serial2;
	u8	i, Temp;

	//用芯片号用为USB序列号
	if(g_tSysConfig.USBIDEnable)
  {	
		Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
		Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
		Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
	
		if (Device_Serial0 != 0)
		{
			for (i=0; i<3; i++)
			{
				Temp=(u8)(Device_Serial1 & 0x0000000F);
				USBD_SERIALNUMBER_FS_STRING[2-i] = Temp<10? 0x30+Temp : 0x37+Temp;
				Device_Serial1 >>= 4;
			}	
		
		    for (i=0;i<8;i++)
			{
				Temp=(u8)(Device_Serial2 & 0x0000000F);
				USBD_SERIALNUMBER_FS_STRING[10 - i] = Temp<10? 0x30+Temp : 0x37+Temp;
				Device_Serial2 >>= 4;
			}	
  		}
	}
	else
	{
		for(i=0; i< sizeof(g_tSysConfig.SerialNumber); i++)
			USBD_SERIALNUMBER_FS_STRING[i] = g_tSysConfig.SerialNumber[i];	
	}
		
}
/*******************************************************************************
* Function Name  : GetPrinterID.
* Description    : Create the Printer ID string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void GetPrinterID(void)
{
	u8 *p;
	u16 i;
	
	i =26;

	//填入厂商
	PRINTER_ID_String[i++] = 'M';
	PRINTER_ID_String[i++] = 'F';
	PRINTER_ID_String[i++] = 'G';
	PRINTER_ID_String[i++] = ':';

	p = g_tSysConfig.Manufacturer;

	while(*p)
	{
		PRINTER_ID_String[i++]= *p;
		p++;
	}
	PRINTER_ID_String[i++]=';';

	//以下为发送型号用，如果加入，会提示安装USB的驱动程序，因为没有开发专用的USB驱动程序，所以不返回型号
/*
	//填入型号
	PRINTER_ID_String[i++]= 'M';
	PRINTER_ID_String[i++]= 'D';
	PRINTER_ID_String[i++]= 'L';
	PRINTER_ID_String[i++]= ':';
	
	p = g_tSysConfig.Model;
	while(*p)
	{
		PRINTER_ID_String[i++]= *p;
		p++;
	}
	PRINTER_ID_String[i++]=';';
 
*/
	PRINTER_ID_String[0] =(u8)(i>>8);
	PRINTER_ID_String[1] =(u8)i;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
