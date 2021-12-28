/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Generic media access Layer.
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

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_prn.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "defconstant.h"
#include "extgvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

//定义打印机类请求
typedef enum _PRINTER_REQUESTS
{
  GET_DEVICE_ID = 0,
  GET_PORT_STATUS,
  SOFT_RESET,
} PRINTER_REQUESTS;

/* Private variables ---------------------------------------------------------*/
static uint8_t USBD_PRN_Status;	//打印机状态

LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

/*子类用 打印机ID 1284标准*/
uint8_t PRINTER_ID_String[34+ 2*SETSTRINGLEN]=
  {
	0x00,			//长度高字节
	36,				//长度低字节，默认没有设备型号
	/* "MFG:IPRT ;CMD:EPSON;CLS:PRINTER;MDL:iP-POS;" , 不用UNICODE码*/
	'C', 'M', 'D', ':', 'E', 'S', 'C', '/', 'P', 'O', 'S', ';',	//指令集
	'C', 'L', 'S', ':', 'P', 'R', 'I', 'N', 'T', 'E', 'R', ';',	//设备类型
	'M', 'F', 'G', ':', 'i', 'P', 'R', 'T', ' ' ,';' ,			//厂商
	'M', 'D', 'L', ':', 'i', 'P', '-', 'P', 'O', 'S', ';' 		//设备型号，注意如果发送了设备型号，则会要求安装驱动程序。
	};


extern APP_Tx_Struct USB_Tx;	//USB输出缓冲区结构

/* Private function prototypes -----------------------------------------------*/
static uint16_t USB_Init     (void);
static uint16_t USB_DeInit   (void);
static uint16_t PRN_Ctrl     (uint32_t Cmd, uint8_t* *Buf, uint16_t *Len);
static uint16_t USB_DataRx   (uint8_t* Buf, uint32_t Len);
static uint16_t USB_DataTx 	 (uint8_t* *Buf, uint32_t *Len);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* *Buf, uint16_t *Len);

static uint8_t *Get_Device_ID(uint16_t *Length);
static uint8_t GetprinterStatus(void);

CDC_IF_Prop_TypeDef CDC_fops = 
{
  USB_Init,
  USB_DeInit,
  PRN_Ctrl,
  USB_DataTx,
  USB_DataRx,
};

extern uint8_t *StartToHostData(APP_Tx_Struct *Tx_Buf, uint32_t *Tx_length);
extern uint8_t PutNetCharToBuffer(uint8_t *Bufer, uint16_t Length, uint8_t Port);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t USB_Init(void)
{
	uint8_t *pbuf;
	
	if(g_tSysConfig.USB_Class == VCP_CLASS)
	{
		CDC_fops.pIf_Ctrl =VCP_Ctrl;
		pbuf = (uint8_t *)USBD_DeviceDesc;
		pbuf[4] = DEVICE_CLASS_CDC;
		pbuf[5] = DEVICE_SUBCLASS_CDC;
	}
	return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t USB_DeInit(void)
{
	return USBD_OK;
}

/**
  * @brief  USB_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t USB_DataRx (uint8_t* Buf, uint32_t Len)
{
	return PutNetCharToBuffer(Buf, Len, USB);

}

/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* *Buffer, uint16_t *Len)
{ 
	uint8_t *Buf;
	Buf = *Buffer;
	
	switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype = Buf[6];
    /* Set the new configuration */
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype; 
    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }
  return USBD_OK;
}

/**
  * @brief  PRN_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t PRN_Ctrl (uint32_t Cmd, uint8_t* *Buf, uint16_t *Len)
{ 

	switch (Cmd)
	{
		case GET_PORT_STATUS:
		{
			if( *Len == 1)
			{
				USBD_PRN_Status = GetprinterStatus();
				*Buf =&USBD_PRN_Status;
			}
			else
			{
				*Len =0;
			}
			break;
		}
		case GET_DEVICE_ID:
		{
			*Buf = Get_Device_ID( Len);
			break;
		}
		case SOFT_RESET:
		{
			if(*Len == 0)
			{
				
			}
			break;
		}
		default:
		break;
	}
	return USBD_OK;
}
/*******************************************************************************
* Function Name  : USB_DataTx (uint8_t* *Buf, uint32_t *Len)
* Description    : USB发送数据
* Input          : Buf：缓冲区地址指针，Len：发送数据长度指针.
* Output         : Buf：缓冲区地址，Len：发送数据长度.
* Return         : 
*******************************************************************************/
static uint16_t USB_DataTx (uint8_t* *Buf, uint32_t *Len)
{
	*Buf =StartToHostData(&USB_Tx, Len);
	
	return USBD_OK;
}
/*******************************************************************************
* Function Name  : Get_Device_ID
* Description    : Handle the Get_Decice_ID request.
* Input          : u16 Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
static uint8_t *Get_Device_ID(uint16_t *Length)
{
	*Length = (*PRINTER_ID_String )*256 + *(PRINTER_ID_String +1);
    return((uint8_t *)PRINTER_ID_String);
}
/*******************************************************************************
* Function Name  : GetprinterStatus
* Description    : 获取打印机机状态字节
* Input          : None.
* Return         : None.
*******************************************************************************/
static uint8_t GetprinterStatus(void)
{
	uint8_t PRNStatus;
	PRNStatus=0x18;
	if ((g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag) != 0)  ///打印机有错误情况 D3
		PRNStatus = PRNStatus & 0xF7;
	if ( g_tError.PaperSensorStatusFlag & 0x01 )  ///打印机缺纸 D5  2016.07.29 判断缺纸
		PRNStatus = PRNStatus | 0x20;
	return	PRNStatus;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
