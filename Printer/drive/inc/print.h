/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   打印控制相关的程序.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PRINT_H
#define PRINT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
//#define	MAX_PRT_DOT_HEIGHT			(MAX_RATIO_Y * MAX_FONT_HEIGHT)	//最大打印高度点数,最大倍高×24点
//#define	MAX_PRT_HEIGHT_BYTE			(MAX_PRT_DOT_HEIGHT / 8)				//最大打印高度字节数,如不能整除需要调整
//#define BYTES_OF_LINE_PRT_BUF		(PRT_DOT_NUMBERS * MAX_PRT_HEIGHT_BYTE)	//行打印缓冲区大小
//#define BYTES_OF_LPB						BYTES_OF_LINE_PRT_BUF

//机芯加热点数
//#define	DOTS_OF_ALL_ELEMENT										(640)
//#define	DOTS_OF_HEAT_ELEMENT									(576)
//#define	DOTS_OF_LEFT_EMPTY_HEAT_ELEMENT				(32)
//#define	DOTS_OF_RIGHT_EMPTY_HEAT_ELEMENT			(32)

//#define	BYTES_OF_ALL_ELEMENT									(76)			//2016.06.16 80改为76,608点
//#define	BYTES_OF_HEAT_ELEMENT									(72)
//#define	BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT			(0)				//2016.06.16 4改为0 
//#define	BYTES_OF_RIGHT_EMPTY_HEAT_ELEMENT			(4)

#define	LINE_PRT_BUF_NONE											(0)
#define	LINE_PRT_BUF_A												(1)
#define	LINE_PRT_BUF_B												(2)

#define	LINE_PRT_BUF_STATUS_ILDE							(0)
#define	LINE_PRT_BUF_STATUS_WRITING						(1)
#define	LINE_PRT_BUF_STATUS_WAIT_TO_PRT				(2)
#define	LINE_PRT_BUF_STATUS_START_PRT					(3)
#define	LINE_PRT_BUF_STATUS_SPI_SENDING				(4)
#define	LINE_PRT_BUF_STATUS_SPI_SEND_OVER			(5)
#define	LINE_PRT_BUF_STATUS_SET_GO_LINE_SPACE	(6)

#define	LPB_STATUS_ILDE								LINE_PRT_BUF_STATUS_ILDE
#define	LPB_STATUS_WRITING						LINE_PRT_BUF_STATUS_WRITING
#define	LPB_STATUS_WAIT_TO_PRT				LINE_PRT_BUF_STATUS_WAIT_TO_PRT
#define	LPB_STATUS_START_PRT					LINE_PRT_BUF_STATUS_START_PRT
#define	LPB_STATUS_SPI_SENDING				LINE_PRT_BUF_STATUS_SPI_SENDING
#define	LPB_STATUS_SPI_SEND_OVER			LINE_PRT_BUF_STATUS_SPI_SEND_OVER
#define	LPB_STATUS_SET_GO_LINE_SPACE	LINE_PRT_BUF_STATUS_SET_GO_LINE_SPACE

#define	UNDERLINE_PRINT_UNFINISHED						(0)
#define	UNDERLINE_PRINT_FINISHED							(1)

#define	LPB_CHECK_WAIT_TO_PRT_INTERVAL_US			(1)
#define	LPB_CHECK_START_PRT_INTERVAL_US				(1)
#define	LPB_CHECK_SEND_OVER_INTERVAL_US				(1)
#define	LPB_CHECK_SET_LINE_SPACE_INTERVAL_US	(1)

#define	LINE_PRT_BUF_CHECK_INTERVAL_US				(1)
#define	LPB_CHECK_INTERVAL_US									LINE_PRT_BUF_CHECK_INTERVAL_US

#define	LPB_PROCESS_STATUS_UNFINISH						(0)
#define	LPB_PROCESS_STATUS_FINISH							(1)

/********** 打印中需要的参数定义 ***********/
//目前打印控制中需要使用的参数,目前准备使用一个行打印缓冲区
//typedef struct
//{
//	uint16_t	PrtDataDotIndex;				//列计数索引
//	uint16_t	CodeLineHigh;						//字符模式下的行高控制,按点计算,影响走行间距	
//	uint16_t	PrtLeftLimit;						//打印区域左边距,与页模式采用不同的值,以横向移动单位计算
//	uint16_t	PrtRightLimit;					//打印区域右边距,与页模式采用不同的值,以横向移动单位计算
//	uint16_t	PrtWidth; 							//打印宽度,实际使用的打印区域
//	uint16_t	PrtLength;							//总打印宽度= 左边距 + 打印宽度,包括左边的空白和规定的打印区域
//	uint16_t	MaxPrtDotHigh;					//编辑/打印缓冲区内容以基线为基准算起,所有打印内容中高度最高的点数值
//	
//	uint8_t	PrtModeFlag;							//打印模式控制,	0 = 普通 and 1 = 页模式
//	uint8_t	BitImageFlag;							//位图打印模式标志
//	uint8_t	VariableBitImageFlag;			//可放大位图打印模式标志
//	uint8_t	MaxLineHigh;							//一行中最高的字符/位图尺寸,一行字符等于字符宽度+字符右间距
//#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
//	uint8_t	MaxRatioX;								//一行内最大横向放大倍数
//#endif
//	uint8_t	MaxRatioY;								//一行内最大纵向放大倍数	
//	uint8_t	CodeAimMode;							//对齐方式,0=左对齐,1=中间对齐,2=右对齐
//	uint8_t	PrintDirection;						//打印方向
//#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
//	uint8_t	CharNotFlag;							//字符反显的控制,高有效
//#endif
//#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
//	uint8_t	EmphaSizeMode;
//#endif
//	uint8_t	CharCycle90;							//旋转90度打印标志,=1表示旋转90度打印,=0表示不旋转打印
//	uint8_t	CharHeightMagnification;	//字符纵向放大倍数
//	uint8_t	HZHeightMagnification;		//汉字纵向放大倍数
//	uint8_t	PrintLineIndex;						//当前打印点行索引数
//	uint8_t	DoUnderlineFlag;					//字符下划线控制,1=on; 0= off
//	uint8_t	UnderlineMaxPrtHigh;			//一行编辑过程中出现的下划线最高的点数,范围0~3,据此确定打印时的取值	
//	uint8_t	UnderlinePrintIndex;			//标志下划线点行数处理过程的变量,正向时从0到最高;反向时从最高到0
//	uint8_t	UnderLinePrtOverFlag;
//}TypeDef_StructPrtMode;

//#define	LINE_PRT_BUF_LEN		(BYTES_OF_LINE_PRT_BUF + PRT_DOT_NUMBERS)
//typedef	struct
//{
//	__IO uint8_t	Status;											//行打印缓冲区的状态
//	TypeDef_StructPrtMode			PrtMode;	//行打印控制参数
//	//将下划线缓存与打印内容缓冲区合并,最后PRT_DOT_NUMBERS个字节作为下划线使用
//	uint8_t	Buf[LINE_PRT_BUF_LEN];
//}TypeDef_StructLinePrtBuf;

#define	UNDER_LINE_PRT_FINISHED				(0)
#define	UNDER_LINE_PRT_UNFINISHED			(1)

/* Funtion Declare -----------------------------------------------------------*/
void	InitLinePrtBuf(void);
void	ClearLinePrtBuf(void);

void	InitHeatElementBuf(void);
void	InitVariableToStartNewLine(void);
void	SetLinePrtModeVariable(void);

void	CopyDotLineDataToHeatElementBuf(const uint8_t * pBuf);
void	DotLinePutDrvBuf(uint8_t * pBuf);
void	SendHeatElementBufToCore(void);
void	PrintOneDotLine(void);
void	PrintLineBuffer(void);
uint32_t	GetLPBProcessStatus(void);
uint32_t	GetPrintStatus(void);
void MaxSetp(void);
#ifdef __cplusplus
}
#endif

#endif /* PRINT_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
