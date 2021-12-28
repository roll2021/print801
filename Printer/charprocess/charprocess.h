/**
******************************************************************************
  * @file    x.h
  * @author  WH Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   字符处理相关的程序.
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
  * <h2><center>&copy; COPYRIGHT 2012 WH</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CHARPROCESS_H
#define CHARPROCESS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"cominbuf.h"
#include	"monobyte.h"
#include	"esc.h"
#include	"fs.h"
#include	"gs.h"

/*	字符处理相关的程序包括：字符的分类、可打印字符的分析、对应字模的读取、
		字模的放大/缩小/上下标/反显/加粗/加重等处理、往行打印缓冲区的存放、
		行打印缓冲区字符取出一点行、用户自定义字符、下划线等内容。
*/
/* Macro Definition ----------------------------------------------------------*/
/******* 定义各字体占用字节数 *************/
#define BYTES_OF_ASCII_FONT_A		(36)		//12 X 24 点阵字符的点阵数据字节数
#define BYTES_OF_ASCII_FONT_B		(27)		//9 X 17 点阵字符的点阵数据字节数
#define BYTES_OF_HZ_FONT_A			(72)		//24 X 24 点阵字符的点阵数据字节数
#define	MAX_BYTES_OF_CHAR				(BYTES_OF_HZ_FONT_A)
#define	MAX_BYTES_OF_HZ					(BYTES_OF_HZ_FONT_A)

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
	uint8_t	CharRBFlag;							//字符颠倒（翻转180°）打印控制，高有效
	uint8_t	CharNotFlag;						//字符反显的控制，高有效
	uint8_t	Underline;							//字符下划线控制,bit7 = on/off; bit1~0 = 下划线样式（单或双线）
}TypeDef_StructTypeSet;		//Print_Mode_Struct

typedef	struct
{
	uint8_t	Status;											//字符读取缓冲区状态
	uint8_t	LanguageType;								//字符种类:英文,汉字,BIG5
	uint8_t	FontType;										//点阵种类:9*17,12*24,24*24
	TypeDef_StructTypeSet	structTypeSet;		//字体排版相关变量值
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

uint8_t		JudgeLineFull(uint8_t CodeType);
void	FillDataPrintBuff(uint8_t Data, uint32_t Width, uint32_t Line);
void	FillPrintBuff(uint8_t * Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t RatioX, uint8_t CodeType);
void	FillPrint(uint8_t * Source, uint8_t CodeType);

void	CodeRollCtrl(uint8_t	CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[]);
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt);
void	ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t * DscPrt);
void	ZoomByteHeight(uint8_t ByteData, uint8_t ZoomRate, uint8_t * DscPrt);

void	GetAscIIFontADotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIFontBDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[]);

/* Private functions ---------------------------------------------------------*/
/* Funtion Declare -----------------------------------------------------------*/
void	DataProcess(uint8_t GetChar);	//分析处理通信接收缓冲区的内容

#ifdef __cplusplus
}
#endif

#endif /* CHARPROCESS_H定义结束 */

/******************* (C) COPYRIGHT 2012 WH *****END OF FILE****/
