/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.1
  * @date    2016-4-2
  * @brief   This file provides all the firmware functions.
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
#include	<string.h>
#include	"print.h"
#include	"heat.h"
#include	"extgvar.h"
#include	"timconfig.h"
#include	"spiheat.h"
#include	"feedpaper.h"
#include	"defconstant.h"
#include	"debug.h"
#include	"parallel.h"
#include	"led.h"
#include	"beep.h"  //2016.06.24
extern	void LongToStr(uint32_t num, uint8_t *Str);
extern	void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);

extern	uc16	MaxStepNum[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern  const uint16_t g_tFeedMotorRushTable[];

extern	TypeDef_StructInBuffer volatile g_tInBuf;  //2016.06.08

#ifdef	DBG_PROCESS_TIME
extern	volatile uint32_t	g_iDbgProcessTimeUs;
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//定义两个行打印缓冲区
//TypeDef_StructLinePrtBuf	g_tLinePrtBuf;
uint8_t	g_tbHeatElementBuf[BYTES_OF_ALL_ELEMENT];	//加热机芯一点行所有的加热寄存器单元数

#ifdef	SPI_PRT_TX_BY_DMA_ENABLE	//全部使用DMA方式发送
	volatile uint8_t	g_bPrtSPIDMATxStatusFlag;
	volatile uint8_t	g_bPrtSPIDMAIntForDriveMode;
#endif

//static uint8_t m_bReversePrintUnderlineFlag = 0;
static uint8_t ReverseByte(uint8_t cSrc);
static void ReverseStreamEachBit(uint8_t * pbBuf, uint32_t iNumber);
static uint16_t	GetLinePrtBufStartColumnIndex(void);
static void	FillHeatElementBuf(uint8_t * PrintBuff, uint8_t Lines);
static void	LineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines);
static void	LineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines);
static void	SelectLineBufToPrint(void);
static void	WaitToPrintLineBufOfChar(void);
static void	WaitToPrintLineBufOfGraphic(void);
static void	WaitToPrintLineBuf(void);
static void	StartPrintLineBufChar(void);
static void	StartPrintLineBufGraphic(void);
static void	StartPrintLineBuf(void);
static void	CheckPrintContentFinishOfChar(void);
static void	CheckPrintContentFinishOfGraphic(void);
static void	CheckPrintContentFinish(void);
static void StartHeatAndRotateMotor(void);
static void SetGoLineSpace(void);
static void	UnderLineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines);
static void	UnderLineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines);
static void	FillUnderLineToHeatElementBuf(uint8_t * PrintBuff, uint8_t Lines);

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化发送到机芯的一点行数据,
*	当前机芯需要总点数640,实际可加热点数576,左右各32点值为无法加热区域
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitHeatElementBuf(void)
{
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
}

void	InitHeatElementBufLeftEmpty(void)
{
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化行打印缓冲区					
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#if	1
void	InitLinePrtBuf(void)
{
	uint8_t i,j;
	
	#if	0
	uint8_t	* pb = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	pb = ptLPB->Buf;
	if (ptLPB->PrtMode.DoUnderlineFlag)	//如果有下划线打印则全部清零
	{
		memset(pb, 0x00, LINE_PRT_BUF_LEN);
	}
	else	//否则无需清零下划线缓冲区
	{
		memset(pb, 0x00, BYTES_OF_LPB);
	}
	
	ptLPB->Status	=	LPB_STATUS_ILDE;
	pb = NULL;
	ptLPB = NULL;
	
	if (U_PrtBuf.g_tLinePrtBuf.PrtMode.DoUnderlineFlag)	//如果有下划线打印则全部清零
	{
		memset(U_PrtBuf.g_tLinePrtBuf.Buf, 0x00, \
		sizeof(U_PrtBuf.g_tLinePrtBuf.Buf) / sizeof(g_tLinePrtBuf.Buf[0]));
		U_PrtBuf.g_tLinePrtBuf.PrtMode.DoUnderlineFlag = 0;
	}
	else	//否则无需清零下划线缓冲区
	{
		memset(U_PrtBuf.g_tLinePrtBuf.Buf, 0x00, BYTES_OF_LPB);
	}
	#endif
	
	//2016.06.16 初始化打印缓冲区
	U_PrtBufCtrl.IdxWR = 0;				  //接收缓冲区字节数
	U_PrtBufCtrl.IdxRD =0;				  //接收缓冲区
	U_PrtBufCtrl.BufHeight =0;			//接收缓冲区
	
	for(i=0;i<DrvPrtBufDeep;i++)		//2016.06.27  清空行打印缓冲区
	{
		for(j=0;j<BYTES_OF_ALL_ELEMENT;j++)
		{
			U_PrtBuf.DrivePrintBuf.DdotLine[i].Buf[j] = 0x00;
		}
	}
}
#else
void	InitLinePrtBuf(void)
{
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	* p = NULL;
	uint8_t	* pbSrc = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	p = ptLPB->Buf;
	
	//计算本行中最高字符占用的列数的字节数,按8字节对齐,不足8字节按补齐8字节计算
	if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
	{
		ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3) + 1;
	}
	else
	{
		ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3);
	}
	//根据放大倍数计算起始位置,相对于首地址的字节数
	StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + ptLPB->PrtMode.PrtLeftLimit;
	ByteLen = ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit;		//一字节行的长度
	pbSrc = p + StartPositon;
	for (i = 0; i < ColumnByteNumber; i++)
	{
		memset(pbSrc, 0x00, ByteLen);	//清除行打印缓冲区内容
		pbSrc += PRT_DOT_NUMBERS;
	}
	
	if (ptLPB->PrtMode.DoUnderlineFlag)	//如果有下划线打印则全部清零
	{
		pbSrc = p + BYTES_OF_LPB + ptLPB->PrtMode.PrtLeftLimit;	//从左边界开始
		memset(pbSrc, 0x00, (ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit));	//清除下划线内容
	}
	
	ptLPB->Status	=	LPB_STATUS_ILDE;
	//控制变量在打印下一行之前会被重新赋值,在打印当前行中已经用不到,所以无需改变.	
}
#endif

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化行打印缓冲区,用在打印缓冲区被使用完成后清空,根据放大倍数
*	部分清空
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	ClearLinePrtBuf(void)
{
#if	0
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	* p = NULL;
	uint8_t	* pbSrc = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	p = ptLPB->Buf;
	if ((ptLPB->PrtMode.PrtDataDotIndex >= ptLPB->PrtMode.PrtLeftLimit) && \
			(ptLPB->PrtMode.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//确保参数不越界
	{
		//计算本行中最高字符占用的列数的字节数,按8字节对齐,不足8字节按补齐8字节计算
		if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
		{
			ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3) + 1;
		}
		else
		{
			ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3);
		}
		if (ColumnByteNumber > MAX_PRT_HEIGHT_BYTE)
			ColumnByteNumber = MAX_PRT_HEIGHT_BYTE;
		
		ptLPB->PrtMode.MaxPrtDotHigh = 0;
		//根据放大倍数计算起始位置,相对于首地址的字节数
		StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + ptLPB->PrtMode.PrtLeftLimit;
		ByteLen = ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit;		//一字节行的长度
		pbSrc = p + StartPositon;
		for (i = 0; i < ColumnByteNumber; i++)
		{
			memset(pbSrc, 0x00, ByteLen);			//清除行打印缓冲区内容
			pbSrc += PRT_DOT_NUMBERS;
		}
		
		if (ptLPB->PrtMode.DoUnderlineFlag)	//如果有下划线打印则全部清零
		{
			ptLPB->PrtMode.DoUnderlineFlag = 0;
			ptLPB->PrtMode.UnderlineMaxPrtHigh = 0;
			ptLPB->PrtMode.UnderlinePrintIndex = 0;
			pbSrc = p + BYTES_OF_LPB + ptLPB->PrtMode.PrtLeftLimit;	//从左边界开始
			memset(pbSrc, 0x00, (ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit));	//清除下划线内容
		}
	}		//参数未越界处理完毕
	else	//如果越界固定全清除
	{
		memset(p, 0x00, BYTES_OF_LPB);			//清除行打印缓冲区内容
		if (ptLPB->PrtMode.DoUnderlineFlag)	//如果有下划线打印则全部清零
		{
			ptLPB->PrtMode.DoUnderlineFlag = 0;
			ptLPB->PrtMode.UnderlineMaxPrtHigh = 0;
			ptLPB->PrtMode.UnderlinePrintIndex = 0;
			pbSrc = p + BYTES_OF_LPB;
			memset(pbSrc, 0x00, PRT_DOT_NUMBERS);	//清除下划线内容
		}
	}
	
	p = NULL;
	pbSrc = NULL;
	ptLPB = NULL;
#endif
	memset(U_PrtBuf.g_tLinePrtBuf.Buf, 0x00, LINE_PRT_BUF_LEN);
}

//在一个行打印缓冲区满后,赋值打印控制变量到行打印缓冲区控制结构.
void	SetLinePrtModeVariable(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	
	ptLPB->PrtMode.PrtDataDotIndex	=	g_tPrtCtrlData.PrtDataDotIndex;
	ptLPB->PrtMode.CodeLineHigh			=	g_tPrtCtrlData.CodeLineHigh;	//包括字符高度和行间距值之和
	ptLPB->PrtMode.PrtLeftLimit			=	g_tPrtCtrlData.PrtLeftLimit;
	ptLPB->PrtMode.PrtWidth					=	g_tPrtCtrlData.PrtWidth;
	ptLPB->PrtMode.PrtLength				=	g_tPrtCtrlData.PrtLength;
	ptLPB->PrtMode.MaxPrtDotHigh 		= g_tPrtCtrlData.MaxPrtDotHigh;
	
	ptLPB->PrtMode.PrtModeFlag			=	g_tSystemCtrl.PrtModeFlag;
	ptLPB->PrtMode.BitImageFlag			=	g_tPrtCtrlData.BitImageFlag;
	ptLPB->PrtMode.VariableBitImageFlag	=	g_tPrtCtrlData.VariableBitImageFlag;
	ptLPB->PrtMode.MaxLineHigh			=	g_tPrtCtrlData.LineHigh;			//最大字符的行高,按照点数计算
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	ptLPB->PrtMode.MaxRatioX				=	g_tPrtCtrlData.MaxRatioX;
	#endif
	ptLPB->PrtMode.MaxRatioY				=	g_tPrtCtrlData.MaxRatioY;
	ptLPB->PrtMode.CodeAimMode			=	g_tPrtCtrlData.CodeAimMode;	
	ptLPB->PrtMode.PrintDirection		=	g_tPrtCtrlData.LineDirection;
#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
	ptLPB->PrtMode.CharNotFlag			= g_tPrtCtrlData.CharNotFlag;
#endif
#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
	if (g_tPrtModeChar.EmphaSizeMode || g_tPrtModeHZ.EmphaSizeMode)
		ptLPB->PrtMode.EmphaSizeMode	= 1;
	else
		ptLPB->PrtMode.EmphaSizeMode	= 0;
#endif
	ptLPB->PrtMode.CharCycle90			= g_tPrtCtrlData.CharCycle90;
	ptLPB->PrtMode.CharHeightMagnification	=	g_tPrtModeChar.HeightMagnification;
	ptLPB->PrtMode.HZHeightMagnification		=	g_tPrtModeHZ.HeightMagnification;
	ptLPB->PrtMode.PrintLineIndex		=	0;
	
	ptLPB->PrtMode.DoUnderlineFlag	=	g_tPrtCtrlData.DoUnderlineFlag;
	ptLPB->PrtMode.UnderlineMaxPrtHigh = g_tPrtCtrlData.UnderlineMaxPrtHigh;
	ptLPB->PrtMode.UnderlinePrintIndex = 0;
	
	ptLPB = NULL;
}

/* extern functions ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 获得打印缓冲区当前状态
*									处理行缓冲区的打印操作,控制全局变量g_tLinePrtBuf
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
uint32_t	GetPrintStatus(void)
{
	return	(U_PrtBuf.g_tLinePrtBuf.Status);
}

/*******************************************************************************
* Function Name  : CopyDotLineDataToHeatElementBuf()
* Description    : 复制按照横向调整好的72字节数据到加热数据缓冲区.
* Input          : 与打印机芯加热单元相对应的字符缓冲区.字符缓冲区从0开始的逐字节,
*	从MSB到LSB与加热单元从左到右一一对应.加热缓冲区为80字节.
* Output         : None
* Return         : None
*******************************************************************************/
void	CopyDotLineDataToHeatElementBuf(const uint8_t * pBuf)
{
	uint8_t	* pb = NULL;
	uint32_t	i;
	
	InitHeatElementBuf();												//清空原值
	pb = g_tbHeatElementBuf;
	pb += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;			//从实际可加热点位置开始
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pb++ = *pBuf++;
	}
	
	pb = NULL;
}

/*******************************************************************************
* Function Name	: IncDrvPrtBufAddr(u8 status, u16 ActiveDots)
* Description	: 修正驱动打印缓冲区参数 2016.06.16
* Input			: status:该点行处理状态，ActiveDots：有效打印点数
* Output		: None
* Return		: None
*******************************************************************************/
void IncDrvPrtBufAddr(u8 status, u16 ActiveDots)
{
	U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].ActiveDots= ActiveDots;   //有效打印点数

// 	NVIC_DisableIRQ(TIM5_IRQn);
// 	U_PrtBufCtrl.BufHeight++;									 //修改需要打印的点行数	 2016.07.10
// 	NVIC_EnableIRQ(TIM5_IRQn);
	
	U_PrtBufCtrl.IdxWR++;										   //写行指针	
	if (U_PrtBufCtrl.IdxWR >= DrvPrtBufDeep)	 //2016.06.28 如有外扩ram则此处需要修改
	{	
		U_PrtBufCtrl.IdxWR = 0x00;
	}
}
/*******************************************************************************
* Function Name	: PrtDrvBufFull(void)
* Description	: 等待驱动打印缓冲区内容减少到10以下 
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void PrtDrvBufFull(void)
{
// 	while (U_PrtBufCtrl.BufHeight  >= (DrvPrtBufDeep - 10));//等待缓冲区空 
	uint16_t Temp=0, IdxRD;			//2016.07.10
	
	while(Temp <5)
	{
		IdxRD =U_PrtBufCtrl.IdxRD;		//中断中可能改变其值
		
		if(U_PrtBufCtrl.IdxWR >=IdxRD )
			Temp =DrvPrtBufDeep -U_PrtBufCtrl.IdxWR + IdxRD;
		else
			Temp = IdxRD - U_PrtBufCtrl.IdxWR;
	}	 
}

/*******************************************************************************
* Function Name  : DotLinePutDrvBuf()
* Description    : 把驱动打印的一行数据送入驱动打印缓冲区
* Input          : 与打印机芯加热单元相对应的字符缓冲区.字符缓冲区从0开始的逐字节,
*	从MSB到LSB与加热单元从左到右一一对应.加热缓冲区为76字节.
* Output         : None
* Return         : None
*******************************************************************************/
void	DotLinePutDrvBuf(uint8_t * pBuf)
{
	uint16_t	i, j, HeatDots = 0;
	uint8_t		mask = 0x01;
	
	PrtDrvBufFull();
	
	for (i=0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (*pBuf & (mask << j))
				HeatDots++;
		}
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = *pBuf++;
	}
	IncDrvPrtBufAddr(0x00, HeatDots);	
}
/*******************************************************************************
* Function Name  : SendHeatElementBufToCore()
* Description    : 将横向调整好的80字节数据到机芯接收寄存器.
* Input          : 与打印机芯加热单元相对应的字符缓冲区.字符缓冲区从0开始的逐字节,
*	从MSB到LSB与加热单元从左到右一一对应.加热缓冲区为80字节.
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef	SPI_PRT_TX_BY_DMA_ENABLE	//全部使用DMA方式发送
void	SendHeatElementBufToCore(void)
{
	g_bPrtSPIDMATxStatusFlag = 1;
	g_bPrtSPIDMAIntForDriveMode = 1;
	sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);	//启动机芯SPI发送
	while (g_bPrtSPIDMATxStatusFlag);		//一直等待DMA发送结束 2016.06.15
	g_bPrtSPIDMAIntForDriveMode = 0;
}
#else
void	SendHeatElementBufToCore(void)
{
	uint32_t	i;
	
	for (i = 0; i < BYTES_OF_ALL_ELEMENT; i++)	//发送加热数据到机芯
	{
		SPI_HeatSendByte(g_tbHeatElementBuf[i]);
	}
}
#endif

/*******************************************************************************
* Function Name  : PrintOneDotLine
* Description    : 打印1点行数据,先将对应加热数据送入机芯;计算加热时间;
*	等待前点行完毕;再锁存数据;调整电机走纸时间;开始本点行加热.
* Input          : 与打印机芯加热单元相对应的字符缓冲区.字符缓冲区从0开始的逐字节,
*	从MSB到LSB与加热单元从左到右一一对应.
*	调用该函数时应确保上一点行已经加热完毕.否则新送入的数据会替代上一点行未完成
*	的任务,导致重叠.程序中已经处理了.
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintOneDotLine(void)
{
	uint16_t	HeatTime;
	uint16_t	totalheatus;
	uint32_t	i;
	#ifdef	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//发送一点行机芯数据耗时,查询发送
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//发送一点行机芯数据耗时,查询发送
		StartProcessTimeCounter();
	#endif
	SendHeatElementBufToCore();	//发送加热数据到机芯
	#ifdef	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//发送一点行机芯数据耗时,查询发送
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	HeatTime = GetHeatTime();		//计算加热时间
	SetHeatNumber();						//计算本点行加热次数,设置加热控制管脚
	
// 	WaitPrintAndFeedPaperFinish();	//等待前点行走纸或者加热完毕
// 	LatchHeatData();							  //锁存机芯数据
	
	g_tHeatCtrl.HeaterGroupNumberCpy = g_tHeatCtrl.HeaterGroupNumber;	//赋值控制变量
	g_tHeatCtrl.HeaterGroupNumber = 0;
	for (i = 0; i < g_tHeatCtrl.HeaterGroupNumberCpy; i++)
	{
		g_tHeatCtrl.HeaterGroupElementCpy[ i ] = g_tHeatCtrl.HeaterGroupElement[ i ];
		g_tHeatCtrl.HeaterGroupElement[ i ] = 0;
	}
	
	g_HeatUs = HeatTime;
	totalheatus = g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy;	//一点行总加热时间
	AdjustMotorStep(totalheatus);					//计算电机延时,调整加速步数	
	
	WaitPrintAndFeedPaperFinish();	//等待前点行走纸或者加热完毕 2016.06.15
	while (g_bPrtSPIDMATxStatusFlag);		//一直等待DMA发送结束
	g_bPrtSPIDMAIntForDriveMode = 0;
	LatchHeatData();							//锁存机芯数据	
	
	g_tHeatCtrl.HeatEnable = HEAT_ENABLE;	//置开始加热变量
	SetTimerCounterWithAutoReload(HEAT_TIMER, g_HeatUs);	//同时设置自动重载值和计时值
	TIM_SetCounter(HEAT_TIMER, 1);
	StartTimer(HEAT_TIMER);								//启动定时器开始处理加热和走电机中断
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 控制行打印缓冲区的打印操作,根据当前行缓冲区的状态进行相应操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#ifdef	DBG_LPB_INT_COUNT
volatile uint32_t	iDbgPrtTotalCount        = 0;
volatile uint32_t	iDbgPrtIldeCount         = 0;
volatile uint32_t	iDbgPrtWritingCount      = 0;
volatile uint32_t	iDbgPrtSendingDataCount  = 0;
volatile uint32_t	iDbgPrtWaitPrtCount      = 0;
volatile uint32_t	iDbgPrtStartPrtCount     = 0;
volatile uint32_t	iDbgPrtSendDataOverCount = 0;
volatile uint32_t	iDbgPrtSetLineSpaceCount = 0;
#endif

void	PrintLineBuffer(void)
{
	
	#ifdef	DBG_LPB_INT_COUNT
		iDbgPrtTotalCount++;
	#endif
	
	switch	(U_PrtBuf.g_tLinePrtBuf.Status)
	{
		case	LPB_STATUS_ILDE:					//空闲状态
			StopTimer(PRINT_CHECK_TIMER);
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtIldeCount++;
			#endif
			break;
		case	LPB_STATUS_WRITING:				//编辑状态
      StopTimer(PRINT_CHECK_TIMER);
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtWritingCount++;
			#endif
			break;
		case	LPB_STATUS_WAIT_TO_PRT:		//等待打印状态
			WaitToPrintLineBuf();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtWaitPrtCount++;
			#endif
			break;
		case	LPB_STATUS_START_PRT:			//开始打印状态
			StartPrintLineBuf();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtStartPrtCount++;
			#endif
			break;
		case	LPB_STATUS_SPI_SENDING:		//SPI发送状态
			StopTimer(PRINT_CHECK_TIMER);
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtSendingDataCount++;
			#endif
			break;
		case	LPB_STATUS_SPI_SEND_OVER:	//机芯送数结束状态
			StartHeatAndRotateMotor();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtSendDataOverCount++;
			#endif
      break;
		case	LPB_STATUS_SET_GO_LINE_SPACE:	//设置走行间距状态
			SetGoLineSpace();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtSetLineSpaceCount++;
			#endif
			break;
		default:	//其他状态不进行处理,往机芯送一点行数据状态时不做处理等待DMA中断修改状态标志位
			break;	//在编辑行开始时对控制行打印有关的全局变量赋值到选中的行打印缓冲区的PrtMode结构体中
	}	
}

uint32_t GetLPBProcessStatus(void)
{
	uint32_t	iStatus;
	
	iStatus = GetPrintStatus();
	if ((LPB_STATUS_WAIT_TO_PRT == iStatus) || \
			(LPB_STATUS_START_PRT == iStatus) || \
			(LPB_STATUS_SPI_SEND_OVER == iStatus) || \
			(LPB_STATUS_SET_GO_LINE_SPACE == iStatus)
		 )
	{
		if (ENABLE != GetTimerEnableStatus(PRINT_CHECK_TIMER))
		{
			return (LPB_PROCESS_STATUS_UNFINISH);
		}
		else
		{
			return (LPB_PROCESS_STATUS_FINISH);
		}
	}
	else
	{
		return (LPB_PROCESS_STATUS_FINISH);
	}
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 设置打印行数等控制全局变量,字符模式时需要处理:
	PrintLineIndex,UnderLinePrtOverFlag,
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	WaitToPrintLineBufOfChar(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印
	{	//下划线之外的内容打印起始行数,默认使用12*24字符时从0开始
		ptLPB->PrtMode.PrintLineIndex	= \
			(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY;
	}
	else		//反向打印
	{	//下划线之外的内容行数,本行中曾经编辑过的最大字符行高*最大纵向放大倍数		
		ptLPB->PrtMode.PrintLineIndex = MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY;
	}
	
	#ifdef	UNDER_LINE_PRT_ENABLE
	if (ptLPB->PrtMode.DoUnderlineFlag)		//2012-9-21添加处理下划线打印
	{
		if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印
		{
			ptLPB->PrtMode.UnderlinePrintIndex = ptLPB->PrtMode.UnderlineMaxPrtHigh;
		}
		else
		{
			ptLPB->PrtMode.UnderlinePrintIndex = 0;
		}
		ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_UNFINISHED;
	}
	#endif
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 设置打印行数等控制全局变量,图形模式时需要处理:
	PrintLineIndex,UnderLinePrtOverFlag,MaxRatioY
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	WaitToPrintLineBufOfGraphic(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.BitImageFlag == 1)	//如果是位图打印模式则放大比例1:1
	{
		ptLPB->PrtMode.MaxRatioY = 1;
	}
	
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)		//正向打印
	{	//下划线之外的内容打印起始行数,默认使用12*24字符时从0开始
		ptLPB->PrtMode.PrintLineIndex	= 0;
	}
	else		//反向打印
	{	//下划线之外的内容行数,本行中曾经编辑过的最大字符行高*最大纵向放大倍数
		ptLPB->PrtMode.PrintLineIndex	= ptLPB->PrtMode.MaxPrtDotHigh;
	}
	
	#ifdef	UNDER_LINE_PRT_ENABLE
	if (ptLPB->PrtMode.DoUnderlineFlag)		//2012-9-21添加处理下划线打印
	{
		ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
	}
	#endif
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检查上一次打印或者走电机是否完成,检查全局变量,
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	WaitToPrintLineBuf(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	//如果打印缓冲区无需要打印内容,无下划线,直接到走行间距
	if (ptLPB->PrtMode.PrtDataDotIndex == ptLPB->PrtMode.PrtLeftLimit)
	{
		ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
	}
	else	//否则逐点行打印
	{	//以下初始化打印控制的全局变量
		if ((ptLPB->PrtMode.BitImageFlag == 1) || \
				(ptLPB->PrtMode.VariableBitImageFlag == 1))
		{
			WaitToPrintLineBufOfGraphic();
		}
		else
		{
			WaitToPrintLineBufOfChar();
		}
		
		ptLPB->Status = LPB_STATUS_START_PRT;	//如果已经完成则更新行缓冲区状态
	}
	
	SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
	StartTimer(PRINT_CHECK_TIMER);
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 正向打印时,将下划线缓冲区给定地址开始的内容,按照给定列高度取值,
*		转换成机芯横向点行模式,正向打印时从下划线缓冲区一个字节内BIT3->BIT0的顺序提取填充
*		将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*		机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*		将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*									操作加热字节内容全局变量
* Input          : 当前需要处理的行缓冲区指针和行数
*	p指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Lines表示该字节中的第几位数,范围[0,8).,目前使用的是一个字节的最低3位表示最多3行下划线
* Input          : 当前需要处理的缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	UnderLineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//横向加热处理结果
	uint8_t	CurrentBit;		//当前字节填充位的位置,从8到1,为0时一个字节填充完毕
	uint16_t	ColumnIndex;	//横向列计数索引,根据起始值补充空白位置和字节位
	uint16_t	i;
	uint16_t	CopyIndex;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//根据对齐方式得到起始列值
	//整数倍数的空白位置填充完毕,因为前面已经全部清空了
	if (ColumnIndex % 8)				//ColumnIndex不是8的整倍数,前后需要衔接
	{
		CurrentBit = 8 - (ColumnIndex % 8);	//当前字节填充位的位置,从8到1,为0		
	}
	else
	{
		CurrentBit = 8;		//一个字节最高有8位
	}
	
	i = (ColumnIndex >> 3);	//准备填充该字节,该字节的后面不足的部分需要在下面程序中补齐
	if (i >= BYTES_OF_HEAT_ELEMENT)		//如果超过最后一个字节
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//防止下标越界
	}
	CopyIndex = i;
	//以上为处理空白部分
	
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//防止非法内存越界
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	for (ColumnIndex = ptLPB->PrtMode.PrtLeftLimit; 
		ColumnIndex < ptLPB->PrtMode.PrtDataDotIndex; ColumnIndex++)
	{
		//判断该字节对应位的值是否为1,Lines需要转换为范围0~7, 目前最大范围0~3
		if (*PrintBuff & ((uint8_t)(0x01 << (Lines - 1))))	//如果对应位置为1
		{
			Buf[i] |= (uint8_t)(1 << (CurrentBit - 1));		//填充一个横向字节
		}
		CurrentBit--;					//字节位数每次不论处理字节对应位是否为1都需要进行处理
		
		if (CurrentBit == 0)	//已经填满一个横向加热字节
		{
			CurrentBit = 8;			//重新开始准备填充下一个字节,从MSB开始填充
			i++;								//只有满字节后才执行
		}
		
		PrintBuff++;					//准备检测下一个字节
	}		//待打印字节处理完毕
#if	0
	if (!CurrentBit)		//说明有转换到不足一个字节而没有完成的,需要递增1个
		i++;
	if (i > BYTES_OF_HEAT_ELEMENT)		//防止溢出
		i = BYTES_OF_HEAT_ELEMENT;
#endif
	
	//以下将处理结果填充到横向加热缓冲区
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += (BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT + CopyIndex);	//跳过左侧32位空白区和加热区中为0的内容,右侧32位空白区已经清零
	for (; CopyIndex < BYTES_OF_HEAT_ELEMENT; CopyIndex++)
	{
		*pBuf++ = Buf[CopyIndex];		//中间实际打印区
	}
	
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//打印调试
	//#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*	机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*	将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*	将行打印缓冲区pBuffer[ ColumnIndex ]开始递增到
*								pBuffer[ PrtDataDotIndex ]结束的字符依次内容颠倒后填充到
*								pBuffer[ PrtLength + LeftLimit - ColumnIndex ]开始递减到
*								pBuffer[ PrtLength + LeftLimit - PrtDataDotIndex ]结束的位置。
*	填充位置对应加热Buf[ PrtLength + LeftLimit - ColumnIndex ]开始递减到
*									Buf[ PrtLength + LeftLimit - PrtDataDotIndex ]结束的位置。
*	程序反向打印时先发送一行的最低位置的Lines值,递减发送到一行的最高位置的Lines值,
*	pBuf都是从左往右,所以处理规律:
*	将lines行, ColumnIndex		列的字符的对应 位值 作为填充字符的LSB,
*	将lines行,(ColumnIndex+1)	列的字符的对应 位值 作为填充字符的(LSB+1),
*	将lines行,(ColumnIndex+2)	列的字符的对应 位值 作为填充字符的(LSB+2),
*	将lines行,(ColumnIndex+3)	列的字符的对应 位值 作为填充字符的(LSB+3),
*	........
*	将lines行,(ColumnIndex+7)	列的字符的对应 位值 作为填充字符的(LSB+7),
*	至此一次填充字节合成完毕。
*	继续下一个循环。
*		  							操作全局变量
* Input          : 当前需要处理的行缓冲区指针PrintBuff和行数Lines,
*	PrintBuff指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Lines表示打印的行数,范围[0,最高字符*最高纵向放大倍数).

* Description    : 反向打印时,将下划线缓冲区给定地址开始的内容,按照给定列高度取值,
*		转换成机芯横向点行模式,正向打印时从下划线缓冲区一个字节内BIT0->BIT3的顺序提取填充
*		将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*		机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*		将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*									操作加热字节内容全局变量
* Input          : 当前需要处理的行缓冲区指针和行数
*	p指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Lines表示该字节中的第几位数,范围[0,8),目前使用的是一个字节的最低3位表示最多3行下划线
* 目前输入Lines变化顺序为由LSB到MSB(3/2/1->0)
* Input          : 当前需要处理的缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#define	UNDER_LINE_MAX_BIT	(2)
static void	UnderLineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//横向加热处理结果
	uint8_t	CurrentBit;		//当前字节填充位的位置,从8到1,为8时一个字节填充完毕		
	uint16_t	ColumnIndex;	//横向列计数索引,根据起始值补充空白位置和字节位
	uint16_t	i;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//根据对齐方式得到起始列值
	i = ptLPB->PrtMode.PrtLength - ColumnIndex;	//指向横向转换后加热缓冲区开始位置
	if (i % 8)
	{
		CurrentBit = 8 - (i % 8);	//可能出现不在8位整数倍位置,从右往左从最低位开始组合起
	}
	else
	{
		CurrentBit = 0;		//如果是8的整数倍则从0开始
	}
	
	i >>= 3;													//转化为字节
	if (i >= BYTES_OF_HEAT_ELEMENT)		//如果超过最后一个字节
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//防止下标越界
	}
	
	pBuf = Buf;
	pBuf += i;
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//防止非法内存越界
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	if (Lines > UNDER_LINE_MAX_BIT)
		Lines = UNDER_LINE_MAX_BIT;
	for (i = ptLPB->PrtMode.PrtLeftLimit; i < ptLPB->PrtMode.PrtDataDotIndex; i++)	//需要处理的列(字节)数目
	{
		//判断该字节对应位的值是否为1,Lines需要转换为范围1~3
		if (*PrintBuff & ((uint8_t)(0x01 << Lines)))	//如果对应位置为1
		{
			*pBuf |= (uint8_t)(1 << CurrentBit);		//填充一个横向字节
		}
		CurrentBit++;					//字节位数每次不论处理字节对应位是否为1都需要进行处理
		
		if (CurrentBit == 8)	//已经填满一个横向加热字节
		{
			CurrentBit = 0;			//重新开始准备填充下一个字节,从LSB开始填充
			pBuf--;							//只有满一个字节后才减少			
		}
		
		PrintBuff++;					//准备检测下一列的字节,每次循环都增加
	}
	
	//以下将处理结果填充到横向加热缓冲区
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;		//跳过左侧32位空白区,右侧32位空白区已经清零
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pBuf++ = Buf[i];		//中间实际打印区
	}
	
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//打印调试
	//#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将给定地址开始的内容,按照给定列高度取值,转换成机芯横向点行模式
*		将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*		机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*		将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*									操作加热字节内容全局变量
* Input          : 当前需要处理的行缓冲区指针和行数
*	p指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Column表示该字节中的第几位数,范围[0,8).
* Input          : 当前需要处理的缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	FillUnderLineToHeatElementBuf(uint8_t * PrintBuff, uint8_t Lines)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	//uint8_t	* pbBuf = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印开始
	{
		UnderLineBufToHeatElementNormalDir(PrintBuff, Lines);
	}	//正向打印处理结束
	else// if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_REVERSE)	//反向打印开始
	{
		UnderLineBufToHeatElementReverseDir(PrintBuff, Lines);
		//UnderLineBufToHeatElementNormalDir(PrintBuff, Lines);
		//pbBuf = g_tbHeatElementBuf;
		//pbBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;
		//ReverseStreamEachBit(pbBuf, BYTES_OF_HEAT_ELEMENT);
	}	//反向打印处理结束
	
	ptLPB = NULL;
	//pbBuf = NULL;
}

#if	0
/*******************************************************************************
* Function Name  : 函数名
* Description    : 将一个字节的数据逐位逆序, 从BIT0逐个取得各个位, 移动到对应位
* Input          : 当前需要处理的字节值
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static uint8_t ReverseByte(uint8_t cSrc)
{
	uint8_t cTemp;
	uint32_t iCount;
	
	cTemp = 0;
	for (iCount = 0; iCount < sizeof(uint8_t); iCount++)
	{
		if ((cSrc >> iCount) & 0x01)
		{
			cTemp |= (1 << (sizeof(uint8_t) - 1 - iCount));
		}
	}
	
	return (cTemp);
}
#endif

#if	0
/*******************************************************************************
* Function Name  : 函数名
* Description    : 将机芯横向点行数据逐位逆序, 以输入指针为准
* Input          : 当前需要处理的行缓冲区指针, 需要处理的字节数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void ReverseStreamEachBit(uint8_t * pbBuf, uint32_t iNumber)
{
	uint8_t bBuf[ BYTES_OF_ALL_ELEMENT ];
	uint32_t iCount;
	uint8_t * pbSrc = NULL;
	uint8_t * pbDst = NULL;

	memset(bBuf, 0x00, BYTES_OF_ALL_ELEMENT);	//清零
	//memset(bBuf, 0x00, sizeof(bBuf) / sizeof(bBuf[0]));	//清零	
	pbSrc = bBuf;
	pbDst = pbBuf;
	for (iCount = 0; iCount < iNumber; iCount++)	//取得待反转数据的备份
	{
		*pbSrc = *pbDst;
		pbSrc++;
		pbDst++;
	}
	
	pbSrc = bBuf;
	pbDst = pbBuf;
	for (iCount = 0; iCount < iNumber; iCount++)	//反转数据
	{
		if ((*pbSrc == 0x00) || (*pbSrc == 0xFF))		//无需反转,正序逆序对称
		{
			*pbDst = *pbSrc;
		}
		else
		{
			*pbDst = ReverseByte(*pbSrc);
		}
		pbSrc++;
		pbDst++;
	}
	
	pbSrc = NULL;
	pbDst = NULL;
}
#endif

/*******************************************************************************
* Function Name  : 函数名
* Description    : 开始打印操作,检查是否有错误,如果没有检查点行是否处理完毕,
*	未处理完毕则将当前点行值处理成机芯横向点行模式,启动机芯SPI发送DMA,处理字符打印
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
	extern volatile uint32_t	g_iDbgSPISendCount;	//往机芯SPI发送数据的次数统计
#endif
#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
	#define	DBG_EDIT_COUNT_BUF_LEN		(100)
	extern	volatile uint32_t	g_iDbgEditCountWhilePrt;
	volatile uint8_t	g_bDbgEditCharCountBuf[DBG_EDIT_COUNT_BUF_LEN];
	static	volatile uint8_t * pbDbgBuf = g_bDbgEditCharCountBuf;
	static	uint8_t	g_bDbgBufCount = 0;
#endif
static void	StartPrintLineBufChar(void)
{
	uint8_t	* pbBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印
	{	//以下为处理下划线之外的打印内容
		if (ptLPB->PrtMode.PrintLineIndex < 
				(MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY))	//行高,默认为24
		{
			//行缓存首地址	最大纵向放大倍数	当前放大倍数默认1	每行点	因为行高最大24点行所以 行数/8,共0,1,2三个字节行
			pbBuf = ptLPB->Buf + \
					(MAX_RATIO_Y - ptLPB->PrtMode.MaxRatioY) * (MAX_FONT_HEIGHT / 8) * PRT_DOT_NUMBERS + \
					(ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS + \
					ptLPB->PrtMode.PrtLeftLimit;
			//此处需要添加处理该点行内容,取出需要发送的一点行数据并放到公共缓冲区中.启动机芯SPI发送.
			//横向转换一行数据并保存到公共缓冲区
			FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);
			ptLPB->PrtMode.PrintLineIndex++;
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//启动机芯SPI发送
			ptLPB->Status = LPB_STATUS_SPI_SENDING;	//如果已经完成则更新行缓冲区状态
			#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时
				StartProcessTimeCounter();
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
				g_iDbgSPISendCount++;		//往机芯SPI发送数据的次数统计
				if (g_iDbgSPISendCount >= 24)
				{
					*pbDbgBuf++ = (uint8_t)g_iDbgEditCountWhilePrt;
					g_bDbgBufCount++;
					if (g_bDbgBufCount >= DBG_EDIT_COUNT_BUF_LEN)
						pbDbgBuf = g_bDbgEditCharCountBuf;
				}
			#endif
		}		//先打印完字符内容再打印下划线,字符打印处理完毕
		#ifdef	UNDER_LINE_PRT_ENABLE		//2012-9-21添加下划线打印处理
		else	//字符已经打完,检查下划线
		{	//如果下划线打印未处理完成,处理然后退出
			if (ptLPB->PrtMode.UnderLinePrtOverFlag == UNDER_LINE_PRT_UNFINISHED)
			{
				if (ptLPB->PrtMode.UnderlinePrintIndex)
				{
					pbBuf = ptLPB->Buf;
					pbBuf += BYTES_OF_LPB;				//指向下划线缓冲区
					pbBuf += ptLPB->PrtMode.PrtLeftLimit;
					//将下划线缓冲区内容对应行的数据取出来填充到横向加热缓冲区
					FillUnderLineToHeatElementBuf(pbBuf, ptLPB->PrtMode.UnderlinePrintIndex);
					ptLPB->PrtMode.UnderlinePrintIndex--;	//下划线打印高度值完成1点行,直到为0表示打印完成
					sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//启动机芯SPI_DMA发送
					ptLPB->Status = LPB_STATUS_SPI_SENDING;	//如果已经完成则更新行缓冲区状态
					#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时
						StartProcessTimeCounter();
					#endif
					#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
						g_iDbgSPISendCount++;	//往机芯SPI发送数据的次数统计
					#endif
				}
				else	//此处代码为防范性处理,防止打印进程进入逻辑错误
				{
					ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
					ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//如果已经完成则更新行缓冲区状态
					SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
					StartTimer(PRINT_CHECK_TIMER);
				}
			}			//下划线未打印完处理完毕
			else	//字符和下划线都已打印完处理开始
			{
				ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//如果已经完成则更新行缓冲区状态
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}			//字符和下划线都已打印完处理完毕
		}		//检查下划线处理完毕
		#endif
	}			//正向打印处理结束
	else	//反向打印
	{	//先打印完下划线再打印字符内容,2012-9-23添加下划线打印处理
		//如果下划线打印未处理完成,处理然后退出
		if (ptLPB->PrtMode.UnderLinePrtOverFlag == UNDER_LINE_PRT_UNFINISHED)
		{
			if (ptLPB->PrtMode.UnderlinePrintIndex < ptLPB->PrtMode.UnderlineMaxPrtHigh)
			{
				pbBuf = ptLPB->Buf;
				pbBuf += BYTES_OF_LPB;				//指向下划线缓冲区
				pbBuf += ptLPB->PrtMode.PrtLeftLimit;
				//将下划线缓冲区内容对应行的数据取出来填充到横向加热缓冲区
				//下划线打印高度值完成1点行,直到为0表示打印完成
				FillUnderLineToHeatElementBuf(pbBuf, ptLPB->PrtMode.UnderlinePrintIndex);
				ptLPB->PrtMode.UnderlinePrintIndex++;	//下划线打印高度值完成1点行,直到为0表示打印完成
				sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//启动机芯SPI_DMA发送
				ptLPB->Status = LPB_STATUS_SPI_SENDING;	//如果已经完成则更新行缓冲区状态
				
				//m_bReversePrintUnderlineFlag = 1;
				#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时
					StartProcessTimeCounter();
				#endif
				#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
					g_iDbgSPISendCount++;	//往机芯SPI发送数据的次数统计
				#endif
			}
			else	//此处代码为防范性处理,防止打印进程进入逻辑错误
			{
				ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
				ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//如果已经完成则更新行缓冲区状态
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}
		}		//下划线处理完毕再打印字符内容,下划线处理结束
		//该处打印行点高数目计数取无符号数,减到1为止
		else	//开始处理字符打印
		{
			if (ptLPB->PrtMode.PrintLineIndex > \
				(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY)
			{
				//将打印缓冲区中数据转换为机芯接收格式,并通过SPI接口送到机芯缓冲中,并分组计算加热点数
				//参数:PrintBuff:打印缓冲区地址,Line：需要处理的点行号
				ptLPB->PrtMode.PrintLineIndex--;	//初始化时从偶数开始,但是实际用时得先从减1后开始
				pbBuf = ptLPB->Buf + 
					(MAX_RATIO_Y - ptLPB->PrtMode.MaxRatioY) * (MAX_FONT_HEIGHT / 8) * PRT_DOT_NUMBERS + 
					(ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS + 
					ptLPB->PrtMode.PrtLeftLimit;
				//此处需要添加处理该点行内容,取出需要发送的一点行数据并放到公共缓冲区中.启动机芯SPI发送.			
				//横向转换一行数据并保存到公共缓冲区
				FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);			
				sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//启动机芯SPI发送
				ptLPB->Status = LPB_STATUS_SPI_SENDING;	//如果已经完成则更新行缓冲区状态
				#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时
					StartProcessTimeCounter();
				#endif
				#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
					g_iDbgSPISendCount++;	//往机芯SPI发送数据的次数统计
				#endif
			}		//字符未打印完处理结束
			else
			{
				ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//如果已经完成则更新行缓冲区状态
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}		//反向处理下划线和字符均结束
		}		//反向处理打印字符结束
	}		//反向打印处理结束
	
	pbBuf = NULL;
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 开始打印操作,检查是否有错误,如果没有检查点行是否处理完毕,
*	未处理完毕则将当前点行值处理成机芯横向点行模式,启动机芯SPI发送DMA,处理图形打印
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	StartPrintLineBufGraphic(void)
{
	uint8_t	cTemp;
	uint8_t	* pbBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印
	{
		if (ptLPB->PrtMode.PrintLineIndex < ptLPB->PrtMode.MaxPrtDotHigh)
		{
			cTemp = ptLPB->PrtMode.MaxPrtDotHigh >> 3;
			if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
			{
				cTemp += 1;		//根据行数换算为字节数,不足一个字节的需要补满
			}
			if (cTemp >= MAX_PRT_HEIGHT_BYTE)
				cTemp = (MAX_PRT_HEIGHT_BYTE - 1);
			pbBuf = ptLPB->Buf + (MAX_PRT_HEIGHT_BYTE - cTemp) * PRT_DOT_NUMBERS;	//指向所有打印内容的首地址
			pbBuf += (ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS;	//指向该行的首地址
			pbBuf += ptLPB->PrtMode.PrtLeftLimit;				//指向有真正打印内容的首地址
			//横向转换一行数据并保存到公共缓冲区
			FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);
			ptLPB->PrtMode.PrintLineIndex++;
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//启动机芯SPI发送
			ptLPB->Status = LPB_STATUS_SPI_SENDING;	//如果已经完成则更新行缓冲区状态
			#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时
				StartProcessTimeCounter();
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
				g_iDbgSPISendCount++;	//往机芯SPI发送数据的次数统计
			#endif
		}		//图形打印完毕
		else	//防止出现DMA传输意外
		{
			ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//如果已经完成则更新行缓冲区状态
			SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
			StartTimer(PRINT_CHECK_TIMER);
		}
	}
	else		//反向打印
	{
		if (ptLPB->PrtMode.PrintLineIndex > 0)
		{	//将打印缓冲区中数据转换为机芯接收格式,并通过SPI接口送到机芯缓冲中,并分组计算加热点数
			//参数:PrintBuff:打印缓冲区地址,Line：需要处理的点行号
			ptLPB->PrtMode.PrintLineIndex--;	//初始化时从偶数开始,但是实际用时得先从减1后开始
			cTemp = ptLPB->PrtMode.MaxPrtDotHigh >> 3;
			if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
			{
				cTemp += 1;		//根据行数换算为字节数,不足一个字节的需要补满
			}
			if (cTemp >= MAX_PRT_HEIGHT_BYTE)
				cTemp = (MAX_PRT_HEIGHT_BYTE - 1);
			pbBuf = ptLPB->Buf + (MAX_PRT_HEIGHT_BYTE - cTemp) * PRT_DOT_NUMBERS;	//指向所有打印内容的首地址
			pbBuf += (ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS;	//指向该行的首地址
			pbBuf += ptLPB->PrtMode.PrtLeftLimit;				//指向有真正打印内容的首地址
			FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);	//横向转换一行数据并保存到公共缓冲区
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//启动机芯SPI发送
			ptLPB->Status = LPB_STATUS_SPI_SENDING;	//如果已经完成则更新行缓冲区状态
			#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时
				StartProcessTimeCounter();
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数
				g_iDbgSPISendCount++;	//往机芯SPI发送数据的次数统计
			#endif
		}
		else	//防止出现DMA传输意外
		{
			ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//如果已经完成则更新行缓冲区状态
			SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
			StartTimer(PRINT_CHECK_TIMER);
		}
	}
	
	pbBuf = NULL;
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 开始打印操作,检查是否有错误,如果没有检查点行是否处理完毕,
*	未处理完毕则将当前点行值处理成机芯横向点行模式,启动机芯SPI发送DMA
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	StartPrintLineBuf(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))
	{
		StartPrintLineBufGraphic();
	}
	else
	{
		StartPrintLineBufChar();
	}
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 计算当前打印行的列计数索引,用在将一行8位数转换为机芯横向数据时
*	根据打印总列数和对齐方式不同而调整,
*	左对齐时Column从设置值递增PrtDataDotIndex,从左到右递增处理打印数据
*	右对齐时Column从设置值递增PrtDataDotIndex,从左往右递增处理打印数据
*	中间对齐时Column从设置值递增PrtDataDotIndex,从左往右递增处理打印数据
*	以下为默认左间距为0时的计算.如果左间距不为0,需要再加上左间距.
*	PrtDataDotIndex(其值从最左端开始,包含左间距的值),指向当前行最后结束时的列
*			|总处理范围PrtLength = LeftLimit + PrtWidth(设置实际打印范围)
*			|g_tPrtCtrlData.PrtWidth = 576;		//设置打印区域 |
*			|g_tPrtCtrlData.PrtLength = PrtWidth + LeftLimit;//硬件有效打印区域,最大576|
*			|<-------------------------|---------------------->|
*			|			 本次实际打印内容长度PrtDataDotIndex			   |
*			|<-----|----->			|<-----|----->	 |<-----|----->|
*	左边距LeftLimit					|		  中间			 |						右边距RightLimit
*	左对齐时Column=PrtLeftLimit    |				 |右对齐时Column=PrtLength-PrtDataDotIndex
*																 |
*								中间对齐时Column=(PrtLength-PrtDataDotIndex)/2
*									操作全局变量
* Input          : 需要知道是计算哪个行打印缓冲区的
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static uint16_t	GetLinePrtBufStartColumnIndex(void)
{
	uint16_t	ColumnIndex;	//横向列计数索引
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.CodeAimMode == AIM_MODE_LEFT)		//左对齐
	{
		ColumnIndex = ptLPB->PrtMode.PrtLeftLimit;
	}
	else if (ptLPB->PrtMode.CodeAimMode == AIM_MODE_MIDDLE)		//中间对齐
	{	// = 有效打印区域 - 打印进程列计数器索引(一次行打印打印点阵计数器)
		if (ptLPB->PrtMode.PrtLength > ptLPB->PrtMode.PrtDataDotIndex)
			ColumnIndex = ptLPB->PrtMode.PrtLength - ptLPB->PrtMode.PrtDataDotIndex;
		else
			ColumnIndex = 0;
		
		ColumnIndex >>= 1;
		ColumnIndex += ptLPB->PrtMode.PrtLeftLimit;
	}
	else if (ptLPB->PrtMode.CodeAimMode == AIM_MODE_RIGHT)	//右对齐
	{
		if (ptLPB->PrtMode.PrtLength > ptLPB->PrtMode.PrtDataDotIndex)
			ColumnIndex = ptLPB->PrtMode.PrtLength - ptLPB->PrtMode.PrtDataDotIndex;
		else
			ColumnIndex = 0;
		
		ColumnIndex += ptLPB->PrtMode.PrtLeftLimit;
	}
	else
	{
		ColumnIndex = 0;
	}
	ptLPB = NULL;
	
	return (ColumnIndex);
}

/*******************************************************************************
* Function Name  : 处理正向打印时的数据转换
* Description    : 将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*	机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*	将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针PrintBuff和行数Lines,
*	PrintBuff指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Lines表示打印的行数,范围[0,最高字符*最高纵向放大倍数).
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	LineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//横向加热处理结果
	uint8_t	CurrentBit;		//当前字节填充位的位置,从8到1,为0时一个字节填充完毕
	uint16_t	ColumnIndex;	//横向列计数索引,根据起始值补充空白位置和字节位
	uint16_t	i;
	uint16_t	CopyIndex;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//根据对齐方式得到起始列值
	//整数倍数的空白位置填充完毕,因为前面已经全部清空了
	if (ColumnIndex % 8)				//ColumnIndex不是8的整倍数,前后需要衔接
	{
		CurrentBit = 8 - (ColumnIndex % 8);	//当前字节填充位的位置,从8到1,为0
	}
	else
	{
		CurrentBit = 8;		//一个字节最高有8位
	}
	i = (ColumnIndex >> 3);	//准备填充该字节,该字节的后面不足的部分需要在下面程序中补齐
	if (i >= BYTES_OF_HEAT_ELEMENT)		//如果超过最后一个字节
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//防止下标越界
	}
	CopyIndex = i;
	//以上为处理空白部分
	
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//防止非法内存越界
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	for (ColumnIndex = ptLPB->PrtMode.PrtLeftLimit; \
		ColumnIndex < ptLPB->PrtMode.PrtDataDotIndex; ColumnIndex++)
	{
		//判断该字节对应位的值是否为1,Lines需要转换为范围0~7,从MSB到LSB
		if (*PrintBuff & (0x80 >> (Lines & 0x07)))	//如果对应位置为1
		{
			Buf[i] |= (uint8_t)(1 << (CurrentBit - 1));	//填充一个横向字节
		}
		CurrentBit--;					//字节位数每次不论处理字节对应位是否为1都需要进行处理
		
		if (CurrentBit == 0)	//已经填满一个横向加热字节
		{
			CurrentBit = 8;			//重新开始准备填充下一个字节,从MSB开始填充
			i++;								//只有满字节后才执行
		}
		
		PrintBuff++;					//准备检测下一个字节
	}		//待打印字节处理完毕
#if	0
	if (!CurrentBit)		//说明有转换到不足一个字节而没有完成的,需要递增1个
		i++;
	if (i > BYTES_OF_HEAT_ELEMENT)		//防止溢出
		i = BYTES_OF_HEAT_ELEMENT;
#endif
	//以下将处理结果填充到横向加热缓冲区
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	if (CopyIndex > BYTES_OF_HEAT_ELEMENT)	//防止非法内存越界
		CopyIndex = BYTES_OF_HEAT_ELEMENT;
	//跳过左侧32位空白区和加热区中为0的内容,右侧32位空白区已经清零
	pBuf = g_tbHeatElementBuf + BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;
	pBuf += CopyIndex;
	for (; CopyIndex < BYTES_OF_HEAT_ELEMENT; CopyIndex++)
	{
		*pBuf++ = Buf[CopyIndex];		//中间实际打印区
	}
	
	pBuf = NULL;
	ptLPB = NULL;
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//打印调试
	//#endif
}

/*******************************************************************************
* Function Name  : 处理反向打印时的数据转换
* Description    : 将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*	机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*	将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*	将行打印缓冲区pBuffer[ ColumnIndex ]开始递增到
*								pBuffer[ PrtDataDotIndex ]结束的字符依次内容颠倒后填充到
*								pBuffer[ PrtLength + LeftLimit - ColumnIndex ]开始递减到
*								pBuffer[ PrtLength + LeftLimit - PrtDataDotIndex ]结束的位置。
*	填充位置对应加热Buf[ PrtLength + LeftLimit - ColumnIndex ]开始递减到
*									Buf[ PrtLength + LeftLimit - PrtDataDotIndex ]结束的位置。
*	程序反向打印时先发送一行的最低位置的Lines值,递减发送到一行的最高位置的Lines值,
*	pBuf都是从左往右,所以处理规律:
*	将lines行, ColumnIndex		列的字符的对应 位值 作为填充字符的LSB,
*	将lines行,(ColumnIndex+1)	列的字符的对应 位值 作为填充字符的(LSB+1),
*	将lines行,(ColumnIndex+2)	列的字符的对应 位值 作为填充字符的(LSB+2),
*	将lines行,(ColumnIndex+3)	列的字符的对应 位值 作为填充字符的(LSB+3),
*	........
*	将lines行,(ColumnIndex+7)	列的字符的对应 位值 作为填充字符的(LSB+7),
*	至此一次填充字节合成完毕。
*	继续下一个循环。
*		  							操作全局变量
* Input          : 当前需要处理的行缓冲区指针PrintBuff和行数Lines,
*	PrintBuff指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Lines表示打印的行数,范围[0,最高字符*最高纵向放大倍数).
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	LineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//横向加热处理结果
	uint8_t	CurrentBit;		//当前字节填充位的位置,从8到1,为8时一个字节填充完毕		
	uint16_t	ColumnIndex;	//横向列计数索引,根据起始值补充空白位置和字节位
	uint16_t	i;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//根据对齐方式得到起始列值
	i = ptLPB->PrtMode.PrtLength - ColumnIndex;	//指向横向转换后加热缓冲区开始位置
	if (i % 8)
	{
		CurrentBit = 8 - (i % 8);	//可能出现不在8位整数倍位置,从右往左从最低位开始组合起
	}
	else
	{
		CurrentBit = 0;		//如果是8的整数倍则从0开始
	}
	
	i >>= 3;													//转化为字节
	if (i >= BYTES_OF_HEAT_ELEMENT)		//如果超过最后一个字节
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//防止下标越界
	}
	pBuf = Buf;
	pBuf += i;
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//防止非法内存越界
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	for (i = ptLPB->PrtMode.PrtLeftLimit; i < ptLPB->PrtMode.PrtDataDotIndex; i++)	//需要处理的列(字节)数目
	{
		//判断该字节对应位的值是否为1,Lines需要转换为范围0~7
		if (*PrintBuff & (0x80 >> (Lines & 0x07)))	//如果对应位置为1
		{
			*pBuf |= (uint8_t)(1 << CurrentBit);		//填充一个横向字节
		}
		CurrentBit++;					//字节位数每次不论处理字节对应位是否为1都需要进行处理
		
		if (CurrentBit == 8)	//已经填满一个横向加热字节
		{
			CurrentBit = 0;			//重新开始准备填充下一个字节,从LSB开始填充
			pBuf--;							//只有满一个字节后才减少			
		}
		
		PrintBuff++;					//准备检测下一列的字节,每次循环都增加
	}
	
	//以下将处理结果填充到横向加热缓冲区
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;		//跳过左侧32位空白区,右侧32位空白区已经清零	
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pBuf++ = Buf[i];		//中间实际打印区
	}
	
	pBuf = NULL;
	ptLPB = NULL;
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//打印调试
	//#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将一行8位数据的某一位取出,组合成字节,填充到机芯加热SPI缓冲区
*	机芯的左右各32点(4字节)位置为空,中间576点(72)字节是实际可加热区,
*	将数据按照打印方向和对齐方式的不同调整后填充到该缓冲区中
*									操作全局变量
* Input          : 当前需要处理的行缓冲区指针PrintBuff和行数Lines,
*	PrintBuff指向一行8位的点阵数据,长度最大576,从左往右排列计数
*	Lines表示打印的行数,范围[0,最高字符高度*最大纵向放大倍数).
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	FillHeatElementBuf(uint8_t *PrintBuff, uint8_t Lines)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印开始
	{
		LineBufToHeatElementNormalDir(PrintBuff, Lines);
	}	//正向打印处理结束
	else if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_REVERSE)	//反向打印开始
	{
		LineBufToHeatElementReverseDir(PrintBuff, Lines);
	}	//反向打印处理结束
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检查目前打印任务中内容是否处理完毕.检查字符模式的内容.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	CheckPrintContentFinishOfChar(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印
	{
		#ifdef	UNDER_LINE_PRT_ENABLE	//2012-9-21添加下划线打印处理
		if (ptLPB->PrtMode.PrintLineIndex < 
				(MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY))
		{	//如果还没有处理完毕,到开始打印处理打印下一点行
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{	//检查下划线是否打印完毕
			if (ptLPB->PrtMode.UnderlinePrintIndex == 0)	//下划线打印完毕
			{
				ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
				ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;	//如果全部处理完毕,准备设置走行间距
			}
			else	//如果还没有处理完毕,到开始打印处理打印下一点行下划线
			{
				ptLPB->Status = LPB_STATUS_START_PRT;
			}
		}
		#else
		if (ptLPB->PrtMode.PrintLineIndex < 
				(MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY))
		{	//如果还没有处理完毕,到开始打印处理打印下一点行
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{	//如果全部处理完毕,准备设置走行间距
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
		#endif
	}			//正向打印处理结束
	else	//反向打印
	{
		#ifdef	UNDER_LINE_PRT_ENABLE	//2012-9-21添加下划线打印处理
		//先检查下划线是否处理完毕
		if (ptLPB->PrtMode.UnderlinePrintIndex >= ptLPB->PrtMode.UnderlineMaxPrtHigh)
		{
			ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;	//下划线打印完毕
			//m_bReversePrintUnderlineFlag = 0;
			//再检查正常打印内容是否处理完毕
			if (ptLPB->PrtMode.PrintLineIndex > \
				(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY)
			{	//如果还没有处理完毕,到开始打印处理打印下一点行
				ptLPB->Status = LPB_STATUS_START_PRT;
			}
			else
			{	//如果全部处理完毕,准备设置走行间距
				ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
			}
		}
		else	//如果还没有处理完毕,到开始打印处理打印下一点行下划线
		{
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		#else
		if (ptLPB->PrtMode.PrintLineIndex > \
				(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY)
		{		//如果还没有处理完毕,到开始打印处理打印下一点行
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{		//如果全部处理完毕,准备设置走行间距
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
		#endif
	}			//反向打印处理结束
	//检查点行数是否全部处理完毕结束
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检查目前打印任务中内容是否处理完毕.检查图形模式的内容.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	CheckPrintContentFinishOfGraphic(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;	
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//正向打印
	{
		if (ptLPB->PrtMode.PrintLineIndex < ptLPB->PrtMode.MaxPrtDotHigh)
		{	//如果还没有处理完毕,到开始打印处理打印下一点行
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{	//如果全部处理完毕,准备设置走行间距
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
	}
	else
	{
		if (ptLPB->PrtMode.PrintLineIndex > 0)
		{		//如果还没有处理完毕,到开始打印处理打印下一点行
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{		//如果全部处理完毕,准备设置走行间距
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
	}
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检查目前打印任务中内容是否处理完毕.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	CheckPrintContentFinish(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;	
	if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))
	{
		CheckPrintContentFinishOfGraphic();
	}
	else
	{
		CheckPrintContentFinishOfChar();
	}
	
	SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
	StartTimer(PRINT_CHECK_TIMER);
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检查上一次加热是否完成,如果未完成则退出.
*	如果完成则锁存机芯数据,计算加热时间,检查走电机是否完成,如果未完成则退出.
*	如果完成则计算电机延时,置开始加热变量,启动定时器开始处理加热和走电机中断.
*	由加热处理中断程序修改开始加热变量值.
*	检查点行数是否全部处理完毕,如果全部处理完毕,准备设置走行间距.
*	如果还没有处理完毕,到开始打印
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void StartHeatAndRotateMotor(void)
{
	static	uint8_t	HeatTimeCalcFlag = 0;	//本次点行加热需要的时间是否计算过了
	static	uint16_t	HeatTime = 0;
	uint16_t	totalheatus;
	uint32_t	i;
	#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
		uint16_t	temp;
		TypeDef_StructLinePrtBuf * ptLPB = NULL;
	#endif
	PELedOff();   //2016.06.24  测试
	
	StopTimer(PRINT_CHECK_TIMER);
	if (g_tHeatCtrl.HeatEnable == HEAT_ENABLE)
	{
		if (HeatTimeCalcFlag == 0)	//充分利用空闲时间做好提前准备工作
		{
			HeatTime = GetHeatTime();	//计算加热时间
			
			#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
				ptLPB = &U_PrtBuf.g_tLinePrtBuf;
				if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
					(ptLPB->PrtMode.VariableBitImageFlag == 1)))
				{	//字符反显打印时加热时间减半处理
					if ((ptLPB->PrtMode.CharNotFlag) && (!g_tPrtCtrlData.PCDriverMode))
					{
						temp = HeatTime >> 2;
						HeatTime >>= 1;
						HeatTime += temp;
					}
				}
			#endif
			SetHeatNumber();					//计算本点行加热次数,设置加热控制管脚
			HeatTimeCalcFlag = 1;			//防止重复计算
		}
		return;		//检查之前的点行加热任务是否完成,如果未完成则退出
	}
	else		//如果上一次加热已经完成处理开始
	{
		if (HeatTimeCalcFlag == 0)	//充分利用空闲时间做好提前准备工作,如果是直接加热则在此防止漏算
		{
			HeatTime = GetHeatTime();	//计算加热时间
			
			#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
				ptLPB = &U_PrtBuf.g_tLinePrtBuf;
				if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
					(ptLPB->PrtMode.VariableBitImageFlag == 1)))
				{	//字符反显打印时加热时间减半处理
					if ((ptLPB->PrtMode.CharNotFlag) && (!g_tPrtCtrlData.PCDriverMode))
					{
						temp = HeatTime >> 2;
						HeatTime >>= 1;
						HeatTime += temp;
					}
				}
			#endif
			SetHeatNumber();					//计算本点行加热次数,设置加热控制管脚
			HeatTimeCalcFlag = 1;			//防止重复计算
		}
		
		if (g_tSystemCtrl.StepNumber != 0)	//检查走电机是否完成,如果未完成则退出
		{
			return;
		}
		else	//走电机已完成处理开始
		{
			g_HeatUs = HeatTime;
			g_tHeatCtrl.HeaterGroupNumberCpy = g_tHeatCtrl.HeaterGroupNumber;	//赋值控制变量
			for (i = 0; i < g_tHeatCtrl.HeaterGroupNumberCpy; i++)
			{
				g_tHeatCtrl.HeaterGroupElementCpy[ i ] = g_tHeatCtrl.HeaterGroupElement[ i ];
				g_tHeatCtrl.HeaterGroupElement[ i ] = 0;
			}
			
			LatchHeatData();		//锁存机芯数据
			
			totalheatus = g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy;	//一点行总加热时间
			AdjustMotorStep(totalheatus);					//计算电机延时,调整加速步数
			SetTimerCounterWithAutoReload(HEAT_TIMER, g_HeatUs);	//同时设置自动重载值和计时值
			g_tHeatCtrl.HeatEnable = HEAT_ENABLE;	//置开始加热变量
			HeatTime = 0;
			HeatTimeCalcFlag = 0;				//清零准备下一点行加热时间计算
			PELedOff();   //2016.06.24  测试
			BeepOff();
			TIM_SetCounter(HEAT_TIMER, 1);
			StartTimer(HEAT_TIMER);								//启动定时器开始处理加热和走电机中断
			CheckPrintContentFinish();	//检查点行数是否全部处理完毕,会启动TIM4
		}			//走电机已完成处理完毕
	}				//上次加热已完成处理完毕
	
	#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
		ptLPB = NULL;
	#endif
}

#define	STEP_TIME_COM	   		(1000000 *10*(PRT_DOT_NUMBERS/12)*2/3)/32	 	//串口标准打印时的最小走步时间系数，按2/3密度计算。
#define	STEP_TIME_COM_DRIVER	 (850)	 									//串口驱动打印时的最小走步时间固定为1000uS	125mm/s
uc32 BpsValue[8] ={1200,2400,4800,9600,19200,38400,57600,115200};	//波特率表 2016.05.23
/*******************************************************************************
* Function Name  : MaxSetp(void )
* Description    : 选择电机加速的最大步数
* Input          : None
* CONput         : SystemCtrlStruct.MaxStep[i]中存放当前条件下的最大加速步数
* Return         : None
*******************************************************************************/
void MaxSetp(void)			//2016.05.23
{
	uint8_t Bps, i;
	uint32_t Time[2];
  uint32_t TempNum[2];
	if(g_tSystemCtrl.PrtSourceIndex == COM)	 			//串口
	{
		{
			Bps = g_tSysConfig.ComBaudRate & 0x07;
			if( Bps <= 3 )										//小于等于9600固定为同一速度
				Bps =3;
		}
		//取最快走步时间
		Time[0] =STEP_TIME_COM / BpsValue[Bps];				//字符模式，
        
		Time[1] =STEP_TIME_COM_DRIVER;								//驱动模式
		
		//查找字符模式、驱动模式下设置最大速度下的最大加速步数
		for(i=0; i<2; i++)			//SystemCtrlStruct.MaxStep[i]中为加数步数，MaxStep[SysConfigStruct.MaxSpeed]中为设置速度的最大步数
		{
			for(g_tSystemCtrl.MaxStepPrtNum[i] =0; g_tSystemCtrl.MaxStepPrtNum[i] < MaxStepNum[g_tSysConfig.MaxSpeed];g_tSystemCtrl.MaxStepPrtNum[i]++ )
			{
				if( g_tFeedMotorRushTable[g_tSystemCtrl.MaxStepPrtNum[i]] < Time[i])
					break;
			}
		}

	}
	else													//并口 usb口等
	{
		g_tSystemCtrl.MaxStepPrtNum[0]  =	MaxStepNum[g_tSysConfig.MaxSpeed]; 	//把最大速度赋值
		
		if((g_tSysConfig.MaxSpeed >=1))			  		//驱动方式 且设置的速度等级大于1
		{
			g_tSystemCtrl.MaxStepPrtNum[1] = MaxStepNum[1];
		}
		else
		{	
			g_tSystemCtrl.MaxStepPrtNum[1] = MaxStepNum[g_tSysConfig.MaxSpeed];	
		}	
		if((g_tSystemCtrl.PrtSourceIndex == LPT))          //2016.08.09并口驱动方式下 最高速度限制  && (g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE)
    {
			 if(g_tSystemCtrl.MaxStepPrtNum[1] > LPT_Drver_MaxStep)
			 {	 
					g_tSystemCtrl.MaxStepPrtNum[1] = LPT_Drver_MaxStep;  
			 } 
    }	
		if((g_tSystemCtrl.PrtSourceIndex == USB) )          //并口驱动方式下 最高速度限制 && (g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE)
    {
			 if(g_tSystemCtrl.MaxStepPrtNum[1] > USB_Drver_MaxStep)
			 {	 
					g_tSystemCtrl.MaxStepPrtNum[1] = USB_Drver_MaxStep;          // MAX_RUSH_STEPS
			 } 
    }
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检查上次走电机是否完成,如果未完成则退出.
*	如果完成则计算电机延时,设置走行间距值,启动定时器开始处理走电机,
*	重新初始化打印缓冲区,状态改为ILDE.
* Input          : 输入参数,行间距g_tPrtCtrlData.CodeLineHigh
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#if	0
static void SetGoLineSpace(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	//如果之前的加热及走电机任务完成,则进行设置行间距处理,否则不处理
	if ((g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && (g_tSystemCtrl.StepNumber == 0))
	{
		ptLPB = &U_PrtBuf.g_tLinePrtBuf;
		if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
				(ptLPB->PrtMode.VariableBitImageFlag == 1)))
		{	//图形打印模式时不走行间距
			if (ptLPB->PrtMode.CodeLineHigh > ptLPB->PrtMode.MaxLineHigh)
			{
				g_tSystemCtrl.StepNumber = 
					(ptLPB->PrtMode.CodeLineHigh - ptLPB->PrtMode.MaxLineHigh) * STEP_NUMBERS_PER_DOT_HEIGHT;
			}
		}
		
		ptLPB->Status	=	LPB_STATUS_ILDE;	//清除放到复制时进行
		TIM_SetCounter(FEED_TIMER, 1);
		StartTimer(FEED_TIMER);						//启动定时器开始处理走电机
		InitHeatElementBuf();							//清零加热缓冲区
	}
}
#else
static void SetGoLineSpace(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	//不判断之前的加热及走电机任务是否完成,直接进行设置行间距处理,如果不走行间距则不处理
	//如果之前的加热及走电机任务完成,则进行设置行间距处理,否则不处理
	if ((g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && (g_tSystemCtrl.StepNumber == 0))
	{
		ptLPB = &U_PrtBuf.g_tLinePrtBuf;
		if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
					(ptLPB->PrtMode.VariableBitImageFlag == 1)))
		{	//图形打印模式时不走行间距
			if (ptLPB->PrtMode.CodeLineHigh > ptLPB->PrtMode.MaxLineHigh)
			{
				g_tSystemCtrl.StepNumber += \
				(ptLPB->PrtMode.CodeLineHigh - ptLPB->PrtMode.MaxLineHigh) * STEP_NUMBERS_PER_DOT_HEIGHT;
			}
		}
		PELedOn();   //2016.06.24  测试
		BeepOn();
		ptLPB->Status	=	LPB_STATUS_ILDE;
		InitHeatElementBuf();							//清零加热缓冲区
		ClearLinePrtBuf();
		TIM_SetCounter(FEED_TIMER, 1);
		StartTimer(FEED_TIMER);						//启动定时器开始处理走电机
		
		ptLPB = NULL;
	}
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
