/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-9
  * @brief   CPCL指令集字符处理相关程序.处理字符。
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
#include	"ctext.h"
#include	"charmaskbuf.h"
#include	"extgvar.h"
#include	"spiflash.h"

#ifdef	CPCL_CMD_ENABLE

/*定义字符读取缓冲区,全局变量,用于CPCL指令*/
TypeDef_StructCharMaskReadBuf		g_tpCharMaskReadBuf;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*******************************************************************************
* Function Name  : InitCharMaskBufParameter
* Description    : 初始化字符读取缓冲区
* Input          : 输入参数
* Return         : 返回参数
*******************************************************************************/
void	InitPageCharMaskBufParameter(void)
{
	g_tpCharMaskReadBuf.Status       = CMRB_STATUS_ILDE;	//设为空闲
	g_tpCharMaskReadBuf.LanguageType = g_tSysConfig.SysLanguage;	//语言
	g_tpCharMaskReadBuf.FontType     = g_tSysConfig.SysFontSize;	//字体
	memset(g_tpCharMaskReadBuf.Buf, 0x00, MAX_BYTES_OF_HZ);
}

/*******************************************************************************
* Function Name  : InitCharMaskBufParameter
* Description    : 设置字符读取缓冲区中点阵的字体号，语言号
* Input          : 输入参数
* Return         : 返回参数
*******************************************************************************/
void	SetPageCharParameter(uint8_t Language, uint8_t FontType)
{
	g_tpCharMaskReadBuf.LanguageType = Language;	//语言,西文，GB18030，BIG5
	g_tpCharMaskReadBuf.FontType     = FontType;	//字体,ASCIIA/B/C;HZA/B
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 页模式下处理打印
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	GetPageCharDotData(uint8_t CharCode, uint8_t DotData[])
{
	if (g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH)	//西文模式
	{
  	GetPageASCIIDotData(CharCode, DotData);
	}	//西文处理结束
  else										//汉字模式
	{	//自定义汉字
  	if ((g_tSystemCtrl.UserSelectFlag) && \
				(CharCode == 0xFE) && \
				((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x02) == 0x02))
		{
			//JudgePrintSelfDefChinese(CharCode);	//处理自定义汉字打印
		}
		else if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)	//GB18030
		{
			JudgePagePrintGB18030(CharCode, DotData);	//处理GB18030打印  	
		}
		else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)	//BIG5汉字方式
  	{
			//JudgePrintBIG5(CharCode);			//处理BIG5汉字打印
		}
		else
		{
		}
	}	//汉字处理结束
}

/*******************************************************************************
* Function Name  : 
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址,并启动读取
*	先处理是否是自定义字符,直接从RAM中复制;否则就在字库FLASH中,启动SPI DMA
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体12*24和9*17,8*16
* Output         : 
* Return         : 
*******************************************************************************/
void	GetPageASCIIDotData(uint8_t AscIICode, uint8_t DotData[])
{
	uint8_t	FontType;
	
	FontType = g_tPageModeCharPara.FontType;	//字体编号
	
	//定义了自定义的字符,且选择了,且对应字符有定义
	if ((g_tSystemCtrl.UserSelectFlag) && \
			(AscIICode < 0x7F) && \
			((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x01) == 0x01))
	{
		GetUserDefCharDotData(AscIICode, FontType, DotData);
	}
	else	//无用户自定义字符
	{
		GetCharFlashDotData(AscIICode, FontType, DotData);	//读取字库中的点阵数据
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : 取字库中字符字模数据,查表获得各种ASCII字符的点阵地址
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体12*24和9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetCharFlashDotData(uint8_t AscIICode, uint8_t FontType, uint8_t DotData[])
{
	uint16_t	Len;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < 0x80)
	{
		if (g_tSystemCtrl.CharacterSet)	//如果设置了国际字符集指令 0x1b 52 n,USA=0
		{
			AsciiCodeAddr = GetInternationalCharStartAddress(AscIICode, FontType);
		}		//End 国际字符
		else
		{
			AsciiCodeAddr = GetUSACharStartAddress(AscIICode, FontType);
		}
	}			//End <0x80
	else	//----FLASH中0x80-0xFF点阵的ASCII字符----//
	{
		AsciiCodeAddr = GetCodepageCharStartAddress(AscIICode, FontType);
	}
	
	if (FontType == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		Len = BYTES_OF_ASCII_FONT_A;
	}
	else if (FontType == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		Len = BYTES_OF_ASCII_FONT_B;
	}
	else if (FontType == ASCII_FONT_C)	//FLASH中英文8x16点阵的ASCII字符
	{
		Len = BYTES_OF_ASCII_FONT_C;
	}
	else																//默认A
	{
		Len = BYTES_OF_ASCII_FONT_A;
	}
	
	sFLASH_ReadBuffer(DotData, AsciiCodeAddr, Len);
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(DotData, Len);
	#endif
	
	SetPageCharParameter(LANGUAGE_ENGLISH ,FontType);
}

/*******************************************************************************
* Function Name  : 
* Description    : 读取自定义字符字模数据,查表获得各种ASCII字符的点阵地址,并读取
*	已定义字符,直接从RAM中复制;否则就在字库FLASH中读取
*	AscIICode = ASCII字符值
* FontType 字符点阵的类型，当前程序处理字体12*24和9*17,8*16
*	DotData[] = 存储ASCII字符的字模数据
* Output         : 
* Return         : 
*******************************************************************************/
void	GetUserDefCharDotData(uint8_t AscIICode, uint8_t FontType, uint8_t DotData[])
{
	uint8_t 	Flag;
	uint16_t	Len;
	uint32_t	StartAddress;
	uint8_t * pselfdef;
	uint8_t * pbuf;
	
	Flag = 0;
	if (FontType == ASCII_FONT_A)				//字体A
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_A;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	else if (FontType == ASCII_FONT_B)	//字体B
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_09[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_B;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_09;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	else if (FontType == ASCII_FONT_C)	//字体C
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_08[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_C;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_C;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_08;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	else																//默认字体A
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_A;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	
	if (Flag == 1)	//存在定义字符
	{
		pbuf = DotData;
		while (Len--)
		{
			*pbuf++ = *pselfdef++;
		}
		
		SetPageCharParameter(LANGUAGE_ENGLISH ,FontType);
	}
	else
	{
		GetCharFlashDotData(AscIICode, FontType, DotData);	//读取字库中的点阵数据
	}
}

/*******************************************************************************
* Function Name  : void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
* Description    : 如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,否则,打印当前行.
* Input          : Code1,Code2,Code3,Code4为汉字代码,当Code3=Code4=0时为双字节代码
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void	GetPageGB18030DotData(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4, uint8_t DotData[])
{
	uint8_t	FontType;
	uint16_t	Len;
	uint32_t	CodeIndex;
	uint32_t	CodeAddr;
	
	//计算字符地址
	if ((Code3 == 0) || (Code4 == 0))	 //双字节方式
	{
		if ((Code1 >= 0xA1) && (Code1 <= 0xA9) && (Code2 >= 0xA1))			//1区符号
		{	
			CodeIndex = (Code1 - 0xA1) * 94 + (Code2 - 0xA1);
		}
		else if ((Code1 >= 0xA8) && (Code1 <= 0xA9) && (Code2 < 0xA1))	//5区符号
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeIndex = (Code1 - 0xA8) * 96 + (Code2 - 0x40) + 846;
		}		
		else if ((Code1 >= 0xB0) && (Code1 <= 0xF7) && (Code2 >= 0xA1))	//2区汉字
		{
			CodeIndex = (Code1 - 0xB0) * 94 + (Code2 - 0xA1) + 1038;
		}
		else if ((Code1 >= 0x81) && (Code1 <= 0xA0))		//3区汉字
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeIndex = (Code1 - 0x81) * 190 + (Code2 - 0x40) + 7806;	//1038 + 6768
		}
		else if ((Code1 >= 0xAA) && (Code2 < 0xA1))			//4区汉字
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeIndex = (Code1 - 0xAA) * 96 + (Code2 - 0x40) + 13886;	//1038 + 12848
		}
		else
		{
			CodeIndex = 0;		//其他编码均为保留码
		}
	}		//双字节处理结束
	else
	{
		//CodeIndex =	(Code1 - 0x81) * 12600 + (Code2 - 0x39) * 1260 + 
		//						(Code3 - 0xEE) * 10 + (Code4 - 0x39) + 22046;
		if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 == 0xEE) && (Code4 == 0x39))
		{	//高位8139的单独一个字符
			CodeIndex = 22046;
		}
		else if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 >= 0xEF))
		{	//8139页剩余部分
			CodeIndex = (Code3 - 0xEF) * 10 + (Code4 - 0x30) + 22047;	//22046 + 1
		}
		else if (	(Code1 == 0x82) && \
							((Code2 >= 0x30) && (Code2 <= 0x34)) && \
							((Code3 >= 0x81) && (Code3 <= 0xFE)))
		{	//8230页~8234页,均为满页
			CodeIndex = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
		}
		else if ((Code1 == 0x82) && \
						 (Code2 == 0x35) && \
						((Code3 >= 0x81) && (Code3 <= 0x87)))
		{	//8235页,只有69个字符
			CodeIndex = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
		}
		else
		{	//其他的一律为保留位,全空
			CodeIndex = 0;
		}
	}
	
	FontType = g_tPageModeHZPara.FontType;
	if (FontType == HZ_FONT_A)
	{
		Len = BYTES_OF_HZ_FONT_A;
		CodeAddr = GB18030_24X24_BASE_ADDR;
	}
	else if (FontType == HZ_FONT_B)
	{
		Len = BYTES_OF_HZ_FONT_B;
		CodeAddr = GB18030_16X16_BASE_ADDR;
	}
	else
	{
		Len = BYTES_OF_HZ_FONT_A;
		CodeAddr = GB18030_24X24_BASE_ADDR;
	}
	CodeAddr = CodeAddr + (CodeIndex * Len);
	
	sFLASH_ReadBuffer(DotData, CodeAddr, Len);		//读取字模
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(DotData, Len);
	#endif
	
	SetPageCharParameter(LANGUAGE_GB18030, FontType);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : GB18030汉字数据处理分析,判断数据是否合格,并处理打印
* Input          : 输入参数,第一字节编码
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	JudgePagePrintGB18030(uint8_t Ch1, uint8_t DotData[])
{
	uint8_t	Ch2;
	uint8_t	Ch3;
	uint8_t	Ch4;
	
	if ((Ch1 < 0x81) || (Ch1 > 0xFE))		//第一代码为无效汉字代码
	{
		GetPageASCIIDotData(Ch1, DotData);	//第一字节为西文字符打印
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x30) || (Ch2 == 0x7F) || (Ch2 > 0xFE))	//无效汉字代码
		{
			GetPageASCIIDotData(Ch1, DotData);	//第一字节按西文字符打印
			DecInBufferGetPoint();		//放回最后读取的字符
		}
		else if (((Ch2 >= 0x40) && (Ch2 <= 0x7E)) || \
						 ((Ch2 >= 0x80) && (Ch2 <= 0xFE)))
		{
			Ch3 = 0;
			Ch4 = 0;
			GetPageGB18030DotData(Ch1, Ch2, Ch3, Ch4, DotData);	//两字节汉字
		}
		else									//四字节方式
		{
			Ch3 = ReadInBuffer();
			if ((Ch3 < 0x81) || (Ch3 > 0xFE))  	//第三字节代码无效
			{
				GetPageASCIIDotData(Ch1, DotData);	//第一字节按西文字符打印
				GetPageASCIIDotData(Ch2, DotData);	//第二字节按西文字符打印
				DecInBufferGetPoint();	//放回最后读取的字符
			}
			else
			{
				Ch4 = ReadInBuffer();
				if ((Ch4 < 0x30) || (Ch4 > 0x39))	//第四字节代码无效
				{
					GetPageASCIIDotData(Ch1, DotData);	//第一字节按西文字符打印
					GetPageASCIIDotData(Ch2, DotData);	//第二字节按西文字符打印
					GetPageASCIIDotData(Ch3, DotData);	//第三字节为有效打印字符
					DecInBufferGetPoint();	//放回最后读取的字符,ch4有可能是合法的双字节编码的低位字节
				}
				else
				{
					GetPageGB18030DotData(Ch1, Ch2, Ch3, Ch4, DotData);	//四字节汉字
				}
			}
		}
	}
}

/* 以下为处理字符或者汉字的点阵按照指定的参数进行变换及填充到打印缓冲区的处理 */
/*******************************************************************************
* Function Name  : FillPageCharHZToEditBuf
* Description    : 以下为处理将读取的字符或者汉字的点阵按照指定的字体及大小，加粗，
	纵向横向放大倍数，反显，旋转，下划线等设置进行点阵变换，将变换后的结果放到指定的
	缓冲区。
	给定的源数据为竖置竖排，变换处理中一律按照底部对齐，竖置竖排处理。
	目前使用ESC的编辑缓冲区进行编辑 g_tLineEditBuf[] 。
	源点阵数据保存在全局变量 g_tpCharMaskReadBuf.Buf[]，另包括数据的字体大小及语言。
	纵向横向放大倍数，加粗，反显，旋转，下划线等设置在 g_tPageModeCharPara,
	g_tPageModeHZPara 变量中。
* Input          : 输入参数
* Return         : 返回参数
*******************************************************************************/
void	FillPageCharHZToEditBuf(uint8_t * pDst, uint8_t * pSrc)
{
	uint8_t	CodeType;
	uint8_t	FontType;
	uint8_t	Width;
	uint8_t	High;
	uint8_t	Temp;
	uint8_t * pbuf;
	
	CodeType = g_tpCharMaskReadBuf.LanguageType;
	FontType = g_tpCharMaskReadBuf.FontType;
	
	if (CodeType == LANGUAGE_ENGLISH)
	{
		Width = g_tPageModeCharPara.Width;
 		High  = g_tPageModeCharPara.Height;
		if (g_tPageModeCharPara.RotateType == 1)	//旋转90
		{
			CodeRollCtrl(Width, High, pSrc);
			Temp 	= Width;
			Width	= High;
			High 	= Temp;
		}
		else if (g_tPageModeCharPara.RotateType == 2)	//旋转180
		{
			//未完待处理
		}
		
	}
	else if (CodeType == LANGUAGE_GB18030)
	{
		Width = g_tPageModeHZPara.Width;
 		High  = g_tPageModeHZPara.Height;
	}
	else
	{
		Width = g_tPageModeHZPara.Width;
 		High  = g_tPageModeHZPara.Height;
	}
	
	
	
	
	
	
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 对读回的字符点阵进行做旋转，加粗，加重，反显，纵向横向放大，
	及下划线的处理。
	各个特效处理次序：
	1.加粗加重,void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
	2.反显,void ReverseDisplayProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
	3.旋转
	4.纵向放大
	5.横向放大
	6.下划线
	字符点阵大小8*16,9*17,12*24,16*16,24*24
	长24宽24的字符旋转处理后依然为24*24点阵
	8*16,16*16按照最底列为0填充，
	
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/


/*******************************************************************************
* Function Name  : 函数名
* Description    : 反显处理，将指定最大24*24点阵空间的数值按照指定长度和宽度反显
* Input          : 输入点阵按照竖置竖排，从左到右的顺序排放
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void ReverseDisplayProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
{
	uint32_t	i;
	
	
}

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/

#endif	/* #ifdef	CPCL_CMD_ENABLE 结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
