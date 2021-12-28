/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-9
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
#include	<string.h>
#include	<math.h>
#include	"cpcl.h"
#include	"ctext.h"
#include	"cominbuf.h"
#include	"debug.h"
#include	"defconstant.h"
#include	"extgvar.h"

#ifdef	CPCL_CMD_ENABLE

#ifdef	DBG_RX_TOTAL_THEN_PRT
	extern volatile uint8_t	g_bRxFinishFlag;	//标示串口通信中数据是否已经接收完毕
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* CPCL指令均为ASCII码 */
/* CPCL每条行指令的最大长度,用于判断指令是哪一条,从每条指令开始读到第一个空格为结束 */
#define		CPCL_CMD_LINE_LEN_MAX			(32)

/* 一行指令中每个部分的分隔符 */
#define		CPCL_CMD_SPACE_CHAR				(uint8_t)(' ')

/* 命令列表的数目 */
#define		CPCL_CMD_LIST_MUMBER			(uint8_t)(100)

/* Private variables ---------------------------------------------------------*/

/* 读取命令字符串,最后添加字符串结束符 */
uint8_t		g_tCmdBuf[CPCL_CMD_LINE_LEN_MAX + 1];

/* CPCL指令的字符串列表 */
const	uint8_t		m_tcCPCLCmdList[CPCL_CMD_LIST_MUMBER][CPCL_CMD_LINE_LEN_MAX + 1] = 
{
	"!",				//0
	"TEXT",			//1
	"T",				//2
	"VTEXT",		//3
	"VT",				//4
	"TEXT90",		//5
	"T90",			//6
	"TEXT180",	//7
	"T180",			//8
	"TEXT270",	//9
	"T270",			//10
	"PAGE-WIDTH",
	"PW",           	//12
	"LINE",
	"L",              //12~13
	"INVERSE-LINE",
	"IL",             //14~15
	"BOX",            //16
	"SETBOLD",        //17 
	"BARCODE",
	"B",
	"VBARCODE",
	"VB",							//18~21
	"BARCODE-TEXT",
	"BT",             //22~23
	"EXPANDED-GRAPHICS",
	"EG",
	"EXPANDED-GRAPHICS",
	"VEG",
	"COMPRESSED-GRAPHICS",
	"CG",
	"VCOMPRESSED-GRAPHICS",
	"VCG",						//24~31
	"LEFT",						//32    
	"CENTER",					//33
	"RIGHT",					//34
	"FORM",						//35
	"PRINT",					//36
	";",							//37
	"GAP-SENSE",			//38
	"SETSP",					//39
	"INVERSE-TEXT",
	"IT",							//40~41
	"UNDERLINE-TEXT",
	"UT",							//42~43
	"PAGE-ROTATE",
	"PR",							//44~45
	"USER-DEFINE-HZ",
	"UDH",						//46~47
	"BA",							//48
	"CUT-PAPER",
	"CP",							//49~50 
};

/* Private function prototypes -----------------------------------------------*/
static void	CPCLCmdProcess(void);
static uint8_t * GetCPCLStringUntilSpace(uint8_t * pBuf, const uint32_t	maxLen);
static uint8_t * GetCPCLStringUntilChangeLine(uint8_t * pBuf, const uint32_t	maxLen);
static uint8_t * GetCPCLString(uint8_t * pBuf, const uint32_t	maxLen);
static uint8_t GetCPCLCmdNumber(const uint8_t * Str);
static void	GetCPCLChangeLine(void);

static void CPCLCmdPageStart(void);

static void SetTextFont(void);
static void SetTextSize(void);
static void SetTextStartPosition(void);
static void CPCLCmdText(uint8_t Type);

/* Public functions ----------------------------------------------------------*/
void	InitPageEditPrtBuf(void)
{
	#ifdef	EXT_SRAM_ENABLE
		g_tPageEditPrtBuf.PageBuf = (uint8_t *)(EXT_RAM_PAGE_BUF_BASE_ADDR);
	#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : CPCL指令集的处理，用于调试时一次性接收数据方便。
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	CPCL_Command(void)
{
	//如果接收缓冲区有内容,而且有可以进行编辑的行打印缓冲区
	if (GetInBufBytesNumber() > 0)
	{
		#ifdef	DBG_RX_TOTAL_THEN_PRT				//收完再打
			if (g_bRxFinishFlag != RX_FINISHED)
			{
				return;		//死等一次通信接收完成
			}
			else	//通信过程已经完成,开始处理用户命令或者待打印内容			
			{
				CPCLCmdProcess();
			}
		#else	//正常边收边打的程序处理,不检查通信是否完成
			CPCLCmdProcess();
		#endif
	}	//通信接收缓冲区非空处理结束
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : CPCL指令集的处理，分析接收指令，依次执行各个指令.流程如下：
	1. 从接收缓冲区读入命令字符，变成字符串。读到遇到的第一个空格为止。
		 如果读入的为错误值，则作为空字符串。
	2. 检查命令字符串在列表中的编号
	3. 根据编号进入对应的指令，继续处理本命令行剩余的内容直到本命令行处理完毕。
	4. 执行该指令后退出本函数
	5. 在主程序中再次进入本函数。
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
static void	CPCLCmdProcess(void)
{
	uint8_t	CmdNumber;
	
	GetCPCLString(g_tCmdBuf, CPCL_CMD_LINE_LEN_MAX);	//读取命令字符串
	CmdNumber = GetCPCLCmdNumber(g_tCmdBuf);	//判断字符串对应的命令编号
	switch	(CmdNumber)
	{
		case	0:
			CPCLCmdPageStart();
			break;
		case	1:
		case	2:
			CPCLCmdText(CmdNumber);
			break;
		default:
			break;
	}
}

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 将指令读入缓冲区，从接收缓冲区开始读起，遇到空格后继续检查后面
	的内容，如果也是空格继续读，直到不是空格为止，防止遇到多个空格的情况。
	到最大长度之前没有遇到第一个空格，也退出
* Input          : 待读入缓冲区的首地址
* Output         : 读入缓冲区的首地址
*******************************************************************************/
static uint8_t * GetCPCLStringUntilSpace(uint8_t * pBuf, const uint32_t	maxLen)
{
	uint32_t	i;
	uint8_t		GetChar;
	uint8_t	* pTemp;
	
	assert_param(maxLen <= CPCL_CMD_LINE_LEN_MAX);
	
	pTemp = pBuf;
	for (i = 0; i < maxLen; i++)
	{
		GetChar = ReadInBuffer();							//读入接收内容
		if (GetChar == CPCL_CMD_SPACE_CHAR)		//遇到空格分隔符就结束
		{
			while (CPCL_CMD_SPACE_CHAR == ReadInBuffer());	//此处继续读删除多余空格
			DecInBufferGetPoint();							//放回读的不是空格的字符
			break;
		}
		else		//如果不是空格就存储
		{
			*pBuf = GetChar;
			pBuf++;
		}
	}
	*pBuf = '\0';	//不管有没有遇到空格，都添加字符串结束符号
	
	return	(pTemp);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将指令读入缓冲区，从接收缓冲区开始读起，遇到0D或者0A后为止
* Input          : 待读入缓冲区的首地址
* Output         : 读入缓冲区的首地址
*******************************************************************************/
static uint8_t * GetCPCLStringUntilChangeLine(uint8_t * pBuf, const uint32_t	maxLen)
{
	uint32_t	i;
	uint8_t		GetChar;
	uint8_t	* pTemp;
	
	assert_param(maxLen <= CPCL_CMD_LINE_LEN_MAX);
	
	pTemp = pBuf;
	for (i = 0; i < maxLen; i++)
	{
		GetChar = ReadInBuffer();							//读入接收内容
		if ((GetChar == 0x0D) || (GetChar == 0x0A))	//遇到换行符就结束
		{
			DecInBufferGetPoint();							//放回读的不是空格的字符
			break;
		}
		else		//如果不是空格就存储
		{
			*pBuf = GetChar;
			pBuf++;
		}
	}
	*pBuf = '\0';	//添加字符串结束符号
	
	return	(pTemp);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将字符读入缓冲区，从接收缓冲区开始读起，到最大长度之前的直到
	遇到第一个空格之前的内容
* Input          : 待读入缓冲区的首地址
* Output         : 读入缓冲区的首地址
*******************************************************************************/
static uint8_t * GetCPCLString(uint8_t * pBuf, const uint32_t	maxLen)
{
	uint32_t	i;
	uint8_t		GetChar;
	uint8_t		SpaceFlag;
	uint8_t	* pTemp;
	
	assert_param(maxLen <= CPCL_CMD_LINE_LEN_MAX);
	
	pTemp = pBuf;
	SpaceFlag = 0;
	for (i = 0; i < maxLen; i++)
	{
		GetChar = ReadInBuffer();							//读入接收内容
		if (GetChar == CPCL_CMD_SPACE_CHAR)		//遇到空格分隔符就结束
		{
			SpaceFlag = 1;
			break;
		}
		else		//如果不是空格就存储
		{
			*pBuf = GetChar;
			pBuf++;
		}
	}
	
	if (0 == SpaceFlag)		//没有遇到过空格
	{
		pBuf = pTemp;
		for (i = 0; i < maxLen; i++)
		{
			*pBuf = 0x00;		//清空接收
		}
	}
	else		//有遇到空格，添加字符串结束符号
	{
		*pBuf = '\0';
	}
	
	return	(pTemp);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 与命令列表中的值进行比较，得到命令的编码号
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
static uint8_t GetCPCLCmdNumber(const uint8_t * Str)
{
	uint8_t	i;
	
	for (i = 0; i < CPCL_CMD_LIST_MUMBER; i++)
	{
		if (strcmp(Str, m_tcCPCLCmdList[i]) == 0)
		{
			break;
		}
	}
	
	return	(i);
}


/*******************************************************************************
* Function Name  : 该程序有问题，'0'-'0'就变成 NUL了
* Description    : 将ASCII码值'0'到'9','a'到'f','A'到'F'组成的字符串转换为16进制0~F的值
* Input          : 
* Output         : 无
* Return         : 无
*******************************************************************************/
void	StrAsciiToStrHex(uint8_t Str[])
{
	uint8_t	i;
	uint8_t	Len;
	uint8_t	buf[255];
	uint8_t	* pTemp;
	
	pTemp = Str;
	Len = 0;
	while (*Str)
	{
		if ((*Str >= '0') && (*Str <= '9'))
		{
			*Str -= '0';
		}
		else if ((*Str >= 'A') && (*Str <= 'F'))	//'A'ASCII码值为十进制65
		{
			*Str -= 55;
		}
		else if ((*Str >= 'a') && (*Str <= 'f'))	//'a'ASCII码值为十进制97
		{
			*Str -= 87;
		}
		else
		{
		}
		
		buf[Len] = *Str;
		Str++;
		Len++;
		if (Len == 254)		//防止下标越界
		{
			break;
		}
	}
	
	Str = pTemp;
	for (i = 0; i < Len; i++)
	{
		*Str = buf[i];
		Str++;
	}
	*Str = '\0';		//添加字符串结尾符号
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将ASCII码值组成的字符串转换为整型值,前面的为高位，最低的为个位。
	码值均为‘0’~‘9’之间。实际按照最高5位发送
	将字符串按照从高到低的顺序逐个挑出，换算成HEX，并保存到缓冲区，记下位数。
	再按照位数算出整数值
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
uint32_t	StrAsciiToInt(uint8_t Str[])
{
	uint8_t	i;
	uint8_t	Len;
	uint8_t	buf[8];
	uint32_t	result;
	uint32_t	ratio;
	uint8_t	* pTemp;
	
	memset(buf, 0x00, 8);
	Len = 0;
	while (*Str)
	{
		if ((*Str >= '0') && (*Str <= '9'))
		{
			*Str -= '0';
		}
		else		//遇到无效字符结束
		{
			break;
		}
		
		buf[Len] = *Str;
		Str++;
		Len++;
		if (Len == 8)		//防止下标越界
		{
			break;
		}
	}
	
	pTemp = buf;
	pTemp += (Len - 1);		//到个位数上开始，递减
	result = 0;
	ratio = 1;
	for (i = 0; i < Len; i++)
	{
		result += (*pTemp * ratio);
		ratio *= 10;
		pTemp--;
	}
	
	return	(result);
}

typedef	struct CPCLCtrl
{
	uint16_t	HoriOffSet;						//整个标签的水平偏移量,UNIT决定的单位
	uint16_t	HoriResolution;				//水平分辨率，203点/英寸（8点/mm）
	uint16_t	VerticalResolution;		//垂直分辨率，203点/英寸（8点/mm）
	uint16_t	MaxHeight;						//标签最大高度，点为单位
	uint16_t	Quantity;							//打印标签的数量，最大1024张
	uint16_t	HoriStart;						//内容编辑时横向起始位置，点为单位
	uint16_t	VerticalStart;				//内容编辑时纵向起始位置，点为单位
	
}TypeDef_StructCPCLCtrl;

TypeDef_StructCPCLCtrl	g_tCPCLCtrl;

/*******************************************************************************
* Function Name  : 函数名
* Description    : 读取换行，正常应该为0D + 0A，额外附加规则单独0D 或单独 0A或者
 0A + 0D均视为换行。
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
static void	GetCPCLChangeLine(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();		//读入接收内容
	if (GetChar == 0x0D)				//前面的为0D
	{
		GetChar = ReadInBuffer();	//读入接收内容
		if (GetChar == 0x0A)			
		{
			return;									//1.标准的预期0D+0A
		}
		else
		{
			DecInBufferGetPoint();	//2.只有0D的情况，回写缓冲区，只更改读数位置值
		}
	}
	else if (GetChar == 0x0A)		//前面的为0A
	{
		GetChar = ReadInBuffer();	//读入接收内容
		if (GetChar == 0x0D)			
		{
			return;									//3.其他预期0A+0D
		}
		else
		{
			DecInBufferGetPoint();	//4.只有0A的情况，回写缓冲区，只更改读数位置值
		}
	}
	else	//均不是
	{
		DecInBufferGetPoint();		//不是换行的位置
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 页起始指令，！ {offset}  200  200  {height}  {qty}
 正常情况下，!和空格已经在进入之前被读取了。依次读取其他值并设置控制全局变量。
 {offset}：整个标签的水平偏移量
	200：水平分辨率，203点/英寸（8点/mm）
	200：垂直分辨率，203点/英寸（8点/mm）
	{height}:标签最大高度，点为单位
	{qty}:打印标签的数量，最大1024张
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
static void CPCLCmdPageStart(void)
{
	uint8_t	Buf[8];
	
	GetCPCLString(Buf, 6);	//读入空格为止的最大65535的数值,整个标签的水平偏移量
	g_tCPCLCtrl.HoriOffSet = StrAsciiToInt(Buf);
	
	GetCPCLString(Buf, 6);	//读入空格为止的最大65535的数值, 水平分辨率
	g_tCPCLCtrl.HoriResolution = StrAsciiToInt(Buf);
	
	GetCPCLString(Buf, 6);	//读入空格为止的最大65535的数值, 垂直分辨率
	g_tCPCLCtrl.VerticalResolution = StrAsciiToInt(Buf);
	
	GetCPCLString(Buf, 6);	//读入空格为止的最大65535的数值, 标签最大高度，点为单位
	g_tCPCLCtrl.MaxHeight = StrAsciiToInt(Buf);
	
	GetCPCLStringUntilChangeLine(Buf, 6);	//读入换行为止的最大65535的数值, 打印标签的数量，最大1024张
	g_tCPCLCtrl.Quantity = StrAsciiToInt(Buf);
	
	GetCPCLChangeLine();		//读入换行符
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : CPCL文本命令，根据设定的参数，填充文本到页打印缓冲区
	{command} {font} {size} {x} {y} {data}
	[说明]       {command} :从下表中选择需要使用的指令
	指令	效果
	TEXT（or T）				打印横向文字
	VTEXT（or VT）			打印文字(纵向)，逆时针旋转90°
	TEXT90（or T90）		与VTEXT指令含义相同
	TEXT180（or T180）	打印文字(反向)，逆时针旋转180°
	TEXT270（or T270）	打印文字(纵向)，逆时针旋转270°

	{font}:选择字体号
	{size}:选择字体大小
	{x}：  水平打印起始位置
	{y}:   垂直打印起始位置
	{data}:打印的文本内容

	font	字体点阵
	24	字符(12*24)，汉字(24*24)
	55	字符(8*16)，汉字(16*16)

	字符高度选择	字符宽度选择
	size	纵向放大	size	横向放大
	0	1（正常）			0	1 （正常）
	1	2（2倍高）		10	2 （2倍宽）
	2	3							20	3
	3	4							30	4
	4	5							40	5
	5	6							50	6
	6	7							60	7
	7	8							70	8
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
/* 设置文本字体，字符大小参数 */
static void SetTextFont(void)
{
	uint8_t	Buf[8];
	uint8_t	GetChar;
	
	GetCPCLStringUntilSpace(Buf, 5);	//读入空格为止的最大65535的数值
	GetChar = StrAsciiToInt(Buf);			//得到font字体点阵
	if (GetChar == 24)			//24	字符(12*24)，汉字(24*24)
	{
		g_tPageModeCharPara.FontType		= ASCII_FONT_A;
		g_tPageModeCharPara.Width				= ASCII_FONT_A_WIDTH;
		g_tPageModeCharPara.Height			=	ASCII_FONT_A_HEIGHT;
		
		g_tPageModeHZPara.FontType			= HZ_FONT_A;
		g_tPageModeHZPara.Width					= HZ_FONT_A_WIDTH;
		g_tPageModeHZPara.Height				=	HZ_FONT_A_HEIGHT;
	}
	else if (GetChar == 55)	//55	字符(8*16)，汉字(16*16)
	{
		g_tPageModeCharPara.FontType		= ASCII_FONT_C;
		g_tPageModeCharPara.Width				= ASCII_FONT_C_WIDTH;
		g_tPageModeCharPara.Height			=	ASCII_FONT_C_HEIGHT;
		
		g_tPageModeHZPara.FontType			= HZ_FONT_B;
		g_tPageModeHZPara.Width					= HZ_FONT_B_WIDTH;
		g_tPageModeHZPara.Height				=	HZ_FONT_B_HEIGHT;
	}
	else	//设置为默认
	{
		g_tPageModeCharPara.FontType		= ASCII_FONT_DEFAULT;
		g_tPageModeCharPara.Width				= ASCII_FONT_WIDTH_DEFAULT;
		g_tPageModeCharPara.Height			=	ASCII_FONT_HEIGHT_DEFAULT;
		
		g_tPageModeHZPara.FontType			= HZ_FONT_DEFAULT;
		g_tPageModeHZPara.Width					= HZ_FONT_WIDTH_DEFAULT;
		g_tPageModeHZPara.Height				=	HZ_FONT_HEIGHT_DEFAULT;
	}
}

/* 设置文本放大倍数*/
static void SetTextSize(void)
{
	uint8_t	Buf[8];
	uint8_t	GetChar;
	uint8_t	temp;
	
	GetCPCLStringUntilSpace(Buf, 5);	//读入空格为止的最大65535的数值
	GetChar = StrAsciiToInt(Buf);			//得到字体大小
	temp = GetChar / 10;		//计算横向放大倍数,标准参数范围0~7
	if (temp < MAX_RATIO_X)
	{
		temp++;								//因为0代表放大倍数1
		g_tPageModeCharPara.WidthMagnification	= temp;
		g_tPageModeHZPara.WidthMagnification		= temp;
	}
	else	//超过最大则不处理
	{
	}
	temp = GetChar % 10;		//计算纵向放大倍数,标准参数范围0~7
	if (temp < MAX_RATIO_Y)
	{
		temp++;								//因为0代表放大倍数1
		g_tPageModeCharPara.HeightMagnification	= temp;
		g_tPageModeHZPara.HeightMagnification		= temp;
	}
	else	//超过最大则不处理
	{
	}
}

/* 设置文本打印起始位置 */
static void SetTextStartPosition(void)
{
	uint8_t	Buf[8];
	uint16_t	StartPos;
	
	GetCPCLStringUntilSpace(Buf, 6);	//读入空格为止的最大65535的数值
	StartPos = StrAsciiToInt(Buf);		//得到x
	g_tCPCLCtrl.HoriStart = StartPos;
	
	GetCPCLStringUntilSpace(Buf, 6);	//读入空格为止的最大65535的数值
	StartPos = StrAsciiToInt(Buf);		//得到y
	g_tCPCLCtrl.VerticalStart = StartPos;
}

/* GetPageCharDotData(,)读入字符串，解码，读取字符点阵，填充缓冲区，直到遇到换行符号 */
/* 对读回的字符点阵进行做旋转，加粗，加重，纵向横向放大，下划线的处理。*/
static void	ProcessTextData(void)
{
	uint8_t	GetChar;
	uint8_t	Temp;
	uint8_t	DotBuf[MAX_BYTES_OF_HZ];
	
	while (1)
	{
		GetChar = ReadInBuffer();
		if (GetChar >= PRINTABLE_START_CODE)
		{
			GetPageCharDotData(GetChar, DotBuf);	//可打印字符处理
			
		}
		else if ((GetChar != 0x0D) || (GetChar != 0x0A))	//到行末尾
		{
			DecInBufferGetPoint();
			break;
		}
	}
}

/* Text指令处理 */
static void CPCLCmdText(uint8_t	Type)
{
	uint8_t	Buf[8];
	uint8_t	GetChar;
	uint8_t	temp;
	uint16_t	StartPos;
	
	if ((Type == 1) || (Type == 2))		//旋转0度
	{
		g_tPageModeCharPara.RotateType	= 0;
		g_tPageModeHZPara.RotateType		= 0;
	}
	else if ((Type == 3) || (Type == 4) || (Type == 5) || (Type == 6))	//旋转90度
	{
		g_tPageModeCharPara.RotateType	= 1;
		g_tPageModeHZPara.RotateType		= 1;
	}
	else if ((Type == 7) || (Type == 8))		//旋转180度
	{
		g_tPageModeCharPara.RotateType	= 2;
		g_tPageModeHZPara.RotateType		= 2;
	}
	else if ((Type == 9) || (Type == 10))		//旋转270度
	{
		g_tPageModeCharPara.RotateType	= 3;
		g_tPageModeHZPara.RotateType		= 3;
	}
	else	//参数错误，不处理
	{
		return;
	}
	
	SetTextFont();		//先处理FONT字体	
	SetTextSize();		//再处理字体大小
	SetTextStartPosition();	//再处理打印起始位置
	ProcessTextData();	//读入实际打印字符并填充打印缓冲区,逐个读入字符直到遇到0D,0A，
											//如果字符为可打印字符则处理
	GetCPCLChangeLine();	//最后读入行结束符号
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/



#endif	/* #ifdef	CPCL_CMD_ENABLE 结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
