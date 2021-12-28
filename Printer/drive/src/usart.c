/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   usart相关程序。
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
  * <h2><center>&copy; COPYRIGHT 2016</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include	"usart.h"
#include	"extgvar.h"
#include	"debug.h"

extern	void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化串口控制管脚
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitUsartPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/************* 设置串口端口 ****************************/
	RCC_APB2PeriphClockCmd(USART_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		=	USART_RXD_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;	//复用输入  
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		=	USART_TXD_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;		//复用输出
	GPIO_Init(USART_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(USART_BUSY_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= USART_BUSY_PIN;	   	//忙
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
  GPIO_Init(USART_BUSY_PORT, &GPIO_InitStructure);
	SetUsartHardBusy();			//设置为忙
}

/*******************************************************************************
* Function Name  : void ConfigUsart(void)
* Description    : Configures the USART.标准全双工模式，中断接收、查询发送
	M		PCE		USART帧组成
	0		0			1位起始位 + 8位数据位 + 停止位
	0		1			1位起始位 + 7位数据位 + 奇偶校验位 + 停止位
	1		0			1位起始位 + 9位数据位 + 停止位
	1		1			1位起始位 + 8位数据位 + 奇偶校验位 + 停止位
	所以当设置数据为8位时,可以设置奇偶校验,也可以不需要奇偶校验.
	但是当设置数据为7位时,必须设置奇偶校验.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	ConfigUsart(void)
{
	USART_InitTypeDef		USART_InitStructure;
	const uint32_t m_tBpsValueTable[] = 
	{
		1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
	};	//波特率表
	
#if	defined	DBG_USART || defined	DBG_ADC	|| defined	DBG_INBUF_RCV	|| defined	DBG_SPI_ZIKU	|| defined	DBG_SPI_ROM
	/*以下为测试功能程序*/
	RCC_APB1PeriphClockCmd(USART_RCC_APBPeriph, ENABLE);	//使能时钟
	
	USART_InitStructure.USART_BaudRate		= 115200;								//设置波特率	
	USART_InitStructure.USART_Parity			= USART_Parity_No; 			//无校验
	USART_InitStructure.USART_WordLength	= USART_WordLength_8b;	//8位数据	
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;			//停止位为1位
	USART_InitStructure.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不使用硬件流控制
	USART_Init(USART_SOURCE, &USART_InitStructure);			//初始化USART_SOURCE
	
	USART_ITConfig(USART_SOURCE, USART_IT_RXNE, ENABLE);	//打开USART接收中断
	USART_Cmd(USART_SOURCE, ENABLE);										//使能USART_SOURCE设备
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TC);				//清发送完成标志
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TXE);				//清发送完成标志 2017.10.18  兼容GD芯片
#else
	/*以下为正常功能程序*/
	uint8_t Temp;
	
	RCC_APB1PeriphClockCmd(USART_RCC_APBPeriph, ENABLE);	//使能时钟
	//设置波特率
	#ifdef	UART_MAX_SPEED_ENABLE
		USART_InitStructure.USART_BaudRate = UART_MAX_BAUD;
		USART_InitStructure.USART_Parity	 = USART_Parity_No;			//无校验
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//7位
	#else
		USART_InitStructure.USART_BaudRate = m_tBpsValueTable[g_tSysConfig.ComBaudRate & 0x07];
		Temp = g_tSysConfig.ComProtocol & 0x03;
		if (Temp == 0 || Temp == 2)	//无校验
		{
			USART_InitStructure.USART_Parity		 = USART_Parity_No;			//无校验
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8位
		}
		else
		{
			if (Temp == 3)						//偶校验
			{
				USART_InitStructure.USART_Parity = USART_Parity_Even;	//偶校验
			}
			else //if( Temp ==1)			//奇校验
			{
				USART_InitStructure.USART_Parity = USART_Parity_Odd;	//奇校验
			}
			
			if (g_tSysConfig.ComProtocol & 0x08)
			{	
				USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//7位
			}
			else
			{
				USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//8位
			}
		}
	#endif
	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;					//停止位为1位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不使用硬件流控制
	USART_Init(USART_SOURCE, &USART_InitStructure);			//初始化USART_SOURCE
	
	USART_ITConfig(USART_SOURCE, USART_IT_RXNE, ENABLE);	//打开USART接收中断
	USART_Cmd(USART_SOURCE, ENABLE);										//使能USART_SOURCE设备
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TC);				//清发送完成标志
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TXE);				//清发送完成标志 2017.10.18  兼容GD芯片	
#endif
}

/*******************************************************************************
* Function Name  : UsartSendStr(USART_TypeDef * USARTx, * uint8_t Str)
* Description    : 从指定的串口发送一字符串
* Input          : USARTX，串口号，Srt:字符串地址
* Output         : None
* Return         : None
*******************************************************************************/
void UsartSendStr(USART_TypeDef * USARTx, uint8_t * Str)
{
	while (*Str)
	{
		while ((!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE))  && (USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TC) == RESET));   //2017.10.18  兼容GD芯片
		USART_SendData(USARTx, *Str);
		Str++;
	}
}

/*******************************************************************************
* Function Name  : SetUsartBusy
* Description    : 设置串口忙信号
* Input          : 根据是硬件还是软件握手决定
* Output         : None
* Return         : None
*******************************************************************************/
void	SetUsartBusy(void)
{
	uint8_t	temp;
	
	if ((g_tSysConfig.ComProtocol & USART_HANDSHAKE_FLAG_VALUE) == USART_HANDSHAKE_HARD_VALUE)
	{
		SetUsartHardBusy();		//硬件握手协议
	}
	else
	{	//设置参数情况下不能回发XON/XOFF以防止设置工具混乱
		if (g_tSystemCtrl.SetMode == 0)
		{
			temp = XOFF;
			UpLoadData(&temp, 1);	//上传数据
		}
	}
}

/*******************************************************************************
* Function Name  : ClearUsartBusy
* Description    : 清除串口忙信号
* Input          : 根据是硬件还是软件握手决定
* Output         : None
* Return         : None
*******************************************************************************/
void	ClearUsartBusy(void)
{
	uint8_t	temp;
	
	if ((g_tSysConfig.ComProtocol & USART_HANDSHAKE_FLAG_VALUE) == USART_HANDSHAKE_HARD_VALUE)
	{
		ClearUsartHardBusy();	//硬件握手协议
	}
	else
	{	//设置参数情况下不能回发XON/XOFF以防止设置工具混乱
		if (g_tSystemCtrl.SetMode == 0)
		{
			temp = XON;
			UpLoadData(&temp, 1);	//上传数据
		}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
