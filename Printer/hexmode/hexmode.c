/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-30
  * @brief   16进制相关的程序.
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
#include	"hexmode.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"feedpaper.h"
#include	"platen.h"
#include	"button.h"
#include	"timconfig.h"
#include	"charmaskbuf.h"

extern TypeDef_StructInBuffer volatile g_tInBuf;

extern	void	PrintString(const uint8_t * Str);
extern uint8_t Eth_ClearBusy;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void	HexModePrintStartHint(void);
static void	HexModePrintStopHint(void);
static void	FillHexModePrintOneLineData(uint8_t * pBuf, uint32_t bLen);

/* Private functions ---------------------------------------------------------*/

//分解接收到字符的高4位,并以ASCII值返回
uint8_t	ChangeHighHalfOfByteToASCII(uint8_t	GetChar)
{
	uint8_t	Ch;
	
	Ch = GetChar >> 4;	//将该字符的高4位取出,结果范围0~F
	if (Ch > 9)		//范围A~F,十进制为10~15,代表ASCII码的A~F,十进制为65~70,差值为十进制55
	{
		Ch += 0x37;	//变换成ASCII码值
	}
	else					//范围0~9,十进制为0~9,代表ASCII码的0~9,十进制为48~57,差值为十进制48
	{
		Ch += 0x30;	//变换成ASCII码值
	}
	
	return	(Ch);
}

//分解接收到字符的低4位,并以ASCII值返回
uint8_t	ChangeLowHalfOfByteToASCII(uint8_t	GetChar)
{
	uint8_t	Ch;

	Ch = GetChar & 0x0F;		//分解低4位
	if (Ch > 9)
	{
		Ch += 0x37;
	}
	else
	{
		Ch += 0x30;
	}
	
	return	(Ch);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 十六进制打印模式,进入16进制后打印出提示,然后等待处理输入字符.
*	一行处理8个数据,剩余数据不满一行的暂时不处理.每次按下上纸键走纸1行,最后一次按下
*	上纸键后打印出最后所有剩余的数据,并打印退出提示符,退出16进制模式.
*	目前程序按照58mm纸宽打印字符宽度12点设计.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
/*01 02 03 04 05 06 07 08????????共384点*/
//一行处理打印的16进制数据量
#define	HEX_DEAL_CHARS_ONE_LINE	(8)

//一行可打印字符总数,数字拆成2个字符+1个空格+1个可打印字符=4个字符位置
#define	HEX_PRT_CHAR_LEN				(HEX_DEAL_CHARS_ONE_LINE * 4)

//一行需要的总存储空间,末尾添加NUL和换行字符
#define	HEX_BUF_LEN							(HEX_PRT_CHAR_LEN + 2)

//一行可打印字符解码字符起始位置,紧挨着十六进制数打印,凑成384点满行
#define	HEX_PRT_CHAR_VISIBLE_START_POSITION		(HEX_DEAL_CHARS_ONE_LINE * 3)

void	HexMode(void)
{
	uint8_t	PrtDataStr[ HEX_BUF_LEN ];
	uint32_t	LeftCharLen;
	
	SetComInBufBusy();					//打印提示信息前先置忙关闭接收主机信息
	HexModePrintStartHint();		//打印提示信息
	g_tSystemCtrl.HexMode = 1;	//进入16进制模式
	ClearComInBufBusy();				//清除忙信号,打印完毕提示信息后再开始接收
	EnableReceiveInterrupt();		//确保允许接收
	Eth_ClearBusy = 0;
	while (g_tSystemCtrl.HexMode)
	{
		if (g_tFeedButton.KeyStatus)	//检查按键状态
		{
			PrintString("\n");					//打印1行
			g_tFeedButton.KeyStatus = 0;
		}
		//检查上纸键按下次数,<3次时处理数据,>=3次时打印最后内容并退出16进制模式
		if (g_tFeedButton.KeyCount < 3)
		{
			if (GetInBufBytesNumber() >= HEX_DEAL_CHARS_ONE_LINE)
			{
				//g_tInBuf.BytesNumber 的值会在读数时自动减少
				FillHexModePrintOneLineData(PrtDataStr, HEX_DEAL_CHARS_ONE_LINE);
				PrintString(PrtDataStr);					//打印该行
			}	//至少8字节剩余数据量处理完毕
			else
			{
				if(g_tInterface.ETH_Type) 
				{	
					GetETH_ConnectState();
				}	
			}	
		}	//按键次数<3次处理完毕
		else	//按下3次FEED键后打印剩余内容并退出16进制模式
		{
			SetComInBufBusy();		//先置忙关闭接收主机信息
			//打印出接收缓冲区剩余数据
			while (GetInBufBytesNumber() > HEX_DEAL_CHARS_ONE_LINE)
			{
				//g_tInBuf.BytesNumber 的值会在读数时自动减少
				FillHexModePrintOneLineData(PrtDataStr, HEX_DEAL_CHARS_ONE_LINE);
				PrintString(PrtDataStr);					//打印该行
			}
			//打印出剩余不足1行的内容
			if (GetInBufBytesNumber())
			{
				LeftCharLen = GetInBufBytesNumber();
				FillHexModePrintOneLineData(PrtDataStr, LeftCharLen);
				PrintString(PrtDataStr);				//打印该行
			}	//剩余数据量处理完毕
			else	//打印出缓冲区可能的不足1行的内容
			{
				PrintString("\n");					//打印1行
			}
			
			InitInBuf();									//如果还有剩余未打印的内容,不打印,全删除
			g_tFeedButton.KeyStatus = 0;
			g_tFeedButton.KeyCount = 0;
			g_tSystemCtrl.HexMode = 0;		//退出16进制模式
			ClearComInBufBusy();		//清除忙信号,打印完毕提示信息后再开始接收
		}	//按键次数超过3次处理完毕
	}	//16进制模式处理完毕
	
	HexModePrintStopHint();
	
	g_tFeedButton.KeyStatus = 0;	//清除可能的多按下的按键
	g_tFeedButton.KeyCount = 0;
}

#if	0
#define	HEX_DEAL_CHARS_ONE_LINE	(8)		//一行处理打印的16进制数据量
#define	HEX_PRT_CHAR_LEN				(HEX_DEAL_CHARS_ONE_LINE * 4 + 1)	//一行可打印字符总数
#define	HEX_BUF_LEN							(HEX_PRT_CHAR_LEN + 2)						//一行需要的总存储空间
#define	HEX_PRT_CHAR_VISIBLE_START_POSITION	(HEX_DEAL_CHARS_ONE_LINE * 3 + 1)	//一行可打印字符解码字符起始位置

void	HexMode(void)
{
	uint8_t	GetChar, Ch;
	uint8_t	PrtDataStr[ HEX_BUF_LEN ];
	uint32_t	i;
	uint32_t	LeftCharLen;
	uint32_t  Temp1;
	
	SetComInBufBusy();		//打印提示信息前先置忙关闭接收主机信息
	while (PLATEN_STATUS_OPEN == ReadPlatenStatus());	//等待关闭纸舱盖再开始打印
	DelayUs(1000 * 1000);												//等待500ms
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
	PrintString("Hexadecimal Dump \n");
	PrintString("To terminate hexadecimal dump,\n");
	PrintString("press FEED button three times.\n");
	PrintString("\n");
	g_tSystemCtrl.HexMode = 1;
	
	ClearComInBufBusy();		//清除忙信号,打印完毕提示信息后再开始接收
	//ReceiveInterrupt(ENABLE);		//开接收中断
	EnableReceiveInterrupt();
	while (g_tSystemCtrl.HexMode)
	{
		if (g_tFeedButton.KeyStatus)	//检查按键状态
		{
			PrintString("\n");					//打印1行
			g_tFeedButton.KeyStatus = 0;
		}
		//检查上纸键按下次数,<3次时处理数据,>=3次时打印最后内容并退出16进制模式
		if (g_tFeedButton.KeyCount < 3)
		{
			//2016.07.07  计算缓冲区内容字节数
			Temp1 = g_tInBuf.PutPosition;	//中断中可能改变其值
			if(Temp1 >= g_tInBuf.GetPosition)
				Temp1 = Temp1 - g_tInBuf.GetPosition;
			else
				Temp1 =(g_tInBuf.BytesSize - g_tInBuf.GetPosition) + Temp1;
			g_tInBuf.BytesNumber = Temp1;
			
			if (g_tInBuf.BytesNumber >= HEX_DEAL_CHARS_ONE_LINE)
			{
				//先清空处理缓冲区,末尾的换行和NUL字符后面修改
				memset(PrtDataStr, 0x20, HEX_BUF_LEN);		//固定格式数据,全部设置为空格
				//memset(PrtDataStr, 0x20, sizeof(PrtDataStr) / sizeof(PrtDataStr[0]));	//固定格式数据,全部设置为空格
				for (i = 0; i < HEX_DEAL_CHARS_ONE_LINE; i++)
				{
					GetChar = ReadInBuffer();
					//接收到的字符分解为两个16进制字符,高4位在前,低4位在后,再加入1个空格
					Ch = ChangeHighHalfOfByteToASCII(GetChar);	//分解接收到字符的高4位
					PrtDataStr[3 * i] = Ch;	//分解后的高4位
					Ch = ChangeLowHalfOfByteToASCII(GetChar);	//分解接收到字符的低4位		
					PrtDataStr[3 * i + 1] = Ch;		//分解后的低4位, 1个空格无需手动加入
					
					if (GetChar < 0x20)						//此处错误,应该是不可打印字符换成'.'
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = '.';		//当前程序为每8个字符一排,不是10个
					else
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = GetChar;	//作为可打印字符打印在右边
				}	//处理8个字节完毕
				PrtDataStr[ HEX_PRT_CHAR_LEN ] = 0x0A;			//添加末尾的换行和NUL字符
				PrtDataStr[ HEX_PRT_CHAR_LEN + 1] = 0x00;		//末尾一个字符用来结束
				PrintString(PrtDataStr);					//打印该行
			}	//至少8字节剩余数据量处理完毕
		}	//按键次数<3次处理完毕
		else	//按下3次FEED键后打印剩余内容并退出16进制模式
		{
			if(g_tInBuf.PutPosition != g_tInBuf.GetPosition) //2016.07.07(g_tInBuf.BytesNumber)
			{
				//先清空处理缓冲区,末尾的换行和NUL字符后面修改
				memset(PrtDataStr, 0x20, HEX_BUF_LEN);		//固定格式数据,全部设置为空格
				//memset(PrtDataStr, 0x20, sizeof(PrtDataStr) / sizeof(PrtDataStr[0]));	//固定格式数据,全部设置为空格
				//2016.07.07 计算g_tInBuf.BytesNumber的值
				Temp1 = g_tInBuf.PutPosition;	//中断中可能改变其值
				if(Temp1 >= g_tInBuf.GetPosition)
					Temp1 = Temp1 - g_tInBuf.GetPosition;
				else
					Temp1 =(g_tInBuf.BytesSize - g_tInBuf.GetPosition) + Temp1;
				g_tInBuf.BytesNumber = Temp1;
				LeftCharLen = g_tInBuf.BytesNumber;
				for (i = 0; i < LeftCharLen; i++)	//当前程序为每8个字符一排
				{
					GetChar = ReadInBuffer();
					//接收到的字符分解为两个16进制字符,高4位在前,低4位在后,再加入1个空格
					Ch = ChangeHighHalfOfByteToASCII(GetChar);	//分解接收到字符的高4位
					PrtDataStr[3 * i] = Ch;	//分解后的高4位
					Ch = ChangeLowHalfOfByteToASCII(GetChar);	//分解接收到字符的低4位
					PrtDataStr[3 * i + 1] = Ch;		//分解后的低4位, 1个空格无需手动加入
					
					if (GetChar < 0x20)						//此处错误,应该是不可打印字符换成'.'
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = '.';		//当前程序为每8个字符一排,不是10个
					else
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = GetChar;	//作为可打印字符打印在右边
				}	//处理剩余字节完毕
				PrtDataStr[ HEX_PRT_CHAR_LEN ] = 0x0A;			//添加末尾的换行和NUL字符
				PrtDataStr[ HEX_PRT_CHAR_LEN + 1] = 0x00;		//末尾一个字符用来结束
				PrintString(PrtDataStr);				//打印该行
				InitInBuf();										//如果还有剩余未打印的内容,不打印,全删除
			}	//剩余数据量处理完毕
			
			g_tFeedButton.KeyStatus = 0;
			g_tFeedButton.KeyCount = 0;
			g_tSystemCtrl.HexMode = 0;		//退出16进制模式
			break;
		}	//按键次数超过3次处理完毕
	}	//16进制模式处理完毕
	
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_MIDDLE;
	PrintString("*** Hexadecimal Completed! *** \n\n");	//从16进制模式中退出来后打印提示
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
	g_tFeedButton.KeyStatus = 0;
	g_tFeedButton.KeyCount = 0;
}
#endif

static void	HexModePrintStartHint(void)
{
	uint32_t i;
	
	//如果是手动方式进入则加等待延时
	#ifdef	PLATEN_CHECK_ENABLE
	if (PLATEN_STATUS_OPEN == ReadPlatenStatus())
	{	
		//等待关闭纸舱盖再开始打印
		while (PLATEN_STATUS_OPEN == ReadPlatenStatus());
		DelayUs(1000 * 1000);
	}
	#endif
	
	//如果是指令方式进入则直接开始打印
    GoDotLine(20);		//
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
	PrintString("Hexadecimal Dump \n");
	PrintString("To terminate hexadecimal dump,\n");
	PrintString("press FEED button three times.\n");
	
	if ((MODAL_NAME_STRING == "C04") || \
			(MODAL_NAME_STRING == "C05") || \
			(MODAL_NAME_STRING == "C04C05"))
	{
		for (i = 0; i < 10; i++)
		{
			PrintString("\n");
		}
	}
	else
	{
		PrintString("\n");
	}
}

static void	HexModePrintStopHint(void)
{
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_MIDDLE;
	PrintString("*** Hexadecimal Completed! *** \n\n");	//从16进制模式中退出来后打印提示
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 填充十六进制打印模式的数据
*	
* Input          : 待填充的缓冲区首地址, 待处理的字符的个数
* Output         : 
* Return         : 
*******************************************************************************/
static void	FillHexModePrintOneLineData(uint8_t * pBuf, uint32_t bLen)
{
	uint8_t	GetChar;
	uint8_t	Ch;
	uint32_t	i;
	uint8_t * pData;
	
	//先清空处理缓冲区, 固定格式数据,全部设置为空格, 末尾的换行和NUL字符后面修改
	pData = pBuf;
	for (i = 0; i < HEX_BUF_LEN; i++)
	{
		*pData++ = 0x20;
	}
	
	if (bLen > HEX_DEAL_CHARS_ONE_LINE)		//防止下标越界
	{
		bLen = HEX_DEAL_CHARS_ONE_LINE;
	}
	
	pData = pBuf;
	for (i = 0; i < bLen; i++)
	{
		GetChar = ReadInBuffer();
		//接收到的字符分解为两个16进制字符,高4位在前,低4位在后,再加入1个空格
		Ch = ChangeHighHalfOfByteToASCII(GetChar);	//分解接收到字符的高4位
		*(pData + 3 * i) = Ch;			//分解后的高4位
		Ch = ChangeLowHalfOfByteToASCII(GetChar);	//分解接收到字符的低4位		
		*(pData + 3 * i + 1) = Ch;	//分解后的低4位, 1个空格无需手动加入
		
		//作为可打印ASCII字符打印在右边, 不可打印字符换成'.', 当前程序为每8个字符一排
		if ((GetChar < PRINTABLE_START_CODE) || (GetChar >= CODEPAGE_START_CODE))
		{
			*(pData + HEX_PRT_CHAR_VISIBLE_START_POSITION + i) = '.';
		}
		else
		{
			*(pData + HEX_PRT_CHAR_VISIBLE_START_POSITION + i) = GetChar;
		}
	}	//处理最多8个字节完毕
	*(pData + HEX_PRT_CHAR_LEN) = 0x0A;			//添加末尾的换行和NUL字符
	*(pData + HEX_PRT_CHAR_LEN + 1) = 0x00;		//末尾一个字符用来结束
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
