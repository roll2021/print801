/**
  ******************************************************************************
  * @file    x.c
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

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"charprocess.h"
#include	"extgvar.h"
//#include	"charmaskbuf.h"

extern TypeDef_StructInBuffer	gstructInBuf;
extern TypeDef_StructCharMaskReadBuf	gstructCharMaskReadBuf;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//可替换的国际字符表
uc8  CharacterSet[] = 
{
	0x23,0x24,0x40,0x5b,0x5c,0x5d,0x5e,0x60,0x7b,0x7c,0x7d,0x7e
};

#define	BYTES_OF_CHAR_SET_TABLE		(sizeof(CharacterSet) / sizeof(CharacterSet[0]))

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 数据处理分析,是打印字符处理打印,是命令处理命令
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	DataProcess(uint8_t GetChar)
{
	while (SystemCtrlStruct.PrtDevOe == 1) 	//打印机被禁止
	{
		Command_1B3D();						//禁止/允许打印机
	}
	
	if (SystemCtrlStruct.PrtModeFlag == 0)	//标准模式
	{
		if (GetChar >= 0x20)
		{
			PrintChar(GetChar);		   //可打印字符处理jsf待进一步修改该函数
		}
		else
		{
			if (GetChar == 0x1B)
			{
				ESC_Command();		  //ESC（1B）指令处理
			}
			else if (GetChar == 0x1C)
			{
				FS_Command();				//FS（1C）指令处理
			}
			else if (GetChar == 0x1D)
			{
				GS_Command();			//GS（1D）指令处理
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
	uint8_t	Ch1;
	uint8_t	*p;
	uint32_t	i;
	Ch1 = CharCode;
	
	if (SysConfigStruct.SYS_Language == LANGUAGE_ENGLISH)		//西文模式
  {
  	PrintAscII(Ch1);			
	}	//西文处理结束
  else										//汉字模式
	{
  	if (Ch1 == 0xFE)			//自定义汉字
		{
			JudgePrintSelfDefChinese(Ch1);	//处理自定义汉字打印
		}
		else if (SysConfigStruct.SYS_Language == LANGUAGE_BIG5)	//BIG5汉字方式
  	{
			JudgePrintBIG5(Ch1);			//处理BIG5汉字打印
		}
		else								//GB18030
		{
			JudgePrintGB18030(Ch1);		//处理GB18030打印  	
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
	uint8_t	*p;
	uint32_t	i;
	
	Ch2 = ReadInBuffer();
	if ((Ch2 > 0xA0) && (Ch2 < (0xA1 + USERHZSIZE)))
	{
		p = RamBuffer.UserCharStruct.UserCode_HZ;	//用户定义汉字低字节代码表
		for (i = 0; i < USERHZSIZE; i++, p++)
		{
			if (Ch2 == *p)
				break;
		}
		if (i < USERHZSIZE)
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
		if ((Ch2 < 0x40) || (Ch2 > 0x7E && Ch2 < 0xA1) || (Ch2 > 0xFE))	//第二字节为无效汉字代码
		{
			PrintAscII(Ch1);			//第一字节按西文字符打印
			DecInBufferGetPoint();		//放回最后读取的字符
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
	
	if ((Ch1 < 0x81) || (Ch1 > 0xFE))		//第一代码为无效汉字代码
	{
		PrintAscII(Ch1);				//第一字节为西文字符打印
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x30) || (Ch2 > 0x39 && Ch2 < 0x40) || (Ch2 == 0x7F) || (Ch2 > 0xFE))	//无效汉字代码
		{
			PrintAscII(Ch1);			//第一字节按西文字符打印
			DecInBufferGetPoint();		//放回最后读取的字符jsf待修改
		}
		else if ((Ch2 >= 0x40 && Ch2 <= 0x7E) || (Ch2 >= 0x80 && Ch2 <= 0xFE))
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
				DecInBufferGetPoint();			//放回最后读取的字符
			}
			else
			{
				Ch4 = ReadInBuffer();
				if ((Ch4 < 0x30) || (Ch4 > 0x39))	//第四字节代码无效
				{
					PrintAscII(Ch1);			//第一字节按西文字符打印
					PrintAscII(Ch2);			//第二字节按西文字符打印
					PrintAscII(Ch3);			//第三字节为有效打印字符
					DecInBufferGetPoint();		//放回最后读取的字符
				}
				else
				{
					PrintGB18030(Ch1, Ch2, Ch3, Ch4);		//四字节汉字
				}
			}
		}
	}
}

/*******************************************************************************
* Function Name  : PrintAscII
* Description    : 如果打印区能放下当前ASCII,则取ASCII字符字模到打印缓冲区,否则,
*	打印当前行.
* Input          : ASCII字符代码
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
#if	0
//原流程的处理方式
void	PrintAscII(uint8_t Code)
{
	uint8_t	TempBuff[MAX_BYTES_OF_CHAR];
	
	GetAscIIDotData(Code, TempBuff);
	
	FillPrint(TempBuff, LANGUAGE_ENGLISH);	
}
#endif
//高速POS流程的处理方式
//
//启动该字符的SPI_DMA读取
//将当前字符环境变量设置到字符读取缓冲区结构体中
void	PrintAscII(uint8_t Code)
{
	uint8_t	TempBuff[MAX_BYTES_OF_CHAR];
	
	GetAscIIDotData(Code, TempBuff);
	
	FillPrint(TempBuff, LANGUAGE_ENGLISH);	
}
/*******************************************************************************
* Function Name  : PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : 如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,否则打印当前行.
* Input          : CodeHigh=汉字代码高位,CodeLow=汉字代码低位
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	Offset, TempBuff[MAX_BYTES_OF_HZ];
	uint32_t	CodeAddr;
	
	if ((CodeLow >= 0x40) && (CodeLow <= 0x7E))
		Offset = CodeLow - 0x40;
	else
		Offset = CodeLow - 0xA1 + 0x3F;
	CodeAddr = BIG5_ADDR + ((CodeHigh - 0xA1) * 0x9D + Offset) * BYTES_OF_HZ_FONT_A;
	
	sFLASH_ReadBuffer(TempBuff, CodeAddr, BYTES_OF_HZ_FONT_A);
	
	FillPrint(TempBuff, LANGUAGE_CHINESE);
}

/*******************************************************************************
* Function Name  : void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
* Description    : 如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,否则,打印当前行.
*	
* Input          : Code1、Code2、Code3、Code4为汉字代码,当Code3=Code4=0时为双字节代码
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
{
	uint8_t	TempBuff[MAX_BYTES_OF_HZ];
	uint32_t	CodeAddr;
	
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
				Code2--;
			CodeAddr = (Code1 - 0xA8) * 96 + (Code2 - 0x40) + 846;
		}		
		else if ((Code1 >= 0xB0) && (Code1 <= 0xF7) && (Code2 >= 0xA1))	//2区汉字
		{
			CodeAddr = (Code1 - 0xB0) * 94 + (Code2 - 0xA1) + 1038;
		}
		else if ((Code1 < 0xA1) && (Code1 >= 0x81))					//3区汉字
		{
			if (Code2 > 0x7F)
				Code2--;
			CodeAddr = (Code1 - 0x81) * 190 + (Code2 - 0x40) + 1038 + 6768;
		}
		else if ((Code1 >= 0xAA) && (Code2 < 0xA1))					//4区汉字
		{
			if (Code2 > 0x7F)
				Code2--;
			CodeAddr = (Code1 - 0xAA) * 96 + (Code2 - 0x40) + 1038 + 12848;
		}
		else
			CodeAddr = 0;
	}
	else
	{
		CodeAddr =	(Code1 - 0x81) * 12600 + 
								(Code2 - 0x39) * 1260 + 
								(Code3 - 0xEE) * 10 + 
								(Code4 - 0x39) + 
								22046;
	}
	
	CodeAddr = GB18030_ADDR + CodeAddr * BYTES_OF_HZ_FONT_A;
	
	//读取字模
	sFLASH_ReadBuffer(TempBuff, CodeAddr, BYTES_OF_HZ_FONT_A);

	FillPrint(TempBuff, LANGUAGE_CHINESE);
}

/*******************************************************************************
* Function Name  : PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : 打印自定义汉字,如果打印区能放下当前汉字,则取汉字字模到打印缓冲区,
*				   否则,打印当前行.
* Input          : CodeHigh=汉字代码高位,CodeLow=汉字代码低位
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	i, TempBuff[MAX_BYTES_OF_HZ], *pZimo;
	
	//用户定义汉字字模区首地址
	pZimo = RamBuffer.UserCharStruct.Buffer_HZ + (CodeLow - 0xA1) * BYTES_OF_HZ_FONT_A;	
	for (i = 0; i < BYTES_OF_HZ_FONT_A; i++, pZimo++)
	{
		TempBuff[i] = *pZimo;
	}
	
	FillPrint(TempBuff, LANGUAGE_CHINESE);
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
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII字符点阵查表程序
{
	if (SysConfigStruct.SYS_FontSize == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		GetAscIIFontADotData(AscIICode, DotData);
	}
	else if (SysConfigStruct.SYS_FontSize == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		GetAscIIFontBDotData(AscIICode, DotData);
	}
}


void	GetAsciiUserDefDotData(uint8_t AscIICode)	//读取用户自定义字符的点阵数据
{
	uint32_t	i;
	uint32_t	len;
	uint32_t	StartAddress;	
	uint8_t * p;
	
	if (SysConfigStruct.SYS_FontSize == ASCII_FONT_A)				//FLASH中英文12x24点阵的ASCII字符
	{
		len = BYTES_OF_ASCII_FONT_A;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		p = RamBuffer.UserCharStruct.Buffer_12;
	}
	else if (SysConfigStruct.SYS_FontSize == ASCII_FONT_B)	//FLASH中英文9x17点阵的ASCII字符
	{
		len = BYTES_OF_ASCII_FONT_B;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
		p = RamBuffer.UserCharStruct.Buffer_09;
	}
	else
	{
		return;
	}
	
	gstructCharMaskReadBuf.Status = CHAR_MASK_READ_BUF_STATUS_WRITING;	//设为正在写入
	for (i = 0; i < len; i++)
	{
		gstructCharMaskReadBuf.Buf[i++] = *p++;
	}
	gstructCharMaskReadBuf.Status = CHAR_MASK_READ_BUF_STATUS_WAIT_TO_DEAL;	//设为等待处理
}

/*******************************************************************************
* Function Name  : void GetAscIIFontBDotData(uint8_t AscIICode,uint8_t DotData[])
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIFontBDotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII字符点阵查表程序
{
	uint8_t	i, CodePage;
	uint32_t	AsciiCodeAddr = 0;
	
	CodePage = SysConfigStruct.DefaultCodePage;		 //国家的代码页的选择
	
	if (AscIICode < 0x80)
	{
		if ((RamBuffer.UserCharStruct.UserdefineFlag == 1) && 
				(SystemCtrlStruct.UserSelectFlag) && 
				(RamBuffer.UserCharStruct.UserCode_09[AscIICode - 0x20]))	//定义了自定义的字符,且选择了
		{
			for (i = 0; i < BYTES_OF_ASCII_FONT_B; i++)
				DotData[i] = RamBuffer.UserCharStruct.Buffer_09[(AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B + i];
		}
		else	//不是自定义字符
		{
			if (SystemCtrlStruct.CharacterSet)	//如果设置了国际字符集指令 0x1b 52 n
			{
				for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//查是否为国际字符
				{
					if (AscIICode == CharacterSet[i])
						break;
				}
				if (i < BYTES_OF_CHAR_SET_TABLE)							//有要替换的字符存在
				{
					AsciiCodeAddr = INTERNATION_09_17_ADDR + 
						(SystemCtrlStruct.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_B;
				}
				else
				{
					AsciiCodeAddr = ASCII_09_17_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B);
				}
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_B);
			}		//End 国际字符
			else
			{
				AsciiCodeAddr = ASCII_09_17_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B);
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_B);
			}
		}		//End 自定义字符
	}		   //End <0x80
	else		//----FLASH中0X80-0xFF 的 9 x 17 点阵的ASCII字符----//
	{
		AsciiCodeAddr = ASCII_09_17_CODEPAGEADDR + 
										(CodePage * 128 * BYTES_OF_ASCII_FONT_B) + 
										((AscIICode - 0x80) * BYTES_OF_ASCII_FONT_B);
		sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_B);	
	}
}

/*******************************************************************************
* Function Name  : void GetAscIIFontADotData(uint8_t AscIICode,uint8_t DotData[])
* Description    : 取字符字模数据,查表获得各种ASCII字符的点阵地址
*	AscIICode = ASCII字符值
*	DotData[] = 存储ASCII字符的字模数据
*	当前程序处理字体12*24
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIFontADotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII字符点阵查表程序
{
	uint8_t	i, CodePage;
	uint32_t	AsciiCodeAddr = 0;

	CodePage = SysConfigStruct.DefaultCodePage;		 //国家的代码页的选择
	
	if (AscIICode < 0x80)
	{
		if ((RamBuffer.UserCharStruct.UserdefineFlag == 1) && 
				(SystemCtrlStruct.UserSelectFlag) && 
				(RamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20]))	  //定义了自定义的字符,且选择了
		{
			for (i = 0; i < BYTES_OF_ASCII_FONT_A; i++)
			{
				DotData[i] = RamBuffer.UserCharStruct.Buffer_12[(AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A + i];
			}
		}
		else	//不是自定义字符
		{
			if (SystemCtrlStruct.CharacterSet)		//如果设置了国际字符集指令 0x1b 52 n
			{
				for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//查是否为国际字符
				{
					if (AscIICode == CharacterSet[i])
						break;
				}
				if (i < BYTES_OF_CHAR_SET_TABLE)							//有要替换的字符存在
				{
					AsciiCodeAddr = INTERNATION_12_24_ADDR + 
						(SystemCtrlStruct.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
				}
				else
				{
					AsciiCodeAddr = ASCII_12_24_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A);
				}
				
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_A);
			}	//End 国际字符
			else	//ascii字符
			{
				AsciiCodeAddr = ASCII_12_24_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A);
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_A);
			}	//end ascii字符
		}		//End 自定义字符
	}		  //End <0x80
	else
	{	//----FLASH中0X80-0xFF 的 12 x 24 点阵的ASCII字符----//
		AsciiCodeAddr = ASCII_12_24_CODEPAGEADDR + 
										(CodePage * 128 * BYTES_OF_ASCII_FONT_A) + 
										((AscIICode - 0x80) * BYTES_OF_ASCII_FONT_A);
		sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_A);
	}
}

/*******************************************************************************
* Function Name  : void FillPrint(uint8_t *Source, uint8_t CodeType)
* Description    : 如果打印区能放下当前字符,则取汉字字模到打印缓冲区,
*				   否则,打印当前行.
* Input          : Source：字模地址,CodeType：代码类型,0＝西文
* Output         : 修改打印缓冲区当前指针
* Return         : None
*******************************************************************************/
void	FillPrint(uint8_t *Source, uint8_t CodeType)
{
	uint8_t	Width, High, RatioX, Temp;
	
	RatioX = JudgeLineFull(CodeType);
	
	if (CodeType)
	{
		Width = PrtModeHZ.CharWidth;
 		High  = PrtModeHZ.CharWidth;
	}
	else
	{
	 	Width = PrtCtrlData.CharWidth;
		High  = PrtCtrlData.CharHigh;
	}
	
	if (PrtCtrlData.CharCycle90 == 1)	 				//旋转90？
	{
		CodeRollCtrl(Width, High, Source);
		Temp 	= Width;
		Width	= High;
		High 	= Temp;
	}
	if (PrtModeChar.EmphaSizeMode == 1)
	{
		EmphaSizeProcess(Width, 3, Source);
	}
	
	FillPrintBuff(Source, Width, High, RatioX, CodeType);
}

/*******************************************************************************
* Function Name  : JudgeLineFull
* Description    : 判断是否能放下该字符,否则,打印当前行.
*				   
* Input          : CodeType: =0,西文,=1汉字
* Output         : 
* Return         : 返回横向放大倍数
*******************************************************************************/
uint8_t	JudgeLineFull(uint8_t CodeType)
{
	uint8_t	RatioX;
	uint16_t	Width;
	
	if (CodeType)	//汉字方式,取横向放大倍数
	{
		if ((PrtModeHZ.WidthMagnification == 1) && (PrtCtrlData.OneLineWidthFlag == 1))	//行内倍宽
		{
			RatioX = 2;
		}
		else
		{
			RatioX = PrtModeHZ.WidthMagnification;
		}
		Width = RatioX * (PrtModeHZ.CharWidth + PrtModeHZ.LeftSpace);
	}
	else					//字符方式
	{
		if ((PrtModeChar.WidthMagnification == 1) && (PrtCtrlData.OneLineWidthFlag == 1))	//行内倍宽
		{
			RatioX = 2;
		}
		else
		{
			RatioX = PrtModeChar.WidthMagnification;
		}
		Width = RatioX * PrtModeChar.CharWidth;
	}
	
	//如果是行首,保证设置的左边距至少能打印一字符
	if ((PrtCtrlData.PrtDataDotIndex == PrtCtrlData.PrtLeftLimit) && 
			((PrtCtrlData.PrtDataDotIndex + Width) > PrtCtrlData.PrtLength))
		PrtCtrlData.PrtDataDotIndex = PrtCtrlData.PrtLength - Width;
	
	if ((PrtCtrlData.PrtDataDotIndex + Width) > PrtCtrlData.PrtLength)
	{
		MoveLine(PrtCtrlData.CodeLineHigh);
		//如果是行首,保证设置的左边距至少能打印一字符
		if ((PrtCtrlData.PrtDataDotIndex + Width) > PrtCtrlData.PrtLength)
			PrtCtrlData.PrtDataDotIndex = PrtCtrlData.PrtLength - Width;
	}
	
	return RatioX;
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
	uint8_t	*p;

	for (i = 0; i < Line; i++)
	{
		p = PrtDataBuf + i * PRTDOTNUMBER + PrtCtrlData.PrtDataDotIndex;
		for (j = 0; j < Width; j++)
		{
			*p = Data;
			p++;
		}
	}
}

/*******************************************************************************
* Function Name  : FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t RatioX ,uint8_t CodeType)
* Description    : 指定地址数据存入打印缓冲区.
* Input          : *Source：输入数据地址, CharWidth：字符宽度, RatioX:横向倍数, 
*										CodeType: 字符类型 =1汉字,=0字符
* Output         : 改变存数指针
* Return         : 
*******************************************************************************/
void	FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t RatioX, uint8_t CodeType)
{
	uint8_t	Line;
	uint8_t	Width, RatioY, Ratio, RightSpace, LeftSpace;
	uint8_t	*p, i, Temp;
	uint8_t	TempBuff[8];
	
	//字符左、右间距,纵向放大倍数等
	if (SystemCtrlStruct.PrtModeFlag == 0)			//行模式
	{
		if (CodeType == LANGUAGE_ENGLISH) 		//字符
		{
			LeftSpace		= RatioX * PrtModeChar.LeftSpace;
			RightSpace	= RatioX *PrtModeChar.RightSpace;
			RatioY			= PrtModeChar.HeightMagnification;
		}
		else		//汉字
		{	LeftSpace		= RatioX * PrtModeHZ.LeftSpace;
			RightSpace	= RatioX * PrtModeHZ.RightSpace;
			RatioY			= PrtModeHZ.HeightMagnification;
		}
	}
	else			//页模式
	{
		if (CodeType == 0) 		//字符
		{
			LeftSpace	 = 0;
			RightSpace = RatioX * PageModeStruct.AscRightSpace;
			RatioY		 = PrtModeChar.HeightMagnification;
		}
		else		//汉字
		{
			LeftSpace	 = RatioX * PageModeStruct.HZLeftSpace;
			RightSpace = RatioX * PageModeStruct.HZRightSpace;
			RatioY		 = PrtModeHZ.HeightMagnification;
		}
	}
	//计算字符高度
	if (PrtCtrlData.LineHigh < CharHigh * RatioY)
		PrtCtrlData.LineHigh = CharHigh * RatioY;
	
	//修正右间距宽度,保证能放下该字符,字符宽＋右间距 < 可打印区
	if ((PrtCtrlData.PrtDataDotIndex + (RatioX * CharWidth) + LeftSpace + RightSpace) > PrtCtrlData.PrtLength)
		RightSpace = PrtCtrlData.PrtLength - (PrtCtrlData.PrtDataDotIndex + (RatioX * CharWidth) + LeftSpace);

	//处理左间距
	
	if (PrtCtrlData.CharNotFlag == 1)		//反白打印（反显）
	{
		FillDataPrintBuff(0xff, LeftSpace, 3 * RatioY);
	}
	
	PrtCtrlData.PrtDataDotIndex	+= LeftSpace;
	
	for (Line = 0; Line < 3; Line++)
	{
		p = PrtDataBuf + ((RatioY * Line * PRTDOTNUMBER) + PrtCtrlData.PrtDataDotIndex);
		
		for (Width = 0; Width < CharWidth; Width++)		//字符宽
		{
			Temp = Source[3 * Width + 2 - Line];
			ByteZoomCtrl(RatioY, Temp, TempBuff);				//纵向放大处理
			
			for (Ratio = 0; Ratio < RatioX; Ratio++)
			{
				for (i = 0; i < RatioY; i++)
				{
					if (PrtCtrlData.CharNotFlag == 0)
						*(p + PRTDOTNUMBER * i) = TempBuff[RatioY - i - 1];
					else
						*(p + PRTDOTNUMBER * i) = ~TempBuff[RatioY - i - 1];	//反白打印
				}		//End 纵向放大倍数
				p++;
			}			//End 横向放大倍数
		}			 	//Een 字符宽
	}				 	//End 字符高
	
	//下划线填充
	if (PrtCtrlData.CharNotFlag == 0 && PrtCtrlData.CharCycle90 == 0)	//反显、旋转90打印时不打印下划线
	{
		if (((CodeType == 0) && ((PrtModeChar.Underline & 0x80) != 0)) || 
				((CodeType != 0) && ((PrtModeHZ.Underline & 0x80) != 0)))		//填充下划线
		{
			p = UnderlineBuf + PrtCtrlData.PrtDataDotIndex - LeftSpace;
			if (CodeType == 0)	  //字符
			{
				Temp = PrtModeChar.Underline & 0x03;
			}
			else				  //汉字
			{
				Temp = PrtModeHZ.Underline & 0x03;
			}
			
			PrtCtrlData.DoUnderlineFlag	= 1;
			if (PrtCtrlData.UnderlineMaxPrtHigh < Temp)
				PrtCtrlData.UnderlineMaxPrtHigh = Temp;
			
			for (Width = 0; Width < (RatioX * CharWidth + LeftSpace + RightSpace); Width++)
			{
				*p = Temp;
				p++;
			}
		}
	}
	
	PrtCtrlData.PrtDataDotIndex += (RatioX * CharWidth);
	
	//处理右间距
	if (PrtCtrlData.CharNotFlag == 1)		//处理反白打印（反显）
	{
		FillDataPrintBuff(0xff, RightSpace, 3 * RatioY);
	}
	PrtCtrlData.PrtDataDotIndex += RightSpace;
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
		uint8_t	Buff[sizeof(unsigned long long)];
	}Temp_Union;
	
	Temp_Union	TempUnion;
	
	if (ZoomRate > MAX_RATIO_Y)	//判断放大倍数是否超过最大定义值
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
		Temp1 = 0xff >> (8 - ZoomRate);
		TempUnion.Temp = 0;
		for (i = 0; i < 8; i++)
		{
			if (Temp0 & 0x01)
				TempUnion.Temp |= Temp1 ;
			Temp1 <<= ZoomRate;
			Temp0 >>= 1;
		}
		
		for (i = 0; i < ZoomRate; i++)
			DscPrt[i] = TempUnion.Buff[ZoomRate - 1 - i];
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
			DscPrt[i] = (uint8_t)(temp >> (8*(ZoomRate - i - 1)));
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
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t *SrcPrt)
{
	uint8_t	ArrayBuf,DataBuf;
	uint8_t	i,j;
	
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
* Description    : 将制定字符的字模数据旋转顺时针90°的程序
* Input          : CodeWidth：字符横向尺寸, 使用点为单位
* Input          : CodeHigh：字符纵向尺寸,使用点为单位
*				   SrcPrt[]：输入数据指针
* Output         : SrcPrt[]：旋转处理后的数据
* Return         : 旋转处理后的数据数量 
* 特别提示：	 ：输入数据按标准排列
				   使用 9*17 或 12*24 点阵字符且旋转方向为90°时.输出数据字节会大于输入字节,在调用时,注意保证SrcPrt[]的大小
				   按底部对齐方式排列输出数据
*	有待分析
*******************************************************************************/
void	CodeRollCtrl(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[])
{
	uint32_t	ChangedByte;
	uint8_t 	i, j;
	uint8_t	*ptr;
	uint8_t	Buff[MAX_BYTES_OF_HZ], Buff1[MAX_BYTES_OF_HZ];
	uint8_t	HighByte;
	
  HighByte = CodeHigh / 8;
	if (CodeHigh % 8)
		HighByte++;
	
	ptr = Buff1;
  MemSet(Buff1, 0x00, MAX_BYTES_OF_HZ);
  MemSet(Buff, 0x00, MAX_BYTES_OF_HZ);
	
	for (i=0; i < CodeWidth * HighByte; i++)
		Buff[i] = SrcPrt[i];
	
	HighByte--;
	//顺时针90°
 	for (i=0; i < CodeHigh; i++)
	{
		ChangedByte = 0;
		for (j = 0; j < CodeWidth; j++)
		{
			ChangedByte <<= 1;
			//先把横排中所有字节的低位取出来放在转换后的低位上
			if (Buff[HighByte - i / 8 + j * 3] & (0x01 << (i % 8)))
				ChangedByte |= 0x01;
		}
		
		*ptr++ = (ChangedByte >> 16);
		*ptr++ = (ChangedByte >> 8);
		*ptr++ = ChangedByte;
	}
	memcpy(SrcPrt, Buff1, MAX_BYTES_OF_HZ);
}

/******************* (C) COPYRIGHT 2012 WH *****END OF FILE****/
