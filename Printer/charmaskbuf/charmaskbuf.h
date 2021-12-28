/**
******************************************************************************
  * @file    x.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CHARMASKBUF_H
#define CHARMASKBUF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"print.h"

/*	字符处理相关的程序包括：字符的分类、可打印字符的分析、对应字模的读取、
		字模的放大/缩小/上下标/反显/加粗/加重等处理、往行打印缓冲区的存放、
		行打印缓冲区字符取出一点行、用户自定义字符、下划线等内容。
*/
/* Macro Definition ----------------------------------------------------------*/
#define	LINE_BUF_NOT_FULL				(0)
#define	LINE_BUF_FULL						(1)

/*字符读取缓冲区的三种状态*/
#define	CHAR_MASK_READ_BUF_STATUS_ILDE						(0)		/*空闲,可以写入*/
#define	CHAR_MASK_READ_BUF_STATUS_WRITING					(1)		/*正在由SPI写入*/
#define	CHAR_MASK_READ_BUF_STATUS_WAIT_TO_DEAL		(2)		/*写入完毕,等待处理*/
#define	CMRB_STATUS_ILDE						CHAR_MASK_READ_BUF_STATUS_ILDE
#define	CMRB_STATUS_WRITING					CHAR_MASK_READ_BUF_STATUS_WRITING
#define	CMRB_STATUS_WAIT_TO_DEAL		CHAR_MASK_READ_BUF_STATUS_WAIT_TO_DEAL

/*编辑行打印缓冲区的状态*/
#define	LINE_EDIT_BUF_STATUS_ILDE						(0)		/*空闲,可以写入*/
#define	LINE_EDIT_BUF_STATUS_WRITING				(1)		/*正在编辑中*/
#define	LINE_EDIT_BUF_STATUS_WAIT_TO_PRINT	(2)		/*本行处理完毕,等待处理*/
#define	LEB_STATUS_ILDE							LINE_EDIT_BUF_STATUS_ILDE
#define	LEB_STATUS_WRITING					LINE_EDIT_BUF_STATUS_WRITING
#define	LEB_STATUS_WAIT_TO_PRINT		LINE_EDIT_BUF_STATUS_WAIT_TO_PRINT

/********** 字符排版参数全局变量 ***********/
typedef struct
{
	uint8_t	CharWidth;							//字符或汉字宽
	uint8_t	CharHigh;								//字符高24或17
	uint8_t	HeightMagnification;		//纵向放大倍数Ratio
	uint8_t	WidthMagnification;			//横向放大倍数Ratio
	uint8_t OneLineWidthFlag;				//行内倍宽标志
	
	uint8_t	LeftSpace;							//左间距
	uint8_t	RightSpace;							//右间距
	uint8_t	CodeAimMode;						//字符对齐模式,0 = 左对齐, 1 = 中间对齐, 2 = 右对齐
	
	uint8_t	EmphaSizeMode;					//加粗模式
	uint8_t	CodeInversion;					//倒置打印控制,0 = normal Mode ; 1= Inversion Mode
	uint8_t	CharCycle90;						//字符旋转90°控制, 高有效
	uint8_t	CharRBFlag;							//字符颠倒（翻转180°）打印控制,高有效
	uint8_t	CharNotFlag;						//字符反显的控制,高有效
	uint8_t	Underline;							//字符下划线控制,bit3 = on/off; bit1~0 = 下划线样式（单或双线）
}TypeDef_StructTypeSet;		//Print_Mode_Struct

typedef	struct
{
	uint8_t	Status;											//字符读取缓冲区状态
	uint8_t	LanguageType;								//字符种类:英文,汉字,BIG5
	uint8_t	FontType;										//点阵种类:9*17,12*24,24*24	
	uint8_t	Buf[MAX_BYTES_OF_CHAR];
}TypeDef_StructCharMaskReadBuf;

/* Private function prototypes -----------------------------------------------*/
void	PrintChar(uint8_t CharCode);
void	PrintAscII(uint8_t Code);
void	PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow);
void	PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4);
void	PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow);

void	JudgePrintSelfDefChinese(uint8_t Ch1);
void	JudgePrintBIG5(uint8_t Ch1);
void	JudgePrintGB18030(uint8_t Ch1);

void	FillDataPrintBuff(uint8_t Data, uint32_t Width, uint32_t Line);
void	FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t CodeType);
//void	FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t RatioX, uint8_t CodeType);

void	CodeRollCtrl(uint8_t	CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[]);
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t *SrcPrt);
void	ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t *DscPrt);
void	ZoomByteHeight(uint8_t ByteData, uint8_t ZoomRate, uint8_t * DscPrt);

void	GetAscIIFontADotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIFontBDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIUserDefDotData(uint8_t AscIICode);

uint32_t	GetUSACharacterStartAddress(uint8_t AscIICode);
uint32_t	GetInternationalCharacterStartAddress(uint8_t	AscIICode);
uint32_t	GetCodepageCharacterStartAddress(uint8_t	AscIICode);

/*支持三种字体*/
uint32_t	GetUSACharStartAddress(uint8_t	AscIICode, uint8_t FontType);
uint32_t	GetInternationalCharStartAddress(uint8_t	AscIICode, uint8_t FontType);
uint32_t	GetCodepageCharStartAddress(uint8_t	AscIICode, uint8_t FontType);

void	GetAscIIFlashDotData(uint8_t AscIICode);

/* Funtion Declare -----------------------------------------------------------*/
uint32_t	GetPrintBusyStatus(void);
uint32_t	GetPrintProcessStatus(void);
uint32_t	GetFeedPaperStatus(void);

void	InitCharMaskBufParameter(void);
void	PrintOneLine(void);
void	ProcessCharMaskReadBuf(void);
void	DataProcess(uint8_t GetChar);	//分析处理通信接收缓冲区的内容
void	UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);	//上传数据
void	CopyEditDataToLinePrtStruct(void);
void	PrepareNewLine(void);
void	PrintString(const uint8_t * Str);
void	FlashDecryption(uint8_t	* Src, uint32_t ByteNum);

static uint8_t	SelectLineBufToEdit(void);
static uint8_t	GetRatioXValue(uint8_t CodeType);
uint8_t	JudgeLineBufFull(uint8_t CodeType);
void	FillLinePrintBuf(uint8_t *Source, uint8_t CodeType);

static void	ProcessCharMaskReadBufIlde(void);
static void	ProcessCharMaskReadBufEdit(void);

void MovDotLine (u8 *VBuf, u8 LineID);             //2016.06.28	
void PrtOneLineToBuf(uint32_t LineHigh);
void PrtAsciiToBuf(uint8_t Code);
#ifdef __cplusplus
}
#endif

#endif /* CHARMASKBUF_H定义完毕 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
