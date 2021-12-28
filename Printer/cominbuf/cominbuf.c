/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   通信接收缓冲区相关的程序.
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
#include	"cominbuf.h"
#include	"extgvar.h"
#include	"feedpaper.h"
#include	"button.h"
#include	"usart.h"
//#include	"usb_prop.h"
#include	"parallel.h"
#include	"timconfig.h"

extern	void PrintString(uint8_t *Str);
extern	void MaxSetp(void);
extern  void USB_ClearBusy(void);
extern uint8_t ETH_ConnectState;   //2018.03.05
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// TypeDef_StructInBuffer volatile g_tInBuf;

uint8_t Eth_ClearBusy=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : void SetComInBufBusy(void)
* Description    : 清除通信端口忙
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	SetComInBufBusy(void)
{
	if (g_tInterface.LPT_Type)
		SetParallelBusy();			//并口置忙
	
	if (g_tInterface.COM_Type)
		SetUsartBusy();					//置忙
	if ((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))
			BT_BUSY_SET;
// 	if (g_tInterface.USB_Type)
// 		Set_USB_Busy();					//不允许USB接收下帧数据
}

/*******************************************************************************
* Function Name  : void ClearComInBufBusy(void)
* Description    : 清除通信端口忙
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	ClearComInBufBusy(void)
{
	if (g_tInterface.LPT_Type)
		LPT_ACK_Out();					//并口清忙
	
	if (g_tInterface.COM_Type)
		ClearUsartBusy();				//清忙
	
	if (g_tInterface.USB_Type)
		USB_ClearBusy();				//允许USB接收下帧数据
	
	if(g_tInterface.ETH_Type) 
		Eth_ClearBusy =1;		//以太网清忙标志2016.10.08
		if (g_tInterface.BT_Type)
	{
		BT_BUSY_CLR;
	}	
	if(g_tInterface.WIFI_Type)
		WIFI_BUSY_CLR;
}

/*******************************************************************************
* Function Name  : void EnableReceiveInterrupt(void)
* Description    : 关闭或打开接收的中断
* Input          : NewState：DISABLE 关闭，ENABLE 打开
* Output         : None
* Return         : None
*******************************************************************************/
void	EnableReceiveInterrupt(void)
{

	if (g_tInterface.USB_Type)					//USB
	{
		NVIC_EnableIRQ(OTG_FS_IRQn);
	}
	if (g_tInterface.COM_Type)					//USB+S
	{
		NVIC_EnableIRQ(USART_IRQ_NUMBER);
	}	
	  if ((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))					 //BT 2016.07.29 
	{
		NVIC_EnableIRQ(BT_UART_IRQn);
	}	

}

/*******************************************************************************
* Function Name  : void DisableReceiveInterrupt(void)
* Description    : 关闭或打开接收的中断
* Input          : NewState：DISABLE 关闭，ENABLE 打开
* Output         : None
* Return         : None
*******************************************************************************/
void	DisableReceiveInterrupt(void)
{

	if (g_tInterface.USB_Type)					//USB
	{
		NVIC_DisableIRQ(OTG_FS_IRQn);
	}
	if (g_tInterface.COM_Type)					//S
	{
		NVIC_DisableIRQ(USART_IRQ_NUMBER);
	}	
		if ((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))					 //BT 2016.07.29 
	{
		NVIC_DisableIRQ(BT_UART_IRQn);
	}	
	//2016.06.03 以后需增加蓝牙串口的相应开关
}

/*******************************************************************************
* Function Name  : void ReceiveInterrupt(FunctionalState NewState)
* Description    : 关闭或打开接收的中断
* Input          : NewState：DISABLE 关闭，ENABLE 打开
* Output         : None
* Return         : None
*******************************************************************************/
void	ReceiveInterrupt(FunctionalState NewState)
{
	if (NewState == DISABLE)
	{
		if (g_tInterface.COM_Type)							//串口
		{
			NVIC_DisableIRQ(USART_IRQ_NUMBER);
		}
		if (g_tInterface.LPT_Type)							//并口
		{
		#ifdef	STM32F10X_HD
			NVIC_DisableIRQ(PARALLEL_STB_INT_IRQn);
		#endif
		}
		if (g_tInterface.USB_Type)							//USB
		{
		#ifdef STM32F10X_CL
			NVIC_DisableIRQ(OTG_FS_IRQn);
		#else
			NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
		#endif
		}
	}
	else
	{
		if (g_tInterface.COM_Type)
		{
			NVIC_EnableIRQ(USART_IRQ_NUMBER);
		}
		if (g_tInterface.LPT_Type)							//并口
		{
		#ifdef	STM32F10X_HD
			NVIC_EnableIRQ(PARALLEL_STB_INT_IRQn);
		#endif
		}
		if (g_tInterface.USB_Type)
		{
		#ifdef STM32F10X_CL
			NVIC_EnableIRQ(OTG_FS_IRQn);
		#else
			NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
		#endif
		}
	}
}

/*******************************************************************************
* Function Name  : void SetActiveComInBusy(void)
* Description    : 清除活跃的通信端口忙
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	SetActiveComInBusy(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)	//串口
	{
		SetUsartBusy();					//置忙
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)	//并口
	{
		SetParallelBusy();			//并口置忙
	}
// 	else if (g_tSystemCtrl.PrtSourceIndex == USB)			//USB
// 	{
// 		Set_USB_Busy();					//不允许USB接收下帧数据
// 	}
	else
	{
	}
}

/*******************************************************************************
* Function Name  : void ClearActiveComInBusy(void)
* Description    : 清除活跃的通信端口忙
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	ClearActiveComInBusy(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)	//串口
	{
		ClearUsartBusy();				//清忙
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)	//并口
	{
		LPT_ACK_Out();					//并口清忙
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)			//USB
	{
		USB_ClearBusy();				//允许USB接收下帧数据
	}
	else
	{
	}
}

/*******************************************************************************
* Function Name  : void EnableActiveReceiveInterrupt(void)
* Description    : 打开活跃的接收的中断
* Input          : NewState：DISABLE 关闭，ENABLE 打开
* Output         : None
* Return         : None
*******************************************************************************/
void	EnableActiveReceiveInterrupt(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)
	{
	#ifdef	STM32F10X_CL
		NVIC_EnableIRQ(UART4_IRQn);
	#else
		NVIC_EnableIRQ(USART_IRQ_NUMBER);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)		//并口
	{
	#ifdef	STM32F10X_HD
		NVIC_EnableIRQ(PARALLEL_STB_INT_IRQn);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)
	{
	#ifdef STM32F10X_CL
		NVIC_EnableIRQ(OTG_FS_IRQn);
	#else
		NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	#endif
	}
	else
	{
	}
}

/*******************************************************************************
* Function Name  : void DisableActiveReceiveInterrupt(void)
* Description    : 关闭活跃的接收的中断
* Input          : NewState：DISABLE 关闭，ENABLE 打开
* Output         : None
* Return         : None
*******************************************************************************/
void	DisableActiveReceiveInterrupt(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)	//串口
	{
	#ifdef	STM32F10X_CL
		NVIC_DisableIRQ(UART4_IRQn);
	#else
		NVIC_DisableIRQ(USART_IRQ_NUMBER);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)	//并口
	{
	#ifdef	STM32F10X_HD
		NVIC_DisableIRQ(PARALLEL_STB_INT_IRQn);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)			//USB
	{
	#ifdef STM32F10X_CL
		NVIC_DisableIRQ(OTG_FS_IRQn);
	#else
		NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	#endif
	}
	else
	{
	}
}

/*******************************************************************************
* Function Name : void	InitInBuf(void)
* Description		: 初始化接收缓冲区
* Input	      	: 
* Output	     	: 操作 g_tInBuf 全局变量结构
* Return	     	: None
*******************************************************************************/
void	InitInBuf(void)
{
	g_tInBuf.BytesNumber = 0;
	g_tInBuf.PutPosition = 0;
	g_tInBuf.GetPosition = 0;
	g_tInBuf.PE_BytesNumber = 0;
	g_tInBuf.PE_GetPosition = 0;
	g_tInBuf.BufFullFlag = COM_IN_BUF_NOT_FULL;
}

/*******************************************************************************
* Function Name : 
* Description		: 获取接收缓冲区数据个数
* Input	      	: 
* Output	     	: 操作 g_tInBuf 全局变量结构
* Return	     	: 
*******************************************************************************/
uint32_t	GetInBufBytesNumber(void)
{
	uint32_t Temp1;								//2016.07.07
	Temp1 = g_tInBuf.PutPosition;	//中断中可能改变其值
	if(Temp1 >= g_tInBuf.GetPosition)
		Temp1 = Temp1 - g_tInBuf.GetPosition;
	else
		Temp1 =(g_tInBuf.BytesSize - g_tInBuf.GetPosition) + Temp1;
	g_tInBuf.BytesNumber = Temp1;
	return	(g_tInBuf.BytesNumber);			
}

#ifdef	PAPER_OUT_RE_PRINT_ENABLE
extern	void	EndHeat(void);
extern	void	InitHeatCtrlStruct(void);
extern	void	CutMotorStandby(void);
extern	void	InitCharMaskBufParameter(void);
extern	void	PrepareNewLine(void);
extern	void	InitLinePrtBuf(void);

/*******************************************************************************
* Function Name  : ClearInBuf(void)
* Description    : 2016.10.08
                   执行切刀后把当前的读指针作为下一单的起始放到g_tInBuf.PE_BytesNumber中
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClearInBuf(void)
{
	//SetComInBufBusy();
	DisableReceiveInterrupt();				//关接收中断
	//g_tInBuf.PE_BytesNumber = g_tInBuf.BytesNumber;
	g_tInBuf.PE_GetPosition = g_tInBuf.GetPosition;	//修改取数指针
	EnableReceiveInterrupt();					//开接收中断
}

/*******************************************************************************
* Function Name  : void PaperOutDetect(void)
* Description    : 缺纸自动重新打印处理.如果缺纸且当前单据接收的数据不满缓冲区,
*	准备重新打印.如果当前单据接收的数据大于满缓冲区,则不重新打印.
把读数指针移到一单的开始位置，情况编辑缓冲区和打印缓冲区，清各种标志位等
*	PE_BytesNumber统计本单据的字节量.PE_GetPosition存放本单据处理开始地址.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	PaperOutDetect(void)
{
	static uint8_t	PaperEndNum = 0;
	static uint8_t	PaperEndFlag = 0x55;
	
	if (g_tSysConfig.PaperOutReprint)
	{
		if ((g_tError.PaperSensorStatusFlag & 0x01) && \
				(g_tSystemCtrl.PaperType == 0))				
		{
			if(PaperEndFlag == 0x55)
			{	
				PaperEndNum++;
			}	
		}
		else
		{
			PaperEndNum = 0;
			PaperEndFlag = 0x55;
		}	
    if( PaperEndNum >50)
    {			
			PaperEndNum = 0;
			PaperEndFlag = 0xAA;
			//SetComInBufBusy();
			DisableReceiveInterrupt();			//关接收中断
			//g_tInBuf.BytesNumber = g_tInBuf.PE_BytesNumber;	 //修改数据字节数
			g_tInBuf.GetPosition = g_tInBuf.PE_GetPosition;	   //修改取数指针
			EnableReceiveInterrupt();				//开接收中断
			GetInBufBytesNumber();
// 			if (g_tInBuf.BytesNumber > (g_tInBuf.BytesSize - MIN_RSV_BYTES_OF_IN_BUF))
// 			{	//缓冲区小于最小保留区
// 				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			     //缓冲区满标志
// 			}
// 			if (g_tInBuf.BufFullFlag == COM_IN_BUF_NOT_FULL)   //需增加网口清忙
// 			{
// 				ClearComInBufBusy();
// 			}
			
			//清空打印缓冲区和编辑缓冲区,丢弃当前的打印内容
			TIM_Cmd(HEAT_TIMER, DISABLE);					//关定时器3
			TIM_Cmd(DealPrtData_TIMER, DISABLE);  //关定时器5
			EndHeat();										//关加热
			EndFeedMotor();								//关上纸电机
			#if defined (CUTTER_ENABLE)
				CutMotorStandby();					//关切刀电机
			#endif
			InitHeatCtrlStruct();					//初始化加热控制结构
			InitCharMaskBufParameter();		//清空字模读取缓冲区,字符缓冲区标志改为ILDE
			PrepareNewLine();
			InitLinePrtBuf();
			g_tSystemCtrl.StepNumber = 0;
			g_bMotorRunFlag = 0;
			//一直等待重新装纸且关闭纸仓盖才退出
			//while ((g_tError.PaperSensorStatusFlag & 0x01) || (g_tError.AR_ErrorFlag & 0x02));
		}
	}
}
#endif

/*******************************************************************************
* Function Name : static void FillRealTimeCommand(uint8_t Data)
* Description		: 填充实时指令
* Input	      	: 字节形式的接收数据
* Output	     	: 操作 g_tRealTimeCmd 全局变量结构
* Return	     	: None
*******************************************************************************/
static void FillRealTimeCommand(uint8_t Data)
{
	static uint8_t CommandLength = 0;
	
	//----实时指令数据的填充控制----//
	if (g_tRealTimeCmd.RealTimeCntr == 0)
	{
		#ifdef	DBG_REALTIME_1B76_ENABLE
	 	if ((Data == 0x10) || (Data == 0x1B))
		#else
		if (Data == 0x10)
		#endif
		{
			g_tRealTimeCmd.RealTimeCntr = 1;			//实时指令数据计数器
			g_tRealTimeCmd.RealTimeBuf[0] = Data;
		}
	}
	else if (g_tRealTimeCmd.RealTimeCntr == 1)
	{
		#ifdef	DBG_REALTIME_1B76_ENABLE
		if ((Data == 0x04) || (Data == 0x05) || (Data == 0x06) || (Data == 0x14) || (Data == 0x76))
		#else
		if ((Data == 0x04) || (Data == 0x05) || (Data == 0x06) || (Data == 0x14))		//2016.08.02增加1006
		#endif
		{
			g_tRealTimeCmd.RealTimeCntr = 2;			//实时指令数据计数器
			g_tRealTimeCmd.RealTimeBuf[1] = Data;
			#ifdef	DBG_REALTIME_1B76_ENABLE
			if (g_tRealTimeCmd.RealTimeBuf[1] == 0x76)	//1B 76
			{
				g_tRealTimeCmd.RealTimeFlag = 1;
			}
			#endif
		}
		else
		{
			g_tRealTimeCmd.RealTimeCntr = 0;			//实时指令数据计数器
		}
	}
	else if (g_tRealTimeCmd.RealTimeCntr == 2)
	{
		if ((Data > 0x00) && (Data < 0x08))
		{
			g_tRealTimeCmd.RealTimeCntr = 3;			//实时指令数据计数器
			g_tRealTimeCmd.RealTimeBuf[2] = Data;
			if ((g_tRealTimeCmd.RealTimeBuf[1] == 4) || (g_tRealTimeCmd.RealTimeBuf[1] == 5)|| (g_tRealTimeCmd.RealTimeBuf[1] == 6))	//1004, 1005 ，1006
			{
				g_tRealTimeCmd.RealTimeFlag = 1;
			}
			else 		//10 14
			{
				if (Data < 0x03)
				{
					CommandLength =	g_tRealTimeCmd.RealTimeCntr + 2;
				}
				else if (Data == 0x08)
				{
					CommandLength =	g_tRealTimeCmd.RealTimeCntr + 7;
				}
				else
				{
					g_tRealTimeCmd.RealTimeCntr = 0;
				}
			}
		}
		else
		{
			g_tRealTimeCmd.RealTimeCntr = 0;			//实时指令数据计数器
		}
	}
	else if ((g_tRealTimeCmd.RealTimeCntr < CommandLength) && \
					 (g_tRealTimeCmd.RealTimeFlag != 1)) //10 14指令
	{
		g_tRealTimeCmd.RealTimeBuf[g_tRealTimeCmd.RealTimeCntr] = Data;
		g_tRealTimeCmd.RealTimeCntr++;
		if (g_tRealTimeCmd.RealTimeCntr == CommandLength)
		{
			g_tRealTimeCmd.RealTimeFlag = 1;
			CommandLength = 0;
		}
	}
}

/*******************************************************************************
* Function Name  : void	PutNetCharToBuffer(uint8_t Data);
* Description		: 将接收的一字节数据存入到输入缓冲区内
* Input	      	: 字节形式的接收数据
* Output	     	: 操作 g_tInBuf 全局变量结构
* Return	     	: None
*******************************************************************************/
#ifdef	DBG_RCV_DATA_LOSE
volatile	uint32_t	g_uiTotalRcvByte = 0;
volatile	uint32_t	g_uiTotalReadByte = 0;
#endif

uint8_t PutNetCharToBuffer(uint8_t *Bufer, uint16_t Length, uint8_t Port)//06
{
	uint32_t Temp;
	uint16_t i;
	uint8_t	Data;
	
	g_tSystemCtrl.PrtSourceIndex = Port;			//设置数据源
	g_tSystemCtrl.SleepCounter=0;	 			//清进入休眠计数

	for(i=0; i< Length; i++)
	{
		Data = Bufer[i];
		g_tInBuf.Buf[g_tInBuf.PutPosition] = Data;				//存入当前字符
		g_tInBuf.PutPosition++;														//更新头指针内容
		if (g_tInBuf.PutPosition >= g_tInBuf.BytesSize)	  //到达缓冲区最后位置
		{
			g_tInBuf.PutPosition = 0;												//重新从0开始计数
		}
		//g_tInBuf.BytesNumber++;														//修改数据字节数 2016.07.28
		
		FillRealTimeCommand(Bufer[i]);									  //填充实时指令

		
		if ((g_tRealTimeCmd.RealTimeEnable == 1) && \
					(g_tRealTimeCmd.RealTimeFlag == 1))
		{
			RealTimeCommand();	//实时指令
		}
	}

  
	//缺纸重打印情况下,PutPosition和PE_GetPosition之间差值小于MIN_RSV_BYTES_OF_IN_BUF则致忙
	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
// 		if (g_tSysConfig.PaperOutReprint)
// 		{
// 			if(g_tInBuf.PutPosition >= g_tInBuf.PE_GetPosition )
// 				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.PE_GetPosition;
// 			else
// 				Temp = g_tInBuf.PE_GetPosition - g_tInBuf.PutPosition;

// 			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE大于等于USB、以太网的接收包，是为了保留一包数据
// 				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//缓冲区满		
// 		}		
// 		else		//正常情况
		{
				//计算缓冲区剩余空间
			if(g_tInBuf.PutPosition >= g_tInBuf.GetPosition )
				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.GetPosition;
			else
				Temp = g_tInBuf.GetPosition - g_tInBuf.PutPosition;

			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE大于等于USB、以太网的接收包，是为了保留一包数据
				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//缓冲区满
		}	
	#endif
		
  return g_tInBuf.BufFullFlag;
}
/*******************************************************************************
* Function Name  : void	PutCharToInBuffer(uint8_t Data);
* Description		: 将接收的一字节数据存入到输入缓冲区内
* Input	      	: 字节形式的接收数据
* Output	     	: 操作 g_tInBuf 全局变量结构
* Return	     	: None
*******************************************************************************/

void PutCharToInBuffer(uint8_t Data)
{
	uint32_t Temp;
	
	g_tSystemCtrl.SleepCounter = 0;                    //2016.06.11
// 	if (g_tInBuf.BytesNumber < g_tInBuf.BytesSize)			//接收总数未超过极限
	{
		g_tInBuf.Buf[g_tInBuf.PutPosition] = Data;				//存入当前字符
		g_tInBuf.PutPosition++;														//更新头指针内容
		if (g_tInBuf.PutPosition >= g_tInBuf.BytesSize)	//到达缓冲区最后位置
		{
			g_tInBuf.PutPosition = 0;												//重新从0开始计数
		}
			
 		//if (g_tInBuf.BytesNumber < g_tInBuf.BytesSize)
		//g_tInBuf.BytesNumber++;														//修改数据字节数 2016.07.28
		#ifdef	DBG_RCV_DATA_LOSE
		g_uiTotalRcvByte++;
		#endif		

	  //缺纸重打印情况下,PutPosition和PE_GetPosition之间差值小于MIN_RSV_BYTES_OF_IN_BUF则致忙
  	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
// 		if (g_tSysConfig.PaperOutReprint)
// 		{
// 			if(g_tInBuf.PutPosition >= g_tInBuf.PE_GetPosition )
// 				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.PE_GetPosition;
// 			else
// 				Temp = g_tInBuf.PE_GetPosition - g_tInBuf.PutPosition;

// 			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE大于等于USB、以太网的接收包，是为了保留一包数据
// 				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//缓冲区满		
// 		}		
// 		else		//正常情况
		{		
			if(g_tInBuf.PutPosition >= g_tInBuf.GetPosition )			//2016.07.28 计算剩余空间
				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.GetPosition;
			else
				Temp = g_tInBuf.GetPosition - g_tInBuf.PutPosition;

			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE大于等于USB、以太网的接收包，是为了保留一包数据
			{	
				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//缓冲区满
			}
		}		
		#endif
		
		if(g_tPrtCtrlData.PCDriverMode != 0)			  //2016.06.11 驱动模式不处理实时指令
		{
			return;
		}	
		
		FillRealTimeCommand(Data);									//填充实时指令
	}
// 	else
// 	{
// 		g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//缓冲区满标志
// 	}
    
        
	if ((g_tRealTimeCmd.RealTimeEnable == 1) && \
			(g_tRealTimeCmd.RealTimeFlag == 1))
	{
		RealTimeCommand();	//实时指令
	}
}

/*******************************************************************************
* Function Name  : void ReadInBuffer(void)
* Description    : 从输入缓冲区读取一节，如果缓冲区中没有数据，则等待并自动进入休眠。
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
#define	DRV_PRT_COM_MIN_DELAY_MS		(1)
#define	DRV_PRT_MIN_DELAY_MS				(1)

uint8_t	ReadInBuffer(void)
{
	#ifdef	DRIVE_PRT_MODE_RX_DELAY_ENABLE
		static uint8_t DriverFullFlag = 0;
	#endif
	uint32_t Temp;
	static uint8_t ReadWaitFlag = 0;			//2016.05.23  
	uint8_t	GetChar;
	uint16_t i;
	uint8_t Bps;
	
	if (g_tMacro.RunFlag)		//执行宏时从宏缓冲区取数
	{
		if (g_tMacro.GetPointer < g_tMacro.Number)
		{
			GetChar = g_tMacro.Buffer[g_tMacro.GetPointer];
			g_tMacro.GetPointer++;
		}
		else
		{
			GetChar = 0;
		}
		return	(GetChar);
	}
	
	while (1)
	{
		if (g_tInBuf.PutPosition == g_tInBuf.GetPosition)	//没有数据 2016.07.07
  	{
			#ifdef	DRIVE_PRT_MODE_RX_DELAY_ENABLE
				DriverFullFlag = 0;
			#endif
			CheckFeedButton();
			if(g_tInterface.ETH_Type) 
			{	
				GetETH_ConnectState();
			}	
			if (g_tSystemCtrl.SleepCounter > SLEEP_TIME_MS)	//休眠定时时间到
			{
				g_tSystemCtrl.SleepCounter = 0;
				#ifndef DEBUG
//					EnterLowPower();	//进入休眠
				#endif
			}
			ReadWaitFlag = 0;	//2016.05.23			
			if(NetReloadCounter>65000)           //2018.12.10 超时1分钟 复位网口
			{																		 
				ETH_ConnectState=0;
				if(g_tInterface.ETH_Type) 
				{
					uIPMain();    //初始化网口
        }	               
				NetReloadCounter = 0;
			}	
			//语音返回
			if(g_tSpeaker.SpeakWriteHandle == WRITE_END)
				return 0xFF;
		}			//缓冲区无数处理结束
		else	//缓冲区有数据
		{
			//#ifdef	DRIVE_PRT_MODE_RX_DELAY_ENABLE
			#if	0
			if ((g_tSystemCtrl.PrtSourceIndex == COM) && \
					(g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE) && \
					(DriverFullFlag == 0))	//驱动模式且缓冲区不满
			{
				while (1)
				{	//等待缓冲区满或最后超时
					if ((g_tInBuf.BufFullFlag == COM_IN_BUF_FULL) || \
							(g_tSystemCtrl.SleepCounter > DRV_PRT_MIN_DELAY_MS) || \
							(g_tInBuf.BytesNumber > 576)
						)
					{
						DriverFullFlag = 1;
						break;
					}
				}
			}
			#endif
			
			if(ReadWaitFlag ==0 )	
			{
				MaxSetp();									//接口类型初始化最大加速步数 2016.06.04
				ReadWaitFlag = 1;
				
				//当串口在缓冲区没数时，开始接收数据时，等待2.0秒
				if(g_tSystemCtrl.PrtSourceIndex == COM ) //2016.06.08 COM 
				{
// 					if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)			//驱动模式
					{
						//等待，如果没有数据，则退出等待
						for(i=0; i<3000; i++)					
						{
							DelayMs(1);					//延时不准 大概时间 
							if(g_tSystemCtrl.SleepCounter >30)	//30ms没有收到数退出  2016.05.23
								break;
						}
					}	
				}
			}
			
			g_tSystemCtrl.SleepCounter = 0;								//复位休眠计数器
						
			GetChar = g_tInBuf.Buf[g_tInBuf.GetPosition];	//取数
			
 			DisableReceiveInterrupt();										//关接收中断  2016.07.28 只开放对usb的操作
			g_tInBuf.GetPosition++;												//更新取数指针
			if (g_tInBuf.GetPosition >= g_tInBuf.BytesSize)
			{
				g_tInBuf.GetPosition = 0;										//到达最大值时重新从0开始
			}
// 			
// // 	g_tInBuf.BytesNumber--;												//修改数据字节数
// 			
 			EnableReceiveInterrupt();											//开接收中断
			
			#ifdef	DBG_RCV_DATA_LOSE
				g_uiTotalReadByte++;
			#endif
			
			if (g_tInBuf.BufFullFlag == COM_IN_BUF_FULL) //2016.07.08
			{				
// 				if (g_tSysConfig.PaperOutReprint)						//2016.10.09 缺纸重打印情况
// 				{
// 					Temp = g_tInBuf.PutPosition;	//中断中可能改变其值
// 				  if(Temp >= g_tInBuf.PE_GetPosition)
// 						Temp =(g_tInBuf.BytesSize - Temp) + g_tInBuf.PE_GetPosition;
// 					else
// 						Temp =g_tInBuf.PE_GetPosition - Temp;
// 				}		
// 				else		//正常情况
				{		
					Temp = g_tInBuf.PutPosition;	//中断中可能改变其值
				  if(Temp >= g_tInBuf.GetPosition)
						Temp =(g_tInBuf.BytesSize - Temp) + g_tInBuf.GetPosition;
					else
						Temp =g_tInBuf.GetPosition - Temp;
				}		
			
				if( Temp > MAX_RSV_BYTES_OF_IN_BUF)	
				{										  
					g_tInBuf.BufFullFlag = COM_IN_BUF_NOT_FULL;	//缓冲区不满
					ClearComInBufBusy();					
				}
			}
			
			if ((g_tMacro.DefinitionFlag == 1) && (g_tMacro.Number < MACRO_SIZE))	//正执行宏定义
		 	{
				g_tMacro.Buffer[g_tMacro.Number] = GetChar;	//宏数据存入宏缓冲区
				g_tMacro.Number++;
			}
			g_tSpeaker.SpeakWriteTime = 0;  //语音flash计数器
			return (GetChar);
		}		//缓冲区有数处理结束
	}		//处理读数结束
}

/*******************************************************************************
* Function Name  : DecInBufferGetPoint
* Description    : 输入缓冲区取数指针减1，目的是为了将之前读取的数据重新放回
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DecInBufferGetPoint(void)
{
#if	1
// 	if (g_tMacro.RunFlag == 0)				//不是宏执行定义 2016.07.07
// 	{
// 		if (g_tInBuf.GetPosition == 0)
// 		{
// 			g_tInBuf.GetPosition = (g_tInBuf.BytesSize - 1);
// 		}
// 		else
// 		{
// 			g_tInBuf.GetPosition--;
// 		}

// 		DisableReceiveInterrupt();			//关接收中断
// 		g_tInBuf.BytesNumber++;			  	//修改数据字节数
// 		EnableReceiveInterrupt();				//开接收中断
// 		#ifdef	DBG_RCV_DATA_LOSE
// 			g_uiTotalReadByte--;
// 		#endif
// 		
// 		if (g_tInBuf.BufFullFlag == COM_IN_BUF_NOT_FULL)
// 		{
// 			ClearComInBufBusy();
// 		}
// 	}
// 	
// 	if (g_tMacro.DefinitionFlag == 1)	//宏定义中
// 	{
// 		g_tMacro.Number--;							//除去读入宏缓冲的数
// 	}
#endif
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
