/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   字符字模读取缓冲区相关的程序.
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
#include	"charmaskbuf.h"
#include	"esc.h"
#include	"fs.h"
#include	"gs.h"
#include	"rs.h"
#include	"monobyte.h"
#include	"extgvar.h"
#include	"cominbuf.h"
#include	"spiflash.h"
#include	"usart.h"
#include	"feedpaper.h"
#include	"heat.h"
#include	"print.h"
#include	"timconfig.h"
#include	"debug.h"
//#include	"usb_prop.h"
#include	"button.h"
#include	"led.h"
#include	"beep.h"
#include	"uip.h"

/*外部变量声明*/
extern TypeDef_StructInBuffer volatile g_tInBuf;		//通信接收缓冲区
//extern TypeDef_StructLinePrtBuf	g_tLinePrtBuf;
extern TypeDef_StructHeatCtrl		g_tHeatCtrl;
extern	const unsigned char	CodepageFontA[];
extern	const unsigned char	CodepageFontB[];
extern  APP_Tx_Struct ETH_Tx;
extern  APP_Tx_Struct USB_Tx;	//USB输出缓冲区结构 2016.07.29

extern uip_ipaddr_t uip_hostaddr, uip_draddr, uip_netmask;  //2017.02.07
extern uint8_t ETH_ConnectState;   //2018.03.05
#ifdef	DBG_PROCESS_TIME
extern	volatile uint32_t	g_iDbgProcessTimeUs;
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT
extern volatile uint8_t	g_bRxFinishFlag;	//标示串口通信中数据是否已经接收完毕
#endif

/*外部函数声明*/
extern	void	DelayUs(uint32_t	delaytime);
extern void GetETH_ConnectState(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t	g_bLineEditBufStatus = LEB_STATUS_ILDE;

//可替换的国际字符表
uc8  m_tbCharacterSetTable[] = 
{
	0x23,0x24,0x40,0x5B,0x5C,0x5D,0x5E,0x60,0x7B,0x7C,0x7D,0x7E
};

#define	BYTES_OF_CHAR_SET_TABLE	(sizeof(m_tbCharacterSetTable) / sizeof(m_tbCharacterSetTable[0]))

/*定义字符读取缓冲区,全局变量*/
TypeDef_StructCharMaskReadBuf		g_tCharMaskReadBuf;

#ifdef	DBG_ONE_LINE_TIME
uint32_t	g_iDealOneLineTimeCount;

#define	TIME_COUNT_END			(0)
#define	TIME_COUNT_START		(1)
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : InitCharMaskBufParameter
* Description    : 初始化字符读取缓冲区
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitCharMaskBufParameter(void)
{
	g_tCharMaskReadBuf.Status 			= CMRB_STATUS_ILDE;	//设为空闲
	g_tCharMaskReadBuf.LanguageType = g_tSysConfig.SysLanguage;	//语言
	g_tCharMaskReadBuf.FontType 		= g_tSysConfig.SysFontSize;	//字体
	memset(g_tCharMaskReadBuf.Buf, 0x00, MAX_BYTES_OF_HZ);
	//memset(g_tCharMaskReadBuf.Buf, 0x00, sizeof(g_tCharMaskReadBuf.Buf) / sizeof(g_tCharMaskReadBuf.Buf[0]));
}

/*******************************************************************************
* Function Name  : ProcessCharMaskReadBuf
* Description    : 处理字符读取缓冲区,空闲时启动处理接收命令和字符,正在写入时
* 处理打印行缓冲区,写入完毕时处理字符并写入行缓冲区
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	ProcessCharMaskReadBuf(void)
{
	//如果空闲,读取通信输入缓冲区字符或者命令进行处理
	if (g_tCharMaskReadBuf.Status == CMRB_STATUS_ILDE)
	{
		ProcessCharMaskReadBufIlde();	//字符点阵缓冲区空闲处理完毕
	}
	//如果写入完毕,开始处理该字符
	else if (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
	{
		ProcessCharMaskReadBufEdit();	//字符点阵缓冲区等待编辑处理完毕
	}
}

//检查打印机是否有任务没有处理完成.未完成时为忙,包括以下内容:
//接收缓冲区数据不为零;打印控制缓冲区状态不为ILDE;
//加热和走纸未完成;
uint32_t	GetPrintBusyStatus(void)
{
	if ((g_tInBuf.PutPosition != g_tInBuf.GetPosition) || (0 != g_bMotorRunFlag) ||(0 != g_tFeedButton.StepCount) ||(U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD))				//2016.07.07 更改判断
	{
		return	PRINTER_STATUS_BUSY;
	}
	else
	{
		return	PRINTER_STATUS_ILDE;
	}
}

uint32_t	GetPrintProcessStatus(void)
{
	if ((0 != g_tSystemCtrl.StepNumber) || \
			(HEAT_ENABLE == g_tHeatCtrl.HeatEnable) || \
			(LPB_STATUS_ILDE != GetPrintStatus()))
	{
		return	PRINT_PROCESS_STATUS_BUSY;
	}
	else
	{
		return	PRINT_PROCESS_STATUS_ILDE;
	}
}

uint32_t	GetFeedPaperStatus(void)
{
#if	0
	if ((LPB_STATUS_ILDE != GetPrintStatus()) || \
			(g_tHeatCtrl.HeatEnable == HEAT_ENABLE) || \
			(g_tSystemCtrl.StepNumber != 0)
		)
#endif
	if (g_bMotorRunFlag)
		return	PRINTER_STATUS_BUSY;
	else
		return	PRINTER_STATUS_ILDE;
}


/*******************************************************************************
* Function Name  : CopyEditDataToLinePrtStruct
* Description    : 将g_tLineEditBuf中数据及控制变量赋值到控制结构.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#if	0
//只管复制,不管清除打印缓冲区,打印缓冲区事先已经清空
void	CopyEditDataToLinePrtStruct(void)
{
	#ifdef	DBG_COPY_ONE_LINE_TIME
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	*	pbSrc = NULL;
	uint8_t	*	pbDst = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	if (DealOneLineTimeCountFlag == TIME_COUNT_END)
	{
		DealOneLineTimeCountFlag = TIME_COUNT_START;
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	}
	#endif
	
	//如果缓冲区为空白,则不再复制
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
		if (MAX_RATIO_Y == g_tPrtCtrlData.MaxRatioY)		//必须全复制的时候
		{
			pbSrc = g_tLineEditBuf;
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
			memcpy(pbDst, pbSrc, BYTES_OF_LPB);	//复制行打印缓冲区内容
		}
		else
		{
			//计算本行中最高字符占用的列数的字节数,按8字节对齐,不足8字节按补齐8字节计算
			if (g_tPrtCtrlData.MaxPrtDotHigh % 8)
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3) + 1;
			}
			else
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3);
			}
			//根据放大倍数计算起始位置,相对于首地址的字节数
			StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtLeftLimit;
			ByteLen = g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit;		//一字节行的长度
			pbSrc = g_tLineEditBuf + StartPositon;
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + StartPositon;
			for (i = 0; i < ColumnByteNumber; i++)
			{
				memcpy(pbDst, pbSrc, ByteLen);	//复制行打印缓冲区内容
				pbSrc += PRT_DOT_NUMBERS;
				pbDst += PRT_DOT_NUMBERS;
			}
		}
	}
	
	if (g_tPrtCtrlData.DoUnderlineFlag)	//需要打印下划线时才复制,否则无需复制
	{
		pbSrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;	//从左边界开始
		pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB + g_tPrtCtrlData.PrtLeftLimit;
		memcpy(pbDst, pbSrc, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//赋值下划线内容
	}
	
	SetLinePrtModeVariable();		//在一个行打印缓冲区满后,赋值打印控制变量到行打印缓冲区控制结构
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	StopTimer(DBG_TIMER);					//停止计时
	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
}
#endif

#if	0
//清除打印缓冲区内容的操作放到此处,复制的同时即清空原缓冲区了,不一定需要全清零模式
void	CopyEditDataToLinePrtStruct(void)
{
	#ifdef	DBG_COPY_ONE_LINE_TIME
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//复制编辑行打印缓冲区数据到打印缓冲区耗时
	uint32_t	iTemp;
	#endif
	
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint16_t	MaxPrtDotHigh;
	uint32_t	i;
	uint8_t	*	pbSrc = NULL;
	uint8_t	*	pbDst = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	if (DealOneLineTimeCountFlag == TIME_COUNT_END)
	{
		DealOneLineTimeCountFlag = TIME_COUNT_START;
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	}
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//复制编辑行打印缓冲区数据到打印缓冲区耗时
		StartProcessTimeCounter();
	#endif
	
	//如果缓冲区为空白,则不再复制
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
		if (MAX_RATIO_Y == g_tPrtCtrlData.MaxRatioY)	//必须全复制的时候
		{
			pbSrc = g_tLineEditBuf;
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
			memcpy(pbDst, pbSrc, BYTES_OF_LPB);	//复制行打印缓冲区内容
			
			if (g_tPrtCtrlData.DoUnderlineFlag)	//需要打印下划线时才复制,否则无需复制
			{
				pbSrc = g_tUnderlineBuf;
				pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB;
				memcpy(pbDst, pbSrc, PRT_DOT_NUMBERS);	//赋值下划线内容
			}
		}
		else	//部分复制的时候,用二者中最高的那个为基准复制即可
		{
			if ((g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLeftLimit) && \
				(g_tPrtCtrlData.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//确保参数不越界
			{
				//计算本行中最高字符占用的列数的字节数,按8字节对齐,不足8字节按补齐8字节计算
				ptLPB = &U_PrtBuf.g_tLinePrtBuf;
				if (g_tPrtCtrlData.MaxPrtDotHigh > ptLPB->PrtMode.MaxPrtDotHigh)
					MaxPrtDotHigh = g_tPrtCtrlData.MaxPrtDotHigh;
				else
					MaxPrtDotHigh = ptLPB->PrtMode.MaxPrtDotHigh;
				
				if (MaxPrtDotHigh % 8)
				{
					ColumnByteNumber = (MaxPrtDotHigh >> 3) + 1;
				}
				else
				{
					ColumnByteNumber = (MaxPrtDotHigh >> 3);
				}
				if (ColumnByteNumber > MAX_PRT_HEIGHT_BYTE)
					ColumnByteNumber = MAX_PRT_HEIGHT_BYTE;
				
				//根据放大倍数计算起始位置,相对于首地址的字节数
				StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtLeftLimit;
				ByteLen = g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit;		//一字节行的长度
				pbSrc = g_tLineEditBuf + StartPositon;
				pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + StartPositon;
				for (i = 0; i < ColumnByteNumber; i++)
				{
					memcpy(pbDst, pbSrc, ByteLen);	//复制行打印缓冲区内容
					pbSrc += PRT_DOT_NUMBERS;
					pbDst += PRT_DOT_NUMBERS;
				}
				
				if (g_tPrtCtrlData.DoUnderlineFlag)	//需要打印下划线时才复制,否则无需复制
				{
					pbSrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;	//从左边界开始
					pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB + g_tPrtCtrlData.PrtLeftLimit;
					memcpy(pbDst, pbSrc, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//赋值下划线内容
				}
			}		//部分复制参数合法处理结束
			else	//参数不合法,越界,直接全复制
			{
				pbSrc = g_tLineEditBuf;
				pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
				memcpy(pbDst, pbSrc, BYTES_OF_LPB);	//复制行打印缓冲区内容
				
				if (g_tPrtCtrlData.DoUnderlineFlag)	//需要打印下划线时才复制,否则无需复制
				{
					pbSrc = g_tUnderlineBuf;
					pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB;
					memcpy(pbDst, pbSrc, PRT_DOT_NUMBERS);	//赋值下划线内容
				}
			}		//参数不合法处理结束
		}		//部分复制处理结束
	}		//缓冲区非空处理结束
	else	//编辑缓冲区为空白,但是打印缓冲区可能不是空白,此时需要清空打印缓冲区
	{
		ClearLinePrtBuf();
	}
	
	SetLinePrtModeVariable();		//在一个行打印缓冲区满后,赋值打印控制变量到行打印缓冲区控制结构
	
	pbSrc = NULL;
	pbDst = NULL;
	ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
		StopTimer(DBG_TIMER);					//停止计时
		DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//复制编辑行打印缓冲区数据到打印缓冲区耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
}
#endif

#if	1
//全复制,只处理复制，不清空原打印缓冲区
void	CopyEditDataToLinePrtStruct(void)
{
	#ifdef	DBG_COPY_ONE_LINE_TIME
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//复制编辑行打印缓冲区数据到打印缓冲区耗时
	uint32_t	iTemp;
	#endif
	
	uint8_t	*	pbSrc = NULL;
	uint8_t	*	pbDst = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	if (DealOneLineTimeCountFlag == TIME_COUNT_END)
	{
		DealOneLineTimeCountFlag = TIME_COUNT_START;
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	}
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//复制编辑行打印缓冲区数据到打印缓冲区耗时
		StartProcessTimeCounter();
	#endif
	
	pbSrc = g_tLineEditBuf;
	pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
	memcpy(pbDst, pbSrc, BYTES_OF_LINE_PRT_BUF);	//复制行打印缓冲区内容
	if (g_tPrtCtrlData.DoUnderlineFlag)	//需要打印下划线时才复制,否则无需复制
	{
		pbSrc = g_tUnderlineBuf;
		pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LINE_PRT_BUF;
		memcpy(pbDst, pbSrc, PRT_DOT_NUMBERS);	//赋值下划线内容
	}
	#if	0
	else
	{
		if (ptLPB->PrtMode.DoUnderlineFlag)
		{
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB;
			memset(pbDst, 0x00, PRT_DOT_NUMBERS);	//清空LPB下划线
		}
	}
	#endif
	
	SetLinePrtModeVariable();		//在一个行打印缓冲区满后,赋值打印控制变量到行打印缓冲区控制结构
	
	pbSrc = NULL;
	pbDst = NULL;
	ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
		StopTimer(DBG_TIMER);					//停止计时
		DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//复制编辑行打印缓冲区数据到打印缓冲区耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
}
#endif

/*******************************************************************************
* Function Name  : PrepareNewLine
* Description    : 清空g_tLineEditBuf及下划线等控制变量准备下一行编辑
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	PrepareNewLine(void)
{
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	*	pbSrc = NULL;
	#ifdef	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB	//清空编辑行打印缓冲区耗时
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB	//清空编辑行打印缓冲区耗时
		StartProcessTimeCounter();
	#endif
	
	
	#if	0
	if (MAX_RATIO_Y == g_tPrtCtrlData.MaxRatioY)		//必须全清除的时候
	{
		memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);
		//memset(g_tLineEditBuf, 0x00, sizeof(g_tLineEditBuf) / sizeof(g_tLineEditBuf[0]));
	}
	else
	{
		if ((g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLeftLimit) && \
				(g_tPrtCtrlData.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//确保参数不越界
		{
			//计算本行中最高字符占用的列数的字节数,按8字节对齐,不足8字节按补齐8字节计算
			if (g_tPrtCtrlData.MaxPrtDotHigh % 8)
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3) + 1;
			}
			else
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3);
			}
			if (ColumnByteNumber > MAX_PRT_HEIGHT_BYTE)
				ColumnByteNumber = MAX_PRT_HEIGHT_BYTE;
			
			//根据放大倍数计算起始位置,相对于首地址的字节数
			StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtLeftLimit;
			ByteLen = g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit;		//一字节行的长度
			pbSrc = g_tLineEditBuf + StartPositon;
			for (i = 0; i < ColumnByteNumber; i++)
			{
				memset(pbSrc, 0x00, ByteLen);			//清除行打印缓冲区内容
				pbSrc += PRT_DOT_NUMBERS;
			}
		}
		else	//如果越界固定全清除
		{
			//pbSrc = g_tLineEditBuf;
			memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);	//清除行打印缓冲区内容
			//memset(g_tLineEditBuf, 0x00, sizeof(g_tLineEditBuf) / sizeof(g_tLineEditBuf[0]));
		}
	}
	#else
	memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);
	//memset(g_tLineEditBuf, 0x00, sizeof(g_tLineEditBuf) / sizeof(g_tLineEditBuf[0]));
	#endif
	
	#ifdef	UPPER_LINE_ENABLE	
	if (g_tPrtCtrlData.DoUpperlineFlag) 	//清除上划线缓冲区
	{
		g_tPrtCtrlData.DoUpperlineFlag = 0;
		pbSrc = gstructUpperLine.DataBuf + g_tPrtCtrlData.PrtLeftLimit;	//从左边界开始
		memset(pbSrc, 0x00, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//清除下划线内容
	}
	#endif
	
	#ifdef	UNDER_LINE_PRT_ENABLE	
	if (g_tPrtCtrlData.DoUnderlineFlag)	//清除下划线缓冲区
	{
		g_tPrtCtrlData.DoUnderlineFlag = 0;
		g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
		if ((g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLeftLimit) && \
				(g_tPrtCtrlData.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//确保参数不越界
		{
			pbSrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;	//从左边界开始
			memset(pbSrc, 0x00, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//清除下划线内容
		}
		else	//即使参数越界,确保操作不越界
		{
			pbSrc = g_tUnderlineBuf;
			memset(pbSrc, 0x00, PRT_DOT_NUMBERS);	//清除下划线内容
			//memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
		}
	}
	#endif
	
	g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
	g_tPrtCtrlData.MaxPrtDotHigh = 0;
	g_tPrtCtrlData.BitImageFlag = 0; 					//清位图打印标志
	g_tPrtCtrlData.VariableBitImageFlag = 0; 	//清位图打印标志
	g_tPrtCtrlData.CharWidth = 0;
	g_tPrtCtrlData.CharHigh = 0;
	g_tPrtCtrlData.LineHigh = 0;							//行高
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	g_tPrtCtrlData.MaxRatioX = 1;					//行内最大横向放大倍数
	#endif
	g_tPrtCtrlData.MaxRatioY = 1;					//行内最大纵向倍数
	g_tPrtCtrlData.OneLineWidthFlag = 0;	//取消行内倍宽
	g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_NONE;
	g_bLineEditBufStatus = LEB_STATUS_ILDE;
	pbSrc = NULL;
	
	#ifdef	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB	//清空编辑行打印缓冲区耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	g_tTab.TabIndex = 0;		//2016.09.06
	
}

/*******************************************************************************
* Function Name	: MovDotLine( u8 *PrintBuff, u8 Lines )
* Description	: 将纵向处理缓冲中的一点行数据移至打印缓冲
* Input			:   VBuf:	处理缓冲区地址，纵向排列
				        LineID:	当前需处理的行号
* Output		: None
* Return		: None
*******************************************************************************/
void MovDotLine (u8 *VBuf, u8 LineID)			  //2016.06.28
{
	uint16_t	i, SpaceDot, HeatDots;
	uint8_t	ByteDots;
	
	//转换的一字节横向点阵数据
	ByteDots = 0;
	
	//需加热点数
	HeatDots  = 0;

	if(g_tPrtCtrlData.LineDirection ==PRINT_DIRECTION_NORMAL)			//正常打印
	{
		if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)	           //居中
		{	
			SpaceDot = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex)/2;
		}	
		else if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)				//右对齐
		{	
			SpaceDot = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		}	
		else					//左对齐
		{	
			SpaceDot = 0;
		}	
// 		SpaceDot += g_tPrtCtrlData.PrtLeftLimit;

	}
	else	   								//反向打印
	{
		if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)	//字符对齐模式，0=左对齐，1=居中, 2=右对齐
		{	
			SpaceDot = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex)/2;
		}	
		else if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)
		{	
			SpaceDot = 0;
		}	
		else
		{	
			SpaceDot = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		}	
// 		SpaceDot += g_tPrtCtrlData.PrtLeftLimit;
	}

	PrtDrvBufFull();						      //行打印缓冲区有空间

	for(i=0; i< SpaceDot/8; i++)			//左边不用缓冲区清零
	{
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = 0x00;
	}
	if(g_tPrtCtrlData.LineDirection ==PRINT_DIRECTION_NORMAL)			//正向打印
	{
		for(i=0; i < g_tPrtCtrlData.PrtDataDotIndex; i++)
		{
			if(VBuf[i] & (0x80 >> (LineID & 0x07)))		 //从上到下的顺序  
			{
				ByteDots |= 0x80>> (SpaceDot & 0x07);
				HeatDots++;
			}
			SpaceDot++;
			if((SpaceDot & 0x07 )==0)
			{
				U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[SpaceDot/8-1] = ByteDots;
				ByteDots = 0;
			}
		}
	}
	else										//反向打印
	{	
		for(i=0; i < (g_tPrtCtrlData.PrtDataDotIndex) ; i++)
		{
			if(VBuf[g_tPrtCtrlData.PrtDataDotIndex-1-i] & (0x80 >> (LineID & 0x07)))		
			{
				ByteDots |= 0x80>> (SpaceDot & 0x07);
				HeatDots++;
			}
			SpaceDot++;
			if((SpaceDot & 0x07 )==0)
			{
				U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[ (SpaceDot/8-1)] = ByteDots;
				ByteDots = 0;
			}
		}
	}

	i= SpaceDot/8;
	if( (SpaceDot &0x07) !=0)				//最后不是8整数倍的处理
	{
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = ByteDots;
		i++;
	}
		
	for(; i< PRT_DOT_NUMBERS/8; i++)			//右边不用缓冲区清零
	{
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = 0;
	}	
	IncDrvPrtBufAddr(0x00, HeatDots);
}

/*******************************************************************************
* Function Name  : 
* Description    : 打印当前行打印缓冲区中的内容.检查打印缓冲区状态直到空闲,
* 然后复制行打印缓冲区数据,复制控制变量,清零编辑行打印缓冲区,开始打印本行
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
	volatile uint32_t	g_iDbgSPISendCount;			//往机芯SPI发送数据的次数统计
	volatile uint32_t	g_iDbgEditCountWhilePrt;
#endif
void	PrtOneLineToBuf(uint32_t LineHigh)
{
	s16	Lines;
	uint8_t	*pbBuf,*p;
	
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//打印一行耗时
		uint32_t	iTemp;
	#endif
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//打印一行开始前等待状态变为空闲的耗时
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//打印一行开始前等待状态变为空闲的耗时
		StartProcessTimeCounter();
	#endif
	
 //WaitPreviousPrintFinish();	//查询行打印缓冲区状态,一直等到有空闲行缓冲区可以进行打印
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//打印一行开始前等待状态变为空闲的耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//打印一行耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	pbBuf = g_tLineEditBuf + \
			(MAX_RATIO_Y - g_tPrtCtrlData.MaxRatioY) * (MAX_FONT_HEIGHT / 8) * PRT_DOT_NUMBERS;  //字符在编辑缓冲区的首地址
					
	if( g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
	{										//反向打印
		for(Lines=(8-g_tPrtCtrlData.UnderlineMaxPrtHigh); Lines<8; Lines++)  //打印下划线
		{			
			MovDotLine( g_tUnderlineBuf, Lines);	  
		}
		for( Lines=g_tPrtCtrlData.MaxPrtDotHigh; Lines >0; )              //打印内容行
		{
			Lines--;
			p = pbBuf + (Lines>>3 )* PRT_DOT_NUMBERS;
			MovDotLine( p, Lines);	           
		}
	}
	else		 					 //正向打印
	{
		for( Lines=0; Lines < g_tPrtCtrlData.MaxPrtDotHigh; Lines++ )	  	     //打印内容行 内容在编辑缓冲区的底部，内容从上到下的点行，序号是0到最大行数
		{
			p = pbBuf +  (Lines>>3 )* PRT_DOT_NUMBERS ;													 //计算点行所在的字节行的位置
			MovDotLine( p,  Lines);	                 			              
		}

		for(Lines=(8-g_tPrtCtrlData.UnderlineMaxPrtHigh); Lines<8; Lines++)  //打印下划线
		{			
			MovDotLine( g_tUnderlineBuf, Lines);	  
		}			
	}
	
	g_tPrtCtrlData.MaxPrtDotHigh +=g_tPrtCtrlData.UnderlineMaxPrtHigh;
	//走行间距
	if(LineHigh > g_tPrtCtrlData.MaxPrtDotHigh)
	{	
		GoDotLine((LineHigh - g_tPrtCtrlData.MaxPrtDotHigh));			
	}
	PrepareNewLine();	//清空g_tLineEditBuf及下划线等控制变量准备下一行编辑
	
	//填满一行的编辑缓冲区耗时,包括读字库和编辑填充时间
	#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
		StartProcessTimeCounter();
	#endif
}



void	PrintOneLine(void)
{
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//打印一行耗时
		uint32_t	iTemp;
	#endif
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//打印一行开始前等待状态变为空闲的耗时
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//打印一行开始前等待状态变为空闲的耗时
		StartProcessTimeCounter();
	#endif
	
	WaitPreviousPrintFinish();	//查询行打印缓冲区状态,一直等到有空闲行缓冲区可以进行打印
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//打印一行开始前等待状态变为空闲的耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//打印一行耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	if ((g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit) && \
			(g_tPrtCtrlData.DoUnderlineFlag == 0))
	{
		if (!((g_tPrtCtrlData.BitImageFlag) || (g_tPrtCtrlData.VariableBitImageFlag)))
		{	//直接走行间距,图形打印模式时不走行间距
			GoDotLine((uint32_t)g_tPrtCtrlData.CodeLineHigh);
		}
	}
	else
	{
		CopyEditDataToLinePrtStruct();	//将g_tLineEditBuf中数据及控制变量赋值到控制结构
		U_PrtBuf.g_tLinePrtBuf.Status = LPB_STATUS_WAIT_TO_PRT;	//赋值打印控制变量
		SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);	//启动新一行的打印
		StartTimer(PRINT_CHECK_TIMER);
		
		#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//打印一行耗时
			StartProcessTimeCounter();
		#endif
		#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
			g_iDbgSPISendCount = 0;
			g_iDbgEditCountWhilePrt = 0;
		#endif
	}
	
	PrepareNewLine();	//清空g_tLineEditBuf及下划线等控制变量准备下一行编辑
	
	//填满一行的编辑缓冲区耗时,包括读字库和编辑填充时间
	#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
		StartProcessTimeCounter();
	#endif
}


#if defined	DBG_DEAL_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME || defined DBG_PRT_SPEED
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
#endif

static void	ProcessCharMaskReadBufIlde(void)
{
	uint8_t	ReadChar;
	static uint8_t WaitFlag = 0;			//2016.05.23  
	uint16_t i;
	uint8_t Bps;
	//如果接收缓冲区有内容,而且有可以进行编辑的行打印缓冲区
	if ((g_tInBuf.PutPosition != g_tInBuf.GetPosition) || g_tMacro.RunFlag)	//2016.07.07
	{
			if(WaitFlag ==0 )	
			{
				WaitFlag = 1;
				MaxSetp();									//接口类型初始化最大加速步数
				
				//当串口在缓冲区没数时，开始接收数据时，等待2.0秒
				if(g_tSystemCtrl.PrtSourceIndex == COM ) 		//COM 2016.06.08 
				{
 					//if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)			//驱动模式
					{
						//等待，如果没有数据，则退出等待
						for(i=0; i<3000; i++)			//2016.06.30  8000改为3000		
						{
							DelayMs(1);					//延时不准 大概时间 
							if(g_tSystemCtrl.SleepCounter >30)	//30ms没有收到数退出  2016.05.23
								break;
						}
					}	
				}
		  }
		  g_tSystemCtrl.SleepCounter = 0;								//复位休眠计数器
			
		#ifdef	DBG_RX_TOTAL_THEN_PRT				//收完再打
		if (g_bRxFinishFlag != RX_FINISHED)
		{
			return;		//死等一次通信接收完成
		}
		else	//通信过程已经完成,开始处理用户命令或者待打印内容			
		{
			#if defined	DBG_DEAL_ONE_LINE_TIME || defined DBG_PRT_SPEED
				if (DealOneLineTimeCountFlag == TIME_COUNT_END)
				{
					DealOneLineTimeCountFlag = TIME_COUNT_START;
					g_iDealOneLineTimeCount = 0;
					SetTimerCounterWithAutoReload(DBG_TIMER, 1);
					StartTimer(DBG_TIMER);
				}
			#endif
			
			if (g_tSystemCtrl.PrtDeviceDisable) 	//打印机被禁止
			{
				Command_1B3D();
			}
			else
			{
				ReadChar = ReadInBuffer();	//读取接收缓冲区
				DataProcess(ReadChar);			//处理该字符,可能是命令或者可打印字符,如果是可打印字符则启动SPI读取
			}
		}
		#else	//正常边收边打的程序处理,不检查通信是否完成
		if (g_tSystemCtrl.PrtDeviceDisable) 	//打印机被禁止
		{
			Command_1B3D();
		}
		else
		{
			ReadChar = ReadInBuffer();	//读取接收缓冲区
			DataProcess(ReadChar);			//处理该字符,可能是命令或者可打印字符,如果是可打印字符则启动SPI读取
		}
		#endif
	}			//通信接收缓冲区非空处理结束
	else	//通信接收缓冲区为空处理开始
	{
		WaitFlag = 0;	//2016.05.23
	  if(NetReloadCounter>65000)           //2018.12.10  延时超过1分钟复位网口
		{			
			ETH_ConnectState=0;
			if(g_tInterface.ETH_Type) 
			{
				uIPMain();    //初始化网口
			}	                        //网口初始化
			NetReloadCounter = 0;
		}		
		if (g_tSystemCtrl.SleepCounter > SLEEP_TIME_MS)	//休眠定时时间到
		{
				g_tSystemCtrl.SleepCounter = 0;
		}
		if(g_tInterface.ETH_Type) 
		{	
			GetETH_ConnectState();
		}	
		CheckFeedButton();
		
		if(g_tSysConfig.DHCPEnable)
		{	
			if((DhcpGetFlag ==0) && (uip_hostaddr[0] != 0))
			{	
				ETH_SelfTest();               //2017.02.07 打印自动获取的IP地址
				DhcpGetFlag = 1;							//置位标志位
			}	
		}
		
		#if defined DBG_PRT_SPEED
			StopTimer(DBG_TIMER);					//停止计时
			DealOneLineTimeCountFlag = TIME_COUNT_END;
		#endif
	}			//通信接收缓冲区为空处理结束
}

static void	ProcessCharMaskReadBufEdit(void)
{
	uint8_t	Result;
	uint8_t	CodeType;
	#ifdef	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB	//填满一行的编辑缓冲区耗时
		uint32_t	iTemp;
	#endif
	//填满一行的编辑缓冲区耗时,包括读字库和编辑填充时间
	#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
		uint32_t	iTemp;
	#endif

	if (g_bLineEditBufStatus == LEB_STATUS_ILDE)	//有空闲行缓冲区可以进行编辑
	{
		
		#ifdef	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB	//填满一行的编辑缓冲区耗时
			StartProcessTimeCounter();
		#endif
		CodeType = g_tCharMaskReadBuf.LanguageType;	//字符还是汉字
		Result = JudgeLineBufFull(CodeType);
		if (Result ==	LINE_BUF_NOT_FULL)	//该行不满可以放得下当前字符
		{	//将该字符放入当前编辑行缓冲区
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);
			InitCharMaskBufParameter();		//清空字模读取缓冲区,字符缓冲区标志改为ILDE
			#ifdef	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB	//填满一行的编辑缓冲区耗时
				StopProcessTimeCounter();
				iTemp = 0;
				g_iDbgProcessTimeUs = 0;
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
				g_iDbgEditCountWhilePrt++;
			#endif
		}
		else if (Result == LINE_BUF_FULL)	//当前行缓冲区已满无法放下当前字符
		{
			g_bLineEditBufStatus = LEB_STATUS_WAIT_TO_PRINT;	//置该行状态为等待打印
			//填满一行的编辑缓冲区耗时,包括读字库和编辑填充时间
			#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
				StopProcessTimeCounter();
				iTemp = 0;
				g_iDbgProcessTimeUs = 0;
			#endif

			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);         //2016.06.28
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);		//继续填充
			InitCharMaskBufParameter();		//清空字模读取缓冲区,字符缓冲区标志改为ILDE
			
			#ifdef	DBG_DEAL_ONE_LINE_TIME
				StopTimer(DBG_TIMER);					//停止计时
				DealOneLineTimeCountFlag = TIME_COUNT_END;
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
				g_iDbgEditCountWhilePrt++;
			#endif
		}
	}		//有空闲编辑行打印缓冲区处理完毕
	else	//没有空闲编辑行打印缓冲区时检查处理打印行缓冲区
	{
		if (LPB_STATUS_ILDE == GetPrintStatus())	//查询行打印缓冲区状态
		{			
			
			#ifdef	DBG_COPY_AND_CLEAR_ONE_LINE_TIME
				if (DealOneLineTimeCountFlag == TIME_COUNT_END)
				{
					DealOneLineTimeCountFlag = TIME_COUNT_START;
					g_iDealOneLineTimeCount = 0;
					SetTimerCounterWithAutoReload(DBG_TIMER, 1);
					StartTimer(DBG_TIMER);
				}
			#endif
			
			CopyEditDataToLinePrtStruct();		//将g_tLineEditBuf中数据及控制变量赋值到控制结构
			U_PrtBuf.g_tLinePrtBuf.Status = LPB_STATUS_WAIT_TO_PRT;	//赋值打印控制变量
			SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);	//启动新一行的打印
			StartTimer(PRINT_CHECK_TIMER);
			
			#ifdef	DBG_COPY_AND_CLEAR_ONE_LINE_TIME
				StopTimer(DBG_TIMER);					//停止计时
				DealOneLineTimeCountFlag = TIME_COUNT_END;
			#endif
			
			PrepareNewLine();							//清空g_tLineEditBuf及下划线等控制变量准备下一行编辑
		}
	}		//没有空闲编辑行打印缓冲区时检查处理打印行缓冲区处理结束
}

/*******************************************************************************
* Function Name  : static void	FillLinePrintBuf(uint8_t * pstruct, uint8_t CodeType)
* Description    : 将当前字模读取缓冲区中内容放入指定行打印缓冲区地址
* Input          : Source：字模地址,CodeType：代码类型,0＝西文,=1汉字
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void	FillLinePrintBuf(uint8_t * Source, uint8_t CodeType)
{
	//uint8_t	Width, High, RatioX, Temp;
	uint8_t	Width, High, Temp;
	
	//RatioX = GetRatioXValue(CodeType);
	if (CodeType)
	{
		Width = g_tPrtModeHZ.CharWidth;
 		High  = g_tPrtModeHZ.CharWidth;	//CharHigh暂时未处理
	}
	else
	{
		Width = g_tPrtModeChar.CharWidth;
		High  = 24;             //g_tPrtModeChar.CharHigh;  2016.07.01 把行高都固定在24点 后续需要完善 对于6*8 9*17 下对齐的又不等高的一行 处理上部为空白
	}	
	
	if (g_tPrtCtrlData.CharCycle90)		//旋转90？
	{
		CodeRollCtrl(Width, High, Source);
		Temp 	= Width;
		Width	= High;
		High 	= Temp;
	}
	if (g_tPrtModeChar.EmphaSizeMode)	//加粗加重
	{
		EmphaSizeProcess(Width, 3, Source);
	}
	
	FillPrintBuff(Source, Width, High, CodeType);
//	FillPrintBuff(Source, Width, High, RatioX, CodeType);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 数据处理分析,是打印字符处理打印,是命令处理命令
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
//#define	DBG_RS232_ENABLE
void	DataProcess(uint8_t GetChar)
{
	#if	0
		while (g_tSystemCtrl.PrtDeviceDisable) 	//打印机被禁止
		{
			Command_1B3D();						//禁止/允许打印机
		}
	#endif
	
	#ifdef	DBG_RS232_ENABLE
		UpLoadData(&GetChar, 1);		//原样回发
	#else
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//标准模式
		{
			if (GetChar >= 0x20)
			{
				PrintChar(GetChar);	  //可打印字符处理
			}
			else
			{
				if (GetChar == 0x1B)
				{
					ESC_Command();		          //ESC(1B)指令处理
				}
				else if (GetChar == 0x1C)
				{
					FS_Command();				        //FS(1C)指令处理
				}
				else if (GetChar == 0x1D)
				{
					GS_Command();				        //GS(1D)指令处理
				}
				#ifdef	DBG_SELF_DEF_CMD_ENABLE
					else if (GetChar == 0x1E)
					{
						RS_Command();			        //1E指令处理
					}
				#endif
				else if (GetChar == 0x1F)			
				{
					US_Command();				        //1F设置参数指令处理
				}
				else
				{
					MonoByte_Command(GetChar);	//单字节指令处理
				}
			}	//指令处理完毕
		}	//标准模式处理完毕
		else							//页模式处理
		{
		}
	#endif
}

/*******************************************************************************
* Function Name  : void UpLoadData(uint8_t *DataBuf, uint32_t Length)
* Description    : 上传数据到主机
* Input          : Length：数据长度
*				   DataBuf：数据地址指针
* Output         : None
* Return         : None
*******************************************************************************/
void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length)
{
	uint32_t i;
	unsigned char * pbtemp;
	
	uint32_t Len;
  APP_Tx_Struct *Tx_Buf=NULL;	
	
	if (g_tSystemCtrl.PrtSourceIndex == USB)			//当前数据源为USB
	{
		Tx_Buf =&USB_Tx;
		i =0;
		while(Length)
		{
			//计算缓冲区剩余空间
			Len =Tx_Buf->ptr_out;	//中断中可能改变其值
			if(Tx_Buf->ptr_in >= Len )
				Len =Tx_Buf->size - (Tx_Buf->ptr_in - Len);
			else
			{
				Len =Len - Tx_Buf->ptr_in;
			}
			if( Len )
			{
				Tx_Buf->Buffer[Tx_Buf->ptr_in]= DataBuf[i];
				Tx_Buf->ptr_in++;
				if(Tx_Buf->ptr_in ==Tx_Buf->size)
				{
					Tx_Buf->ptr_in=0;

					if(g_tInterface.WIFI_Type)
					{
						//clear_busy = 0 ;
					}
				}			
				Length--;
				i++;
			}
			else
			{
				if (g_tSystemCtrl.PrtSourceIndex == USB)		//当前数据源为USB
				{		
					break;
				}
			}
		}
		return;
	}
	if (g_tSystemCtrl.PrtSourceIndex == COM)			//当前数据源为COM
	{
		for (i = 0; i < Length; i++)
		{
			//while (!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE));	//等待发送缓冲空
			/* Loop until the end of transmission */
			while ((!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE))  && (USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TC) == RESET));   //2017.10.18  兼容GD芯片
			USART_SendData(USART_SOURCE, DataBuf[i]);
		}
		return;
	}
	if(g_tSystemCtrl.PrtSourceIndex == ETHERNET )		//2016.07.07
	{	
		Tx_Buf =&ETH_Tx;
		
		i =0;
		while(Length)
		{
			//计算缓冲区剩余空间
			Len =Tx_Buf->ptr_out;	//中断中可能改变其值
			if(Tx_Buf->ptr_in >= Len )
				Len =Tx_Buf->size - (Tx_Buf->ptr_in - Len);
			else
			{
				Len =Len - Tx_Buf->ptr_in;
			}
			if( Len )
			{
				Tx_Buf->Buffer[Tx_Buf->ptr_in]= DataBuf[i];
				Tx_Buf->ptr_in++;
				if(Tx_Buf->ptr_in ==Tx_Buf->size)
				{
					Tx_Buf->ptr_in=0;
				}			
				Length--;
				i++;
			}
			else
			{
				if ((g_tSystemCtrl.PrtSourceIndex == ETHERNET))		//当前数据源为网口
				{		
					
				}
			}
		}
		return;
	}	
	if (g_tSystemCtrl.PrtSourceIndex == BLUETOOTH)			//2018.04.19 当前数据源为蓝牙或者wifi  串口2
	{
		for (i = 0; i < Length; i++)
		{
			//while (!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE));	//等待发送缓冲空
			/* Loop until the end of transmission */
			while ((!USART_GetFlagStatus(WIFI_COM, USART_FLAG_TXE))  && (USART_GetFlagStatus(WIFI_COM, USART_FLAG_TC) == RESET));   //2017.10.18  兼容GD芯片
			USART_SendData(WIFI_COM, DataBuf[i]);
		}
		return;
	}
}

/*******************************************************************************
* Function Name  : PrintChar(uint8_t CharCode)
* Description    : 接收到的有效打印代码的处理
* Input          : CharCode：接收到的字符代码
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintChar(uint8_t CharCode)
{
	if (g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH)	//西文模式
	{
  	PrintAscII(CharCode);
	}	//西文处理结束
  else										//汉字模式
	{	//自定义汉字
                                            //(g_tSystemCtrl.UserSelectFlag) && 
	if ( (CharCode == 0xFE) && \
		 ((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x02) == 0x02))
		{
			JudgePrintSelfDefChinese(CharCode);	//处理自定义汉字打印
		}
		else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)	//BIG5汉字方式
  	{
			JudgePrintBIG5(CharCode);			//处理BIG5汉字打印
		}
		else								//GB18030
		{
			JudgePrintGB18030(CharCode);	//处理GB18030打印  	
		}
	}	//汉字处理结束
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 用户自定义汉字数据处理分析,判断数据是否合格,并处理打印
* Input          : 输入参数,第一字节编码
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	JudgePrintSelfDefChinese(uint8_t Ch1)
{
	uint8_t	Ch2;
	uint8_t	* p = NULL;
	uint32_t	i;
	
	Ch2 = ReadInBuffer();
	if ((Ch2 > 0xA0) && (Ch2 < (0xA1 + USER_HZ_SIZE)))
	{
		p = g_tRamBuffer.UserCharStruct.UserCode_HZ;	//用户定义汉字低字节代码表
		for (i = 0; i < USER_HZ_SIZE; i++, p++)
		{
			if (Ch2 == *p)
				break;
		}
		if (i < USER_HZ_SIZE)
		{
			PrintUserHZ(Ch1, Ch2);			//打印自定义汉字
		}
		else
		{
			PrintGB18030(0xA1, 0xA1, 0, 0);	//如果没有自定义汉字,则打印一全角空
		}
	}
	else
	{
		PrintAscII(Ch1);
		PrintAscII(Ch2);
	}
	
	p = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : BIG5汉字数据处理分析,判断数据是否合格,并处理打印
* Input          : 输入参数,第一字节编码
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	JudgePrintBIG5(uint8_t Ch1)
{
	uint8_t	Ch2;
	
	if ((Ch1 < 0xA1) || (Ch1 > 0xF9))	//第一代码为无效汉字代码
	{
		PrintAscII(Ch1);				//第一字节为西文字符
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x40) || ((Ch2 > 0x7E) && (Ch2 < 0xA1)) || (Ch2 > 0xFE))
		{	//第二字节为无效汉字代码
			PrintAscII(Ch1);			//第一字节按西文字符打印
			DecInBufferGetPoint();	//放回最后读取的字符
		}
		else							//有效汉字代码
		{
			PrintBIG5(Ch1, Ch2);		//打印汉字
		}
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : GB18030汉字数据处理分析,判断数据是否合格,并处理打印
* Input          : 输入参数,第一字节编码
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	JudgePrintGB18030(uint8_t Ch1)
{
	uint8_t	Ch2, Ch3, Ch4;
	#ifdef	DBG_PROCESS_TIME
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_ANALYSE_AND_READ_ONE_HZ	//分析一个汉字的点阵字库地址等信息耗时
		StartProcessTimeCounter();
	#endif
	
	if ((Ch1 < 0x81) || (Ch1 > 0xFE))		//第一代码为无效汉字代码
	{
		PrintAscII(Ch1);				//第一字节为西文字符打印
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x30) || (Ch2 == 0x7F) || (Ch2 > 0xFE))	//无效汉字代码
		{
			PrintAscII(Ch1);			//第一字节按西文字符打印
 			DecInBufferGetPoint();		//放回最后读取的字符待修改
		}
		else if (((Ch2 >= 0x40) && (Ch2 <= 0x7E)) || \
						 ((Ch2 >= 0x80) && (Ch2 <= 0xFE)))
		{
			Ch3 = 0;
			Ch4 = 0;
			PrintGB18030(Ch1, Ch2, Ch3, Ch4);		//两字节汉字
		}
		else									//四字节方式
		{
			Ch3 = ReadInBuffer();
			if ((Ch3 < 0x81) || (Ch3 > 0xFE))  	//第三字节代码无效
			{
				PrintAscII(Ch1);				//第一字节按西文字符打印
				PrintAscII(Ch2);				//第二字节按西文字符打印
				DecInBufferGetPoint();	//放回最后读取的字符
			}
			else
			{
				Ch4 = ReadInBuffer();
				if ((Ch4 < 0x30) || (Ch4 > 0x39))	//第四字节代码无效
				{
					PrintAscII(Ch1);			//第一字节按西文字符打印
					PrintAscII(Ch2);			//第二字节按西文字符打印
					PrintAscII(Ch3);			//第三字节为有效打印字符
					DecInBufferGetPoint();	//放回最后读取的字符,ch4有可能是合法的双字节编码的低位字节
				}
				else
				{
					PrintGB18030(Ch1, Ch2, Ch3, Ch4);		//四字节汉字
				}
			}
		}
	}
	
	#ifdef	DBG_PROCESS_TIME_ANALYSE_AND_READ_ONE_HZ	//分析一个汉字的点阵字库地址等信息耗时
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
}

/*******************************************************************************
* Function Name  : PrintAscII
* Description    : 如果打印区能放下当前ASCII,则取ASCII字符字模到打印缓冲区,否则,
*	打印当前行.
* Input          : ASCII字符代码
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
//高速POS流程的处理方式
//启动该字符的SPI_DMA读取
//将当前字符环境变量设置到字符读取缓冲区结构体中
void	PrintAscII(uint8_t Code)
{
	//uint8_t	TempBuf[MAX_BYTES_OF_CHAR];
	//GetAscIIDotData(Code, TempBuf);	
	GetAscIIDotData(Code, g_tCharMaskReadBuf.Buf);	
}

/*******************************************************************************
* Function Name  : PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : 如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,否则打印当前行.
* Input          : CodeHigh=汉字代码高位,CodeLow=汉字代码低位
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
#ifdef	BIG5_ZK_INDEPENDENT
void PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	Offset;
	uint32_t	CodeAddr;
	
	if ((CodeLow >= 0x40) && (CodeLow <= 0x7E))
	{	
		Offset = CodeLow - 0x40;
	}
	else if ((CodeLow >= 0xA1) && (CodeLow <= 0xFE))
	{
		Offset = CodeLow - 0xA1 + 0x3F;
	}
	else
	{
		Offset = 0;
	}
	
	CodeAddr = BIG5_BASE_ADDR + ((CodeHigh - 0xA1) * 0x9D + Offset) * BYTES_OF_HZ_FONT_A;
	
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_BIG5;
	g_tCharMaskReadBuf.FontType = HZ_FONT_A;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//设为正在写入
	//有待添加启动SPI DMA内容
	//sFLASH_ReadBuffer(TempBuf, CodeAddr, BYTES_OF_HZ_FONT_A);
	sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, CodeAddr, BYTES_OF_HZ_FONT_A);
	
	#ifdef	DBG_SPI_ZIKU
		UpLoadData(g_tCharMaskReadBuf.Buf, BYTES_OF_HZ_FONT_A);	//上传到PC
		InitCharMaskBufParameter();	//设为空闲,并清空BUF
	#else
		g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//设为等待处理
	#endif
}
#else
void PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	Offset;
	uint8_t	bBuf[2];			//存放2字节GB码
	uint32_t	CodeAddr;
	
	if ((CodeLow >= 0x40) && (CodeLow <= 0x7E))
	{	
		Offset = CodeLow - 0x40;
	}
	else if ((CodeLow >= 0xA1) && (CodeLow <= 0xFE))
	{
		Offset = CodeLow - 0xA1 + 0x3F;
	}
	else
	{
		Offset = 0;
	}
	
	CodeAddr = BIG5_BASE_ADDR + ((CodeHigh - 0xA1) * 0x9D + Offset) * 2;	//BIG5对应的GB码均为2个字节
	sFLASH_ReadBuffer(bBuf, CodeAddr, 2);		//读取转换表地址
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(bBuf, 2);
	#endif
	PrintGB18030(bBuf[0], bBuf[1], 0, 0);		//其中处理了读取缓冲区各项设置及测试处理，解密处理
	
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_BIG5;
	
}
#endif

/*******************************************************************************
* Function Name  : void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
* Description    : 如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,否则,打印当前行.
* Input          : Code1,Code2,Code3,Code4为汉字代码,当Code3=Code4=0时为双字节代码
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
{
	uint32_t	CodeAddr;
	#ifdef	DBG_PROCESS_TIME
		uint32_t	iDbgProcessTimeTemp;
	#endif
	
	//计算字符地址
	if ((Code3 == 0) || (Code4 == 0))	 //双字节方式
	{
		if ((Code1 >= 0xA1) && (Code1 <= 0xA9) && (Code2 >= 0xA1))			//1区符号
		{	
			CodeAddr = (Code1 - 0xA1) * 94 + (Code2 - 0xA1);
		}
		else if ((Code1 >= 0xA8) && (Code1 <= 0xA9) && (Code2 < 0xA1))	//5区符号
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeAddr = (Code1 - 0xA8) * 96 + (Code2 - 0x40) + 846;
		}		
		else if ((Code1 >= 0xB0) && (Code1 <= 0xF7) && (Code2 >= 0xA1))	//2区汉字
		{
			CodeAddr = (Code1 - 0xB0) * 94 + (Code2 - 0xA1) + 1038;
		}
		else if ((Code1 >= 0x81) && (Code1 <= 0xA0))		//3区汉字
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeAddr = (Code1 - 0x81) * 190 + (Code2 - 0x40) + 7806;	//1038 + 6768
		}
		else if ((Code1 >= 0xAA) && (Code2 < 0xA1))			//4区汉字
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeAddr = (Code1 - 0xAA) * 96 + (Code2 - 0x40) + 13886;	//1038 + 12848
		}
		else
		{
			CodeAddr = 0;		//其他编码均为保留码
		}
	}		//双字节处理结束
	else
	{
	    CodeAddr =	(Code1 - 0x81) * 12600 + (Code2 - 0x39) * 1260 + (Code3 - 0xEE) * 10 + (Code4 - 0x39) + 22046;
		 						  
// 		if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 == 0xEE) && (Code4 == 0x39))
// 		{	//高位8139的单独一个字符
// 			CodeAddr = 22046;
// 		}
// 		else if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 >= 0xEF))
// 		{	//8139页剩余部分
// 			CodeAddr = (Code3 - 0xEF) * 10 + (Code4 - 0x30) + 22047;	//22046 + 1
// 		}
// 		else if (	(Code1 == 0x82) && \
// 							((Code2 >= 0x30) && (Code2 <= 0x34)) && \
// 							((Code3 >= 0x81) && (Code3 <= 0xFE)))
// 		{	//8230页~8234页,均为满页
// 			CodeAddr = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
// 		}
// 		else if ((Code1 == 0x82) && \
// 						 (Code2 == 0x35) && \
// 						((Code3 >= 0x81) && (Code3 <= 0x87)))
// 		{	//8235页,只有69个字符
// 			CodeAddr = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
// 		}
// 		else
// 		{	//其他的一律为保留位,全空
// 			CodeAddr = 0;
// 		}
	}
	
	CodeAddr = GB18030_24X24_BASE_ADDR + CodeAddr * BYTES_OF_HZ_FONT_A;
	//读取字模
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_GB18030;
	g_tCharMaskReadBuf.FontType = HZ_FONT_A;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//设为正在写入
	#if defined	DBG_PROCESS_TIME_READ_ZIKU_ONE_HZ	//读一个汉字的点阵字库数据耗时
		StartProcessTimeCounter();
		sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, CodeAddr, BYTES_OF_HZ_FONT_A);
		StopProcessTimeCounter();
		iDbgProcessTimeTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#else
		sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, CodeAddr, BYTES_OF_HZ_FONT_A);
	#endif
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(g_tCharMaskReadBuf.Buf, BYTES_OF_HZ_FONT_A);
	#endif

	#ifdef	DBG_SPI_ZIKU
		UpLoadData(g_tCharMaskReadBuf.Buf, BYTES_OF_HZ_FONT_A);	//上传到PC
		InitCharMaskBufParameter();	//设为空闲,并清空BUF
	#else
		g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//设为等待处理
	#endif
}

/*******************************************************************************
* Function Name  : PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : 打印自定义汉字,如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,
*				   否则,打印当前行.
* Input          : CodeHigh=汉字代码高位,CodeLow=汉字代码低位
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
//以下为高速POS处理流程
void PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint32_t	i;
	uint8_t	* pselfdef;	
	uint8_t * pbuf = NULL;
	
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_CHINESE;
	g_tCharMaskReadBuf.FontType = HZ_FONT_A;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//设为正在写入
	pbuf		 = g_tCharMaskReadBuf.Buf;
	pselfdef = g_tRamBuffer.UserCharStruct.Buffer_HZ + (CodeLow - 0xA1) * BYTES_OF_HZ_FONT_A;	//用户定义汉字字模区首地址
	for (i = 0; i < BYTES_OF_HZ_FONT_A; i++)
	{
		 *pbuf++ = *pselfdef++;
	}
	pbuf = NULL;
	
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//设为等待处理
}


/*******************************************************************************
* Function Name  : void GetAscIIDotData(uint8_t AscIICode,uint8_t DotData[])
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理两种字体9*17和12*24
* Output         : 
* Return         : 
*******************************************************************************/
//以下为两种字体合并处理的程序
/*******************************************************************************
* Function Name  : 
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址,并启动读取
*	先处理是否是自定义字符,直接从RAM中复制;否则就在字库FLASH中,启动SPI DMA
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII字符点阵查表程序
{
	//定义了自定义的字符,且选择了,且对应字符有定义
	if ((g_tSystemCtrl.UserSelectFlag) && \
			(AscIICode < 0x7F) && \
			((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x01) == 0x01))
	{
		if (g_tCharMaskReadBuf.FontType == ASCII_FONT_A)	//字体A
		{
			if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
				GetAscIIUserDefDotData(AscIICode);	//读取用户自定义字符的点阵数据
			else
				GetAscIIFlashDotData(AscIICode);		//读取字库中的点阵数据
		}
		else if (g_tCharMaskReadBuf.FontType == ASCII_FONT_B)	//字体B
		{
			if (g_tRamBuffer.UserCharStruct.UserCode_09[AscIICode - 0x20])
				GetAscIIUserDefDotData(AscIICode);	//读取用户自定义字符的点阵数据
			else
				GetAscIIFlashDotData(AscIICode);		//读取字库中的点阵数据
		}
		else	//默认A
		{
			if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
				GetAscIIUserDefDotData(AscIICode);	//读取用户自定义字符的点阵数据
			else
				GetAscIIFlashDotData(AscIICode);		//读取字库中的点阵数据
		}
	}
	else
	{
		GetAscIIFlashDotData(AscIICode);		//读取字库中的点阵数据
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址,并启动读取
*	处理是否是自定义字符,直接从RAM中复制
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIUserDefDotData(uint8_t AscIICode)	//读取用户自定义字符的点阵数据
{	
	uint32_t	len;
	uint32_t	StartAddress;
	uint8_t * pselfdef;
	uint8_t * pbuf;
	
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		len = BYTES_OF_ASCII_FONT_A;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_A;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
		pselfdef += StartAddress;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		len = BYTES_OF_ASCII_FONT_B;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_B;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
		pselfdef = g_tRamBuffer.UserCharStruct.Buffer_09;
		pselfdef += StartAddress;
	}
	else		//防止参数错误,使用默认值
	{
		len = BYTES_OF_ASCII_FONT_A;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_A;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
		pselfdef += StartAddress;
	}
	
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//设为正在写入
	pbuf = g_tCharMaskReadBuf.Buf;
	while (len--)
	{
		*pbuf++ = *pselfdef++;
	}
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_ENGLISH;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//设为等待处理
}

#if	defined (DBG_SELF_DEF_CODEPAGE)
void	innerFLASH_ReadBuffer(uint8_t * pChar, uint32_t iAddressBuf, uint32_t	iLen)
{
	uc8	* pbFlash;
	
	if (g_tCharMaskReadBuf.FontType == ASCII_FONT_A)
	{
		pbFlash = CodepageFontA;
		pbFlash += iAddressBuf;
	}
	else if (g_tCharMaskReadBuf.FontType == ASCII_FONT_B)
	{
		pbFlash = CodepageFontB;
		pbFlash += iAddressBuf;
	}
	
	while (iLen--)
	{
		*pChar++ = *pbFlash++;
	}
}
#endif

#ifdef	DBG_025
uc8	mcAscII_0_0917_0[] = 
{
	/*  0[0x0030]   9x17  */
0x0F,0xF8,0x00,0x1F,0xFE,0x00,0x20,0x02,0x00,0x20,0x03,0x00,0x60,0x01,0x00,0x20,
0x03,0x00,0x30,0x06,0x00,0x1F,0xFC,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_2_0917_0[] = 
{
	/*  2[0x0032]   9x17  */
0x08,0x07,0x00,0x18,0x0F,0x00,0x30,0x13,0x00,0x60,0x23,0x00,0x60,0x63,0x00,0x60,
0xC3,0x00,0x31,0x83,0x00,0x1F,0x03,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_5_0917_0[] = 
{
	/*  5[0x0035]   9x17  */
0x00,0x0C,0x00,0x3F,0x86,0x00,0x21,0x03,0x00,0x21,0x01,0x00,0x21,0x01,0x00,0x21,
0x03,0x00,0x21,0xCE,0x00,0x20,0x78,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_0_0917_1[] = 
{
	/*  0[0x0030]   9x17  */
0x00,0x00,0x00,0x03,0xF8,0x00,0x04,0x04,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,
0x02,0x00,0x04,0x04,0x00,0x03,0xF8,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_2_0917_1[] = 
{
	/*  2[0x0032]   9x17  */
0x00,0x00,0x00,0x07,0x06,0x00,0x08,0x0A,0x00,0x08,0x12,0x00,0x08,0x22,0x00,0x08,
0xC2,0x00,0x07,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_5_0917_1[] = 
{
	/*  5[0x0035]   9x17  */
0x00,0x00,0x00,0x0F,0xCC,0x00,0x08,0x42,0x00,0x08,0x82,0x00,0x08,0x82,0x00,0x08,
0x82,0x00,0x08,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_0_1224_0[] = 
{
	/*  0[0x0030]  12x24  */
0x0F,0xFF,0xE0,0x1F,0xFF,0xF0,0x18,0x00,0x18,0x30,0x00,0x0C,0x30,0x00,0x0C,0x30,
0x00,0x0C,0x30,0x00,0x0C,0x30,0x00,0x0C,0x18,0x00,0x18,0x1F,0xFF,0xF0,0x07,0xFF,
0xC0,0x00,0x00,0x00,
};
uc8	mcAscII_2_1224_0[] = 
{
	/*  2[0x0032]  12x24  */
0x06,0x00,0x3C,0x0E,0x00,0xFC,0x18,0x01,0xCC,0x30,0x03,0x8C,0x30,0x07,0x0C,0x60,
0x06,0x0C,0x60,0x0C,0x0C,0x30,0x18,0x0C,0x38,0x30,0x0C,0x1F,0xE0,0x0C,0x0F,0x80,
0x0C,0x00,0x00,0x00,
};
uc8	mcAscII_5_1224_0[] = 
{
	/*  5[0x0035]  12x24  */
0x00,0x00,0x00,0x00,0x00,0x70,0x3F,0xF0,0x38,0x30,0x30,0x0C,0x30,0x60,0x0C,0x30,
0x60,0x0C,0x30,0x60,0x0C,0x30,0x60,0x0C,0x30,0x70,0x18,0x30,0x3C,0x78,0x30,0x1F,
0xE0,0x00,0x00,0x00,
};
uc8	mcAscII_0_1224_1[] = 
{
	/*  0[0x0030]  12x24  */
0x00,0x00,0x00,0x00,0x7F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x06,0x00,0x18,0x04,
0x00,0x08,0x04,0x00,0x08,0x06,0x00,0x18,0x03,0x80,0x70,0x01,0xFF,0xE0,0x00,0x7F,
0x80,0x00,0x00,0x00,

};
uc8	mcAscII_2_1224_1[] = 
{
	/*  2[0x0032]  12x24  */
0x00,0x00,0x00,0x01,0xC0,0x38,0x02,0xC0,0x58,0x04,0x00,0x98,0x04,0x01,0x18,0x04,
0x02,0x18,0x04,0x04,0x18,0x06,0x1C,0x18,0x03,0xF8,0x18,0x01,0xE0,0xF8,0x00,0x00,
0x00,0x00,0x00,0x00,
};
uc8	mcAscII_5_1224_1[] = 
{
	/*  5[0x0035]  12x24  */
0x00,0x00,0x00,0x00,0x00,0xE0,0x07,0xFC,0xD0,0x06,0x08,0x08,0x06,0x10,0x08,0x06,
0x10,0x08,0x06,0x10,0x08,0x06,0x18,0x38,0x06,0x0F,0xF0,0x06,0x07,0xC0,0x00,0x00,
0x00,0x00,0x00,0x00,
};

void	innerFLASH_ReadBuffer(uint8_t * pChar, uint32_t iAddressBuf, uint32_t	iLen)
{
	uc8	* pbFlash;
	
	if (iAddressBuf == 0x00)	//0,
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_0_1224_0;
			else
				pbFlash = mcAscII_0_1224_1;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_0_0917_0;
			else
				pbFlash = mcAscII_0_0917_1;
		}
	}
	else if (iAddressBuf == 0x02)	//2,
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_2_1224_0;
			else
				pbFlash = mcAscII_2_1224_1;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_2_0917_0;
			else
				pbFlash = mcAscII_2_0917_1;
		}
	}
	else if (iAddressBuf == 0x05)	//5
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_5_1224_0;
			else
				pbFlash = mcAscII_5_1224_1;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_5_0917_0;
			else
				pbFlash = mcAscII_5_0917_1;
		}
	}
	
	//pbFlash += iAddressBuf;
	
	while (iLen--)
	{
		*pChar++ = *pbFlash++;
	}
}

#endif

/*******************************************************************************
* Function Name  : 
* Description    : 获得ASCII字符的点阵起始地址,
*	AscIICode = 字符值
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t	GetUSACharacterStartAddress(uint8_t AscIICode)
{
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
		return	(0);

#ifdef	DBG_025
	if (g_bDbg025EnalbeFlag)
	{
		if ((AscIICode == 0x30) || (AscIICode == 0x32) || (AscIICode == 0x35))	//0,2,5
		{
			AsciiCodeAddr = AscIICode & 0x0F;
		}
		else
		{
			if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
			{
				AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
			}
			else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
			{
				AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
			}
		}
	}
	else
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
		{
			AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
		{
			AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
		}
	}
#else
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
	}
#endif
	
	return (AsciiCodeAddr);
}

/*支持三种字体*/
uint32_t	GetUSACharStartAddress(uint8_t	AscIICode, uint8_t FontType)
{
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
	{
		return	(0);
	}
	
	if (FontType == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
	}
	else if (FontType == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
	}
	else if (FontType == ASCII_FONT_C)	//FLASH中英文8x16点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_08_16_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_C;
	}
	else
	{
		AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
	}
	
	return (AsciiCodeAddr);
}

/*******************************************************************************
* Function Name  : 
* Description    : 获得国际字符的点阵起始地址,如不是国际字符则返回ASCII字符的地址
*	AscIICode = 字符值
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t	GetInternationalCharacterStartAddress(uint8_t	AscIICode)
{
	uint32_t	i;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
		return	(0);
	
	for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//查是否为国际字符
	{
		if (AscIICode == m_tbCharacterSetTable[i])
			break;
	}
	if (i < BYTES_OF_CHAR_SET_TABLE)							//有要替换的字符存在
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_12_24_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文12x24点阵的ASCII字符
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_09_17_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_B;
		}
	}		//有要替换字符处理完毕
	else
	{
		AsciiCodeAddr = GetUSACharacterStartAddress(AscIICode);
	}		//相同的字符处理完毕

	return (AsciiCodeAddr);
}

/*支持三种字体*/
uint32_t	GetInternationalCharStartAddress(uint8_t	AscIICode, uint8_t FontType)
{
	uint32_t	i;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
		return	(0);
	
	for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//查是否为国际字符
	{
		if (AscIICode == m_tbCharacterSetTable[i])
		{
			break;
		}
	}
	
	if (i < BYTES_OF_CHAR_SET_TABLE)							//有要替换的字符存在
	{
		if (FontType == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_12_24_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
		}
		else if (FontType == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_09_17_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_B;
		}
		else if (FontType == ASCII_FONT_C)	//FLASH中英文8x16点阵的ASCII字符
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_08_16_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_C;
		}
		else
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_12_24_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
		}
	}		//有要替换字符处理完毕
	else
	{
		AsciiCodeAddr = GetUSACharStartAddress(AscIICode, FontType);
	}		//相同的字符处理完毕

	return (AsciiCodeAddr);
}

/*******************************************************************************
* Function Name  : 
* Description    : 获得指定代码页字符的点阵起始地址
*	AscIICode = 字符值
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t	GetCodepageCharacterStartAddress(uint8_t	AscIICode)
{
	uint8_t	CodePage;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < CODEPAGE_START_CODE)
		return	(0);
	
	CodePage = g_tSysConfig.DefaultCodePage;		 //国家的代码页的选择
#if	defined (DBG_SELF_DEF_CODEPAGE)
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		AsciiCodeAddr = (CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		AsciiCodeAddr = (CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_B;
	}
#else
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_12_24_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_09_17_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_B;
	}
#endif

	return (AsciiCodeAddr);
}

/* 支持三种字体 */
uint32_t	GetCodepageCharStartAddress(uint8_t	AscIICode, uint8_t FontType)
{
	uint8_t	CodePage;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < CODEPAGE_START_CODE)
	{
		return	(0);
	}
	
	CodePage = g_tSysConfig.DefaultCodePage;		 //国家的代码页的选择
	if (FontType == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_12_24_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	else if (FontType == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_09_17_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_B;
	}
	else if (FontType == ASCII_FONT_C)	//FLASH中英文8x16点阵的ASCII字符
	{
		AsciiCodeAddr = ASCII_08_16_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_C;
	}
	else
	{
		AsciiCodeAddr = ASCII_12_24_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	
	return (AsciiCodeAddr);
}

/*******************************************************************************
* Function Name  : 
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址,并启动读取
*	处理在字库FLASH中,启动SPI DMA
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIFlashDotData(uint8_t AscIICode)	//读取字库中的点阵数据
{
	uint32_t	len;
	uint32_t	AsciiCodeAddr;
	#ifdef	DBG_PROCESS_TIME
		uint32_t	iDbgTemp;
	#endif
	
	if (AscIICode < 0x80)
	{
		if (g_tSystemCtrl.CharacterSet)	//如果设置了国际字符集指令 0x1b 52 n,USA=0
		{
			AsciiCodeAddr = GetInternationalCharacterStartAddress(AscIICode);
		}		//End 国际字符
		else
		{
			AsciiCodeAddr = GetUSACharacterStartAddress(AscIICode);
		}
	}			//End <0x80
	else	//----FLASH中0x80-0xFF点阵的ASCII字符----//
	{
		AsciiCodeAddr = GetCodepageCharacterStartAddress(AscIICode);
	}
	
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH中英文12x24点阵的ASCII字符
	{
		len = BYTES_OF_ASCII_FONT_A;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		len = BYTES_OF_ASCII_FONT_B;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_B;
	}
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_ENGLISH;
	
	//下面设置SPI字库DMA读取开始
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//设为正在写入
	
	#if	defined (DBG_SELF_DEF_CODEPAGE)
		if (AscIICode < 0x80)
		{
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
		else	//目前只有代码页测试
		{
			innerFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
	#elif	defined (DBG_025)
		//目前只测试0,2,5
		if ((AscIICode == 0x30) || (AscIICode == 0x32) || (AscIICode == 0x35))
		{
			innerFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
		else
		{
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
	#else
		#ifdef	DBG_PROCESS_TIME_READ_ZIKU_ONE_ASCII		//读一个ASCII的点阵字库数据耗时
			StartProcessTimeCounter();
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
			StopProcessTimeCounter();
			iDbgTemp = 0;
			g_iDbgProcessTimeUs = 0;
		#else
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		#endif
	#endif
	
	#ifdef	DBG_SPI_ZIKU
		UpLoadData(g_tCharMaskReadBuf.Buf, len);	//上传到PC
		InitCharMaskBufParameter();	//设为空闲,并清空BUF
	#else
		g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//设为等待处理
	#endif

	#ifdef	ZK_ENCRYPTION
		FlashDecryption(g_tCharMaskReadBuf.Buf, len);
	#endif
}

/*******************************************************************************
* Function Name  : FlashDecryption()
* Description    : 字库字模解码
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	FlashDecryption(uint8_t	* Src, uint32_t ByteNum)
{
	uint32_t	i;
	
	if (ByteNum > MAX_BYTES_OF_HZ)
	{
		return;
	}
	
	for (i = 0; i < ByteNum; i++)
	{
		Src[i] = Src[i] ^ 0x79;        
		Src[i] = ~Src[i];
	}
}

/*******************************************************************************
* Function Name  : PrtAsciiToBuf
* Description    : 打印测试内容 2017.03.20
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrtAsciiToBuf(uint8_t Code)
{
		PrintAscII(Code);
		while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
		{
			ProcessCharMaskReadBuf();
		}
}	
/*******************************************************************************
* Function Name  : PrintString
* Description    : 打印字符串.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrintString(const uint8_t * Str)
{
	uint8_t	Ch;
	uint8_t	bTemp;
	const uint8_t * pStr = NULL;
	
	pStr = Str;
	bTemp = *pStr;
	while (bTemp)
	{
		if ((bTemp == 0x0A) || (bTemp == 0x0D))
		{
// 			PrintOneLine();
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		}
		else if ((bTemp >= 0x20) && (bTemp < 0x80))
		{
			while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
			{
				;
			}
			PrintAscII( bTemp );			//存入数据
			while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
			{
				ProcessCharMaskReadBuf();
			}
		}
		else if	(bTemp >= 0x80)
		{
			if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
			{
				Ch = *pStr;
				pStr++;
				while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
				{
					;
				}
				PrintGB18030(Ch, *pStr, 0, 0);
				while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
				{
					ProcessCharMaskReadBuf();
				}
			}
			else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)
			{
				Ch = *pStr;
				pStr++;
				while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
				{
					;
				}
				PrintBIG5(Ch, *pStr);
				while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
				{
					ProcessCharMaskReadBuf();
				}
			}
			else
			{
				while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
				{
					;
				}
				PrintAscII( *pStr );
				while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
				{
					ProcessCharMaskReadBuf();
				}
			}
		}
		
		pStr++;
		bTemp = *pStr;
	}
#if	0	
	while (*pStr)
	{
		if ((*pStr == 0x0A) || (*pStr == 0x0D))
		{
			PrintOneLine();
		}
		else if ((*pStr >= 0x20) && (*pStr < 0x80))
		{
			PrintAscII( *pStr );			//存入数据
			ProcessCharMaskReadBuf();
		}
		else if	(*pStr >= 0x80)
		{
			if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
			{
				Ch = *pStr;
				pStr++;
				PrintGB18030(Ch, *pStr, 0, 0);
				ProcessCharMaskReadBuf();
			}
			else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)
			{
				Ch = *pStr;
				pStr++;
				PrintBIG5(Ch, *pStr);
				ProcessCharMaskReadBuf();
			}
			else
			{
				PrintAscII( *pStr );
				ProcessCharMaskReadBuf();
			}
		}
		
		pStr++;
	}
#endif
}

/*******************************************************************************
* Function Name  : GetRatioXValue
* Description    : 计算当前字符的横向放大倍数
*				   
* Input          : CodeType: =0,西文,=1汉字
* Output         : 
* Return         : 返回横向放大倍数
*******************************************************************************/
static uint8_t	GetRatioXValue(uint8_t CodeType)
{
	uint8_t	RatioX;
		
	if (CodeType)	//汉字方式,取横向放大倍数
	{
		if ((g_tPrtModeHZ.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//行内倍宽
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeHZ.WidthMagnification;
		}		
	}
	else					//字符方式
	{
		if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//行内倍宽
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeChar.WidthMagnification;
		}		
	}
	
	return RatioX;
}

/*******************************************************************************
* Function Name  : JudgeLineBufFull
* Description    : 判断是否能放下该字符,否则,打印当前行.
* Input          : CodeType: =0,西文,=1汉字
* Output         : 
* Return         : 能放下返回0,不能放下返回1
*******************************************************************************/
uint8_t	JudgeLineBufFull(uint8_t CodeType)
{
	uint8_t	RatioX;
	uint8_t	Result;
	uint16_t	Width;
	
	if (CodeType)	//汉字方式,取横向放大倍数
	{
		if ((g_tPrtModeHZ.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//行内倍宽
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeHZ.WidthMagnification;
		}
		
		if (g_tPrtCtrlData.CharCycle90)
		{
			Width = RatioX * g_tPrtModeHZ.CharHigh;
		}
		else
		{
			Width = RatioX * (g_tPrtModeHZ.LeftSpace + g_tPrtModeHZ.CharWidth);
		}
	}
	else					//字符方式
	{
		if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//行内倍宽
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeChar.WidthMagnification;
		}
		
		if (g_tPrtCtrlData.CharCycle90)
		{
			Width = RatioX * g_tPrtModeChar.CharHigh;
		}
		else
		{
			Width = RatioX * (g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth + g_tPrtModeChar.RightSpace);    //填充字符时候 需要增加上有间距
		}
	}		//计算宽度点数
	#if	1
	//如果是行首,保证设置的左边距至少能打印一字符
	if ((g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit) && \
			((g_tPrtCtrlData.PrtDataDotIndex + Width) >= g_tPrtCtrlData.PrtLength))
	{
		if (g_tPrtCtrlData.PrtLength > Width)
		{
			g_tPrtCtrlData.PrtLeftLimit = g_tPrtCtrlData.PrtLength - Width;
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
		}
	}
	#endif
	
	if ((g_tPrtCtrlData.PrtDataDotIndex + Width) > g_tPrtCtrlData.PrtLength)
	{
		Result = LINE_BUF_FULL;
	}
	else
	{
		Result = LINE_BUF_NOT_FULL;
	}
	
	return (Result);
}

/*******************************************************************************
* Function Name  : FillDataPrintBuff(uint8_t Data, uint8_t Width, uint8_t Line)
* Description    : 打印缓冲区填充相同的数据.
* Input          : Data：输入数据, Width：数据宽度,Line:行数
* Output         : 改变存数指针
* Return         : 
*******************************************************************************/
void	FillDataPrintBuff(uint8_t Data, uint32_t Width, uint32_t Line)
{
	uint32_t	i,j;
	uint8_t	* p = NULL;
	uint8_t	* ptemp = NULL;
	
	if (Line <= MAX_PRT_HEIGHT_BYTE)		//防止错误输入数据造成下标越界
	{
		//先找到列的首地址
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - Line) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
		for (i = 0; i < Line; i++)
		{
			//p = ptemp + i * PRT_DOT_NUMBERS;
			p = ptemp;
			for (j = 0; j < Width; j++)
			{
				*p++ = Data;
			}
			ptemp += PRT_DOT_NUMBERS;
		}
	}
	
	p = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t RatioX ,uint8_t CodeType)
* Description    : 指定地址数据存入打印缓冲区.
* Input          : *Source：输入数据地址, CharWidth：字符宽度, RatioX:横向倍数, 
*										CodeType: 字符类型 =1汉字,=0字符
* Output         : 改变存数指针
* Return         : 
*******************************************************************************/
void	FillPrintBuff(uint8_t * Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t CodeType)
{
	uint8_t	Line;
	uint8_t	RatioY; 
	uint8_t	Ratio;
	uint8_t	i;
	uint8_t	Temp;
	uint8_t	RatioX;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	iTemp;
	uint16_t	Width;
	uint16_t	LeftSpace;
	uint16_t	RightSpace;
	uint8_t	* p = NULL;
	uint8_t	* ptemp = NULL;
	
	if (CharWidth > MAX_FONT_WIDTH)
		CharWidth = MAX_FONT_WIDTH;
	if (CharHigh > MAX_FONT_HEIGHT)
		CharHigh = MAX_FONT_HEIGHT;
	
	RatioX = GetRatioXValue(CodeType);
	
	//字符左.右间距,纵向放大倍数等
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)			//行模式
	{
		if (CodeType == LANGUAGE_ENGLISH) 		//字符
		{
			LeftSpace		= RatioX * g_tPrtModeChar.LeftSpace;
			RightSpace	= RatioX * g_tPrtModeChar.RightSpace;
			RatioY			= g_tPrtModeChar.HeightMagnification;
			if (g_tPrtCtrlData.MaxRatioY < g_tPrtModeChar.HeightMagnification)
				g_tPrtCtrlData.MaxRatioY = g_tPrtModeChar.HeightMagnification;
			
			#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
			if (g_tPrtCtrlData.MaxRatioX < g_tPrtModeChar.WidthMagnification)
				g_tPrtCtrlData.MaxRatioX = g_tPrtModeChar.WidthMagnification;
			#endif
		}
		else		//汉字
		{
			LeftSpace		= RatioX * g_tPrtModeHZ.LeftSpace;
			RightSpace	= RatioX * g_tPrtModeHZ.RightSpace;
			RatioY			= g_tPrtModeHZ.HeightMagnification;
			if (g_tPrtCtrlData.MaxRatioY < g_tPrtModeHZ.HeightMagnification)
				g_tPrtCtrlData.MaxRatioY = g_tPrtModeHZ.HeightMagnification;
			
			#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
			if (g_tPrtCtrlData.MaxRatioX < g_tPrtModeHZ.WidthMagnification)
				g_tPrtCtrlData.MaxRatioX = g_tPrtModeHZ.WidthMagnification;
			#endif
		}
	}
	else			//页模式
	{
		if (CodeType == LANGUAGE_ENGLISH) 		//字符
		{
			LeftSpace	 = 0;
			RightSpace = RatioX * g_tPageMode.AscRightSpace;
			RatioY		 = g_tPrtModeChar.HeightMagnification;
		}
		else		//汉字
		{
			LeftSpace	 = RatioX * g_tPageMode.HZLeftSpace;
			RightSpace = RatioX * g_tPageMode.HZRightSpace;
			RatioY		 = g_tPrtModeHZ.HeightMagnification;
		}
	}
	
	if (g_tPrtCtrlData.LineHigh < CharHigh)	//一行中最高字符处理
	{
		g_tPrtCtrlData.LineHigh = CharHigh;		//计算字符高度
	}
	if (g_tPrtCtrlData.MaxPrtDotHigh < CharHigh * RatioY)	//一行中最高打印点数处理
	{
		g_tPrtCtrlData.MaxPrtDotHigh = CharHigh * RatioY;	//临时处理
	}
	
	
	//处理左间距
	if (g_tPrtCtrlData.CharNotFlag == 1)		//反白打印(反显)
	{
		FillDataPrintBuff(0xFF, LeftSpace, 3 * RatioY);
	}
	g_tPrtCtrlData.PrtDataDotIndex += LeftSpace;		//更新列计数值
	
	//处理字符本身
	//按照24点高纵向分成3排,每次处理1排,处理每排时从左到右,每次处理1个字节,
	//每排处理前指向该排首地址
	ptemp = g_tLineEditBuf + (MAX_RATIO_Y - RatioY) * 3 * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
	for (Line = 0; Line < 3; Line++)
	{
		p = ptemp + RatioY * Line * PRT_DOT_NUMBERS;
		for (Width = 0; Width < CharWidth; Width++)	//字符宽
		{
			Temp = Source[3 * Width + Line];					//依次处理每列,共3列,因为字符最大24点高
			if (Temp)	//不为零
			{
				if (RatioY == 1)
				{
					TempBuf[0] = Temp;
				}
				else
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);	//字节处理缓冲区使用前先清空
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
					ByteZoomCtrl(RatioY, Temp, TempBuf);	//纵向放大处理
				}
			}
			else	//为零
			{
				if (RatioY == 1)
				{
					TempBuf[0] = Temp;
				}
				else
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
				}
			}
			
			for (Ratio = 0; Ratio < RatioX; Ratio++)
			{
				for (i = 0; i < RatioY; i++)
				{
					if (g_tPrtCtrlData.CharNotFlag == 0)
						*(p + PRT_DOT_NUMBERS * i) = TempBuf[i];
					else
						*(p + PRT_DOT_NUMBERS * i) = (uint8_t)(~TempBuf[i]);	//反白打印
				}		//End 纵向放大倍数
				p++;
			}			//End 横向放大倍数
		}			 	//Een 字符宽
	}				 	//End 字符高
	g_tPrtCtrlData.PrtDataDotIndex += (RatioX * CharWidth);	//更新列计数值
	
	//处理右间距
	//修正右间距宽度,保证能放下该字符,字符宽＋右间距 < 可打印区
	if ((g_tPrtCtrlData.PrtDataDotIndex + RightSpace) > g_tPrtCtrlData.PrtLength)
		RightSpace = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
	if (g_tPrtCtrlData.CharNotFlag == 1)		//处理反白打印(反显)
	{
		FillDataPrintBuff(0xFF, RightSpace, 3 * RatioY);
	}
	g_tPrtCtrlData.PrtDataDotIndex += RightSpace;	//更新列计数值
	
	//下划线填充,反显 / 旋转90 打印时不打印下划线
	#ifdef	UNDER_LINE_PRT_ENABLE		//2012-9-21添加处理下划线打印点行数
	if ((g_tPrtCtrlData.CharNotFlag == 0) && (g_tPrtCtrlData.CharCycle90 == 0))
	{
		Temp = 0;
		if (CodeType == LANGUAGE_ENGLISH)	  //字符
		{
			if (g_tPrtModeChar.Underline & UNDERLINE_ENABLE_MASK)
				Temp = g_tPrtModeChar.Underline & 0x07;
		}
		else				  //汉字
		{
			if (g_tPrtModeHZ.Underline & UNDERLINE_ENABLE_MASK)
				Temp = g_tPrtModeHZ.Underline & 0x07;
		}
		
		if (Temp)	//下划线点数不为0
		{
			if (g_tPrtCtrlData.DoUnderlineFlag == 0)
			{
				g_tPrtCtrlData.DoUnderlineFlag = 1;
			}
			
			if (Temp == 0x07)				//3点高
			{
				g_tPrtCtrlData.UnderlineMaxPrtHigh = 3;
			}
			else if (Temp == 0x03)	//2点高
			{
				//if (g_tPrtCtrlData.UnderlineMaxPrtHigh < Temp)
				{
					g_tPrtCtrlData.UnderlineMaxPrtHigh = 2;
				}
			}
			else if (Temp == 0x01)	//1点高
			{
				//if (g_tPrtCtrlData.UnderlineMaxPrtHigh < Temp)
				{
					g_tPrtCtrlData.UnderlineMaxPrtHigh = 1;
				}
			}
			//2012-9-21添加结束
			iTemp = LeftSpace + (RatioX * CharWidth) + RightSpace;	//前面全部加完了
			p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex - iTemp;
			for (Width = 0; Width < iTemp; Width++)
			{
				*p++ |= Temp;
			}
		}		//下划线点数不为0修改结束
		else
		{
			if((g_tPrtModeChar.Underline & UNDERLINE_ENABLE_MASK == 0) && (g_tPrtModeHZ.Underline & UNDERLINE_ENABLE_MASK == 0))
			{
				g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;			//2016.07.09
			}	
		}	
	}		//下划线处理结束
	#endif
	
	p = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : void ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t DscPrt[])
* Description    : 将一字节的数据进行放大处理
* Input          :	ZoomRate:放大倍数
*									ByteData: 原始数据
*									DscPrt[]:输出数据指针
* Output         : 
* Return         : 
* 特别说明：	 ：
*******************************************************************************/
void	ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t *DscPrt)
{
	uint8_t	Temp0, i;
	unsigned long long	Temp1;
	
	typedef union
	{
		unsigned long long	Temp;
		uint8_t	bBuf[sizeof(unsigned long long)];
	}Temp_Union;
	
	Temp_Union	TempUnion;
	
	if (ZoomRate > 8)	//判断放大倍数是否超过最大定义值  //2017.08.25 MAX_RATIO_Y改为8
	{
		return;
	}
	
	if (ZoomRate < 2)		//放大倍数为1时
	{
		DscPrt[0] = ByteData;
	}
	else
	{
		Temp0 = ByteData;
		Temp1 = 0xFF >> (8 - ZoomRate);
		TempUnion.Temp = 0;
		for (i = 0; i < 8; i++)
		{
			if (Temp0 & 0x01)
				TempUnion.Temp |= Temp1 ;
			Temp1 <<= ZoomRate;
			Temp0 >>= 1;
		}
		
		for (i = 0; i < ZoomRate; i++)
			DscPrt[i] = TempUnion.bBuf[ZoomRate - 1 - i];
			//DscPrt地址低存放的是字节高,地址高存放的字节低
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将传入的一个字符在高度上进行比例缩放,目前的程序只能整数倍放大
*	放大时有最大放大倍数的限制.在此限制内可以进行任意倍数的放大.放大结果存放时,
*	指针指向的起始地址存放的是字符点阵的MSB部分放大后变换的结果,最终地址存放的是
*	字符点阵的LSB部分放大后变换的结果.放大后字符点阵为1的点的总数目没有变化,所以
*	打印出来后显得高但是稀疏.
*	处理的算法：从MSB开始依次取该字节的某1位,如果为1,则在对应结果位置处置1,然后
*	添入0.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	ZoomByteHeight(uint8_t ByteData, uint8_t ZoomRate, uint8_t * DscPrt)
{
	uint32_t	i;
	unsigned long long temp;		//存放变换后结果,放大n倍需要n个字节空间,当前的用法最大可放大8倍
	
	if (ZoomRate > MAX_RATIO_Y)	//判断放大倍数是否超过最大定义值
	{
		return;
	}
	
	if (ZoomRate == 1)		//放大倍数为1时,不需要进行处理
	{
		DscPrt[0] = ByteData;
	}
	else									//放大倍数 > 1时
	{
		temp = 0;
		for (i = 0; i < 8; i++)		//从该字节的MSB开始处理每位的值
		{
			if ((ByteData >> 7) == 1)	//判断该字节MSB位是否为1
			{
				temp |= (1<<(ZoomRate * (8 - i) - 1));	//将放大后结果的对应位置1
			}
			ByteData <<= 1;					//去掉前面处理过的位,将后面待处理的位移到最高位
		}
		//处理完毕后,temp的(ZoomRate * 8 - 1)位开始存放的是ByteData的MSB开始处理后的结果.
		for (i = 0; i < ZoomRate; i++)	//不同放大倍数时占用到的位次不同
		{
			DscPrt[i] = (uint8_t)(temp >> (8 * (ZoomRate - i - 1)));
		}
	}

}

/*******************************************************************************
* Function Name  :	EmphaSizeProcess(uint8_t CodeWidth,uint8_t CodeHigh, uint8_t *SrcPrt)
* Description    : 	将字符的字模数据加粗
* Input          : 	CodeWidth：字符横向尺寸, 使用点为单位
* Input          : 	CodeHigh：字符纵向尺寸,使用点为单位
*				   					SrcPrt[]：输入数据指针
* Output         : 	SrcPrt[]：加粗后处理后的数据
* Return         : 	None
* 特别提示：	 ：		输入数据按标准排列
*	使用9*17或12*24点阵字符且旋转方向为90°时,输出数据字节会大于输入字节,在调用时,
*	注意保证SrcPrt[]的大小,按底部对齐方式排列输出数据
*	有待分析
*******************************************************************************/
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
{
	uint8_t	ArrayBuf;
	uint8_t	DataBuf;
	uint8_t	i;
	uint8_t	j;
	
	if (CodeWidth > MAX_FONT_WIDTH)
	{
		CodeWidth = MAX_FONT_WIDTH;
	}
	if (CodeHigh > MAX_FONT_HEIGHT)
	{
		CodeHigh = MAX_FONT_HEIGHT;
	}
	
	for (i = 0; i < CodeHigh; i++)
	{
		ArrayBuf = 0;
		for (j = 0; j < CodeWidth; j++)
		{
			DataBuf = SrcPrt[j * CodeHigh + i];
			SrcPrt[j * CodeHigh + i] |= ArrayBuf;
			ArrayBuf = DataBuf;
		}
	}
}

/*******************************************************************************
* Function Name  : uint8_t	CodeRollCtrl(uint8_t	CodeWidth, uint8_t CodeHigh,  uint8_t SrcPrt[])
* Description    : 将指定字符的字模数据旋转顺时针90°的程序
* Input          : CodeWidth：字符横向尺寸, 使用点为单位
* Input          : CodeHigh：字符纵向尺寸,使用点为单位
*				   SrcPrt[]：输入数据指针
* Output         : SrcPrt[]：旋转处理后的数据
* Return         : 旋转处理后的数据数量 
* 特别提示：	 ：输入数据按标准排列
				   使用 9*17 或 12*24 点阵字符且旋转方向为90°时.
					输出数据字节会大于输入字节,在调用时,注意保证SrcPrt[]的大小
				   按底部对齐方式排列输出数据
*	有待分析
*******************************************************************************/
void	CodeRollCtrl(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[])
{
	uint8_t i, j;
	uint8_t	HighByte;
	uint8_t	bBuf[MAX_BYTES_OF_HZ], bBuf1[MAX_BYTES_OF_HZ];
	uint32_t	ChangedByte;
	uint8_t	* ptr = NULL;
	
	if (CodeWidth > MAX_FONT_WIDTH)
		CodeWidth = MAX_FONT_WIDTH;
	if (CodeHigh > MAX_FONT_HEIGHT)
		CodeHigh = MAX_FONT_HEIGHT;
	
	memset(bBuf, 0x00, MAX_BYTES_OF_HZ);
  memset(bBuf1, 0x00, MAX_BYTES_OF_HZ);
	//memset(bBuf, 0x00, sizeof(bBuf) / sizeof(bBuf[0]));
	//memset(bBuf1, 0x00, sizeof(bBuf1) / sizeof(bBuf1[0]));
	HighByte = CodeHigh >> 3;
	if (CodeHigh % 8)
		HighByte++;
	for (i = 0; i < CodeWidth * HighByte; i++)
		bBuf[i] = SrcPrt[i];
	
	HighByte--;
 	ptr = bBuf1;
	for (i = 0; i < CodeHigh; i++)		//顺时针90°
	{
		ChangedByte = 0;
		for (j = 0; j < CodeWidth; j++)
		{
			ChangedByte <<= 1;
			//先把横排中所有字节的低位取出来放在转换后的低位上
			//if (bBuf[HighByte - i / 8 + j * 3] & (0x01 << (i % 8)))
			if (bBuf[HighByte - (i >> 3) + j + (j << 1)] & (uint8_t)(0x01 << (i % 8)))
				ChangedByte |= 0x01;
		}		
		*ptr++ = (ChangedByte >> 16);
		*ptr++ = (ChangedByte >> 8);
		*ptr++ = ChangedByte;
	}
	memcpy(SrcPrt, bBuf1, MAX_BYTES_OF_HZ);
	
	ptr = NULL;
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
