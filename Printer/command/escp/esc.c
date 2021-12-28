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
#include	"esc.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"feedpaper.h"
#include	"print.h"
#include	"charmaskbuf.h"
#include	"timconfig.h"
#include	"init.h"
#include	"cutter.h"
#include	"defconstant.h"
#include	"drawer.h"

extern	uint8_t	g_bLineEditBufStatus;
extern  TypeDef_StructLinePrtBuf	g_tLinePrtBuf;

extern	uint32_t	Pdf417_Print(uint8_t * pBuf, uint8_t Width, uint8_t CorID, uint16_t CharLen);
extern	uint32_t	Matrix_Print(uint8_t * pBuf, uint8_t vHeight, uint8_t vWidth, uint16_t charlen);
extern	uint32_t	QR_Print(uint8_t * pBuf, uint8_t Version, uint8_t CorID, uint16_t CharLen);

extern	void ClearInBuf(void);

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
* Function Name  : Command_1B0C
* Description    : 页模式下打印命令(ESC FF )的处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B0C(void)
{
	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)	
	{
		
	}
}

/*******************************************************************************
* Function Name  : Command_1B13
* Description    : 打印位图,高度8点,宽度不超过机器可打印宽度的黑白位图,
*	受字符放大命令,反向打印命令影响
*	处理方法:计算接收字节长度;逐个接收位图数据;根据放大指令等情况逐个将位图数据填充
*	到打印缓冲区;如果满时则打印当前行;直到填充完毕为止
*	2012-10-7添加
*	1B   4B   nL nH d1…dk 
*	0 ≤ nL ≤ 255 
* 0 ≤ nH ≤ 255
* 0 ≤ d ≤ 255
*	本命令只能打印高度为8点,宽度不超过可打印区域的黑白位图.
* nL nH分别为无符号型双字节整数N的低位和高位字节.表示水平方向上位图中的点数.
* 打印(nL + nH * 256) * 8点阵图形.该图像的宽度为(nL + nH * 256)点,高度为8点.
* 每一列的8个点可以由一个8位的字节来表示,最高位在上.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1B13(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	Line;
	uint8_t	Width;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	i;
	uint16_t	RevNumber;
	uint16_t	iMaxNumber;
	uint8_t	* p = NULL;
	uint8_t	* pstart = NULL;
	uint8_t	* ptemp = NULL;
	
	GetChar = ReadInBuffer();
	iMaxNumber = GetChar + ReadInBuffer() * 256;	//数据长度
	//iMaxNumber = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);	//数据长度
	if (iMaxNumber == 0)	//如果数据字节数为0直接退出
		return;
	
	//在编辑行打印缓冲区之前需要先检查其是否空闲,如果已满等待打印,则需要等待行打印缓冲区打印结束,
	//然后将编辑行打印缓冲区的内容复制到打印行打印缓冲区,然后启动打印,清空编辑缓冲区及变量,再开始编辑.
	//如果编辑行打印空闲,编辑过程中满,也需要进行以上处理
	//if (g_bLineEditBufStatus == LEB_STATUS_WAIT_TO_PRINT)	//编辑行打印缓冲区等待打印
	{
	//	PrintOneLine();		//先打印该编辑缓冲区
	}
	//WaitPreviousPrintFinish();
	
	if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))
	{
		RatioX = 2;
	}
	else
	{
		RatioX = g_tPrtModeChar.WidthMagnification;	//确定横向放大倍数,包括行内倍宽
	}
	if (RatioX > MAX_RATIO_X)
		RatioX = MAX_RATIO_X;
	
	RatioY = g_tPrtModeChar.HeightMagnification;	//确定纵向放大倍数
	if (RatioY > MAX_RATIO_Y)
		RatioY = MAX_RATIO_Y;
	
	RevNumber = iMaxNumber;
	if ((RevNumber * RatioX) > g_tPrtCtrlData.PrtLength)	//确定最大计算接收字节长度
	{
		RevNumber = g_tPrtCtrlData.PrtLength / RatioX;
	}
	
	//指针初始化及各个控制变量初始化
	ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
	for (i = 0; i < RevNumber; i++)		//逐个接收位图数据
	{
		//不满一行则放入行打印缓冲区
		if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) <= g_tPrtCtrlData.PrtLength)
		{
			GetChar = ReadInBuffer();		//接收一个位图数据
			if (GetChar)
			{
				if (RatioY > 1)
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
					ByteZoomCtrl(RatioY, GetChar, TempBuf);		//纵向放大
					//放入打印缓冲区,从位图放大后的高位MSB开始放起,因为每次处理一个位图字节,
					//所以填充时数据字节数在高度上=RatioY,在宽度上=RatioX
					//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					for (Line = 0; Line < RatioY; Line++)
					{	//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
						p = pstart;
						for (Width = 0; Width < RatioX; Width++)	//横向放大倍数
						{
							*(p + Width) = TempBuf[Line];	//水平方向上填充
						}
						pstart += PRT_DOT_NUMBERS;
					}		//一个位图字节横向纵向都填充完毕
				}
				else
				{
					TempBuf[0] = GetChar;
					//放入打印缓冲区,从位图放大后的高位MSB开始放起,因为每次处理一个位图字节,
					//所以填充时数据字节数在高度上=RatioY,在宽度上=RatioX
					//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					for (Width = 0; Width < RatioX; Width++)	//横向放大倍数
					{
						*(pstart + Width) = TempBuf[0];	//水平方向上填充
					}
				}			//一个位图字节横向纵向都填充完毕
			}
			
			g_tPrtCtrlData.PrtDataDotIndex += RatioX;
		}
		else
		{
			break;					//多余数据全部丢弃,不再打印
		}
	}		//End 位图宽度
	
	//检查本次编辑的内容是否已经打印了, 如果没打印则启动打印
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
		g_bLineEditBufStatus = LEB_STATUS_WAIT_TO_PRINT;
		g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_QT;	//将其作为驱动模式实现
		g_tPrtCtrlData.BitImageFlag = 1; 		//位图打印标志
// 		if (g_tPrtCtrlData.MaxPrtDotHigh < (uint16_t)((uint16_t)RatioY << 3))
// 			g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//打印行高
		if (g_tPrtCtrlData.MaxPrtDotHigh < (RatioY * 8))
			g_tPrtCtrlData.MaxPrtDotHigh = (RatioY * 8);	//打印行高
// 		PrintOneLine();	//打印该编辑缓冲区
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	
	for (; i < iMaxNumber; i++)
		ReadInBuffer();		//接收多余的位图数据
	
	p = NULL;
	pstart = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B20
* Description    : 设置字符右间距,
*	设置字符的右间距为[n× 横向移动单位或纵向移动单位],当前程序单位按照mm处理
*	当字符放大时,间距随之放大相同的倍数
*	该指令不影响汉字的设置
*	此指令设置的值在页模式和标准模式下是相互独立的
*	横向或纵向移动单位由GS P指定.改变横向或纵向移动单位不改变当前右间距
*	标准模式下,使用横向移动单位
*	在页模式下,根据区域的方向和起始位置来选择使用横向移动单位或纵向移动单位,
*	其选择方式如下:
*	① 当打印起始位置由ESC T 设置为打印区域的左上角或右下角时,使用横向移动单位；
*	② 当打印起始位置由ESC T 设置为打印区域的左下角或右上角时,使用纵向移动单位；
* 最大右间距是255/203英寸.任何超过这个值的设置都自动转换为最大右间距
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B20(void)
{
	uint8_t	GetChar;
	uint32_t	nRightSpace;
	
	GetChar = ReadInBuffer();
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		nRightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
		if (nRightSpace > MAX_RIGHT_SPACE_HORIZONTAL)	//添加,最大值限制
		{
			nRightSpace = MAX_RIGHT_SPACE_HORIZONTAL;
		}
		g_tPrtModeChar.RightSpace = nRightSpace;
	}
	else
	{
		g_tPageMode.AscRightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;	//页模式
	}
}

/*******************************************************************************
* Function Name  : Command_1B21
* Description    : 设置字符打印模式,包括字符点阵,倍宽,倍高,加粗,下划线等
* 位	1/0  	HEX	Decimal		功能 
*	0		0			00		0				标准ASCII字体A (12 × 24)
*	0		1			01		1				压缩ASCII字体B (9 ×   17) 
*	1,2											未定义
*	3		0			00		0				取消加粗模式
*	3		1			08		8				选择加粗模式
*	4		0			00		0				取消倍高模式
*	4		1			10		16			选择倍高模式
*	5		0			00		0				取消倍宽模式
*	5		1			20		32			选择倍宽模式
*	6												未定义
*	7		0			00		0				取消下划线模式
*	7		1			80		128			选择下划线模式
*	除了HT设置的空格和顺时针旋转90°的字符,其余任何字符都可以加下划线
*	下划线宽度由  ESC -  确定,与字符无关
*	当一行中部分字符为倍高或更高,所有字符以底端对齐
*	ESC E也能选择或取消加粗模式,最后被执行的指令有效
*	ESC –也能选择或取消下划线模式,最后被执行的指令有效
*	GS ! 也能设置字符大小,最后被执行的指令有效
*	加粗模式对字符和汉字都有效,除加粗模式外,其他模式只对字符有效
*	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B21(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar & 0x01))	//使用9 x 17 的ASCII字符(标准模式和页模式同时有效)
	{
		g_tSysConfig.SysFontSize = ASCII_FONT_B;
		g_tPrtModeChar.CharWidth = ASCII_FONT_B_WIDTH;
		g_tPrtModeChar.CharHigh  = ASCII_FONT_B_HEIGHT;
	}
	else									//使用12 x 24 的ASCII字符(标准模式和页模式同时有效)
	{
		g_tSysConfig.SysFontSize = ASCII_FONT_A;
		g_tPrtModeChar.CharWidth = ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh  = ASCII_FONT_A_HEIGHT;
	}
	
	if ((GetChar & 0x08))	//选择字符加粗模式(标准模式和页模式同时有效)
	{
		g_tPrtModeChar.EmphaSizeMode = 1;
		g_tPrtModeHZ.EmphaSizeMode = 1;
	}
	else									//不加粗
	{
		g_tPrtModeChar.EmphaSizeMode = 0;
		g_tPrtModeHZ.EmphaSizeMode = 0;
	}
	
	if ((GetChar & 0x10))	//选择字符倍高模式(标准模式和页模式同时有效)
	{
		g_tPrtModeChar.HeightMagnification = 2;
	}
	else									//不倍高
	{
		g_tPrtModeChar.HeightMagnification = 1;
	}
	
	if ((GetChar & 0x20))	//选择字符倍宽模式(标准模式和页模式同时有效)
	{
		g_tPrtModeChar.WidthMagnification = 2;
	}
	else									//不倍宽
	{
		g_tPrtModeChar.WidthMagnification = 1;
	}
		
	if ((GetChar & 0x80))	//选择字符下划线模式(标准模式和页模式同时有效)
	{
		g_tPrtModeChar.Underline |= UNDERLINE_ENABLE_MASK;
	}
	else									//取消下划线功能
	{
		g_tPrtModeChar.Underline &= UNDERLINE_DISABLE_MASK;
	}
}

/*******************************************************************************
* Function Name  : Command_1B24
* Description    : 设置绝对打印位置,驱动模式下,只有1字节参数,用字节表示位置
*	将当前位置设置到距离行首(nL + nH×256 )× (横向或纵向移动单位)处
*	如果设置位置在指定打印区域外,该命令被忽略
*	横向和纵向移动单位由GS P 设置
*	标准模式下使用横向移动单位
*	在页模式下,根据打印区域的方向和打印起始位置来选择使用横向移动单位或纵向
*	移动单位,其选择方式如下: 
*	① 当打印起始位置由ESC T设置为打印区域的左上角或右下角时,使用横向移动单位； 
*	② 当打印起始位置由ESC T设置为打印区域的左下角或右上角时,使用纵向移动单位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B24(void)
{
	uint8_t	GetChar;
	uint16_t	temp;
	
	if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)		//IP驱动用命令
	{
		g_tPrtCtrlData.DriverOffset = ReadInBuffer();
	}
	else
	{
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//标准模式下
		{
			GetChar = ReadInBuffer();
			//temp = (ReadInBuffer() * 256 + GetChar) * g_tPrtCtrlData.StepHoriUnit;
			temp = ((uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8)) * g_tPrtCtrlData.StepHoriUnit;
			if ((temp >= g_tPrtCtrlData.PrtLeftLimit) && \
					(temp <= (g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtLength)) && \
					(temp < PRT_DOT_NUMBERS))       //2016.05.08  小于等于576改为小于
			{
				g_tPrtCtrlData.PrtDataDotIndex = temp;    //2017.04.24
			}
			
		}
		else		//添加,页模式下有待完善
		{
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B25
* Description    : 选择/取消用户自定义字符,只有最低位D0有效
*	当n的最低位为0时,取消用户自定义字符
*	当n的最低位为1时,选择用户自定义字符
*	当取消使用用户自定义字符的时候,自动使用内部字库
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B25(void)
{
	if (ReadInBuffer() & 0x01)
	{
		g_tSystemCtrl.UserSelectFlag = 1;
	}
	else
	{
		g_tSystemCtrl.UserSelectFlag = 0;
	}
}

/*******************************************************************************
* Function Name  : Command_1B26	 1B 26 3  c1 c2 [x1 d1...d(y × x1)]...[xk d1...d(y × xk)]
* Description    : 定义用户自定义字符,按当前设置的字体点阵自动选择字符宽度12*24点或9*17点
*	32 ≤   c1 ≤   c2 ≤   126 
*	0  ≤   x  ≤   12	标准ASCII字体A(12×24) 
*	0  ≤   x  ≤   9		压缩ASCII字体B(9×17)
*	0  ≤   d1 ... d(y ×   xk)  ≤   255
*	K = c2 - c2 + 1 
*	y指定纵向字节数, x指定横向点数,c1是起始字符代码,c2是终止字符代码
* 允许的字符代码范围是ASCII码的 <20>H  到 <7E>H (95个字符).
*	可以连续定义多个字符,如果只定义一个字符c1 = c2
*	d是下载字符的数据.各点的数据从左边开始
*	自定义字符的大小是 (y  ×   x) 字节
*	数据的各个位为1表示打印这个点,为0表示不打印
*	当下列情况,用户自定义字符被清除: 
*	①  ESC @  被执行 
*	②  ESC ? 被执行. 
*	③  打印机复位或电源关闭. 
*	当自定义字符为字体B(9 ×17)时,纵向第3字节数据只有最高位有效
*	可先用ESC :指令将字库中字形全部复制到RAM,然后再执行本指令.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B26(void)
{
	uint8_t	i, j;
	uint8_t	CodeStart, CodeStop;
	uint8_t	CharWidth, Number;
	uint8_t	* pCode = NULL;
	uint8_t	* pZimo = NULL;
	
	if (ReadInBuffer() != 3)		//高度不等于3,为无效命令
		return;
	
	CodeStart = ReadInBuffer();		//定义字符的起始和结束
	CodeStop = ReadInBuffer();
	if ((CodeStop < CodeStart) || \
			(CodeStart < 0x20) || (CodeStart > 0x7E) || \
			(CodeStop < 0x20) || (CodeStop > 0x7E) || \
			((CodeStop - CodeStart + 1) > USER_ZF_SIZE)
			)	//参数无效
		return;		//c2>=c1, 0x20=< c <=0x7e
	
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)  //12*24点阵字符
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12×24用户定义字符代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12×24用户定义字符字模表
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)  //9*17点阵字符
	{
		CharWidth = ASCII_FONT_B_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_09;	//9×17用户定义字符代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_09;		//9*17用户定义字符字模表
	}
	else
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12×24用户定义字符代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12×24用户定义字符字模表
	}
	
	pZimo += ((CodeStart - 0x20) * 3 * CharWidth);	//指向此次定义字符存储字模起始地址
	for (i = CodeStart; i <= CodeStop; i++)
	{
		pCode[i - 0x20] = i;			//自定义字符索引,i最小从0x20开始
		Number = ReadInBuffer();	//指定横向点数
		if (Number > CharWidth)		//参数无效退出
			return;
		
		for (j = 0; j < (Number * 3); j++)	//字符横向点数由Number指定,不足CharWidth的才用0补足
		{
			*pZimo++ = ReadInBuffer();	//存储字模值
		}
		for (; j < (CharWidth * 3); j++)
		{
			*pZimo++ = 0x00;						//不足的补零
		}	//End 字符宽,以上为修改程序
	}	// End 代码数
	
	g_tRamBuffer.UserCharStruct.UserdefineFlag |= 0x01;	//定义了自定义字符
	
	pCode = NULL;
	pZimo = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B2A
* Description    : 按选择的位图模式,将位图数据存入打印缓冲区
*	1B    2A  m   nL  nH  d1...dk
*	m = 0, 1, 32, 33
*	0 ≤ nL ≤ 255
*	0 ≤ nH ≤ 7
*	0 ≤ d  ≤ 255
* 1  ≤  ( nL + nH ×  256)  ≤  2047 (0 ≤  nL ≤  255, 0 ≤  nH ≤   7)
*	选择由m指定的一种位图模式,将位图数据保存到打印缓冲区中,位图数据个数由nL和nH确定:
*	m  模式 					纵向  					横向
*								点数  分辨率  分辨率  	数据个数(k)
*	0  8点单密度  	8 	68 DPI  101 DPI   nL + nH × 256
*	1  8点双密度  	8  	68 DPI  203 DPI  	nL + nH × 256
*	32 24点单密度  24  	203 DPI 101 DPI   (nL + nH × 256) × 3
*	33 24点双密度  24  	203 DPI 203 DPI  	(nL + nH × 256) × 3
*	如果m 的值超出规定范围,nL  和其后的数据被作为普通数据处理
*	横向打印点数由nL和nH决定,总的点数为 nL + nH × 256
*	位图超出当前区域的部分被截掉
*	d 是位图的数据.每个字节的相应位为1表示打印该点,为0不打印该点.
*	位图数据发送完成后,打印机返回普通数据处理模式
*	除了倒置模式,这条指令不受其它打印模式影响(加粗,双重打印,下划线,字符放大和反显)
*	这条指令不受其它打印模式影响 (加粗,双重打印,下划线,字符放大和反显).
* 纵向高度上只有1字节或者3字节,高度固定.
* 横向长度上不固定,有可能超过最大打印点数.如果是倍宽模式,有可能扩展为两行.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B2A(void)
{
	uint8_t	GetChar;
	uint8_t	BmpMode;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	Width;
	uint8_t	Line;
	uint8_t	LEBFullFlag;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	i;
	uint16_t	RevNumber;
	uint8_t	* p = NULL;
	uint8_t	* pstart = NULL;
	uint8_t	* ptemp = NULL;
	
	BmpMode = ReadInBuffer();	
	if ((BmpMode == 0) || (BmpMode == 1) || (BmpMode == 32) || (BmpMode == 33))
	{
		if ((BmpMode == 0) || (BmpMode == 32))
		{
			RatioX = 2;					   //横向单密度
		}
		else
		{
			RatioX = 1;					   //横向双密度
		}
		if ((BmpMode == 0) || (BmpMode == 1))
		{
			RatioY = 3;					   //单字节8点放大为24点
		}
		else
		{
			RatioY = 1;					   //三字节24点
		}
		GetChar = ReadInBuffer();
		RevNumber = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);	//图形横向点数
		if (RevNumber > g_tPrtCtrlData.PrtLength)
		{
			RevNumber = g_tPrtCtrlData.PrtLength;
		}
		if (RevNumber > PRT_DOT_NUMBERS)	//确定最大计算接收字节长度
		{
			RevNumber = PRT_DOT_NUMBERS;		//剩余数据作为普通数据处理
		}
		
		if ((BmpMode == 32) || (BmpMode == 33))
		{
			RevNumber *= 3;			//24点阵图形数据量比8点阵图形大3倍
		}
	}		//模式参数正确处理结束
	else
	{
		return;	//模式参数错误,不处理
	}
	
	LEBFullFlag = 0;	//先假定不满行
	if ((BmpMode == 0) || (BmpMode == 1))		//以下处理8点高的情况
	{
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - 3) * PRT_DOT_NUMBERS;
		for (i = 0; i < RevNumber; i++)		//逐个接收位图数据
		{	//不满一行则放入行打印缓冲区
			if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) <= g_tPrtCtrlData.PrtLength)
			{
				GetChar = ReadInBuffer();		//接收一个位图数据
				if (GetChar)
				{
					//放入打印缓冲区,从位图放大后的高位MSB开始放起,因为每次处理一个位图字节,
					//所以填充时数据字节数在高度上=RatioY=1,在宽度上=RatioX
					//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
					//纵向放大三倍
					ByteZoomCtrl(RatioY, GetChar, TempBuf);	 //纵向放大处理
					for (Line = 0; Line < 3; Line++)
					{	//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
						p = pstart + Line * PRT_DOT_NUMBERS;
						for (Width = 0; Width < RatioX; Width++)	//横向放大倍数
						{
							*(p + Width) = TempBuf[Line];	//水平方向上填充
						}		//一个位图字节横向填充完毕
					}
				}
				g_tPrtCtrlData.PrtDataDotIndex += RatioX;
			}		//一个位图字节横向填充完毕
			else
			{
				LEBFullFlag = 1;
				break;		//如果已满一行则停止编辑
			}
		}
		for (; i < RevNumber; i++)	//逐个接收位图数据
			ReadInBuffer();		//后续多余数据全部丢掉
		
		//先设置打印控制变量
		g_tPrtCtrlData.BitImageFlag = 1; 	//位图打印标志
		if (g_tPrtCtrlData.MaxPrtDotHigh < 24)
		{
			g_tPrtCtrlData.MaxPrtDotHigh = 24;
		}
		if (LEBFullFlag)		//如果编辑过程中满一行则打印
		{
      //打印该编辑缓冲区
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		}
	}			//8点高处理结束
	else if ((BmpMode == 32) || (BmpMode == 33))	//以下处理24点高的情况
	{
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - 3) * PRT_DOT_NUMBERS;
		for (i = 0; i < RevNumber; i += 3)	//逐个接收位图数据,一次循环要接收3字节
		{	//不满一行则放入行打印缓冲区
			if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) <= g_tPrtCtrlData.PrtLength)
			{
				memset(TempBuf, 0x00, MAX_RATIO_Y);
				//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
				TempBuf[0] = ReadInBuffer();		//接收一个位图数据
				TempBuf[1] = ReadInBuffer();		//接收一个位图数据
				TempBuf[2] = ReadInBuffer();		//接收一个位图数据
				if (TempBuf[0] || TempBuf[1] || TempBuf[2])	//图形值不为0
				{
					//放入打印缓冲区,从位图放大后的高位MSB开始放起,因为每次处理一个位图字节,
					//所以填充时数据字节数在高度上=RatioY=1,在宽度上=RatioX
					//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					for (Line = 0; Line < 3; Line++)
					{	//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
						p = pstart + Line * PRT_DOT_NUMBERS;
						for (Width = 0; Width < RatioX; Width++)	//横向放大倍数
						{
							*(p + Width) = TempBuf[Line];	//水平方向上填充
						}		//一个位图字节横向填充完毕
					}
				}
				g_tPrtCtrlData.PrtDataDotIndex += RatioX;
			}
			else
			{
				LEBFullFlag = 1;
				break;		//如果已满一行则停止编辑
			}
		}		//End 位图宽度
		for (; i < RevNumber; i++)	//逐个接收位图数据
			ReadInBuffer();		//后续多余数据全部丢掉
		
		//先设置打印控制变量
		g_tPrtCtrlData.BitImageFlag = 1; 	//位图打印标志
		if (g_tPrtCtrlData.MaxPrtDotHigh < 24)
		{
			g_tPrtCtrlData.MaxPrtDotHigh = 24;
		}
		if (LEBFullFlag)		//如果编辑过程中满一行则打印
		{
      //打印该编辑缓冲区
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		}
	}			//24点高处理结束
	
	p = NULL;
	pstart = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B2D
* Description    : 设置/取消字符下划线
*	1B   2D   n
*	0  ≤   n  ≤ 2, 48 ≤ n  ≤   50 
*		n						功能 
*	0, 48			取消下划线模式 
*	1, 49			选择下划线模式(1 点宽)  
*	2, 50			选择下划线模式(2 点宽)  
*	下划线可加在所有字符下(包括右间距),但不包括HT设置的空格
*	下划线不能作用在顺时针旋转90° 和反显的字符下
*	当取消下划线模式时,后面的字符不加下划线,下划线的宽度不改变.默认宽度是一点宽
*	改变字符大小不影响当前下划线宽度
*	下划线选择取消也可以由  ESC ! 来设置.最后执行的指令有效
*	该指令不影响汉字的设置
*	默认n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B2D(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar == 0 || GetChar == 48)	   	//取消下划线
		g_tPrtModeChar.Underline &= UNDERLINE_DISABLE_MASK;
	else if (GetChar == 1 || GetChar == 49)	//设置1点模式
		g_tPrtModeChar.Underline |= UNDERLINE_ENABLE_ONE_DOT_MASK;
	else if (GetChar == 2 || GetChar == 50)	//设置2点模式
		g_tPrtModeChar.Underline |= UNDERLINE_ENABLE_TWO_DOT_MASK;
}

/*******************************************************************************
* Function Name  : Command_1B31
* Description    : 设置行间距为n点行
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B31(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
		g_tPrtCtrlData.CodeLineHigh = ReadInBuffer();
}

/*******************************************************************************
* Function Name  : Command_1B32
* Description    : 设置默认行高, 选择32点(4mm,约1/6英寸)行高,行模式与页模式分开
*	行高在标准模式和页模式下是独立的
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B32(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
		g_tPrtCtrlData.CodeLineHigh = DEFAULT_CODE_LINE_HEIGHT;
	else
		g_tPageMode.PageLineHigh = DEFAULT_CODE_LINE_HEIGHT;
}

/*******************************************************************************
* Function Name  : Command_1B33n
* Description    : 设置行,行模式与页模式分开
*	1B   33   n 
*	设置行高为  [ n × 纵向或横向移动单位]
*	行高设置在标准模式和页模式下是相互独立的
*	横向和纵向移动单位由  GS P  设置,改变这个设置不影响当前行高
*	标准模式下,使用纵向移动单位
*	在页模式下,根据打印区域的方向和打印起始位置来选择使用横向移动单位或纵向移动单位,
*	其选择方式如下: 
*	① 当打印起始位置由ESC T设置为打印区域的左上角或右下角时,使用纵向移动单位； 
*	② 当打印起始位置由ESC T设置为打印区域的左下角或右上角时,使用横向移动单位； 
* 最大走纸距离是1016 mm (40 英寸).  如果超出这个距离,取最大距离
*	默认行高为 4mm (约1/6  英寸)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B33(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//标准模式
	{
    g_tPrtCtrlData.CodeLineHigh = GetChar * g_tPrtCtrlData.StepVerUnit;				 //2017.02.09 客户大都是参数为行高
  //g_tPrtCtrlData.CodeLineHigh = (GetChar /(2*g_tPrtCtrlData.StepVerUnit));   //兼容爱普生的行间距
		if (g_tPrtCtrlData.CodeLineHigh > MAX_LINE_SPACE_VERTICAL)	//最大40吋,1016mm,不能超过该值
			g_tPrtCtrlData.CodeLineHigh = MAX_LINE_SPACE_VERTICAL;
	}
	else	//页模式
	{
		if ((g_tPageMode.PageFeedDir == 0) || (g_tPageMode.PageFeedDir == 2))
		{
			g_tPageMode.PageLineHigh = GetChar * g_tPrtCtrlData.StepHoriUnit;
			if (g_tPageMode.PageLineHigh > MAX_LINE_SPACE_HORIZONTAL)
			{
				g_tPageMode.PageLineHigh = MAX_LINE_SPACE_HORIZONTAL;
			}
		}
		else
		{
			g_tPageMode.PageLineHigh = GetChar * g_tPrtCtrlData.StepVerUnit;
			if (g_tPageMode.PageLineHigh > MAX_LINE_SPACE_VERTICAL)			//添加
			{	
				g_tPageMode.PageLineHigh = MAX_LINE_SPACE_VERTICAL;				//
			}
		}
		g_tPageMode.PageCurrentLineHigh = g_tPageMode.PageLineHigh;
	}
}

/*******************************************************************************
* Function Name  : Command_1B3D
* Description    : 选择打印机
*	1B   3D   n 
*	1≤   n  ≤   3
*	选择打印机,被选择的打印机可以接收主计算机发送的数据
*	Hex  Decimal	功能
*	1,3		1,3  		打印机允许
*	2			2  			打印机禁止
*	当打印机被禁止时,打印机忽略除实时指令(DLE EOT, DLE ENQ, DLE DC4 )外的所有
*	其他指令,直到用该指令允许为止
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//以下的程序参数是按照EPSON T88IV的,n=1,3允许;n=2禁止打印机
void Command_1B3D(void)
{
	if (g_tSystemCtrl.PrtDeviceDisable)	   //打印机已禁止
	{
		while (g_tSystemCtrl.PrtDeviceDisable)
		{
			if (ReadInBuffer() == 0x1B)
			{
				if (ReadInBuffer() == 0x3D)
				{
					switch (ReadInBuffer())
					{
						case	1:
						case	3:
						case	'1':
						case	'3':
							g_tSystemCtrl.PrtDeviceDisable = 0;		//允许打印机
							break;
						default:
							break;
					}
				}
			}
		}
	}
	else	//打印机原本是允许
	{
		switch (ReadInBuffer())
		{
			case	2:
			case	'2':
				g_tSystemCtrl.PrtDeviceDisable = 1;		//禁止打印机
				break;
			default:
				break;
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B3F
* Description    : 取消用户自定义字符
*	1B   3F   n
*	32 ≤ n  ≤ 126
*	取消用户自定义字符
*	取消用户自定义字符中代码为n的字符.取消后,此字符使用内部字库
*	该指令删除由ESC ！所选择的字体集中指定代码的字模
*	如果自定义字符中没有该字符,该指令被忽略
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B3F(void)
{
	uint8_t	GetChar;
	uint8_t	CharWidth;
	uint8_t	i;
	uint8_t	* pCode = NULL;
	uint8_t	* pZimo = NULL;
	
	GetChar = ReadInBuffer();
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)	//12*24点阵字符
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12×24用户定义字符代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12×24用户定义字符代码表
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//9*17点阵字符
	{
		CharWidth = ASCII_FONT_B_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_09;	//9×17用户定义字符代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_09;		//9*17用户定义字符的存储BUFFER
	}
	else
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12×24用户定义字符代码表
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12×24用户定义字符代码表
	}
	
	if ((GetChar >= 0x20) && (GetChar < 0x7F) && (GetChar < (0x20 + USER_ZF_SIZE)))
	{
		pCode += (GetChar - 0x20);	//找到该字符应该存储的位置
		if (*pCode == GetChar)			//如果该字符已经被定义则清除
		{
			*pCode = 0;								//清除代码
			pZimo += ((GetChar - 0x20) * CharWidth * 3);
			for (i = 0; i < (CharWidth * 3); i++)		//清除字模数据
				*pZimo++ = 0;
		}
	}
	
	pCode = NULL;
	pZimo = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B40
* Description    : 初始化打印机
*	清除打印缓冲区数据,打印模式被设为上电时的默认值模式
*	DIP开关的设置不进行再次检测
*	接收缓冲区内容保留
*	宏定义保留
*	flash位图数据不擦除
*	flash用户数据不擦除
*	维护计数器值不擦除
*	由GS ( E 指定的设置值不擦除
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B40(void)
{
	while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus())
	{
		;
	}
	if(g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_NONE)	//2016.06.21
	{	
		InitParameter(1);
		//清除页打印缓冲区
	}	
}

/*******************************************************************************
* Function Name  : Command_1B44
* Description    : 设置横向跳格位置
*	1B   44  n1...nk  NUL
*	1  ≤   n1 ≤ n2 ≤ …≤ nk≤   255
*	0  ≤   k  ≤   32
*	设置横向跳格位置
*	由行首起第n列设置一个跳格位置
*	共有k 个跳格位置
*	横向跳格位置由下式计算
*	字符宽度 × n ,字符宽度包括右间距,如果字符为倍宽,跳格距离也随之加倍
*	该指令取消以前的跳格位置设置
*	跳格位置按升序排列,结束符为NUL
*	当[n]k小于或等于前一个[n]k-1值时,跳格设置结束,后面的数据作为普通数据处理
*	ESC D NUL  取消所有的跳格位置设置
*	最多设置32个( k = 32)跳格位置,超过32的跳格位置数据被作为普通数据处理
*	改变字符宽度,先前指定的跳格位置并不发生变化
*	字符宽度在标准模式和页模式下是独立的
*	默认跳格设置是每8个标准ASCII字符(12  ×  24)一个跳格位置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B44(void)
{
	uint8_t	GetChar;
	uint8_t	PreChar;
	uint8_t	TebNum;
	uint16_t	Width;
	uint32_t	i;
	
	
 	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//标准模式
	{
		Width = (g_tPrtModeChar.CharWidth + g_tPrtModeChar.RightSpace) * g_tPrtModeChar.WidthMagnification;
	}
	else
	{
		Width = (g_tPrtModeChar.CharWidth + g_tPageMode.AscRightSpace) * g_tPrtModeChar.WidthMagnification;
	}
	
	PreChar = 0x00;
	TebNum = 0x00;
	
	while ( TebNum < MAX_HT_INDEX_NUM)				
	{
		GetChar = ReadInBuffer();
		if ((GetChar <= PreChar) || (GetChar == 0x00))				//如果nk ≤ n[k-1] 或n＝0x00，则结束
		{
			break;
		}
		else
		{
			PreChar = GetChar;
			//--跳格位置 = 设置值 x （当前ASCII字符宽 + 当前ASCII字符右间距）x 当前ASCII字符横向放大倍数
			g_tTab.TabPositionIndex[TebNum] = GetChar * Width;
			TebNum++;
		}
	}

	g_tTab.TabSetNumber = TebNum;
	g_tTab.TabIndex = 0;
	for (i = TebNum; i < MAX_HT_INDEX_NUM; i++)			//清除以前定义的跳格
	{
		g_tTab.TabPositionIndex[i] = 0;
	}
}

/*******************************************************************************
* Function Name  : Command_1B42
* Description    : 
1B 42 n t
1<=n<=9 1<=t<=9
打印机来单打印蜂鸣提示功能
n是蜂鸣器叫的次数
t是蜂鸣器每次鸣叫时间t*50ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B42(void)
{
	uint8_t	TempN;
	uint16_t	TempT;
	
	TempN = ReadInBuffer();
	TempT = ReadInBuffer();
	
	if(TempN >0 && TempN <10 )
	{
		if(TempT >0 && TempT <10 )
		{
			 BuzzerRunFlag = 1;
       BuzzerNum = TempN;
       BuzzerTime = TempT*50;
		}	
	}	
	
}
/*******************************************************************************
* Function Name  : Command_1B45
* Description    : 选择/取消加粗模式,只有最低位有效
*	1B   45   n
*	当n的最低位为0时,取消加粗模式
*	当n的最低位为1时,选择加粗模式
*	n只有最低位有效
* ESC !同样可以选择/ 取消加粗模式,最后接收的命令有效
*	默认n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B45(void)
{
	g_tPrtModeChar.EmphaSizeMode = ReadInBuffer() & 0x01;
	g_tPrtModeHZ.EmphaSizeMode = g_tPrtModeChar.EmphaSizeMode;
}

/*******************************************************************************
* Function Name  : Command_1B47
* Description    : 选择/取消双重打印模式,只有最低位有效,打印效果同Command_1B45
*	1B   47   n
*	当n的最低位为0时,取消双重打印模式
*	当n的最低位为1时,选择双重打印模式
*	n只有最低位有效
* 该指令与加粗打印效果相同
*	默认n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B47(void)
{
	g_tPrtModeChar.EmphaSizeMode = ReadInBuffer() & 0x01;
	g_tPrtModeHZ.EmphaSizeMode = g_tPrtModeChar.EmphaSizeMode;
}

/*******************************************************************************
* Function Name  : Command_1B4A
* Description    : 打印并走纸
*	1B   4A   n
*	打印缓冲区数据并走纸 [ n  ×  纵向或横向移动单位]  英寸
*	打印结束后,将当前打印位置置于行首
*	走纸距离不受  ESC 2 或  ESC 3 指令设置的影响
*	横向和纵向移动单位由GS P设定
*	标准模式下,使用纵向移动单位
*	在页模式下,根据打印区域的方向和打印起始位置来选择使用纵向移动单位或横向移动单位,
*	其选择方式如下: 
*	① 当打印起始位置由ESC T 设置为打印区域的左上角或右下角时,使用纵向移动单位； 
*	② 当打印起始位置由ESC T 设置为打印区域的左下角或右上角时,使用横向移动单位； 
* 最大走纸距离是1016 mm (40 英寸).  如果超出这个距离,取最大距离
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4A(void)
{
	uint8_t	GetChar;
	uint16_t	temp;
	uint32_t	LineSpace;
	
	GetChar = ReadInBuffer();
	if(g_tPrtCtrlData.BMPBitNum > 0 )	//2016.08.04
	{
		if( GetChar >=24 )
		{
			GetChar -=24;
		}
		else
		{
			if( GetChar >= g_tPrtCtrlData.BMPBitNum )
			{
				GetChar -= g_tPrtCtrlData.BMPBitNum;
			}
			else
				GetChar	=0;
		}
	}
	g_tPrtCtrlData.BMPBitNum =0;	
	
	LineSpace = GetChar / (2* g_tPrtCtrlData.StepVerUnit);         //兼容爱普生
	
	if (LineSpace > MAX_LINE_SPACE_VERTICAL)	//最长走纸40英吋 1016mm
		LineSpace = MAX_LINE_SPACE_VERTICAL;		//修改,还需要添加页模式时的处理
	
	//等待行打印缓冲区打印结束,然后将编辑行打印缓冲区的内容复制到打印行打印缓冲区,
	//然后启动打印,清空编辑缓冲区及变量,再开始编辑.
	//查询行打印缓冲区状态,一直等到有空闲行缓冲区可以进行打印
	//WaitPreviousPrintFinish();
	
	temp = g_tPrtCtrlData.CodeLineHigh;		//当前打印行间距
//	g_tPrtCtrlData.CodeLineHigh = g_tPrtCtrlData.LineHigh + LineSpace;	//本指令要求走行间距
  g_tPrtCtrlData.CodeLineHigh =  LineSpace;	//本指令要求走行间距 兼容爱普生
// 	PrintOneLine();												//打印该编辑缓冲区
	PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	g_tPrtCtrlData.CodeLineHigh = temp;		//恢复原打印行间距
}

/*******************************************************************************
* Function Name  : Command_1B4B
* Description    : 打印并反向走纸
*	1B   4B   n
*	打印缓冲区数据并反向走纸 [ n  ×  纵向或横向移动单位]  英寸
*	打印结束后,将当前打印位置置于行首
*	走纸距离不受  ESC 2 或  ESC 3 指令设置的影响
*	横向和纵向移动单位由GS P设定
*	标准模式下,使用纵向移动单位
*	在页模式下,根据打印区域的方向和打印起始位置来选择使用纵向移动单位或横向移动单位,
*	其选择方式如下: 
*	① 当打印起始位置由ESC T 设置为打印区域的左上角或右下角时,使用纵向移动单位； 
*	② 当打印起始位置由ESC T 设置为打印区域的左下角或右上角时,使用横向移动单位； 
* 最大走纸距离是机器所能允许的值.  如果超出这个距离,取最大距离
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4B(void)
{
#if	0
	uint8_t	GetChar;
	uint16_t	temp;
	uint32_t LineSpace;
	
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON命令集,打印并反向走纸
	{
		GetChar = ReadInBuffer();
		LineSpace = GetChar * g_tPrtCtrlData.StepVerUnit;
		
		if (LineSpace > FEED_BACK_MAX_STEPS)	//最长走纸,反向进纸最大值
			LineSpace = FEED_BACK_MAX_STEPS;		//还需要添加页模式时的处理
		
		//等待行打印缓冲区打印结束,然后将编辑行打印缓冲区的内容复制到打印行打印缓冲区,
		//然后启动打印,清空编辑缓冲区及变量,再开始编辑.
		
		//查询行打印缓冲区状态,一直等到有空闲行缓冲区可以进行打印
		WaitPreviousPrintFinish();
		
		temp = g_tPrtCtrlData.CodeLineHigh;				//当前打印行间距
		g_tPrtCtrlData.CodeLineHigh = 0;					//本指令要求走行间距
// 		PrintOneLine();														//打印该编辑缓冲区
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		g_tPrtCtrlData.CodeLineHigh = temp;				//恢复原打印行间距
		FeedPaper(FEED_BACKWARD, LineSpace);			//反向走纸
		DelayUs(1000 * 1000);											//等待走纸完成
	}
	else		//微打指令集,图形打印
	{
		Command_1B13();
	}
#endif

	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	Line;
	uint8_t	Width;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	i;
	uint16_t	RevNumber;
	uint16_t	iMaxNumber;
	uint8_t	* p = NULL;
	uint8_t	* pstart = NULL;
	uint8_t	* ptemp = NULL;
	
	GetChar = ReadInBuffer();
	iMaxNumber = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);	//数据长度
	if (!iMaxNumber)	//如果数据字节数为0直接退出
		return;
	
	if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))
	{
		RatioX = 2;
	}
	else
	{
		RatioX = g_tPrtModeChar.WidthMagnification;	//确定横向放大倍数,包括行内倍宽
	}
	RatioY = g_tPrtModeChar.HeightMagnification;	//确定纵向放大倍数
	
	RevNumber = iMaxNumber;
	if ((RevNumber * RatioX) > PRT_DOT_NUMBERS)	//确定最大计算接收字节长度
	{
		RevNumber = g_tPrtCtrlData.PrtLength / RatioX;
	}
	
	//在编辑行打印缓冲区之前需要先检查其是否空闲,如果已满等待打印,则需要等待行打印缓冲区打印结束,
	//然后将编辑行打印缓冲区的内容复制到打印行打印缓冲区,然后启动打印,清空编辑缓冲区及变量,再开始编辑.
	//如果编辑行打印空闲,编辑过程中满,也需要进行以上处理
	if (g_bLineEditBufStatus == LEB_STATUS_WAIT_TO_PRINT)	//编辑行打印缓冲区等待打印
	{
// 		PrintOneLine();		//先打印该编辑缓冲区
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	
	g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_QT;	//将其作为驱动模式实现
	
	//先设置打印控制变量及指针初始化
	if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
	{
		g_tPrtCtrlData.BitImageFlag = 1; 	//可变位图打印标志
		g_tPrtCtrlData.LineHigh = 8;			//修改行高
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//打印行高
	}
	ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
	
	for (i = 0; i < RevNumber; i++)		//逐个接收位图数据
	{
		//不满一行则放入行打印缓冲区
		if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) < g_tPrtCtrlData.PrtLength)
		{
			GetChar = ReadInBuffer();		//接收一个位图数据
			if (GetChar)
			{
				if (RatioY > 1)
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
					ByteZoomCtrl(RatioY, GetChar, TempBuf);		//纵向放大
				}
				else
				{
					TempBuf[0] = GetChar;
				}
				//放入打印缓冲区,从位图放大后的高位MSB开始放起,因为每次处理一个位图字节,
				//所以填充时数据字节数在高度上=RatioY,在宽度上=RatioX
				//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
				pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
				for (Line = 0; Line < RatioY; Line++)
				{	//行打印缓冲区起始 + 第几字节行起始 + 当前设置列
					p = pstart;
					for (Width = 0; Width < RatioX; Width++)	//横向放大倍数
					{
						*(p + Width) = TempBuf[Line];	//水平方向上填充
					}
					pstart += PRT_DOT_NUMBERS;
				}		//一个位图字节横向纵向都填充完毕
			}
			g_tPrtCtrlData.PrtDataDotIndex += RatioX;
		}
		else
		{
// 			PrintOneLine();	//打印该编辑缓冲区
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
			i--;						//如果中途打印一行,计数值相应要减少
			
			//重新设置打印控制变量及指针初始化
			if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
			{
				g_tPrtCtrlData.BitImageFlag = 1; 	//可变位图打印标志
				g_tPrtCtrlData.LineHigh = 8;			//修改行高
				g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//打印行高
			}
			ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
		}
	}		//End 位图宽度
		
	for (; i < iMaxNumber; i++)
		ReadInBuffer();		//接收多余的位图数据
}

/*******************************************************************************
* Function Name  : Command_1B4C
* Description    : 选择页模式
*	从标准模式转换到页模式,该命令只有在标准模式且行首才有效
*	在页模式下,该指令无效
*	当执行FF  或  ESC S后,打印机返回到标准模式
*	该指令将打印位置设置到ESC T 命令和ESC W命令确定的位置上
*	该指令将下列指令的设置转换到页模式下的值:
*	① 设置字符右间距:ESC SP, FS S 
*	② 设置行间距:ESC 2, ESC 3 
*	以下指令在页模式下只改变标志位,转换到标准模式后起作用.
*	① 顺时针旋转90° :ESC V 
*	② 选择字符对齐模式:ESC a 
*	③ 选择倒置模式:ESC { 
*	④ 设置左空白:GS L 
*	⑤ 设置打印区域宽度:GS W 
*	以下指令在页模式下被忽略. 
*	① 执行测试打?.篏S ( A 
*	以下指令在页模式下是不可用
*	① 打印 NV 位图:FS p 
*	② 定义 NV 位图:FS q 
*	③ 写用户 NV 存贮器:FS g 1 
*	④ 打印光栅位图:GS v 0 
*	当电源打开,打印机复位,或执行ESC @  指令,打印机回到标准模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4C(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE && g_tPrtCtrlData.PrtDataDotIndex == 0)
	{
		g_tSystemCtrl.PrtModeFlag = PAGE_MODE;
	}
}

/*******************************************************************************
* Function Name  : Command_1B4D
* Description    : 选择字体
*	1B   4D   n
*	n = 0, 1,48, 49
*		n  							功能 
*	0,48  选择标准ASCII字体 (12 × 24)  
*	1,49  选择压缩ASCII字体 (9 × 17) 
*	默认n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4D(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar == 0) || (GetChar == 48))	//使用12x24的ASCII字符
	{
 		g_tSysConfig.SysFontSize = ASCII_FONT_A;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh	= ASCII_FONT_A_HEIGHT;
	}
	else if ((GetChar == 1) || (GetChar == 49))	//使用9x17的ASCII字符(标准模式和页模式同时有效)
	{
		g_tSysConfig.SysFontSize = ASCII_FONT_B;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_B_WIDTH;
		g_tPrtModeChar.CharHigh	= ASCII_FONT_B_HEIGHT;
	}
}

/*******************************************************************************
* Function Name  : Command_1B52
* Description    : 选择国际字符集
*	1B   52   n
*	0  ≤ n  ≤   15	
*	从下表选择国际字符集n 
*	n  Character  Set 
*	0		U.S.A. 
*	1		France 
*	2		Germany 
*	3		U.K. 
*	4		DenmarkⅠ 
*	5		Sweden 
*	6		Italy 
*	7		Spain Ⅰ 
*	8		Japan 
*	9		Norway 
*	10	Denmark Ⅱ 
*	11	Spain Ⅱ 
*	12	Latin 
*	13	Korea  
*	14	Slovenia/Croatia 
*	15	China
*	默认 n = 15  ［简体中文］；n = 0 ［除简体中文外的其他型号］
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B52(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if (GetChar <= 15)
	{
		g_tSystemCtrl.CharacterSet = GetChar;
	}
}

/*******************************************************************************
* Function Name  : Command_1B53
* Description    : 选择标准模式
*	设置标准模式
*	该指令在页模式下有效. 
*	该指令清除页缓冲区打印数据. 
*	该指令将当前位置置于行首. 
*	页模式区域被初始化为默认值. 
*	该指令将下列指令的设置转换到标准模式下的值:
*	① 设置右间距:ESC SP, FS S 
*	② 选择行间距:ESC 2, ESC 3 
*	下列命令在标准模式下可以设定相关参数但其设定值进入页模式后才起作用: 
*	① 设置页模式下打印区域:  ESC W 
*	② 设置页模式下区域方向:  ESC T 
*	以下指令在标准模式下被忽略: 
*	① 页模式下设置纵向绝对打印位置:  GS $ 
*	② 页模式下设置相对打印位置:GS \
*	打印机电源打开,硬件复位或执行ESC @  指令后,打印机回到标准模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B53(void)
{
	//该处程序还需要进一步修改
	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)
	{
		g_tSystemCtrl.PrtModeFlag = STD_MODE;
	}
}

/*******************************************************************************
* Function Name  : Command_1B54
* Description    : 在页模式下选择打印区域方向
*	1B   54   n
*	0  ≤   n  ≤  3    48  ≤   n  ≤  51
*	在页模式下选择打印区域的方向和起始位置
*		n  	打印方向  起始位置 
*	0,48  由左到右  左上(图中A )
*	1,49  由下到上  左下(图中B )
*	2,50  由右到左  右下(图中C)
*	3,51  由上到下  右上(图中D)
*	如果当前模式为标准模式,只设置内部标志位,不影响打印
*	该指令可以设置打印内容在打印区域中的起始位置
*	根据区域打印起始位置的不同,横向或纵向移动单位的使用也不相同: 
*	① 如果打印起始位置为打印区域的左上角或右下角,那么打印内容排列方向与打印进纸方向垂直. 
*  下列命令使用横向移动单位:ESC SP, ESC $, ESC \
*  下列命令使用纵向移动单位:ESC 3, ESC J, GS $, GS \
*	② 如果打印机起始位置为打印区域的左下角或右上角,则打印内容排列方向与打印机进纸方向相同. 
*  下列命令使用横向移动单位:ESC 3, ESC J, GS $, GS \
*下列命令使用纵向移动单位:ESC SP, ESC $, ESC \
*	[ 默认值]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//还缺少该指令处理
void Command_1B54(void)
{
//	uint8_t	GetChar;
	
//	GetChar = ReadInBuffer();
	ReadInBuffer();
}

/*******************************************************************************
* Function Name  : Command_1B55
* Description    : 设置字符横向放大(ASCII字符)放大倍数,但是HRI字符除外.
*	POS指令集中作为双向打印.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B55(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON	命令集,选择/取消双向打印
	{
		
	}
	else  //微打命令集,横向放大
	{
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
		{
			return;
		}
		else
		{
			g_tPrtModeChar.WidthMagnification = GetChar;		//字符横向倍数
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B56
* Description    : 选择/取消顺时针旋转90度,该命令只在标准模式下有效
*	1B   56   n
*	0  ≤   n  ≤   2, 48 ≤   n  ≤   50
*		n  			功能
*	0,48 取消顺时针旋转90度模式 
*	1,49	选择顺时针旋转90度模式 
*	2,50	选择顺时针旋转90度模式 
*	当选择下划线模式时,下划线不能顺时针旋转90度
*	顺时针旋转90度模式下的倍高和倍宽与正常模式下的方向相反
*	[ 默认值]    n = 0
* 1B 56 n			微打中作为设置字符高度放大n倍,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B56(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON	命令集,选择/取消顺时针旋转90度
	{
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//该命令只在标准模式下有效
		{
			if ((GetChar == 0) || (GetChar == 1) || (GetChar == 48) || (GetChar == 49))
			{
				g_tPrtCtrlData.CharCycle90 = GetChar & 0x01;
			}
			else if ((GetChar == 2) || (GetChar == 50))		//修改,添加2,50的情况
			{
				g_tPrtCtrlData.CharCycle90 = 0x01;
			}
		}
	}
	else  //微打命令集,纵向放大
	{
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
		{
			return;
		}
		else
		{
			g_tPrtModeChar.HeightMagnification = GetChar;		//字符纵向倍数
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B57
* Description    : 设置字符(ASCII字符)纵向横向放大倍数,但是HRI字符除外.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B57(void)
{
	uint8_t	GetChar;
	uint32_t	i;
	
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON命令集,页模式下设置打印区域
	{
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//设置纵向横向放大倍数
		{
			GetChar = ReadInBuffer();
			GetChar &= 0x0F;
			if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X) || (GetChar > MAX_RATIO_Y))
			{
				return;
			}
			else
			{
				g_tPrtModeChar.WidthMagnification	= GetChar;	//字符横向倍数
				g_tPrtModeChar.HeightMagnification = GetChar;	//字符纵向向倍数
			}
		}
		else if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)	//页模式下设置打印区域
		{
			for (i = 0; i < 8; i++)
				GetChar = ReadInBuffer();	//暂时只读取不处理
		}
	}
	else  //微打命令集,纵向横向放大
	{
		GetChar = ReadInBuffer();
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X) || (GetChar > MAX_RATIO_Y))
		{
			return;
		}
		else
		{
			g_tPrtModeChar.WidthMagnification	= GetChar;	//字符横向倍数
			g_tPrtModeChar.HeightMagnification = GetChar;	//字符纵向倍数
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B58
* Description    : 设置字符(ASCII字符)纵向横向放大倍数,
*	1B 58 n1 n2	微打中作为设置字符横向放大n1倍,纵向放大n2倍,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B58(void)
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
		g_tPrtModeChar.WidthMagnification	= GetChar;	//字符横向倍数
	}
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
	{
		;
	}
	else
	{
		g_tPrtModeChar.HeightMagnification = GetChar;	//字符纵向倍数
	}
}

/*******************************************************************************
* Function Name  : Command_1B5C
* Description    : 设置相对横向打印位置
*	1B    5C   nL   nH
*	0  ≤   nl ≤   255     0 ≤   nH ≤  255
*	以横向或纵向移动单位设置横向相对位移
*	该指令将打印位置设置到距当前位置[( nL + nH  ×  256) × 横向或纵向移动单位]处
*	超出可打印区域的设置将被忽略
*	当打印位置向右移动时:nL+ nH ×  256 = N
*	当打印位置向左移动时采用补码:nL+ nH ×  256 = 65536 – N
*	打印起始位置从当前位置移动到[ N  ×  横向移动单位或者纵向移动单位] 
*	打印起始位置从当前位置移动到[ N  ×  横向移动单位或者纵向移动单位] 
*	在标准模式下,使用横向移动单位
*	在页模式下,根据打印区域的方向和起始位置来选择使用横向移动单位或纵向移动单位,
*	其选择方式如下: 
*	① 当打印起始位置由ESC T 设置为打印区域的左上角或右下角时,使用横向移动单位； 
*	② 当打印起始位置由ESC T 设置为打印区域的左下角或右上角时,使用纵向移动单位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B5C(void)
{
	uint8_t	GetChar;
	uint16_t	Temp, Temp2;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		GetChar = ReadInBuffer();
		//Temp = GetChar + 256 * ReadInBuffer();
		Temp = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);
		Temp *= g_tPrtCtrlData.StepHoriUnit;
		//该部分程序还需要根据打印方向修改设置值
		Temp2 = Temp + g_tPrtCtrlData.PrtDataDotIndex;
		if (Temp2 < g_tSystemCtrl.LineWidth)		//修改
			g_tPrtCtrlData.PrtDataDotIndex = Temp2;
	}
	else	//还需要添加页模式的代码
	{
	}
}

/*******************************************************************************
* Function Name  : Command_1B61
* Description    : 设置字符对齐模式
*	1B    61   n
*	0  ≤   n  ≤   2, 48 ≤   n  ≤   50
*	使所有的打印数据按某一指定对齐方式排列
*		n			对齐方式 
*	0, 48		左对齐 
*	1, 49		中间对齐 
*	2, 50		右对齐
*	该指令只在标准模式下的行首有效
*	该指令在页模式下只改变内部标志位
*	该指令根据HT, ESC $ 或  ESC \ 指令来调整空白区域
*	[ 默认值]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B61(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
		{
			if ((GetChar == 0) || (GetChar == 1) || (GetChar == 2) || \
					(GetChar == 48) || (GetChar == 49) || (GetChar == 50))
			{
				g_tPrtCtrlData.CodeAimMode = GetChar & 0x03;	//只保留最后两位的值
			}
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B63
* Description    : 选择传感器(按键,纸尽,纸将尽控制方式),兼容微打的打印方向控制指令
[ 名称]     选择纸张类型 
[ 格式]   ASCII   ESC  c  0  n 
Hex   1B  63  30  n 
Decimal  27   99  40  n 
[ 范围]     0  ≤   n  ≤   2 
[ 描述]    选择纸张类型 
   n = 0, 设置纸张类型为连续纸 
   n = 1, 2, 设置纸张类型为标记纸 
[ 注释]   .  标记纸是指印有黑白标记的打印纸 
   . 当纸张类型设置为标记纸时,请不要使用连续纸,否则执行GS FF 命令会导致打 
    印机走纸很长距离,当纸张类型设置为连续纸时,请不要使用标记纸,否则打印机 
        会误报缺纸 
[ 默认值]    n = 0 
[ 参考]     GS FF 

*	1B   63  33  n 
*	选择纸传感器来输出并行接口的缺纸信号
*	位	0/1		Hex		Decimal		功能 
*	0		0			00		0					纸将尽传感器无效
*			1			01		1					纸将尽传感器有效
*	1		0			00		0					纸将尽传感器无效
*			1			02		2					纸将尽传感器有效
*	2		0			00		0					纸尽传感器无效
*			1			04		4					纸尽传感器有效
*	3		0			00		0					纸尽传感器无效
*			1			08		8					纸尽传感器有效
*	4-7												未定义
*	该指令可以选择两个传感器输出缺纸信号,如果其中任何一个被选择的传感器检测
*	到缺纸时则输出缺纸信号.
*	该指令只有在接口方式为并口时有效,当接口为串口时无效
*	如果将 n  的第0位或第1位设为1,那么纸将尽传感器用来输出缺纸信号
*	如果将 n  的第2位或第3位设为1,那么纸尽传感器用来输出缺纸信号
*	当两个传感器都被禁止时,并行接口的缺纸信号总是输出有纸状态
*	[ 默认值]    n = 15 
*
*	1B   63   34   n
*	选择纸传感器以终止打印
*	位	0/1		Hex		Decimal		功能 
*	0		0			00		0					纸将尽传感器无效
*			1			01		1					纸将尽传感器有效
*	1		0			00		0					纸将尽传感器无效
*			1			02		2					纸将尽传感器有效
*	2-7												未定义
*	当 n  的第0位或第1 位为1 时,纸将尽传感器有效,当检测到纸将尽时,打印完当 
*	前任务后停止打印,打印机进入离线状态.
*	[ 默认值]    n = 0 
*	
*	1B   63   35   n
*	允许/ 禁止按键
*	当 n  的最低位为0 时,按键起作用. 
* 当 n  的最低位为1 时,按键被禁止
*	只有 n  的最低位有效
*	在执行宏命令时,按键总是可用的,但不能通过按键来走纸
*	[ 默认值]    n = 0
*	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B63(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	switch (GetChar)
	{
		case 0x00:			//选择正向打印,兼容T3
		{
			g_tPrtCtrlData.LineDirection = GetChar;
			break;
		}
		case 0x01:			//选择反向打印,兼容T3
		{
			g_tPrtCtrlData.LineDirection = GetChar;
			break;
		}
		case 0x30:			//选择纸张类型
		{
			GetChar = ReadInBuffer();
			GetChar &= 0x03;
			if (GetChar == 0x00)	//纸类型,0 = 连续纸 1 = 标记纸
				g_tSystemCtrl.PaperType = PAPER_TYPE_CONTINUOUS;
			else
				g_tSystemCtrl.PaperType = PAPER_TYPE_BLACKMARK;
			break;
		}
		case 0x33:			//选择纸传感器输出纸尽信号,只对并口有效
		{
			GetChar = ReadInBuffer();
			if (g_tInterface.LPT_Type)				//并口
			{
				if (GetChar & 0x03)
					g_tError.LPTPaperEndSensorSelect |= 0x01;	//纸将尽传感器有效
				else
					g_tError.LPTPaperEndSensorSelect &= (uint8_t)(~0x01);
				if (GetChar & 0x0C)
					g_tError.LPTPaperEndSensorSelect |= 0x02;	//纸尽传感器有效
				else
					g_tError.LPTPaperEndSensorSelect &= (uint8_t)(~0x02);
			}
			break;
		}
		case 0x34:			//选择纸传感器以终止打印
		{
			GetChar = ReadInBuffer();
			if (GetChar & 0x03)
				g_tError.PaperEndStopSensorSelect |= 0x01;		//纸将尽传感器有效
			else
				g_tError.PaperEndStopSensorSelect &= (uint8_t)(~0x01);
			#if	0																//本种机型缺纸传感器一直有效
			if (GetChar & 0x0C)
				g_tError.PaperEndStopSensorSelect |= 0x02;		//纸尽传感器有效
			else
				g_tError.PaperEndStopSensorSelect &= (uint8_t)(~0x02);
			#endif
			break;
		}
		case 0x35:			//允许/禁止按键,只有最低位有效,当n的最低位为0时按键起作用
		{
			g_tFeedButton.KeyEnable = ReadInBuffer() & 0x01;
			break;
		}
		case 0x39:			//兼容以前驱动
		{
			GetChar = ReadInBuffer();
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : Command_1B64
* Description    : 打印并走纸n行
*	1B   64   n
*	打印缓冲区里的数据并向前走纸n行(字符行)
*	该指令将打印机的打印起始位置设置在行首
*	该指令不影响由ESC 2 或  ESC 3设置的行间距
*	最大走纸距离为1016 mm,当所设的值大于1016 mm时,取最大值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B64(void)
{
	uint8_t	GetChar;
	uint16_t LineSpace;
	
	GetChar = ReadInBuffer();
    if(GetChar>0)
    {
        GetChar =GetChar-1;                     //兼容爱普生
    }
 	LineSpace = GetChar * g_tPrtCtrlData.CodeLineHigh;      
	if (LineSpace > MAX_LINE_SPACE_VERTICAL)	//最长走纸40英吋 1016mm
		LineSpace = MAX_LINE_SPACE_VERTICAL;		//修改,还需要添加页模式时的处理
	//等待行打印缓冲区打印结束,然后将编辑行打印缓冲区的内容复制到打印行打印缓冲区,
	//然后启动打印,清空编辑缓冲区及变量,再开始编辑.
	
	//查询行打印缓冲区状态,一直等到有空闲行缓冲区可以进行打印
	//WaitPreviousPrintFinish();
// 	PrintOneLine();								//打印该编辑缓冲区
	PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	GoDotLine((uint32_t)(LineSpace));
}

/*******************************************************************************
* Function Name  : Command_1B66
* Description    : 打印并走纸n个空格或者空行
*	1B	66	m	n
*	向前走纸n个空格或者空行(字符行)
* 如果m=0,将打印n个空格。n的值应该在打印机的行宽之内。 
*	如果m=1,将打印n个空行。n的值应该在0~255之间。
*	该指令不影响由ESC 2 或  ESC 3设置的行间距
*	最大走纸距离为1016 mm,当所设的值大于1016 mm时,取最大值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B66(void)
{
	uint8_t	bMode;
	uint8_t	bValue;
	uint32_t LineSpace;
	
	bMode = ReadInBuffer();
	if (0 == bMode)	/* 走n个空格 */
	{
		bValue = ReadInBuffer();
		if ((g_tPrtCtrlData.PrtDataDotIndex + bValue) < g_tPrtCtrlData.PrtLength)
		{
			g_tPrtCtrlData.PrtDataDotIndex += bValue;
		}
		else
		{
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		}
	}
	else if (1 == bMode)	/* 走n个空行 */
	{
		bValue = ReadInBuffer();
		LineSpace = bValue * g_tPrtCtrlData.CodeLineHigh;
		if (LineSpace > MAX_LINE_SPACE_VERTICAL)	//最长走纸40英吋 1016mm
			LineSpace = MAX_LINE_SPACE_VERTICAL;
		GoDotLine(LineSpace);
	}
}

/*******************************************************************************
* Function Name  : Command_1B69
* Description    : ESC/POS指令集中为切纸,
[ 描述]   执行一次半切纸,不走纸. 
[ 注释]   打印机进行半切纸. 
微打指令集中为允许/禁止反白打印,只有最低位有效,兼容微打指令
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B69(void)
{
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)	//EPSON	命令集,切纸
	{	//切刀指令只有在行首才有效
		if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
			return;
		DriveCutPaper(CUT_TYPE_PART);
	  #ifdef	PAPER_OUT_RE_PRINT_ENABLE
		  if (g_tSysConfig.PaperOutReprint)
			  ClearInBuf();				//清已经打印完成的单据数据
	  #endif
	}
	else
	{
		g_tPrtCtrlData.CharNotFlag = ReadInBuffer() & 0x01;
	}
}


#ifdef	WH_SELFDEF_CUT_CMD_ENABLE
/*******************************************************************************
* Function Name  : Command_1B6B
* Description    : 炜煌公司自定义指令,切纸
*	1B		6B		n
*	n = 0, 1, 48, 49 
*		n			动作
*		0			全切
*		1			半切
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1B6B(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	switch (GetChar)
	{
		case 0x00:			 						//全切纸方式
		{	//切刀指令只有在行首才有效
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_FULL);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
			#endif
			break;
		}
		case 0x01:			 						//半切纸方式
		{	//切刀指令只有在行首才有效
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_PART);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
			#endif
			break;
		}
		default:
			break;
	}
}
#endif

/*******************************************************************************
* Function Name	: Command_1B6C
* Description		: 设置左边距,在标准模式时,只在行首有效,
1B 6C	n			微打中作为设置左边距,
左边距设置为	[n × ASCII字符宽度]
·	该命令只有在行首才有效. 
·	如果设置超出了最大可用打印宽度,则取最大可用打印宽度 
[ 默认值]		n = 0
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1B6C(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	uint16_t	MiniCharWidth;
	
	GetChar = ReadInBuffer();
	if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//行首
	{
		Temp = GetChar * (g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth);
		if (Temp > g_tSystemCtrl.LineWidth)	//如果超过最大行宽,则设置为最大行宽
		{
			Temp = g_tSystemCtrl.LineWidth;
		}
		
		//如果不超出最大可打印区域但是不足一个字符则设置为能放下一个字符
		if (g_tSystemCtrl.CharacterSet == INTERNATIONAL_CHAR_SET_INDEX_ENGLISH)	//英文模式
		{
			MiniCharWidth = g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth;	//最小字符宽度
		}
		else	//中文模式
		{
			MiniCharWidth = g_tPrtModeHZ.LeftSpace + g_tPrtModeHZ.CharWidth;	//最小字符宽度
		}
		if (MiniCharWidth > g_tSystemCtrl.LineWidth)	//确保参数合法
		{
			MiniCharWidth = g_tSystemCtrl.LineWidth;
		}
		if (g_tPrtCtrlData.PrtWidth < MiniCharWidth)
		{
			g_tPrtCtrlData.PrtWidth = MiniCharWidth;	//调整打印区域确保能放下一个字符
		}
		
		//左边距与打印区域超过最大可打印区域,需要调整打印区域或者左边距,否则无需调整二者
		if ((Temp + g_tPrtCtrlData.PrtWidth) > g_tSystemCtrl.LineWidth)
		{
			g_tPrtCtrlData.PrtWidth = g_tSystemCtrl.LineWidth - Temp;	//调整打印区域
			if (g_tPrtCtrlData.PrtWidth < MiniCharWidth)
			{
				g_tPrtCtrlData.PrtWidth = MiniCharWidth;	//调整打印区域确保能放下一个字符
			}
			Temp = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtWidth;	//调整左边距
		}
		
		g_tPrtCtrlData.PrtLeftLimit = Temp;
		g_tPrtCtrlData.PrtDataDotIndex = Temp;
		g_tPrtCtrlData.PrtLength = Temp + g_tPrtCtrlData.PrtWidth;
	}
}

/*******************************************************************************
* Function Name 	: void DriveDrawer (u8 DrwID, u8 Status )
* Description		: 驱动钱箱函数， 2016.06.18
* Input				: DrwID:钱箱号
				  	    Status：为0表示关闭钱箱，1打开钱箱
* Output			: None
* Return			: None
*******************************************************************************/
void DriveDrawer(uint8_t DrwID, uint8_t Status)
{
	
	if (Status == DISABLE)			//停止钱箱
	{
		if (DrwID == 0)
		{
			MBOX_CTRL1_CLR;			//关钱箱1控制
		}
		else
		{
			MBOX_CTRL2_CLR;			//关钱箱2控制
		}
	}
	else
	{
		if (DrwID == 0)
		{
			MBOX_CTRL1_SET;			//开钱箱1控制
		}
		else
		{
			MBOX_CTRL2_SET;			//开钱箱2控制
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B70
* Description    : 钱箱控制指令,产生脉冲,
*	1B   70   m   t1   t2
*	m = 0, 1, 48, 49 
*	0  ≤   t1 ≤   255, 0  ≤   t2 ≤  255
*	输出由t1 和t2 设定的钱箱开启脉冲到由m 指定的引脚
*		m			连接引脚 
*	0, 48		钱箱插座的引脚 2 
*	1, 49		钱箱插座的引脚 5 
*	钱箱开启脉冲开的时间为 [ t1 ×  2 ms]  ,钱箱开启脉冲关的时间为 [ t2 ×  2 ms]
*	如果t2 < t1,关的时间为 [ t1 ×  2 ms]
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B70(void)
{
	uint8_t	Number;
	uint16_t OpenTime;
	
	Number = ReadInBuffer();

	if ((Number == 0) || (Number == '0'))	//'0'=48,'1'=49
	{
		g_tDrawer1.Status = ENABLE;				  //为了防止在进行钱箱开启时间内 防止实时指令101401n 的影响
		while (g_tDrawer1.ON_Time);					//等待上次执行完成
		
		g_tDrawer1.DrwID = Number & 0x01;
		OpenTime		= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		g_tDrawer1.OFF_Time	= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		if (g_tDrawer1.OFF_Time < OpenTime)
		{
			g_tDrawer1.OFF_Time = OpenTime;
		}
		OpenTime += g_tDrawer1.OFF_Time;	//总控制时间
		if(OpenTime)
		{	
			DriveDrawer(g_tDrawer1.DrwID, ENABLE);			//开钱箱
		}	
		g_tDrawer1.ON_Time = OpenTime;
		g_tDrawer1.Status = DISABLE;								//在定时器中实现定时和关闭钱箱
	}
	else if((Number == 1) || (Number == '1'))
	{
		g_tDrawer2.Status = ENABLE;				  //为了防止在进行钱箱开启时间设置   防止实时指令101401n 
		while (g_tDrawer2.ON_Time);					//等待上次执行完成
		
		g_tDrawer2.DrwID = Number & 0x01;
		OpenTime		= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		g_tDrawer2.OFF_Time	= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		if (g_tDrawer2.OFF_Time < OpenTime)
		{
			g_tDrawer2.OFF_Time = OpenTime;
		}
		OpenTime += g_tDrawer2.OFF_Time;	//总控制时间
		if(OpenTime)
		{	
			DriveDrawer(g_tDrawer2.DrwID, ENABLE);		//开钱箱
		}	
		g_tDrawer2.ON_Time = OpenTime;
		g_tDrawer2.Status = DISABLE;								//在定时器中实现定时和关闭钱箱
	}	
}

/*******************************************************************************
* Function Name  : Command_1B72
* Description    : 选择打印颜色
*	1B   70   m
*	m = 0, 1, 48, 49 
*		m			连接引脚 
*	0, 48		黑色
*	1, 49		红色
* 默认0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B72(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
}

/*******************************************************************************
* Function Name  : Command_1B74
* Description    : 选择代码页
*	N  代码页  									Code Page 
*	0  CP437 [美国,欧洲标准]  CP437 [U. S.A., Standard  Europe] 
*	1  KataKana [片假名]  Katakana 
*	2  PC850 [多语言]  PC850 [Multilingual]  
*	3  PC860 [葡萄牙]  PC860 [Portuguese]  
*	4  PC863 [加拿大- 法语]  PC863 [Canadian-French] 
*	5  PC865 [北欧]  PC865 [Nordic]  
*	6  WCP1251 [斯拉夫语]  WCP1251 [Cyrillic] 
*	7  CP866  斯拉夫2  CP866 Cyrilliec #2   
*	8  MIK[斯拉夫/保加利亚]  MIK[Cyrillic  /Bulgarian ] 
*	9  CP755  [东欧,拉脱维亚 2] CP755 [East Europe,Latvian 2] 
*	10  [ 伊朗,波斯]  Iran   
*	11  保留  reserve 
*	12  保留  reserve 
*	13  保留  reserve 
*	14  保留  reserve 
*	15  CP862  [希伯来]  CP862  [Hebrew] 
*	16  ＷCP1252 [ 拉丁语 1]  ＷCP1252 Latin I 
*	17  WCP1253 [希腊]  WCP1253 [Greek]   
*	18  CP852 [拉丁语  2]  CP852 [Latina 2] 
*	19  CP858 [多种语言拉丁语 1+欧元符]  
*	CP858  Multilingual Latin Ⅰ+Euro) 
*	20  伊朗Ⅱ[ 波斯语]  Iran  II   
*	21  拉脱维亚  Latvian 
*	22  CP864 [阿拉伯语]  CP864  [Arabic] 
*	23  ISO-8859-1 [ 西欧]  ISO-8859-1 [West Europe]        
*	24  CP737 [希腊]  CP737  [Greek] 
*	25  WCP1257 [波罗的海]  WCP1257 [Baltic] 
*	26  [泰文1]  Thai 1  
*	27  CP720[阿拉伯语]  CP720[Arabic] 
*	28  CP855  CP855 
*	29  CP857[土耳其语]  CP857[Turkish] 
*	30  WCP1250[中欧]  WCP1250[Central Eurpoe] 
*	31  CP775  CP775 
*	32  WCP1254[土耳其语]  WCP1254 [Turkish] 
*	33  WCP1255[希伯来语]  WCP1255[Hebrew] 
*	34  WCP1256[阿拉伯语]  WCP1256[Arabic] 
*	35  WCP1258[越南语]  WCP1258[Vietnam] 
*	36  ISO-8859-2[拉丁语2]  ISO-8859-2[Latin 2 ]  
*	37  ISO-8859-3[拉丁语3]  ISO-8859-3[Latin 3 ]  
*	38  ISO-8859-4[波罗的语]  ISO-8859- 4[Baltic] 
*	39  ISO-8859-5[斯拉夫语]  ISO-8859-5[Cyrillic]  
*	40  ISO-8859-6[阿拉伯语]  ISO-8859-6[Arabic ]  
*	41  ISO-8859-7[希腊语]  ISO-8859-7[Greek]  
*	42  ISO-8859-8[希伯来语]  ISO-8859-8[Hebrew]  
*	43  ISO-8859-9[土耳其语]  ISO-8859-9[Turkish] 
*	44  ISO-8859-15[拉丁语9]  ISO-8859-15  [Latin 3] 
*	45  [泰文2]  Thai2 
*	46  CP856  CP856 
*	[ 默认值]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B74(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar < MAX_CODEPAGE_SET_NUM)
		g_tSysConfig.DefaultCodePage = GetChar;
}

/*******************************************************************************
* Function Name  : Command_1B7B
* Description    : 设置/取消反向打印,只有最低位有效	,只在行首有效
*	当n 的最低位为0 时,取消倒置打印模式. 
* 当n 的最低位为1 时,选择倒置打印模式.
*	只有n的最低位有效； 
* 该指令只在标准模式下的行首有效. 
* 该指令在页模式下,只改变内部标志位. 
* 该指令对页模式打印无影响. 
* 在倒置打印模式下,打印机先将要打印的行旋转180度然后打印.
*	[ 默认值]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B7B(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//必须在行首
			g_tPrtCtrlData.LineDirection = ReadInBuffer() & 0x01;
	}
	else
	{
		ReadInBuffer();
	}
}

/*******************************************************************************
* Function Name  : Command_1B6D
* Description    : 执行半切,兼容EPSON废弃指令
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B6D(void)
{
	//切刀指令只有在行首才有效
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
		return;
	DriveCutPaper(CUT_TYPE_PART);
#ifdef	PAPER_OUT_RE_PRINT_ENABLE
	if (g_tSysConfig.PaperOutReprint)
		ClearInBuf();				//清已经打印完成的单据数据
#endif
}

/*******************************************************************************
* Function Name  : Command_1B75
* Description    : 返回外设状态(钱箱状态),为兼容EPSON废弃指令
*	该指令需要查找命令定义进行修改
*	
*	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B75(void)
{
	uint8_t	GetChar;
	uint8_t	Status;
	
	GetChar = ReadInBuffer();
	if (GetChar == 0 || GetChar == 48)
	{
		Status = READ_MONEYBOX_STATUS;	//钱箱接口3脚电平 2016.06.19
		Status &= 0x01;
		UpLoadData(&Status, 1);					//上传数据,只有最低位有效
	}
}

/*******************************************************************************
* Function Name  : Command_1B76
* Description    : 返回纸传感器状态,为兼容EPSON废弃指令
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B76(void)
{
	uint8_t	Status;
	
	if ((g_tRealTimeStatus.PrinterStatus & 0x08) == 0x08)		//离线状态下不回发
		return;
	
	Status = 0;
	if (g_tError.PaperSensorStatusFlag & 0x01)		//纸尽状态
		Status |= 0x0C;
	if (g_tError.PaperSensorStatusFlag & 0x02)		//纸将尽状态
		Status |= 0x03;
	
	UpLoadData(&Status, 1);					//上传数据
}


/*******************************************************************************
* Function Name  : PrintBarCode_2D
* Description    : 打印二维条码
* Input          : Buf:位图区首地址,二维码数据按照竖置横排的方式排列,
*                  X_Length,横向点数,Y_Length,纵向字节数,
*	               RatioX:横向放大倍数,RatioY:纵向放大倍数；
* Output         : None
* Return         : None
*******************************************************************************/
void PrintBarCode_2D(uint16_t X_Length, uint16_t Y_Length, uint8_t RatioX, uint8_t RatioY)
{
	uint8_t	 TempBuf[8];
	uint8_t	GetChar;
	uint8_t	ratiox;
	uint8_t	ratioy;
	uint16_t x;
	uint16_t y;
	uint8_t	* pbByteStart = NULL;
	uint8_t	* pbTemp = NULL;
	uint8_t	* pbBuf = NULL;
	uint8_t	* p = NULL;
	uint16_t  Index;
	s16	Lines;
  
	//在标准模式中定义了二维码图形数据在行首的情况
	
  if ((g_tSystemCtrl.PrtModeFlag == STD_MODE))			
	{
		Index = g_tPrtCtrlData.PrtDataDotIndex;
			
		for (y = 0; y < Y_Length; y++)		//纵向高度,以字节为单位
		{	//指向当前编辑数据缓冲区起始字节行
								//(Y_Length - y) * RatioY改为RatioY
			pbByteStart = g_tLineEditBuf + \
				(MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS + Index;   
				
			for (x = 0; x < X_Length; x++)	//横向宽度,以点为单位
			{
				pbTemp = pbByteStart + (x * RatioX);		//指向当前编辑数据缓冲区列
				if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)	//反向打印
				{
					GetChar = g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[ X_Length * (Y_Length - 1 - y) + x];
				}
				else		//按照正常打印顺序填充
				{
					GetChar = g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[ X_Length * y + x];
				}
				if (GetChar)
				{
					if (RatioY > 1)	//需要放大
					{
						memset(TempBuf, 0x00, 8);     //2017.08.25 MAX_RATIO_Y改为8
						ByteZoomCtrl(RatioY, GetChar, TempBuf);
					}
					else		//纵向放大倍数为1
					{
						TempBuf[0] = GetChar;
						//TempBuf[1] = GetChar;
					}
					for (ratiox = 0; ratiox < RatioX; ratiox++)
					{
						for (ratioy = 0; ratioy < RatioY; ratioy++)
						{
							*(pbTemp + ratiox + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
						}
					}	//一个字节填充完毕
				}
			}	//一行字节横向填充完毕
			g_tPrtCtrlData.PrtDataDotIndex = Index+(X_Length * RatioX);
			g_tPrtCtrlData.MaxPrtDotHigh = ((uint16_t)(RatioY) << 3);	//纵向总点数
			
			//送入行打印缓冲区
			pbBuf = g_tLineEditBuf + \
			       (MAX_PRT_HEIGHT_BYTE - RatioY) *  PRT_DOT_NUMBERS;  //字符在编辑缓冲区的首地址
					
			if( g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
			{										//反向打印
				for( Lines=g_tPrtCtrlData.MaxPrtDotHigh; Lines >0; )              //打印内容行
				{
					Lines--;
					p = pbBuf + (Lines>>3 )* PRT_DOT_NUMBERS;
					MovDotLine( p, Lines);	           
				}
			}
			else		 					 //正向打印
			{
				for( Lines=0; Lines < g_tPrtCtrlData.MaxPrtDotHigh; Lines++ )	  	     //打印内容行 内容在编辑缓冲区的底部，内容从上到下的点行，序号是0到最大行数
				{
					p = pbBuf +  (Lines>>3 )* PRT_DOT_NUMBERS ;													 //计算点行所在的字节行的位置
					MovDotLine( p,  Lines);	                 			              
				}	
			}
			PrepareNewLine();	    //清空g_tLineEditBuf及下划线等控制变量准备下一行编辑
			
		}	//纵向按列全部填充完毕
	}	//分成多次打印处理完成
	//参数有效处理完成
	//在行首且定义了下载位图处理完成
	
	pbByteStart = NULL;
	pbTemp = NULL;
     
}

/*******************************************************************************
* Function Name  : ProcessBarCode_2D
* Description    : 二维条码指令处理
* Input          : 
	Buf:位图区首地址,
	X_Length,横向字节数,Y_Length,纵向字节数,
	RatioX:横向放大倍数,RatioY:纵向放大倍数
* Output         : None
* Return         : None
*******************************************************************************/
void ProcessBarCode_2D(uint8_t CommandType)
{
	uint8_t	 GetChar;
	uint8_t	 version, vColl, vRateX, vRateY;
	uint16_t i, datalen ;
	uint32_t res;
	uint8_t  QR_ECCLevel[]={'L','M','Q','H'};			    //QR CODE纠错等级

	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =3;	//二维码打印标志，修改标志方式及值

	version = ReadInBuffer();					//参数V
	vColl = ReadInBuffer();						//参数r

	if( CommandType == 0x5A )                   //1B 5A指令 进入
	{											
		vRateY = ReadInBuffer();				//参数k
		
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType ==2)					//QR CODE 1D5A选择的二维码类型
		{
			for(i=0; i<4; i++)
			{
				if(vColl == QR_ECCLevel[i])		//查找QR CODE纠错等级
					break;
			}
			if(i<4)
				vColl = i+1;
			else
				vColl = 1;						//错误时自动设置为1

		}
	}
	else                                        //1D 6B指令进入
	{											
		vRateY = g_tBarCodeCtrl.BarCodeThinWidth ;
		if(CommandType ==32 || CommandType ==97 )
        {    
			g_tRamBuffer.BarCode2DStruct.BarCode2DType =2;
        }   
		else if(CommandType ==33 || CommandType ==98 )
        {    
			g_tRamBuffer.BarCode2DStruct.BarCode2DType =1;
        }    
		else if(CommandType ==34 || CommandType ==99 )
        {    
			g_tRamBuffer.BarCode2DStruct.BarCode2DType =0;
        }    
	}

	//放大倍数限制
	if(vRateY ==0)	
    {    
		vRateY =1;
    }    
	else if(vRateY > 8)         //更改放大倍数为8
    {    
		vRateY =8;
    }
	vRateX = vRateY;

	if(	CommandType >=32 && CommandType <=34) 
	{
		datalen=0;
		while(1)										//读取以00结束的条码数据 指令格式一
		{
			GetChar =  ReadInBuffer();
			if(GetChar ==0x00)
				break;
			g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[datalen] = GetChar;
			datalen++;									

		}
	}
	else                                            //指令格式二 接收参数个数
	{
		GetChar = ReadInBuffer();
		datalen = GetChar  + ReadInBuffer()*256;

		for (i = 0; i < datalen; i++)
        {
			g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[i] = ReadInBuffer();
        }    
	}

	if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)        //二维码 PDF417
	{
		res = Pdf417_Print( g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		vRateX =2;				//PDF417横向固定为2倍宽

	}
	else if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 1)   //二维码 Matrix码
	{
		res = Matrix_Print(g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		if(((res >> 16) * vRateX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex ))	//校验是否能放下该图形
			res =0;
	}
	else if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)   //二维码 QR码
	{
		res = QR_Print(g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		if(((res >> 16) * vRateX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex ))	//校验是否能放下该图形
			res =0;
	}
	if ((res) == 0x0000)
    {    
		PrintString("BarCode2D Data/Parameter Error\r\n");
    }    
	else
	{
		PrintBarCode_2D((res >> 16),(res & 0xffFF), vRateX, vRateY);
	}
	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =0;		//修改标志方式
}


/*******************************************************************************
* Function Name  : ProcessBarCode_2D_1D28
* Description    : 二维条码指令处理
* Input          : 
	Buf:位图区首地址,
	X_Length,横向字节数,Y_Length,纵向字节数,
	RatioX:横向放大倍数,RatioY:纵向放大倍数
* Output         : None
* Return         : None
*******************************************************************************/
void ProcessBarCode_2D_1D28(void)
{
	uint8_t	 GetChar;
	uint8_t	 version, vColl, vRateX, vRateY;
	uint16_t i, datalen ;
	uint32_t res;
	uint8_t  QR_ECCLevelNum[]={48,49,50,51};			      //QR CODE纠错等级 对应L M Q H

	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =3;	//二维码打印标志，修改标志方式及值

	if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)	       //PDF417
	{
			vRateY = PDF417_Rate;				                          //放大倍数
		  version = PDF417_version;
	}		
  else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)	  //QR Code
	{
			vRateY = QR_Rate;				                                //放大倍数
		  version = QR_version;																		//版本
	}		
		
	if(g_tRamBuffer.BarCode2DStruct.BarCode2DType ==2)					//QR CODE 1D5A选择的二维码类型
	{
		for(i=0; i<4; i++)
		{
			if(QR_vColl == QR_ECCLevelNum[i])		                  //查找QR CODE纠错等级
				break;
		}
		if(i<4)
			vColl = i+1;
		else
			vColl = 1;						//错误时自动设置为1
	}
	else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType ==0)		 //PDF417
	{
		vColl = PDF417_vColl;
	}	


	//放大倍数限制
	if(vRateY ==0)	
  {    
		vRateY =1;
  }    
	else if(vRateY > 8)            //更改放大倍数为8
  {    
		vRateY =8;
  }
	vRateX = vRateY;

  datalen = TwoCodeDataNum;   //条码数据长度
	
	if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)        //二维码 PDF417
	{
		res = Pdf417_Print( g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		vRateX =2;				//PDF417横向固定为2倍宽

	}
	else if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)   //二维码 QR码
	{
		res = QR_Print(g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		if(((res >> 16) * vRateX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex ))	//校验是否能放下该图形
			res =0;
	}
	if ((res) == 0x0000)
    {    
		PrintString("BarCode2D Data/Parameter Error\r\n");
    }    
	else
	{
		PrintBarCode_2D((res >> 16),(res & 0xffFF), vRateX, vRateY);
	}
	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =0;		//修改标志方式
}

/*******************************************************************************
* Function Name  : Command_1B5A
* Description    : 打印二维条码
*                  PDF417  DATA MATRIX   QR CODE
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B5A(void)
{
    if(g_bLineEditBufStatus == LEB_STATUS_ILDE)
    {    
        ProcessBarCode_2D(0x5A);
    }    
}

/*******************************************************************************
* Function Name  : EscCommand
* Description    : ESSC命令集的处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Command(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x09:
		{
			SetCommandFun();	//设置系统参数
			break;
		}
		case 0x0C:
		{
			Command_1B0C();	//页模式下打印命令(ESC FF )的处理
			break;
		}
		case 0x13:
		{
			Command_1B13();	//炜煌驱动打印,同1B4B
			break;
		}
		case 0x20:
		{
			Command_1B20();	//设置字符右间距
			break;
		}
		case 0x21:				//设置打印模式
		{
			Command_1B21();
			break;
		}
		case 0x24:			  //设置绝对打印位置
		{
			Command_1B24();
			break;
		}
		case 0x25:			  //选择/取消用户自定义字符模式
		{
			Command_1B25();
			break;
		}
		case 0x26:			  //定义自定义字符
		{
			Command_1B26();
			break;
		}
		case 0x2A:			  //打印位图
		{
			Command_1B2A();
			break;
		}
		case 0x2D:				//下划线打印
		{
			Command_1B2D();
			break;
		}
		case 0x31:			  //设置行间距
		{
			Command_1B31();
			break;
		}
		case 0x32:			  //设置默认行间距
		{
			Command_1B32();
			break;
		}
		case 0x33:			  //设置行间距
		{
			Command_1B33();
			break;
		}
		case 0x3D:			  //选择打印机
		{
			Command_1B3D();
			break;
		}
		case 0x3F:			  //清除自定义字符
		{
			Command_1B3F();
			break;
		}
		case 0x40:			  //初始化打印机
		{
			Command_1B40();
			break;
		}
		case 0x42:			  //2016.09.13 控制蜂鸣器
		{
			Command_1B42();
			break;
		}		
		case 0x44:			  //设置横向跳格位置
		{
			Command_1B44();
			break;
		}
		case 0x45:			  //选择/取消加粗模式
		{
			Command_1B45();
			break;
		}
		case 0x47:			  //选择/取消双重打印模式
		{						
			Command_1B47();
			break;
		}
		case 0x4A:			  //打印并走纸n点行
		{
			Command_1B4A();
			break;
		}
		case 0x4B:			  //ESCPOS打印并反向走纸n点行,微打指令集为图形打印
		{
			Command_1B4B();
			break;
		}
		case 0x4C:			  //选择页模式
		{
// 			Command_1B4C();   //2017.03.16
			break;
		}
		case 0x4D:			  //选择字体
		{
			Command_1B4D();
			break;
		}
		case 0x52:			  //选择国际字符
		{
			Command_1B52();
			break;
		}
		case 0x55:			  //POS指令集选择单双向打印模式,微打指令集设置横向放大倍数
		{
			Command_1B55();
			break;
		}
		case 0x56:			  //POS指令集选择顺时钟方向旋转90度打印模式,微打指令集设置纵向放大倍数
		{
			Command_1B56();
			break;
		}
		case 0x57:			  //POS指令集设置打印区域范围,微打指令集设置纵向横向放大倍数
		{
			Command_1B57();
			break;
		}
		case 0x58:			  //微打指令集设置纵向横向放大倍数
		{
			Command_1B58();
			break;
		}
		case 0x5A:			  //打印二维条码
		{
			Command_1B5A();
			break;
		}
		case 0x5C:			  //设置相对打印位置
		{
			Command_1B5C();
			break;
		}
		case 0x61:			  //选择字符对齐模式
		{
			Command_1B61();
			break;
		}
		case 0x63:			  //选择纸传感器终止打印
		{
			Command_1B63();
			break;
		}
		case 0x64:			  //打印并走纸n行
		{
			Command_1B64();
			break;
		}
		case 0x66:			  //打印并走纸n个空格或者空行
		{
			Command_1B66();
			break;
		}
		case 0x69:			  //POS指令集切纸,微打指令集允许/禁止反白打印
		{
			Command_1B69();
			break;
		}
		#ifdef	WH_SELFDEF_CUT_CMD_ENABLE
		case 0x6B:
		{
			Command_1B6B();	//炜煌驱动实现切纸指令
			break;
		}
		#endif
		case 0x6C:
		{
			Command_1B6C();	//微打中作为设置左边距,n个字符宽度
			break;
		}
		case 0x6D:
		{
			Command_1B6D();	//半切纸
			break;
		}
		case 0x70:
		{
			Command_1B70();	//钱箱控制
			break;
		}
		case 0x72:
		{
			Command_1B72();	//选择打印颜色
			break;
		}
		case 0x74:			  //选择字符代码页
		{
			Command_1B74();
			break;
		}
		case 0x75:
		{
			Command_1B75();	//返回外设状态(钱箱)
			break;
		}
		case 0x76:
		{
			Command_1B76();	//返回纸传感器状态
			break;
		}
		case 0x7B:
		{
			Command_1B7B();	//反向打印
			break;
		}
		
		default:
			break;
	}
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
