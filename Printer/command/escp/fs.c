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
#include	"fs.h"
#include	"cominbuf.h"
#include	"extgvar.h"					//全局外部变量声明
#include	"charmaskbuf.h"
#include	"spiflash.h"

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
* Function Name  : Command_1C21
* Description    : 设置汉字打印模式，包括倍宽,倍高,划线等
位  	0/1  Hex  Decimal 		功能 
0, 1        							未定义 
2			0  		00  		0  		取消倍宽
2			1  		04  		4  		选择倍宽
3			0  		00  		0  		取消倍高
3			1  		08  		8  		选择倍高
4-6        								未定义 
7			0  		00  		0  		取消下划线
7			1  		80  		128		选择下划线
·  当倍宽与倍高度模式同时被设定时，字符横向和纵向同时被放大两倍(包括左右间距)。 
·  打印机能对所有字符加下划线，包括左右间距。但不能对由于HT指令(横向跳格)引起
的空格进行加划线，也不对顺时针旋转90度的字符加下划线。 
·  下划线线宽由FS - 设定，与字符大小无关。 
·  当一行中字符高度不同时，该行中的所有字符以底线对齐。 
·  可以用FS W  或者  GS ! 对字符加粗，最后一条指令有效。 
·  也可以用FS – 选择或取消下划线模式，最后一条指令有效。
[ 默认值]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C21(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	//----选择字符倍宽模式(标准模式和页模式同时有效)----
	if (GetChar & 0x04)
	{
		g_tPrtModeHZ.WidthMagnification = 2;
	}
	else
	{
		g_tPrtModeHZ.WidthMagnification = 1;
	}
	
	//----选择字符倍高模式(标准模式和页模式同时有效)----
	if (GetChar & 0x08)
	{
		g_tPrtModeHZ.HeightMagnification = 2;
	}
	else
	{
		g_tPrtModeHZ.HeightMagnification = 1;
	}
	
	//----选择字符下划线模式(标准模式和页模式同时有效)----
	if (GetChar & 0x80)
	{
		g_tPrtModeHZ.Underline |= UNDERLINE_ENABLE_MASK;
	}
	else
	{
		g_tPrtModeHZ.Underline &= UNDERLINE_DISABLE_MASK;
	}
}

/*******************************************************************************
* Function Name  : Command_1C26
* Description    : 进入汉字方式，//如果默认为英文模式，则进入简体中文，否则进入默认的中文模式
·  当选中汉字模式时，打印机判断字符是否为汉字内码，如是汉字内码，先处理第一字节，
然后判断第二字节是否为汉字内码。 
·  打印机上电后自动选择汉字模式；
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C26(void)
{
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//系统参数
	uint16_t Len;
	
	Len = sizeof(g_tSysConfig);
	sFLASH_ReadBuffer(SysConfigUnionData.bSysParaBuf, SYS_PARA_BASE_ADDR, Len);
	
	//如果默认为英文模式，则进入简体中文，否则进入默认的中文模式
	if (SysConfigUnionData.SysConfigStruct.SysLanguage == LANGUAGE_ENGLISH)
		g_tSysConfig.SysLanguage = LANGUAGE_CHINESE;
	else
		g_tSysConfig.SysLanguage = SysConfigUnionData.SysConfigStruct.SysLanguage;
}

/*******************************************************************************
* Function Name  : Command_1C2D
* Description    : 设置/取消汉字下划线
1C   2D   n
0  ≤   n  ≤   2, 48 ≤   n  ≤   50
根据 n  的值，选择或取消汉字的下划线： 
	n  					功能 
0, 48  取消汉字下划线 
1, 49  选择汉字下划线(1点宽) 
2, 50  选择汉字下划线(2点宽)
·  打印机能对所有字符加下划线，包括左右间距。但不能对由HT指令(横向跳格)引起的
空格加下划线，也不对顺时针旋转90度的字符加下划线。 
·  消下划线模式后，不再执行下划线打印，但原先设置的下划线线宽不会改变。默认下划线
线宽为1点。 
·  即使改变字符大小，设定的下划线线宽也不会改变。 
·  用FS  !也可选择或取消下划线模式，最后一条指令有效。 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C2D(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	if (GetChar == 0 || GetChar == 48)	   	//取消下划线
		g_tPrtModeHZ.Underline &= UNDERLINE_DISABLE_MASK;
	else if (GetChar == 1 || GetChar == 49)	//设置1点模式
		g_tPrtModeHZ.Underline |= UNDERLINE_ENABLE_ONE_DOT_MASK;
	else if (GetChar == 2 || GetChar == 50)	//设置2点模式
		g_tPrtModeHZ.Underline |= UNDERLINE_ENABLE_TWO_DOT_MASK;
}

/*******************************************************************************
* Function Name  : Command_1C2E
* Description    : 退出汉字方式,取消汉字模式
·  当汉字模式被取消时，所有字符都当作作ASCII字符处理，每次只处理一个字节。  
·  上电自动选择汉字模式。 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C2E(void)
{
	g_tSysConfig.SysLanguage = LANGUAGE_ENGLISH;
}

/*******************************************************************************
* Function Name  : Command_1C32
* Description    : 自定义汉字,最多定义10个汉字，代码高字节=0xfe,低字节=0xa1~0xaa
1C   32   c1  c2  d1...dk
定义由c1，c2指定的汉字
c1 ,c2 代表定义字符的字符编码
c1 = FEH   
A1H ≤    c2 ≤   FEH 
0  ≤   d  ≤   255 
k = 72
·  c1  ，c2代表用户自定义汉字的编码，c1指定第一个字节，c2指定第二个字节。 
·  d代表数据。每个字节的相应位为1表示打印该点，为0不打印该点。
[ 默认值]   没有自定义汉字
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C32(void)
{
	uint8_t	CodeStart;
	uint8_t	CodeStop;
	uint32_t	i;
	uint8_t	* pZimo = NULL;
		
	CodeStart = ReadInBuffer();
	CodeStop = ReadInBuffer();
	//检查参数范围
	//code1 = 0xFE, 0xA1<= code2 <= 0xFE, 不过内存有限只能定义有限的汉字
	if ((CodeStart == 0xFE) && \
			(CodeStop > 0xA0) && \
			(CodeStop < (0xA1 + USER_HZ_SIZE)))
	{
		g_tRamBuffer.UserCharStruct.UserCode_HZ[CodeStop - 0xA1] = CodeStop;	//用户定义汉字低字节代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_HZ;												//用户定义汉字字模区首地址
		pZimo += (MAX_BYTES_OF_HZ * (CodeStop - 0xA1));
		for (i = 0; i < MAX_BYTES_OF_HZ; i++)
		{
			*pZimo++ = ReadInBuffer();
		}
		g_tRamBuffer.UserCharStruct.UserdefineFlag |= 0x02;	//定义了自定义汉字
		
		pZimo = NULL;
	}
}

/*******************************************************************************
* Function Name  : Command_1C3F
* Description    : 取消用户自定义汉字
*	1C   3F   c1	c2
*	取消由c1，c2指定的用户自定义汉字
c1 ,c2 代表定义字符的字符编码
c1 = FEH   
A1H ≤    c2 ≤   FEH 
*	取消后,此字符为空白
*	该指令删除该自定义汉字的字模
*	如果自定义字符中没有该字符,该指令被忽略
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C3F(void)
{
	uint8_t	CodeStart;
	uint8_t	CodeStop;
	uint32_t	i;
	uint8_t	* pCode = NULL;
	uint8_t	* pZimo = NULL;
	
	CodeStart = ReadInBuffer();
	CodeStop = ReadInBuffer();
	if (g_tSysConfig.SysLanguage != LANGUAGE_ENGLISH)		//非英文
	{
		//检查参数范围
		//code1 = 0xFE, 0xA1<= code2 <= 0xFE, 不过内存有限只能定义有限的汉字
		if ((CodeStart == 0xFE) && \
				(CodeStop > 0xA0) && \
				(CodeStop < (0xA1 + USER_HZ_SIZE)))
		{
			pCode = g_tRamBuffer.UserCharStruct.UserCode_HZ;	//检查该字是否已经被定义了
			pCode += (CodeStop - 0xA1);												//找到该汉字应该存储的位置
			if (*pCode == CodeStop)			//如果该汉字已经被定义则清除
			{
				*pCode = 0;								//清除代码
				pCode = NULL;
				pZimo = g_tRamBuffer.UserCharStruct.Buffer_HZ;	//用户定义汉字字模区首地址
				pZimo += (MAX_BYTES_OF_HZ * (CodeStop - 0xA1));
				
				for (i = 0; i < MAX_BYTES_OF_HZ; i++)		//清除字模数据
					*pZimo++ = 0;
				pZimo = NULL;
			}
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1C43
* Description    : 选择汉字代码集，n＝0或48 简体，n1或49 繁体 
1C   43   n1  n2 
n=0 ，1，48，49
选择汉字代码系统 
	n			选择汉字代码系统 
0, 48		简体中文(GB2312或GB18030) 
1, 49		繁体中文(BIG5) 
·  该指令不改变Flash中的参数设置。 
·  在执行ESC @指令,关闭电源或打印机复位后，恢复为默认值 
[ 默认值]		n = 0	简体中文型号 
						n = 1	繁体中文型号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C43(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar == 0) || (GetChar == 48))
	{	
		g_tSysConfig.SysLanguage = LANGUAGE_CHINESE;
	}
	else if ((GetChar == 1) || (GetChar == 49))
	{
		g_tSysConfig.SysLanguage = LANGUAGE_BIG5;
	}
}

/*******************************************************************************
* Function Name  : Command_1C50
* Description    : 打印预下载位图
[名称]  打印预存储位图 
[格式] ASCII 码   FS P   n 
十六进制码 1C 50  n 
十进制码  28 80  n 
[范围] 0  ≤  n  ≤  7 
[描述]  本命令打印预先存储在打印机非易失存储器中的2值位图。打印机非易失存储器中的
位图可通过PC机上的专用工具软件生成并写入，位图宽度最大为576，最大高度为64。
图形最大为64K.
n 为指定的位图编号。
[注意]   
	指定编号的位图还未定义时，该命令无效。
  位图必须是2值位图。 
  该命令不受打印模式(粗体,重叠,下划线,字符大小,或反白打印)影响。   
  如果要打印的位图宽度超过一行，则超出的部分不打印。 
  需用专用的工具上传打印位图，请参见【三,JPM112 打印机工具软件】。通过这种方式
	上传的位图不会丢失，除非重新上传其他位图将其覆盖。
	位图存储时按照横向取模,从左往右,从上往下,逐排取模方式.不足一个字节的补足一个字节.
	宽度方向上按照字节计数, 高度方向上按照点计数.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C50(void)
{
	uint8_t	FlashBitBmpNum;
	uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT];
	uint16_t High;
	uint16_t Width;
	uint16_t RealWidth;
	uint32_t SectorAddress;
	uint32_t i;
	
	FlashBitBmpNum = ReadInBuffer();	//图片的序列号
	if (FlashBitBmpNum < MAX_DOWNLOAD_NVFLASH_BITMAP_NUM)	//检查参数合法范围, 否则不处理  7副图
	{
		memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
		//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
		SectorAddress = BIT_BMP_DATA_BASE_ADDR + 0x10000 * FlashBitBmpNum;	//起始存储地址
		sFLASH_ReadBuffer(DataBuf, SectorAddress, 5);
		if (DataBuf[0] == FlashBitBmpNum)	//检查图片是否已经下载了
		{
			//Width = DataBuf[1] + 256 * DataBuf[2];
			//High = DataBuf[3] + 256 * DataBuf[4];
			Width	= (uint16_t)DataBuf[1] + (uint16_t)(((uint16_t)DataBuf[2]) << 8);	//宽度方向上字节数
			High	= (uint16_t)DataBuf[3] + (uint16_t)(((uint16_t)DataBuf[4]) << 8);	//高度方向上点数
			
			if (Width > (g_tSystemCtrl.LineWidth >> 3))
				RealWidth = (g_tSystemCtrl.LineWidth >> 3);		//实际打印的宽度字节数
			else
				RealWidth = Width;
			
			//while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus());	//等待之前打印结束
			
			SectorAddress += 5;
			for (i = 0; i < High; i++)				//打印一点行
			{
				memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
				//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
				sFLASH_ReadBuffer(DataBuf, SectorAddress, RealWidth);
				SectorAddress += Width;
				DotLinePutDrvBuf( DataBuf );				//2016.06.16 送一行数据到驱动打印缓冲区
			}
			g_tTab.TabIndex = 0;		//2016.09.06
		}	//待打印图片已经下载了处理结束
	}	//参数范围合法处理
}

/*******************************************************************************
* Function Name  : Command_1C53
* Description    : 设置汉字左右间距	，
1C   53   n1  n2
分别将汉字的左间距和右间距设置为 n1  和 n2 。 
·  当打印机支持GS P指令时，左间距是 [n1 × 横向或者纵向移动单位] 英寸，右间距是
	[n2 ×  横向或者纵向移动单位] 英寸。 
·  设置倍宽模式后，左右间距也加倍。 
·  移动单位是由GS P 指令设置的。  即使横向和纵向移动单位被GS P改变，原先设定的
	字符间距也不改变。 
·  在标准模式下，用横向移动单位。  
·  在页模式下，根据打印区域起始位置来选择是用横向移动单位还是纵向移动单位：  
	① 当起始位置在打印区域的左上角或右下角，用横向移动单位。 
	② 当起始位置在打印区域的右上角或左下角，用横向移动单位。 
	③ 汉字的最大右间距约为36mm ，超过此值取最大值。
[ 默认值]    n1 = 0, n2 = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C53(void)
{
	uint8_t	GetChar;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		GetChar = ReadInBuffer();
		g_tPrtModeHZ.LeftSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
		GetChar = ReadInBuffer();
		g_tPrtModeHZ.RightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
	}
	else																	 //	页模式
	{
		GetChar = ReadInBuffer();
		g_tPageMode.HZLeftSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
		GetChar = ReadInBuffer();
		g_tPageMode.HZRightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
	}
}

/*******************************************************************************
* Function Name  : Command_1C55
* Description    : 设置汉字横向放大倍数,
*	1C 55 n	微打中作为设置汉字横向放大n倍,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C55(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.WidthMagnification	= GetChar;	//汉字横向倍数
	}
}

/*******************************************************************************
* Function Name  : Command_1C56
* Description    : 设置汉字纵向放大倍数,
*	1C 56 n	微打中作为设置汉字纵向放大n倍,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C56(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.HeightMagnification = GetChar;	//汉字纵向倍数
	}
}

/*******************************************************************************
* Function Name  : Command_1C57
* Description    : 选择/取消汉字倍宽倍高
1C   57   n
选择或者取消汉字倍高倍宽模式。 
·  当n的最低位为0，取消汉字倍高倍宽模式。 
·  当n的最低位为1，选择汉字倍高倍宽模式。 
·  只有n的最低位有效              
·  在汉字倍高倍宽模式模式下，打印汉字的大小，与同时选择倍宽和倍高模式时相同。  
·  取消汉字倍高倍宽模式后，以后打印出的汉字为正常大小。 
·  当一行字中字符高度不同时，该行中的所有字符都按底线对齐。 
·  也可以通过FS ! 或者  GS !指令(选择倍高和倍宽模式)来选择或取消汉字倍高倍宽
	模式，最后接收到的指令有效。 
[ 默认值]    n = 0 
微打中作为设置汉字横向纵向放大n倍.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C57(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON命令集,页模式下设置打印区域
	{
		GetChar &= 0x01;		//最低位有效
		GetChar++;					//为0时放大倍数为1
		
		g_tPrtModeHZ.WidthMagnification = GetChar;		//汉字宽
		g_tPrtModeHZ.HeightMagnification = GetChar;		//汉字高
	}
	else  //微打命令集,纵向横向放大
	{
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X) || (GetChar > MAX_RATIO_Y))
		{
			return;
		}
		else
		{
			g_tPrtModeHZ.WidthMagnification = GetChar;		//汉字宽
			g_tPrtModeHZ.HeightMagnification = GetChar;		//汉字高
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1C58
* Description    : 设置汉字纵向横向放大倍数,
*	1C 58 n1 n2	微打中作为设置汉字横向放大n1倍,纵向放大n2倍,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C58(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.WidthMagnification	= GetChar;	//汉字横向倍数
	}
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.HeightMagnification = GetChar;	//汉字纵向倍数
	}
}

void Command_1C67(void)	//待添加
{
}

/*******************************************************************************
* Function Name  : Command_1C70
* Description    : 打印预定义NV位图
Hex 1C 70 n	m
1 ≤ n ≤ 255
0 ≤ m ≤ 3, 48 ≤ m ≤ 51
Prints NV bit image n using the process of  FS q and using the mode  specified by m.
[ 描述]	 根据m 确定的模式打印位图。
	m					模式		纵向分辨率 (DPI)	横向分辨率 (DPI)
0, 48		正常						203									203
1, 49		倍宽						203									101
2, 50		倍高						101									203
3, 51		倍宽,倍高			101									101
[Recommended Functions]
This function is supported only by some printer models and may not be supported by 
future models.It is recommended that NV graphics function 
(GS ( L GS 8 L : <Function 51> and <Function 64> ~ <Function 69>) be used because 
they offer the follow ing additional features: 
	Multiple logo data and mark  data can be specified (except for some models).
	Data can be controlled by key code.
	Redefining or deleting the same data is possible for each key code.
	Color can be specified  for the definition data.
	Data can be defined by raster format.
	The remaining capacity of the definition area can be confirmed.
[Notes]
■ This command is not effective when the NV bit image specified by n has not been defined.
■ In standard mode, this command is effective only when there is no data in the print 
	buffer and the printer is at the beginning of the line.
■ In page mode, the NV bit image is only stored in the print buffer and is not printed.
■ If the NV bit image exceeds one line of pr int area, the printe r does not print it.
■ The scales for width and height of NV bit images are specified by m. Therefore, in 
	page mode with 90° or 270 ° clockwise-rotated NV bit image, the printer applies print 
	area and dot density from [width: direction of paper feed, height: perpendicular to 
	direction of paper feed].
■ This command is not affected by print modes (such as emphasized, underline, character size, 
	or 90° rotated characters), except upside-down print mode.
	倒置打印有效.其他无效.
■ This command executes paper feed for amount needed for printing the NV bit image regardless 
	of paper feed amount set by a paper feed setting command.
■ After printing the bit image, this command sets the print position to the beginning of the line.
■ When printing the NV bit image, selecting unidirectional print mode with ESC U enables 
	printing patterns in which the top and bottom parts are aligned vertically. 
■ The NV bit image is defined by FS q.
■ NV bit image is printed in the default dot density (dot density of vertical and horizontal
	direction in normal mode) defined by GS L <Function 49>.
■ Bit image data and print result are as follows:

d1	dY + 1	...		:				MSB
													LSB
d2	dY + 2	...		dk - 2	MSB
													LSB
:		:				...		dk - 1	MSB
													LSB
dY	dY * 2	...		dk			MSB
													LSB
Y = yL + yH × 256

* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	BIT_BMP_X_BYTE_MAX				(1023)		//下载位图X方向最大字节数
#define	BIT_BMP_Y_BYTE_MAX				(288)			//下载位图Y方向最大字节数
#define	BIT_BMP_XH_MAX						(3)				//下载位图X方向最大字节数高8位最大值
#define	BIT_BMP_YH_MAX						(1)				//下载位图Y方向最大字节数高8位最大值

//下载位图最大可能的字节数
#define	BIT_BMP_MAX_BYTE					(BIT_BMP_X_BYTE_MAX * BIT_BMP_Y_BYTE_MAX * 8)

//下载位图所能占用的最大空间,受限于机型的存储空间
#define	BIT_BMP_MAX_STORE_BYTE		(BIT_BMP_Y_BYTE_MAX * PRT_DOT_NUMBERS)

#define	MAX_BIT_BMP_NUM						(255)			//最大可能的下载图片的数目

/******* 定义NV下载位图类型结构 *************/
typedef struct
{
	uint8_t	ImageNum;		//保存的图片数目
	
	//存放每幅图片X,Y两个方向的字节数目值,每幅图片X方向在前,Y方向在后,xyL在前,xyH在后;
	uint8_t	XYByteNum[MAX_BIT_BMP_NUM * 4];
}
NVFlASH_BMP_Struct;

void Command_1C70(void)
{
	
	uint8_t	FlashBitBmpNum;
	uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT];
	uint16_t High;
	uint16_t Width;
	uint16_t RealWidth;
	uint32_t SectorAddress;
	uint32_t i;
	
	FlashBitBmpNum = ReadInBuffer();	//图片的序列号
	if(FlashBitBmpNum>0)							//2016.08.30  1C70参数n是0到8 和设置工具下载位图位置对应
	{
		FlashBitBmpNum = FlashBitBmpNum-1;
	}	
	
	if (FlashBitBmpNum < MAX_DOWNLOAD_NVFLASH_BITMAP_NUM)	//检查参数合法范围, 否则不处理  7副图
	{
		memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
		//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
		SectorAddress = BIT_BMP_DATA_BASE_ADDR + 0x10000 * FlashBitBmpNum;	//起始存储地址
		sFLASH_ReadBuffer(DataBuf, SectorAddress, 5);
		if (DataBuf[0] == FlashBitBmpNum)	//检查图片是否已经下载了
		{
			//Width = DataBuf[1] + 256 * DataBuf[2];
			//High = DataBuf[3] + 256 * DataBuf[4];
			Width	= (uint16_t)DataBuf[1] + (uint16_t)(((uint16_t)DataBuf[2]) << 8);	//宽度方向上字节数
			High	= (uint16_t)DataBuf[3] + (uint16_t)(((uint16_t)DataBuf[4]) << 8);	//高度方向上点数
			
			if (Width > (g_tSystemCtrl.LineWidth >> 3))
				RealWidth = (g_tSystemCtrl.LineWidth >> 3);		//实际打印的宽度字节数
			else
				RealWidth = Width;
			
			//while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus());	//等待之前打印结束
			
			SectorAddress += 5;
			for (i = 0; i < High; i++)				//打印一点行
			{
				memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
				//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
				sFLASH_ReadBuffer(DataBuf, SectorAddress, RealWidth);
				SectorAddress += Width;
				DotLinePutDrvBuf( DataBuf );				//2016.06.16 送一行数据到驱动打印缓冲区
			}
		}	//待打印图片已经下载了处理结束
	}	//参数范围合法处理
	
// 	uint8_t	GetChar;
// 	uint8_t	cImageNum;	//待打印的位图编号
// 	uint8_t	RatioX;
// 	uint8_t	RatioY;			//横向纵向放大倍数
// 	uint8_t	ratiox;
// 	uint8_t	ratioy;			//填充时的循环变量
// 	uint8_t	TempBuf[MAX_RATIO_Y];
// 	
// 	uint16_t	SpaceDot;						//打印时的空白区
// 	uint16_t	sXByteNum;					//横向字节数
// 	uint16_t	sYByteNum;					//纵向字节数
// 	uint16_t	MaxX_width;					//实际处理打印点宽度
// 	uint16_t	x;
// 	
// 	uint32_t	iCount;							//循环变量
// 	uint32_t	iAddress;						//位图存储地址
// 	uint32_t	iColumnAddress;
// 	
// 	NVFlASH_BMP_Struct tNVFlashImage;	//NV位图控制参数变量
// 	
// 	uint8_t	* pbByteStart = NULL;
// 	uint8_t	* pbTemp = NULL;
// 	uint8_t	* pbColumn = NULL;
// 	
// 	//只在标准模式中且在行首才有效
// 	if (!((g_tSystemCtrl.PrtModeFlag == STD_MODE) && \
// 				(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)))
// 		return;
// 	
// 	cImageNum = ReadInBuffer();	//待打印的图片编号
// 	GetChar = ReadInBuffer();		//打印模式
// 	if (((GetChar > 3) && (GetChar < 48)) || (GetChar > 51))	//m参数无效返回
// 		return;
// 	
// 	if (cImageNum == 0)					//参数不正确
// 		return;
// 	
// 	//读取存储的总图片数目
// 	sFLASH_ReadBuffer(&tNVFlashImage.ImageNum, BIT_BMP_PARA_BASE_ADDR, 1);
// 	if (cImageNum > tNVFlashImage.ImageNum)	//待打印的图片还未定义
// 		return;
// 	
// 	RatioX = (GetChar & 0x01) + 1;					//宽度倍数,最大为2
// 	RatioY = ((GetChar & 0x02) >> 1) + 1;		//高度倍数,最大为2
// 	
// 	//读出到指定图片为止的所有图片的参数
// 	pbByteStart = tNVFlashImage.XYByteNum;
// 	sFLASH_ReadBuffer(pbByteStart, (BIT_BMP_PARA_BASE_ADDR + 1), (cImageNum * 4));
// 	
// 	//计算本次需要打印的图片的存储首地址
// 	iAddress = BIT_BMP_DATA_BASE_ADDR;
// 	pbByteStart = tNVFlashImage.XYByteNum;
// 	for (iCount = 0; iCount < (cImageNum - 1); iCount++)
// 	{
// 		sXByteNum = *pbByteStart++;									//横向字节数低位
// 		sXByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//横向字节数高位
// 		pbByteStart++;
// 		sYByteNum = *pbByteStart++;									//纵向字节数低位
// 		sYByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//纵向字节数高位
// 		pbByteStart++;
// 		//逐幅图片计算占用字节数得出本次待打印图片的首地址
// 		iAddress += (((uint32_t)sXByteNum) * ((uint32_t)sYByteNum) * 8);
// 	}
// 	
// 	//计算本次需要打印的图片的X,Y方向的字节数
// 	sXByteNum = *pbByteStart++;									//横向字节数低位
// 	sXByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//横向字节数高位
// 	pbByteStart++;
// 	sYByteNum = *pbByteStart++;									//纵向字节数低位
// 	sYByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//纵向字节数高位
// 	
// 	x = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit;	//横向可打印最大点数
// 	if (((sXByteNum << 3) * RatioX) > x)
// 		MaxX_width = x / RatioX;
// 	else
// 		MaxX_width = (sXByteNum << 3);
// 	
// 	SpaceDot = g_tPrtCtrlData.PrtLeftLimit;
// 	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)			//字符对齐模式,居中
// 		SpaceDot += ((x - MaxX_width * RatioX) >> 1);
// 	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//居右
// 		SpaceDot += (x - MaxX_width * RatioX);
// 	
// 	//正向打印从首行开始取数,反向打印从最后一行开始取数
// 	if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
// 	{
// 		iAddress += (sYByteNum - 1);	//从高度上最后一行开始
// 	}
// 	
// 	//根据高度字节数每次读取1字节的内容打印.每次处理一字节行.
// 	//按列计算的填充起始指针
// 	pbByteStart = g_tLineEditBuf + SpaceDot + \
// 		(MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
// 	for (iCount = 0; iCount < sYByteNum; iCount++)
// 	{
// 		iColumnAddress = iAddress;	//每一行点阵的数据起始地址
// 		//指向当前编辑数据缓冲区起始字节行,每处理完纵向1个字节,改变其值
// 		pbColumn = pbByteStart;
// 		for (x = 0; x < MaxX_width; x++)	//横向宽度,以点为单位,列值
// 		{
// 			sFLASH_ReadBuffer(&GetChar, iColumnAddress, 1);	//读取1字节数据
// 			if (GetChar)				//数据不为0时全处理
// 			{
// 				if (RatioY > 1)		//需要放大
// 				{
// 					memset(TempBuf, 0x00, MAX_RATIO_Y);
// 					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
// 					ByteZoomCtrl(RatioY, GetChar, TempBuf);
// 				}
// 				else		//纵向放大倍数为1
// 				{
// 					TempBuf[0] = GetChar;
// 				}
// 				pbTemp = pbColumn;
// 				for (ratiox = 0; ratiox < RatioX; ratiox++)
// 				{
// 					for (ratioy = 0; ratioy < RatioY; ratioy++)
// 					{
// 						*(pbTemp + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
// 					}
// 					pbTemp++;
// 				}
// 			}	//一个不为0字节填充完毕,数据为0时直接修改指针
// 			iColumnAddress += sYByteNum;	//指向下一列的数据的存储地址
// 			pbColumn += RatioX;
// 		}		//填充处理1行完毕
// 		
// 		if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
// 			iAddress--;	//指向下一行数据存储首地址,倒置打印时从最后一行倒取数
// 		else
// 			iAddress++;	//指向下一行数据存储首地址,正向打印从首行顺序取数
// 		
// 		//填充完毕打印该行
// 		g_tPrtCtrlData.PrtDataDotIndex += (SpaceDot + MaxX_width * RatioX);
// 		g_tPrtCtrlData.BitImageFlag = 1; 	//位图打印标志
// 		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//纵向总点数
// // 		PrintOneLine();									//打印该编辑缓冲区
// 		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
// 	}	//所有列处理完毕
}

void	WriteNVFlashImageParameter(uint8_t *pbBuf, uint32_t	iAddress, uint16_t iLen)
{
	sFLASH_EraseSector(iAddress);
	sFLASH_WriteBuffer(pbBuf, iAddress, iLen);
}

/*******************************************************************************
* Function Name  : Command_1C71
* Description    : 预定义NV位图
Hex 1C 71 n[xL xH yL yH d1...dk]1...[xL xH yL yH d1...dk]n
1 ≤ n ≤ 255
1 ≤ (xL + xH × 256) ≤ 1023 (0 ≤ xL ≤ 255, 0 ≤ xH ≤ 3)
1 ≤ (yL + yH × 256) ≤ 288  (0 ≤ yL ≤ 255, yH = 0, 1)
0 ≤ d ≤ 255
k = (xL + xH × 256) × (yL + yH × 256) × 8
The definition area is 256 KB
Defines the NV bit image in the NV graphics area.
 n specifies the number of defined NV bit images.
 xL,xH specifies (xL + xH × 256) bytes in the horizontal direction for the NV 
			bit image you defined.
 yL,yH specifies (yL + yH × 256) bytes in the vertical directi on for the NV 
			bit image you defined.
 d specifies the definition data fo r the NV bit image (column format).
 k indicates the number of the definition data. k is an explanation parameter;
			therefore it does not need to be transmitted.
参数给出水平方向字节数,垂直方向字节数,按列取点阵,总字节数需要*8.
[Recommended Functions]
	This function is supported only by some printer models and may not be 
	supported by future models. It is recommended that NV graphics function 
	( GS ( L  GS 8 L :  <Function 51> and <Function 64> ~ <Function 69>) be used 
	instead of FS q because the NV graphics  function offers t he following 
	additional features: 
	Multiple logo data and mark  data can be specified (except for some models).
	Data can be controlled by key code.
	Redefining or deleting is possible for each key code.
	Color can be specified  for the definition data.
	Data can be defined by raster format.
	The remaining capacity of the definition area can be confirmed.
	Continuous processing possible (without a software reset when a command has been processed).
[Notes]
	NV bit image means a bit image which is defined in a non-volatile memory. 
	The NV bit image defined is effective until the next NV bit image is defined.
■ In standard mode, this command is effective only when processed at the beginning of the line. 
■ If this command is processed while a macro is being defined, the printer cancels 
	macro definition and starts processing this command. At this time, the macro becomes undefined.
■ k bytes data of d1...dk is processed as a defined data of a NV bit image. 
	The defined data (d ) specifies a bit printed to 1 and not printed to 0. 
■ All NV bit images previously defined are canceled.
■ After processing this command, the printer executes a software reset. 
	Therefore, processing this command enables the printer to be in the correct 
	status when the power is turned on.
■ The limitations during processing of this command are as follows:
		Even if the PAPER FEED button is pressed, the printer does not feed paper.
		The real-time commands are not processed.
		Even if the ASB function is effective, the ASB status cannot be transmitted.
■ The NV bit image is printed by FS p.
■ Bit image data and print result are as follows:
d1	dY + 1	...		:				MSB
													LSB
d2	dY + 2	...		dk - 2	MSB
													LSB
:		:				...		dk - 1	MSB
													LSB
dY	dY * 2	...		dk			MSB
													LSB
Y = yL + yH × 256
■ Data is written to the non-volatile memory by this command. 
	Note the following when using this command. 
	1.	Do not turn off the power or reset the printer from the interface 
			when this command is being executed.
	2.	The printer is BUSY when writing the data to the non-volatile memory. 
			In this case, be sure not to transmit data from the host because 
			the printer does not receive data.
	3.	Excessive use of this function may destroy the non-volatile memory. 
			As a guideline, do not use any combination of the following commands 
			more than 10 times per day for writing data to the non-volatile memory: 
			GS ( A (part of functions), GS ( C (part of functions), 
			GS ( E (part of functions), GS ( L  /  GS 8 L (part of functions),  
			GS ( M (part of functions),  GS g 0,  FS g 1 , FS q.

* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C71(void)
{
	uint8_t	GetChar;
	uint8_t	cTotalImageNum;			//定义的位图的幅数
	uint8_t	cImageNumCount;			//循环变量
	uint8_t	RealTimeEnable;			//关闭实时指令
	uint8_t	KeyEnable;					//关闭按键允许
	uint8_t	ASBEnable;					//关闭ASB允许
	uint16_t	sXByteNum;				//横向字节数
	uint16_t	sYByteNum;				//纵向字节数
	uint32_t	iOneImageByteNum;	//一幅图像的总字节数,用于计算接收数据值
	uint32_t	iStoreByteNum;		//实际有意义的存储字节数
	uint32_t	iCount;						//循环变量
	uint32_t	iAddress;					//位图存储地址
	uint32_t	iUsedStoreSpace;
	NVFlASH_BMP_Struct tNVFlashImage;	//NV位图控制参数变量
	uint8_t	* pbByte = NULL;

#ifdef	DBG_ZIKU_SPI_WRITE
	uint8_t	bBuf;
	uint8_t	bBufTemp[16];
	uint32_t	iDbgCount;
	uint32_t	iDbgAddress;
#endif
	
	//只在标准模式中且在行首才有效
	if (!((g_tSystemCtrl.PrtModeFlag == STD_MODE) && \
			(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)))
		return;
	
	if (g_tMacro.DefinitionFlag)	//宏定义中,退出宏定义,清除宏
	{
		g_tMacro.Number = 0;				//宏字符个数归零
		g_tMacro.GetPointer = 0;
		g_tMacro.DefinitionFlag = 0;
		g_tMacro.SetFlag = 0;				//宏填充的标志归零
		g_tMacro.RunFlag = 0;
	}
	
	cTotalImageNum = ReadInBuffer();	//定义的位图的幅数
	if (cTotalImageNum == 0x00)
		return;
	
	//关闭自动回传,禁止按键,禁止实时指令
	KeyEnable = g_tFeedButton.KeyEnable;
	g_tFeedButton.KeyEnable	= 1;
	RealTimeEnable = g_tRealTimeCmd.RealTimeEnable;
	g_tRealTimeCmd.RealTimeEnable = 0;
	ASBEnable = g_tError.ASBAllowFlag;
	g_tError.ASBAllowFlag = 0;
	
#if	1
	//擦除此前定义的所有NV位图,以块为单位擦除,每个块64KB
	iAddress = BIT_BMP_DATA_BASE_ADDR;	//每个块的首地址,首地址必须是以64K为单位计数的地址
	for (iCount = 0; iCount < (BIT_BMP_MAX_STORE_SPACE / sFLASH_SPI_BLOCKSIZE); iCount++)
	{
		sFLASH_EraseBlock(iAddress);
		iAddress += sFLASH_SPI_BLOCKSIZE;
	}
#else
	//擦除此前定义的所有NV位图,以扇区为单位擦除,每个扇区4KB
	iAddress = BIT_BMP_DATA_BASE_ADDR;	//每个块的首地址,首地址必须是以4K为单位计数的地址
	for (iCount = 0; iCount < (BIT_BMP_MAX_STORE_SPACE / sFLASH_SPI_SECTORSIZE); iCount++)
	{
		sFLASH_EraseSector(iAddress);
		iAddress += sFLASH_SPI_SECTORSIZE;
	}
#endif

#ifdef	DBG_ZIKU_SPI_WRITE
	iDbgAddress = BIT_BMP_DATA_BASE_ADDR;	//每个块的首地址,首地址必须是以64K为单位计数的地址
	//读取每个块的存储区首字节内容发送到PC
	for (iDbgCount = 0; iDbgCount < (BIT_BMP_MAX_STORE_SPACE / sFLASH_SPI_BLOCKSIZE); iDbgCount++)
	{
		sFLASH_ReadBuffer(&bBuf, iDbgAddress, 1);
		UpLoadData(&bBuf, 1);
		iDbgAddress += sFLASH_SPI_BLOCKSIZE;
		//iDbgAddress += sFLASH_SPI_SECTORSIZE;
	}
#endif

	//擦除控制参数存储区域,以扇区为单位擦除,每扇区4K
	iAddress = BIT_BMP_PARA_BASE_ADDR;
	sFLASH_EraseSector(iAddress);
	tNVFlashImage.ImageNum = 0;			//清空控制参数
#ifdef	DBG_ZIKU_SPI_WRITE
	iDbgAddress = BIT_BMP_PARA_BASE_ADDR;	//每个块的首地址,首地址必须是以64K为单位计数的地址
	for (iDbgCount = 0; iDbgCount < sizeof(tNVFlashImage); iDbgCount++)	//读取整个后续内容发送到PC
	{
		sFLASH_ReadBuffer(&bBuf, iDbgAddress++, 1);
		UpLoadData(&bBuf, 1);
	}
#endif
	
	//根据图片数目,逐个图片进行存储
	pbByte = tNVFlashImage.XYByteNum;
	iUsedStoreSpace = 0;
	iAddress = BIT_BMP_DATA_BASE_ADDR;	//每个块的首地址,首地址必须是以64K为单位计数的地址
	for (cImageNumCount = 0; cImageNumCount < cTotalImageNum; cImageNumCount++)
	{
		//读取图片的横向纵向字节数
		sXByteNum = ReadInBuffer();									//横向字节数低位
		sXByteNum += (uint16_t)(((uint16_t)ReadInBuffer()) << 8);	//横向宽度字节数值
		if ((sXByteNum < 1) || (sXByteNum > BIT_BMP_X_BYTE_MAX))	//检查参数范围
			return;
		
		sYByteNum = ReadInBuffer();									//纵向字节数低位
		sYByteNum += (uint16_t)(((uint16_t)ReadInBuffer()) << 8);	//纵向高度字节数值
		if ((sYByteNum < 1) || (sYByteNum > BIT_BMP_Y_BYTE_MAX))	//检查参数范围
			return;
		
		iOneImageByteNum = sXByteNum * sYByteNum;
		iOneImageByteNum <<= 3;									//1幅图片总数据量,X*Y*8,将来要读取的总数据量
		if (sXByteNum > (PRT_DOT_NUMBERS / 8))	//此处进行参数检查,横向最大字节值
			sXByteNum = (PRT_DOT_NUMBERS / 8);		//打印宽度有限,多存无用
		if (sYByteNum > BIT_BMP_Y_BYTE_MAX)			//纵向最大字节值
			sYByteNum = BIT_BMP_Y_BYTE_MAX;
		
		//受限于预留存储空间及可打印区域,只存储有效的可打印区域,多余数丢弃
		iStoreByteNum = sXByteNum * sYByteNum;
		iStoreByteNum <<= 3;
		
		//检查剩余空间是否可以放下本幅图片
		iUsedStoreSpace += iStoreByteNum;	//需要使用的总空间
		if (iUsedStoreSpace > BIT_BMP_MAX_STORE_SPACE)
		{
			PrintString("No enough NV FLASH space to store.");
			PrintString("\n");
			break;
		}
		
		//逐字节存储数据
		for (iCount = 0; iCount < iStoreByteNum; iCount++)
		{
			GetChar = ReadInBuffer();
			sFLASH_WriteBuffer(&GetChar, iAddress, 1);
#ifdef	DBG_ZIKU_SPI_WRITE
			sFLASH_ReadBuffer(&bBuf, iAddress, 1);
			UpLoadData(&bBuf, 1);
#endif
			iAddress++;
		}
		
		//读取可能的多余数据
		for (; iCount < iOneImageByteNum; iCount++)
		{
			ReadInBuffer();
		}
		
		//处理完一副图片,更新控制参数值
		tNVFlashImage.ImageNum++;
		*pbByte++ = (uint8_t)sXByteNum;
		*pbByte++ = (uint8_t)((sXByteNum & 0xFF00) >> 8);
		*pbByte++ = (uint8_t)sYByteNum;
		*pbByte++ = (uint8_t)((sYByteNum & 0xFF00) >> 8);
	}
	
	//保存NV位图控制参数
	iAddress = BIT_BMP_PARA_BASE_ADDR;
	GetChar = tNVFlashImage.ImageNum;
	sFLASH_WriteBuffer(&GetChar, iAddress++, 1);	//存入图片数目
	pbByte = tNVFlashImage.XYByteNum;
	iStoreByteNum = tNVFlashImage.ImageNum * 4;
	sFLASH_WriteBuffer(pbByte, iAddress, iStoreByteNum);	//存入每个图片的X,Y值
#ifdef	DBG_ZIKU_SPI_WRITE
	iDbgAddress = BIT_BMP_PARA_BASE_ADDR;	//每个块的首地址,首地址必须是以64K为单位计数的地址
	pbByte = bBufTemp;
	for (iDbgCount = 0; iDbgCount < (iStoreByteNum + 1); iDbgCount++)	//读取整个后续内容发送到PC
	{
		sFLASH_ReadBuffer(pbByte, iDbgAddress++, 1);
		UpLoadData(pbByte, 1);
	}
#endif
	
	//恢复现场
	g_tFeedButton.KeyEnable = KeyEnable;
	g_tRealTimeCmd.RealTimeEnable = RealTimeEnable;
	g_tError.ASBAllowFlag = ASBEnable;
	//还需要执行系统软复位
}

/*******************************************************************************
* Function Name  : EscCommand
* Description    : ESSC命令集的处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FS_Command(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x21:
		{
			Command_1C21();		//设置汉字打印模式
			break;
		}
		case 0x26:
		{
			Command_1C26();		//进入汉字模式
			break;
		}
		case 0x2D:
		{
			Command_1C2D();		//设置/取消汉字下划线
			break;
		}
		case 0x2E:
		{
			Command_1C2E();		//退出汉字模式
			break;
		}
		case 0x32:
		{
			Command_1C32();		//自定义汉字
			break;
		}
		case 0x3F:
		{
			Command_1C3F();		//取消用户自定义汉字
			break;
		}
		case 0x43:
		{
			Command_1C43();		//选择汉字代码集
			break;
		}
		case 0x50:
		{
			Command_1C50();		//打印预下载位图
			break;
		}
		case 0x53:
		{
			Command_1C53();		//设置汉字左右间距
			break;
		}
		case 0x55:
		{
			Command_1C55();		//微打中作为设置汉字横向放大n倍,
			break;
		}
		case 0x56:
		{
			Command_1C56();		//微打中作为设置汉字纵向放大n倍,
			break;
		}
		case 0x57:
		{
			Command_1C57();		//选择/取消汉字倍宽倍高模式
			break;
		}
		case 0x58:
		{
			Command_1C58();		//微打中作为设置汉字横向放大n1倍,纵向放大n2倍,
			break;
		}
		case 0x70:
		{
			Command_1C70();		//打印预定义NV位图
			break;
		}
		case 0x71:
		{
			Command_1C71();		//定义NV位图
			break;
		}
		
		default:
			break;
	}		 	
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
