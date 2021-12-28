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
#include	"rs.h"
#include	"cominbuf.h"
#include	"extgvar.h"					//全局外部变量声明
#include	"charmaskbuf.h"
#include	"spiflash.h"
#include	"timconfig.h"
#include	"extgvar.h"
#include	"debug.h"

#ifdef	DBG_SELF_DEF_CMD_ENABLE

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
* Function Name  : Command_1E01
* Description    : 测试SPI字库,写入
* 1E 01 xL xH d1 ... dk
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E01(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	uint32_t	iCount;
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	if (iLen)
	{
#if	0
		iAddress = BIT_BMP_PARA_BASE_ADDR;	//擦除控制参数存储区域,以扇区为单位擦除,每扇区4K
		sFLASH_EraseSector(iAddress);				//删除存储区
		iCount= iLen;
		while (iCount--)
		{
			GetChar = ReadInBuffer();					//读一个字节
			sFLASH_WriteBuffer(&GetChar, iAddress, 1);	//存1个字节
			sFLASH_ReadBuffer(&bBuf, iAddress, 1);			//读回该字节
			UpLoadData(&bBuf, 1);												//回传
			iAddress++;
		}
#else
		iAddress = BIT_BMP_PARA_BASE_ADDR;	//擦除控制参数存储区域,以扇区为单位擦除,每扇区4K
		sFLASH_EraseSector(iAddress);				//删除存储区
		iCount= iLen;
		while (iCount--)
		{
			GetChar = ReadInBuffer();					//读一个字节
			sFLASH_WriteBuffer(&GetChar, iAddress++, 1);	//存1个字节
		}
	#if	0
		DelayMs(1);
		iAddress = BIT_BMP_PARA_BASE_ADDR;	//擦除控制参数存储区域,以扇区为单位擦除,每扇区4K
		iCount= iLen;
		while (iCount--)
		{
			sFLASH_ReadBuffer(&bBuf, iAddress++, 1);			//读回该字节
			UpLoadData(&bBuf, 1);													//回传
		}
	#endif
#endif
	}

}

/*******************************************************************************
* Function Name  : Command_1E02
* Description    : 测试SPI字库,只读
* 1E 02 xL xH
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E02(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	iAddress = BIT_BMP_PARA_BASE_ADDR;	//擦除控制参数存储区域,以扇区为单位擦除,每扇区4K
	while (iLen--)
	{
		sFLASH_ReadBuffer(&GetChar, iAddress++, 1);	//读回该字节
		UpLoadData(&GetChar, 1);										//回传
	}
}

/*******************************************************************************
* Function Name  : Command_1E03
* Description    : 测试SPI字库,根据指定存储区开始结束的地方写入,无数据不写入
* 1E 03 A0 A1 A2 A3 B0 B1 B2 B3 B4 xL xH d1 ... dk
* 写起始地址:A3A2A1A0
* 写结束地址:B3B2B1B0
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E03(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	uint32_t	iStartAddress;
	uint32_t	iStopAddress;
	uint32_t	iCount;
	
	GetChar = ReadInBuffer();								//A0
	iStartAddress = GetChar;
	GetChar = ReadInBuffer();								//A1
	iStartAddress += (((uint32_t)GetChar) << 8);
	GetChar = ReadInBuffer();								//A2
	iStartAddress += (((uint32_t)GetChar) << 16);
	GetChar = ReadInBuffer();								//A3
	iStartAddress += (((uint32_t)GetChar) << 24);
	
	GetChar = ReadInBuffer();								//B0
	iStopAddress = GetChar;
	GetChar = ReadInBuffer();								//B1
	iStopAddress += (((uint32_t)GetChar) << 8);
	GetChar = ReadInBuffer();								//B2
	iStopAddress += (((uint32_t)GetChar) << 16);
	GetChar = ReadInBuffer();								//B3
	iStopAddress += (((uint32_t)GetChar) << 24);
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	if (iStopAddress > iStartAddress)
	{
		iAddress = iStartAddress;
		for (	iCount = 0; \
					iCount < ((iStopAddress - iStartAddress) / sFLASH_SPI_SECTORSIZE); \
					iCount++)
		{
			sFLASH_EraseSector(iAddress);					//删除存储区
			iAddress += sFLASH_SPI_SECTORSIZE;
		}
	}
	
	while (iLen--)
	{
		GetChar = ReadInBuffer();					//读一个字节
		sFLASH_WriteBuffer(&GetChar, iStartAddress++, 1);	//存1个字节
	}
}

/*******************************************************************************
* Function Name  : Command_1E04
* Description    : 测试SPI字库,只读,根据指定地址读回指定长度的内容回传
* 1E 04 A0 A1 A2 A3 xL xH
* 读起始地址:A3A2A1A0
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E04(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	
	GetChar = ReadInBuffer();								//A0
	iAddress = GetChar;
	GetChar = ReadInBuffer();								//A1
	iAddress += (((uint32_t)GetChar) << 8);
	GetChar = ReadInBuffer();								//A2
	iAddress += (((uint32_t)GetChar) << 16);
	GetChar = ReadInBuffer();								//A3
	iAddress += (((uint32_t)GetChar) << 24);
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	while (iLen--)
	{
		sFLASH_ReadBuffer(&GetChar, iAddress++, 1);	//读回该字节
		UpLoadData(&GetChar, 1);										//回传
	}
}

#ifdef	DBG_025
void	Command_1E10(void)	//测试字库
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x01;
	g_bDbg025EnalbeFlag = GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x01;
	g_bDbg025FontFlag = GetChar;
}
#endif

#ifdef	DBG_GB18030_ASCII

#include	"charmaskbuf.h"
#include	"print.h"

/*外部变量声明*/
extern	TypeDef_StructCharMaskReadBuf		g_tCharMaskReadBuf;

/*******************************************************************************
* Function Name  : 
* Description    : 测试GB18030字库,从指定地址开始读指定长度的字符数,并打印出来,
	一个字符占用72字节空间.
*	命令格式:
1E 20 A0 A1 A2 A3 xL xH
* 读起始地址:A3A2A1A0
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E20(void)
{
	uint8_t	GetChar;
	uint8_t	CodeType;
	uint8_t	Result;
	uint32_t	iLen;
	uint32_t	iAddress;
	
	//1.获得起始地址
	iAddress = 0;
	for (iLen = 0; iLen < 4; iLen++)
	{
		GetChar = ReadInBuffer();
		iAddress |= (((uint32_t)GetChar) << (iLen << 3));
	}
	
	//2.获得读取字符个数
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	
	//3.循环读取每个字符
	while (iLen--)
	{
		//3.1	读取字模
		g_tCharMaskReadBuf.LanguageType = LANGUAGE_CHINESE;
		g_tCharMaskReadBuf.FontType = HZ_FONT_A;
		sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, iAddress, BYTES_OF_HZ_FONT_A);
		iAddress += BYTES_OF_HZ_FONT_A;			//准备下次读取的地址
		//3.2	填充打印缓冲区
		CodeType = g_tCharMaskReadBuf.LanguageType;	//字符还是汉字
		Result = JudgeLineBufFull(CodeType);	//3.2.1	判断是否已经满了
		if (Result ==	LINE_BUF_NOT_FULL)	//该行不满可以放得下当前字符
		{																	//3.2.1.1将该字符放入当前编辑行缓冲区
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);
			InitCharMaskBufParameter();			//清空字模读取缓冲区,字符缓冲区标志改为ILDE
		}
		else if (Result == LINE_BUF_FULL)	//当前行缓冲区已满无法放下当前字符
		{
// 		PrintOneLine();									//3.2.1.2如果已满则打印本行
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh); 
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);	//3.2.1再填充
			InitCharMaskBufParameter();			//清空字模读取缓冲区,字符缓冲区标志改为ILDE
		}
	}
	
	//4.循环读取结束,判断编辑缓冲区是否有待打印内容,如果有,打印出来
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
// 		PrintOneLine();										//打印本行
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
}
#endif

/*******************************************************************************
* Function Name  : RsCommand
* Description    : RS命令集的处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RS_Command(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	switch (GetChar)
	{
		case 0x01:
		{
			Command_1E01();		//测试SPI字库
			break;
		}
		case 0x02:
		{
			Command_1E02();		//测试SPI字库
			break;
		}
		case 0x03:
		{
			Command_1E03();		//测试SPI字库
			break;
		}
		case 0x04:
		{
			Command_1E04();		//测试SPI字库
			break;
		}
#ifdef	DBG_025
		case 0x10:
		{
			Command_1E10();		//测试字库
			break;
		}
#endif
#ifdef	DBG_GB18030_ASCII
	case 0x20:
		{
			Command_1E20();		//测试GB18030字库
			break;
		}
#endif

		default:
			break;
	}
}

#endif	/*扩展自定义测试命令结束*/

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
