/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   通信接口检测相关的程序.
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
  * <h2><center>&copy; COPYRIGHT 2016 </center></h2>
  ******************************************************************************
	* 通过检测输入端口的状态判断接口型号，为高时为串口+U口型号，为低时为并口型号。
  */

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"checkif.h"
#include	"debug.h"
#include	"usart.h"
#include	"usb_main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define		INTERFACE_TYPE_A_VALUE					(0x00)    
#define		INTERFACE_TYPE_B_VALUE					(0x03)
#define		INTERFACE_TYPE_C_VALUE					(0x02)

#define		INTERFACE_TYPE_A								(0x00)		//串口 
#define		INTERFACE_TYPE_B								(0x01)		//USB_BT
#define		INTERFACE_TYPE_C								(0x02)		//网口
#define		INTERFACE_TYPE_D								(0x03)		//并口
#define		INTERFACE_TYPE_UNKOWN						(0xFF)		//NONE

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化接口检测端口，一个输入
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitCheckInterfacePort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= INTERFACE_CHECK_IN0_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;											//浮空改为下来输入 判断接口状态
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(INTERFACE_CHECK_IN_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测焊接的通信接口状态
* Input          : 无
* Output         : 输出参数，接口种类
* Return         : 返回参数，通信接口
*******************************************************************************/
uint8_t	GetInterfaceType(void)
{
		uint8_t	i;
		uint8_t	GetData0=0,GetData1=0;
	
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO, ENABLE);
		GPIO_InitStructure.GPIO_Pin			= INTERFACE_CHECK_IN0_GPIO_PIN|INTERFACE_CHECK_IN1_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;											
		GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
		GPIO_Init(INTERFACE_CHECK_IN_GPIO_PORT, &GPIO_InitStructure);
	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN0_GPIO_PIN))
		{
				GetData0|=0x01;
		}	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN1_GPIO_PIN))
		{
				GetData1|=0x01;
		}	

		RCC_APB2PeriphClockCmd(INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO, ENABLE);
		GPIO_InitStructure.GPIO_Pin			= INTERFACE_CHECK_IN0_GPIO_PIN|INTERFACE_CHECK_IN1_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;											//浮空改为下来输入 判断接口状态
		GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
		GPIO_Init(INTERFACE_CHECK_IN_GPIO_PORT, &GPIO_InitStructure);
	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN0_GPIO_PIN))
		{
				GetData0|=0x02;
		}	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN1_GPIO_PIN))
		{
				GetData1|=0x02;
		}	
	
		
// 	GetData0 = INTERFACE_TYPE_B_VALUE;   //2016.06.12
		if (GetData0 == INTERFACE_TYPE_A_VALUE)		
		{
				i	= INTERFACE_TYPE_A;//P 0                //串口
		}
		else if (GetData0 == INTERFACE_TYPE_B_VALUE)
		{
				i	= INTERFACE_TYPE_B;//S+U 1              //USB BT 
		}
		else if (GetData0 == INTERFACE_TYPE_C_VALUE)
		{
				if (GetData1 == INTERFACE_TYPE_A_VALUE)	
				{
						i	= INTERFACE_TYPE_C;                    //NET
				}
				else if (GetData1 == INTERFACE_TYPE_B_VALUE)
				{
						i	= INTERFACE_TYPE_D;                    //并口
				}
				else 
				{
						i	= INTERFACE_TYPE_UNKOWN;
				}	
		}
		else
		{
				i	= INTERFACE_TYPE_UNKOWN;
		}
	
		return	(i);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测焊接的通信接口种类,根据接口种类赋值接口全局变量的值
* Input          : 无
* Output         : 输出参数，接口种类
* Return         : 返回参数，通信接口
*******************************************************************************/
void	CheckInterface(void)
{
	uint8_t	type;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(USART_BUSY_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= USART_BUSY_PIN;	   	 //忙线判断接口类型
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
  GPIO_Init(USART_BUSY_PORT, &GPIO_InitStructure);
	DelayUs(50);
  type = GPIO_ReadInputDataBit(USART_BUSY_PORT,USART_BUSY_PIN);
	GPIO_InitStructure.GPIO_Pin		= USART_BUSY_PIN;	   	 //忙线判断接口类型
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		 //下拉输入
  GPIO_Init(USART_BUSY_PORT, &GPIO_InitStructure);
  DelayUs(50);
	if(type != GPIO_ReadInputDataBit(USART_BUSY_PORT,USART_BUSY_PIN))  //串口
	{
		g_tInterface.COM_Type = 0;	
	}
	else
	{
		g_tInterface.COM_Type = 1;	
	}	
	
	RCC_APB2PeriphClockCmd(USB_RCC_APB2Periph_PORT, ENABLE);	 //2018.02.28 
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;		   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
	DelayUs(50);
  type = GPIO_ReadInputDataBit(USB_DISCONNECT,USB_DISCONNECT_PIN);
	GPIO_InitStructure.GPIO_Pin		= USB_DISCONNECT_PIN;	   	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		 //下拉输入
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
  DelayUs(50);
	if(type != GPIO_ReadInputDataBit(USB_DISCONNECT,USB_DISCONNECT_PIN))  //usb
	{
 		g_tInterface.USB_Type = 0;
	}
	else
	{
	  g_tInterface.USB_Type = 1;		//2016.07.21
	}	
	
	RCC_APB2PeriphClockCmd(CHECK_NET_RCC_APB2Periph_GPIO, ENABLE);  //2018.02.28  PA2接的网口MDIO有上拉 判断此脚
	GPIO_InitStructure.GPIO_Pin		= CHECK_NET_GPIO_PIN;	   	 
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
  GPIO_Init(CHECK_NET_GPIO_PORT, &GPIO_InitStructure);
	DelayUs(50);
  type = GPIO_ReadInputDataBit(CHECK_NET_GPIO_PORT,CHECK_NET_GPIO_PIN);
	GPIO_InitStructure.GPIO_Pin		= CHECK_NET_GPIO_PIN;	   	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		 //下拉输入
  GPIO_Init(CHECK_NET_GPIO_PORT, &GPIO_InitStructure);
  DelayUs(50);
	if(type != GPIO_ReadInputDataBit(CHECK_NET_GPIO_PORT,CHECK_NET_GPIO_PIN))  //不等则不存在网口
	{
		g_tInterface.ETH_Type = 0;
	}
	else
	{
 	  g_tInterface.ETH_Type = 1;
	}	
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
