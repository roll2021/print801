/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   指令相关程序.
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
#include	"monobyte.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"feedpaper.h"
#include	"charmaskbuf.h"
#include	"timconfig.h"
#include	"heat.h"
#include	"button.h"
#include	"led.h"
#include	"spiheat.h"
#include	"parallel.h"
#include	"drawer.h"

extern TypeDef_StructInBuffer volatile g_tInBuf;

extern void PrtDrvBufFull(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/

/*******************************************************************************
* Function Name  : Command_09
* Description    : 跳格处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_09(void)
{
	uint16_t	TempNum;
	
	if (g_tTab.TabIndex >= MAX_HT_INDEX_NUM) 	  //防止下标越界
	{
		return;
	}
	
	//如果当前位置在 打印宽度+1, 打印当前缓冲区并将当前位置移动到行首
	if (g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLength)
	{
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
		
	while(g_tTab.TabIndex < g_tTab.TabSetNumber)	//有设置的跳格, 没有设置跳格不处理
	{
    TempNum = g_tTab.TabPositionIndex[ g_tTab.TabIndex ] + g_tPrtCtrlData.PrtLeftLimit;
		//下一个跳格位置是否大于当前位置
		if (TempNum <= g_tPrtCtrlData.PrtDataDotIndex)
		{ //下一个跳格位置超越可打印区域, 将当前位置设置为 打印宽度+1
			g_tTab.TabIndex++;	//执行跳格数＋1
		}
		else
		{
			if (TempNum >= g_tPrtCtrlData.PrtLength)
			{
				g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
			}
			else
			{
				g_tPrtCtrlData.PrtDataDotIndex = TempNum;
			}
			break;
		}	
	}
	
}

/*******************************************************************************
* Function Name  : Command_0A
* Description    : 换行命令处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_0A(void)
{
	PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);	//打印当前行 2016.06.28
}

/*******************************************************************************
* Function Name  : Command_0C
* Description    : 打印页,该命令只在页模式下有效,为兼容T3,在标准模式下为走纸到黑标处
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_0C(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)		//兼容T3找黑标
	{
// 		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);  2016.11.17 去掉打印只是找黑标
		if ((g_tSysConfig.BlackMarkEnable))	//找黑标开启后		
		{			
			while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);  //2016.06.30			
			FindMarkPoint();
		}
	}
	else
	{
		
	}
}

/*******************************************************************************
* Function Name  : Command_0D
* Description    : 换行命令处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_0D(void)
{	//如果使能以CR指令,并且不是串口方式
	if ((g_tSysConfig.CREnable) && (g_tSystemCtrl.PrtSourceIndex != COM))
	{
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
}

/*******************************************************************************
* Function Name  : Command_15
* Description    : 驱动程序用,走纸n点行
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_15(void)
{
	uint32_t GetChar;
	uint32_t DotLineNumber;
	uint32_t i;
	
	GetChar = ReadInBuffer();	
	if (GetChar > 1)
	{	
		DotLineNumber = GetChar - 1;
		for (i = 0; i < DotLineNumber; i++)
		{
			PrtDrvBufFull();
			IncDrvPrtBufAddr(0x02, 0);
		}
	}
}

extern	uint8_t	g_tbHeatElementBuf[];	//加热机芯一点行所有的加热寄存器单元数
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	uint32_t	g_iDealOneLineTimeCount;

/*******************************************************************************
* Function Name  : Command_16
* Description    : 驱动程序用,接收1点行数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	ONE_LINE_BYTES		(PRT_DOT_NUMBERS / 8)
void Command_16(void)
{
	volatile uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT]={0};
	uint8_t	Width, Number;
	uint8_t	* pSrc = NULL;
	uint32_t	i;
	uint8_t GetChar;	
	
	#if defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	#endif
	
	Number = g_tPrtCtrlData.DriverOffset + ReadInBuffer();
	if (Number == 0)
	{
		return;
	}
	
	if (Number > (g_tSystemCtrl.LineWidth >> 3))
	{
		Width = (g_tSystemCtrl.LineWidth >> 3);
	}
	else
	{
		Width = Number;
	}
	
	memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);	//2016.06.21
	pSrc = DataBuf + g_tPrtCtrlData.DriverOffset;
	for (i = g_tPrtCtrlData.DriverOffset; i < Width; i++)
	{
		GetChar = ReadInBuffer();	//读取本行的待打印数据
		*pSrc++ = GetChar;
	}

// 	CopyDotLineDataToHeatElementBuf(DataBuf);	//复制到加热控制缓冲区  2016.06.16驱动不用此方式	
// 	PrintOneDotLine();					//打印一点行
	DotLinePutDrvBuf( DataBuf );				//2016.06.16 送一行数据到驱动打印缓冲区
	
	Command16RunFlag = 0;         //2016.06.15  16指令运行结束标志  
	
	for (; i < Number; i++)			  //丢弃超过一行的数据
	{	
		ReadInBuffer();
	}
	
	#if defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
		StopTimer(DBG_TIMER);
	#endif
}

/*******************************************************************************
* Function Name  : Command_18
* Description    : 页模式下,清除打印缓冲区内容,该命令只在页模式下有效
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_18(void)
{
	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)	
	{
	
	}
}

/*******************************************************************************
* Function Name  : MonoByte_Command
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void MonoByte_Command(uint8_t CommandCode)
{
	switch (CommandCode)
	{
		case 0x09:				//09命令,执行横向跳格
		{
			Command_09();
			break;
		}
		case 0x0A:				//0A命令,换行
		{
			Command_0A();
			break;
		}
		case 0x0C:
		{
			Command_0C();		//0C命令,页模式下,打印页
			break;
		}
		case 0x0D:				//0D命令,回车
		{
			Command_0D();
			break;
		}
		case 0x15:				//15命令,驱动走纸点行
		{
			Command_15();
			break;
		}
		case 0x16:				//16命令,驱动接收一点行数据
		{
			Command_16();
			break;
		}
		case 0x18:				//18命令,页模式下清除打印缓冲区内容
		{
			Command_18();
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : Command_1004
* Description    : 实时状态返回
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	DEFAULT_CMD_1004_STATUS_VALUE		(0x12)

void Command_1004(void)
{
	if ((g_tRealTimeCmd.RealTimeBuf[2] < 1) || (g_tRealTimeCmd.RealTimeBuf[2] > 4))
		return;
	
	//得到纸传感器状态	10 04 n=4
	g_tRealTimeStatus.PaperStatus = DEFAULT_CMD_1004_STATUS_VALUE;	//固定值
	if (g_tError.PaperSensorStatusFlag & 0x01)		//缺纸
		g_tRealTimeStatus.PaperStatus |= 96;
	if (g_tError.PaperSensorStatusFlag & 0x02)  	//纸将尽
		g_tRealTimeStatus.PaperStatus |=	12;
	
	//得到错误状态	10 04 n=3
	g_tRealTimeStatus.ErrorStatus = DEFAULT_CMD_1004_STATUS_VALUE;
	if (g_tError.R_ErrorFlag & 0x02)				//机械错误(机头原位)
		g_tRealTimeStatus.ErrorStatus |= 4;
	if (g_tError.R_ErrorFlag & 0x01)				//切刀位置错误
		g_tRealTimeStatus.ErrorStatus |= 8;
	if (g_tError.UR_ErrorFlag)							//不可恢复错误
		g_tRealTimeStatus.ErrorStatus |= 32;
	if (g_tError.AR_ErrorFlag)							//可恢复错误
		g_tRealTimeStatus.ErrorStatus |= 64;
	
	//得到离线状态	10 04 n=2
	g_tRealTimeStatus.OffLineStatus = DEFAULT_CMD_1004_STATUS_VALUE;
	if (g_tError.DoorOpen)			//上盖  测试真正的开盖状态 2016.08.04
		g_tRealTimeStatus.OffLineStatus |= 4;
	//按下走纸键因而正在走纸
// #if defined	(FEED_BUTTON_CHECK_BY_INT)
// 	if (g_tFeedButton.KeyStatus)
// #else
// 	if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())
// #endif
	{
		if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())		        //2016.08.04
			g_tRealTimeStatus.OffLineStatus |= 8;
	}
	if (g_tRealTimeStatus.PaperStatus != DEFAULT_CMD_1004_STATUS_VALUE)	//缺纸停止
		g_tRealTimeStatus.OffLineStatus |= 32;
	if (g_tRealTimeStatus.ErrorStatus != DEFAULT_CMD_1004_STATUS_VALUE)		//错误
		g_tRealTimeStatus.OffLineStatus |= 64;
	
	//得到打印机状态  10 04 n=1
	g_tRealTimeStatus.PrinterStatus = DEFAULT_CMD_1004_STATUS_VALUE;

	if (READ_MONEYBOX_STATUS)	                          //钱箱关闭,读取返回电平为高  2016.08.02 
		g_tRealTimeStatus.PrinterStatus |= 4;	            //钱箱开/关信号为高(钱箱插头的第三引脚),表示钱箱关闭
  else
		g_tRealTimeStatus.PrinterStatus &= ~4;
	if (g_tRealTimeStatus.OffLineStatus != DEFAULT_CMD_1004_STATUS_VALUE)	//离线
		g_tRealTimeStatus.PrinterStatus |= 8;
	else
		g_tRealTimeStatus.PrinterStatus &= ~8;
	
// 	//按下走纸键因而正在走纸
// #if defined	(FEED_BUTTON_CHECK_BY_INT)
// 	if (g_tFeedButton.KeyStatus)
// #else
// 	if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())
// #endif
// 	{
// 		if (g_tSystemCtrl.StepNumber)
// 			g_tRealTimeStatus.PrinterStatus |= 64;
// 	}
	
	switch (g_tRealTimeCmd.RealTimeBuf[2])
	{
	 	case 1:
		{
			UpLoadData(&g_tRealTimeStatus.PrinterStatus, 1);	//上传数据
			break;
		}
	 	case 2:
		{
			UpLoadData(&g_tRealTimeStatus.OffLineStatus, 1);	//上传数据
			break;
		}
	 	case 3:
		{
			UpLoadData(&g_tRealTimeStatus.ErrorStatus, 1);		//上传数据
			break;
		}
	 	case 4:
		{
			UpLoadData(&g_tRealTimeStatus.PaperStatus, 1);		//上传数据
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : Command_1005n
* Description    : 给打印机实时请求,将打印机从错误状态恢复并且继续被中断的打印,
*				   除非出现了可恢复错误,否则该指令被忽略。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1005n(void)
{
	if (!g_tError.R_ErrorFlag)		//有可恢复错误才执行该指令
		return;
// 	if (g_tSystemCtrl.PrtSourceIndex == LPT)
// 	{
// 		if (g_tInBuf.BufFullFlag == COM_IN_BUF_FULL)	//并口机型忙时不立即执行
// 			return;
// 	}
	//其他机型立即执行
	//将打印机从错误状态恢复并且继续被中断的打印,除非出现了可恢复错误,否则该指令被忽略
	if (g_tRealTimeCmd.RealTimeBuf[2] == 1)
	{
		g_tError.R_ErrorRecoversFlag = 1;
	}
	//将打印机从错误状态恢复并且清除命令接收缓冲区和打印缓冲区,除非出现了可恢复错误,否则该指令被忽略
	else if (g_tRealTimeCmd.RealTimeBuf[2] == 2)
	{
		//清指令接收缓冲区
		//ReceiveInterrupt(DISABLE);	//关接收中断
		DisableReceiveInterrupt();
		g_tInBuf.BytesNumber = 0;		//接收缓冲区字节数
		g_tInBuf.PutPosition = 0;		//接收缓冲区
		g_tInBuf.GetPosition = 0;		//接收缓冲区
		g_tInBuf.PE_BytesNumber = 0;
		g_tInBuf.PE_GetPosition = 0;
		g_tInBuf.BufFullFlag = COM_IN_BUF_NOT_FULL;
		
		//清打印缓冲区
		PrepareNewLine();								//清空g_tLineEditBuf及下划线等控制变量准备下一行编辑			
		InitLinePrtBuf();								//清空打印缓冲区
		
		TIM_Cmd(FEED_TIMER, DISABLE);		//关闭加热控制定时器
		TIM_Cmd(FEED_TIMER, DISABLE);		//关闭电机控制定时器
		TIM_Cmd(PRINT_CHECK_TIMER, DISABLE);	//关闭打印检查控制定时器
		EndHeat();											//关加热控制
		EndFeedMotor();									//关闭电机
		g_bFeedTaskFlag = 0;
		g_tError.R_ErrorRecoversFlag = 2;
		
		//ReceiveInterrupt(ENABLE);		//最后开接收中断
		EnableReceiveInterrupt();
	}
	g_tError.R_ErrorFlag =0;				//
}

/*******************************************************************************
* Function Name  : Command_1006n
* Description    : 打印机实时请求，读取打印机执行状态 包括切刀执行情况 是否打印内容等。
*				           当缓冲区有数据时标志位置位，当切刀执行完成后标志位清零
* Input          : None
* Output         : None
* Return         : 当前有错误                0001 0000  = 0x10
                   当前单据打印完成并切刀    0000 1000  = 0x08
                   初始状态　　　　          0000 0001  = 0x01
*******************************************************************************/
void Command_1006n(void)	
{
	  uint8_t ErrTemp;
    if(g_tRealTimeCmd.RealTimeBuf[2] == 1)
    {       
			  if(g_tError.DoorOpen |g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag |(g_tError.PaperSensorStatusFlag & 0x01) )  //2018.02.08  增加缺纸
				{
					g_tSystemCtrl.BillPrtFlag |= 0x10;				  //有错误四位置位
				}	
				else
				{
					g_tSystemCtrl.BillPrtFlag &= ~0x10;
				}	
			
        UpLoadData(&g_tSystemCtrl.BillPrtFlag , 1);	   //返回票据打印状态字
        return;
    }
		else if(g_tRealTimeCmd.RealTimeBuf[2] == 2)			  //错误状态返回
    {       
			  ErrTemp = 0x01;
				if (g_tError.PaperSensorStatusFlag & 0x01)		//缺纸
					ErrTemp |= 0x04;
				if (g_tError.DoorOpen)												//上盖  
					ErrTemp |= 0x08;
				if (g_tError.R_ErrorFlag & 0x01)				      //切刀位置错误	
					ErrTemp |= 0x10;
				if (g_tError.AR_ErrorFlag & 0x01)             //bit.0 = 1 打印头过热
				  ErrTemp |= 0x20;
				if (g_tError.PaperSensorStatusFlag & 0x02)  	//纸将尽
					ErrTemp |= 0x40;
				
        UpLoadData(&ErrTemp , 1);	   //返回票据打印状态字
        return;
    }
}

/*******************************************************************************
* Function Name  : Command_1014fnmt
* Description    : 实时产生钱箱开启脉冲,在指定的钱箱插座引脚产生设定的开启脉冲,
	引脚由m指定： 
fn = 1
m = 0, 1
1  ≤  t  ≤  8
m  连接引脚
0  钱箱输出插座引脚2
1  钱箱输出插座引脚5
脉冲高电平时间为[ t × 100 ms]，低电平的时间为[ t × 100 ms]。
·  当在处理此指令而打印机正处于错误状态时，该指令被忽略。
·  当打印机正在执行钱箱开启指令(ESC p   或   DEL DC4)时，该指令被忽略。 
·  串口模式下，打印机接收该指令后立即执行。 
·  并口模式下，打印机忙的时候该指令不执行。 
·  如果打印数据中包含和此指令相同的数据，则这些数据将被当作该指令执行。用户
必须考虑到这种情况。 
  例如：图形数据可能偶然包含了与该指令一致的数据串。 
·  该指令尽量不要插在 2 个或更多字节的指令序列中。 
·  即使打印机被 ESC = (选择外设)指令设置为禁止，该指令依然有效。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1014fnmt(void)
{
	if ((g_tRealTimeCmd.RealTimeBuf[2] == 1) && \
			(g_tRealTimeCmd.RealTimeAllow & 0x01))	//产生钱箱开启实时脉冲
	{	//上次执行完成,无任何错误,并口不忙时才执行该指令		
		if(g_tRealTimeCmd.RealTimeBuf[3] <2 && (g_tRealTimeCmd.RealTimeBuf[4] >0 && g_tRealTimeCmd.RealTimeBuf[4] <9))	//m、t为有效的参数
		{
			if(	g_tRealTimeCmd.RealTimeBuf[3] ==0 && g_tDrawer1.Status == DISABLE && g_tDrawer1.ON_Time ==0)	//1B70指令未执行
			{
				g_tDrawer1.DrwID = g_tRealTimeCmd.RealTimeBuf[3];
				g_tDrawer1.OFF_Time	= 100 * g_tRealTimeCmd.RealTimeBuf[4];	 
				g_tDrawer1.ON_Time	= 2 * g_tDrawer1.OFF_Time;						   
					
				DriveDrawer(g_tDrawer1.DrwID, ENABLE);			//开钱箱
			}
			else if(g_tRealTimeCmd.RealTimeBuf[3] ==1 && g_tDrawer2.Status ==0 && g_tDrawer2.ON_Time ==0)		//如没有正执行钱箱指令相同操作，才有效，否则忽略此次操作
			{
				g_tDrawer2.DrwID = g_tRealTimeCmd.RealTimeBuf[3];
				g_tDrawer2.OFF_Time	= 100 * g_tRealTimeCmd.RealTimeBuf[4];	//关闭后延时时间
				g_tDrawer2.ON_Time	= 2 * g_tDrawer2.OFF_Time;						//总控制时间
				
				DriveDrawer(g_tDrawer2.DrwID, ENABLE);							//开钱箱
			}
		}
	}	//执行关机程序有待添加
	else if (g_tRealTimeCmd.RealTimeBuf[2] == 2 && \
					(g_tRealTimeCmd.RealTimeAllow & 0x02))	//执行关机序列
	{
		
	}
	else if ((g_tRealTimeCmd.RealTimeBuf[2] == 8) && \
					(g_tRealTimeCmd.RealTimeBuf[3] == 1)	&& \
					(g_tRealTimeCmd.RealTimeBuf[4] == 3)	&& \
					(g_tRealTimeCmd.RealTimeBuf[5] == 20)	&& \
					(g_tRealTimeCmd.RealTimeBuf[6] == 1)	&& \
					(g_tRealTimeCmd.RealTimeBuf[7] == 6)	&& \
					(g_tRealTimeCmd.RealTimeBuf[8] == 2)	&& \
					(g_tRealTimeCmd.RealTimeBuf[9] == 8)
					)	//清缓冲区的程序还有待添加
	{
		//清除接收缓冲区和打印缓冲区中的数据并传送清除响应,如果出现了可恢复的错误,则从错误中恢复
		//在发送该指令后,直到接收到返回状态前,不要再发送其他数据
		//清接收缓冲区
		//ReceiveInterrupt(DISABLE);	//关接收中断
		DisableReceiveInterrupt();
		g_tInBuf.BytesNumber = 0;		//接收缓冲区字节数
		g_tInBuf.PutPosition = 0;		//接收缓冲区
		g_tInBuf.GetPosition = 0;		//接收缓冲区			
		//ReceiveInterrupt(ENABLE);		//开接收中断
		EnableReceiveInterrupt();
		//清打印缓冲区
		PrepareNewLine();								//清空g_tLineEditBuf及下划线等控制变量准备下一行编辑
		InitLinePrtBuf();								//清空打印缓冲区
		EndHeat();											//关加热控制
		TIM_Cmd(FEED_TIMER, DISABLE);		//关闭加热控制定时器
		EndFeedMotor();									//关闭电机
		TIM_Cmd(FEED_TIMER, DISABLE);		//关闭电机控制定时器
		TIM_Cmd(PRINT_CHECK_TIMER, DISABLE);	//关闭打印检查控制定时器
		
// 		g_tError.R_ErrorRecoversFlag = 1;
	}
}


/*******************************************************************************
* Function Name  : RealTimeCommand
* Description    : 实时指令处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RealTimeCommand(void)
{
	//if ((g_tRealTimeCmd.RealTimeBuf[0] == 0x1B) && (g_tInterface.COM_Type == COM))
	if (g_tRealTimeCmd.RealTimeBuf[0] == 0x1B)
	{
		UpLoadData(g_tError.ASBStatus, 4); 		//返回状态字
	}
	else
	{
		switch (g_tRealTimeCmd.RealTimeBuf[1])
		{
			case 4:
				Command_1004();
				break;
			case 5:
				Command_1005n();
				break;
			case 6:
			  Command_1006n();
			  break;
			case 0x14:
				Command_1014fnmt();
				break;
		}
	}
	g_tRealTimeCmd.RealTimeFlag = 0;
	g_tRealTimeCmd.RealTimeCntr = 0;
}

/*******************************************************************************
* Function Name  : AutomaticStatusBack(void)
* Description    : 自动状态返回处理程序,在T2中断服务程序中调用,每若干ms执行一次
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	ASB_INTERVAL_MS			(100)			   //2016.07.07  1000改为100
void AutomaticStatusBack(void)
{
	static	uint8_t	Cntr = 0;
	uint8_t	OffLineStatus;
	uint8_t	Status[4];
	uint8_t static OldOffLineStatus =0;		//上一次状态值
	uint8_t	Temp =0;	
	
	Cntr++;
	if (Cntr == ASB_INTERVAL_MS)		     //50
	{
		Cntr = 0;
		memset(Status, 0x00, 4);	//默认固定值,未启用
		Status[0] = 0x10;
		Status[3] = 0x0f;					//2016.07.07
				
		//纸状态
		if (g_tError.PaperSensorStatusFlag & 0x01)		//缺纸
		{
			Status[2] |= 0x0C;
		}
//  if (!(g_tError.AR_ErrorFlag & 0x02))	//状态查询 不管上盖状态
		{
			if (g_tError.PaperSensorStatusFlag & 0x02)	//纸将尽
				Status[2] |= 0x03;
		}
		if(	Status[2] !=g_tError.ASBStatus[2])					
			Temp |= 8;								//纸信状态号有变化
		
		
		//得到打印机信息状态
		Status[1] = 0;
		if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//按下走纸键
			Status[1] |= 2;
		if (g_tError.R_ErrorFlag & 0x02)	//机械错误(机头原位)
			Status[1] |= 4;
		if (g_tError.R_ErrorFlag & 0x01)	//切刀位置错误
			Status[1] |= 8;
		if (g_tError.UR_ErrorFlag)				//不可恢复错误
			Status[1] |= 32;
		if (g_tError.AR_ErrorFlag)				//可自动恢复错误
			Status[1] |= 64;
		if(	Status[1] !=g_tError.ASBStatus[1])					
			Temp |= 4;								//错误状态信号有变化

		
		//得到离线状态
		OffLineStatus = 0x00;
		if (g_tError.AR_ErrorFlag & 0x02)	//上盖
		{
			Status[0] |= 32;
			OffLineStatus |= 4;
		}
		else if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//按下走纸键
		{
			Status[0] |= 64;
			OffLineStatus |= 8;
		}
		else if (Status[2])								//缺纸停止
			OffLineStatus |= 32;
		else if (Status[1])								//错误
			OffLineStatus |= 64;

		if( OldOffLineStatus != OffLineStatus )		//离线状态变化
			Temp |= 2;								      //离线状态信号有变化

		if(OffLineStatus !=0x0)						//离线
			Status[0] |= 8;		

		if( READ_MONEYBOX_STATUS )					//钱箱
			Status[0] |= 4;
		if(( Status[0] & 4) != (g_tError.ASBStatus[0] & 4) )					
			Temp |= 1;			
		
// 		//得到打印机状态
// 		Status[0] = 0x10;
// 		#if	0
// 		if (MONEYBOX_STATUS_CLOSE == ReadMoneyBoxStatus())	//钱箱关闭,读取返回电平为高
// 		{
// 			Status[0] |= 4;
// 		}
// 		#endif
// 		if (OffLineStatus)					//离线
// 		{
// 			Status[0] |= 8;
// 		}
// 		if (g_tError.AR_ErrorFlag & 0x02)		//上盖
// 			Status[0] |= 32;
// 		if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//按下走纸键
// 			Status[0] |= 64;
// 		
// 		for (OffLineStatus = 0; OffLineStatus < (sizeof(Status) / sizeof(Status[0])); OffLineStatus++)
// 			g_tError.ASBStatus[OffLineStatus] = Status[OffLineStatus];

		OldOffLineStatus = OffLineStatus;			//保存离线状态
		g_tError.ASBStatus[0] =Status[0];
		g_tError.ASBStatus[1] =Status[1];
		g_tError.ASBStatus[2] =Status[2];
		g_tError.ASBStatus[3] =Status[3];
		
		if(g_tError.ASBAllowFlag & Temp)			//自动状态返回使能
		{	
			UpLoadData(g_tError.ASBStatus,4); 	//返回状态字
		}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
