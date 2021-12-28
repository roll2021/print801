/**
	******************************************************************************
	* @file		x.c
	* @author	Application Team  
	* @version V0.0.1
	* @date		2012-5-14
	* @brief	 指令相关程序.
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
#include	"gs.h"
#include	"barcode.h"
#include	"charmaskbuf.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"feedpaper.h"
#include	"timconfig.h"
#include	"button.h"
#include	"led.h"
#include	"maintain.h"
#include	"cutter.h"
#include	"hexmode.h"
#include	"selftest.h"
#include	"init.h"
#include	"drawer.h"

extern	void ProcessBarCode_2D(uint8_t CommandType);
extern	void ClearInBuf(void);
extern  TypeDef_StructHeatCtrl	g_tHeatCtrl;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name	: LongToStr(uint32_t num, uint8_t *Str)
* Description		: 将数字量转换为ASCII字符串
* Input					: num:数字量,Str:字符串地址
* Output				 : None
* Return				 : None
*******************************************************************************/
void LongToStr(uint32_t num, uint8_t * Str)
{
	uint32_t Data;
	uint8_t	i;
	uint8_t	bBuf[16];
	
	i = 0;
	Data = num;
	while (1)
	{
		if ((Data == 0) && (i > 0))
			break;
		bBuf[i++] = Data % 10 + 0x30;
		Data /= 10;
	}
	for ( ; i > 0; )
	{
	 	i--;
		*Str++ = bBuf[i];
	}
	*Str = 0;
}

/*******************************************************************************
* Function Name	: 函数名
* Description		: 描述
* Input					: 输入参数
* Output				 : 输出参数
* Return				 : 返回参数
*******************************************************************************/
/*******************************************************************************
* Function Name	: Command_1D21
* Description		: 设置字符(ASCII字符,汉字)放大倍数,
*	但是HRI字符除外.D0~D2设置高倍数,D4~D6设置宽倍数
*	(1	≤	 纵向放大倍数	≤	8 ,1	≤	 横向放达倍数	≤	8 )
*	用 0	到 3	位选择字符高度,4	到 7	位选择字符宽度
* EPSON使用 0 到 2 位选择字符高度,4 到 6 位选择字符宽度
*				表 1														 表 2 
*		字符高度选择							字符宽度选择 
*	Hex	Decimal	纵向放大		Hex	Decimal 		横向放大 
*	00			0 		1(正常) 	00			0				1	(正常) 
*	01			1			2(2倍宽)	10			16			2	(2倍高) 
*	02			2			3					20			32 			3 
*	03			3			4					30			48			4 
*	04 			4			5					40			64			5 
*	05			5			6 			 	50			80			6 
*	06			6			7					60			96			7 
*	07			7			8					70			112			8 
*	这条指令对所有字符(ASCII字符和汉字)都有效,但是HRI字符除外
*	如果n超出了规定的范围,则这条指令被忽略. 
*	在标准模式下,纵向是进纸方向,横向是垂直于进纸的方向.但是当字符顺时针旋转90°时,
*	横向和纵向颠倒. 
*	页模式下,横向和纵向取决于区域的方向.
*	同一行字符的放大倍数不同时,所有的字符以底线对齐.
*	ESC !	指令也可以选择或者取消字符倍宽和倍高,最后接收的指令有效.
*	[ 默认值]		n = 0 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D21(void)
{
	uint8_t	GetChar;
	uint8_t	bTemp;
	
	GetChar = ReadInBuffer();
    if(GetChar & 0x88)									//参数超出定义范围不作处理
    {    
		return;
    }
	bTemp = (GetChar & 0x07);
	if (bTemp < MAX_RATIO_Y)
	{
		g_tPrtModeChar.HeightMagnification = bTemp + 1;	//字符纵向倍数
		g_tPrtModeHZ.HeightMagnification	 = bTemp + 1;	//汉字纵向倍数
	}
	
	bTemp = ((GetChar >> 4) & 0x07);
	if (bTemp < MAX_RATIO_X)
	{
		g_tPrtModeChar.WidthMagnification = bTemp + 1;	//字符横向倍数
		g_tPrtModeHZ.WidthMagnification		= bTemp + 1;	//汉字横向倍数
	}
}

/*******************************************************************************
* Function Name	: 函数名
* Description		: 1D 23 n 
*	1 ≤ n ≤ 8
	选择当前的下载位图图号,下载位图和打印位图根据此图号进行
· 该命令只针对下载到RAM中的位图,设定内容掉电后丢失.
. 下载到FLASH中的位图不使用本命令指定的图号.
* Input					: 输入参数
* Output				 : 输出参数
* Return				 : 返回参数
*******************************************************************************/
void Command_1D23(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if ((GetChar >= 1) && (GetChar <= 8))
	{
		g_bRAMBitMapNumber = GetChar;
	}
}

/*******************************************************************************
* Function Name	: 函数名
* Description		: 1D 24 nL nH
	页模式下设置纵向绝对位置
[ 范围]     0  ≤   nL ≤   255, 0 ≤   nH ≤   255 
[ 描述]     ·  页模式下设置绝对纵向位置. 
·  这条指令将纵向绝对位置设置在 [(nL + nH × 256) × (纵向或横向移动单位)]英寸处. 
[ 注释]     ·  这条指令只有在页模式下有效. 
·  如果[(nL+nH × 256)×(纵向或横向移动单位)]超出设定的打印区域,这条命令被忽略.
·  执行该指令后,横向位置不改变.
·  参考位置由  ESC T   指令设置.
·  根据ESC T 指令设置的打印区域方向和起始位置的不同
该指令使打印机进行如下操作: 
①如果打印起始位置为左上角或者右下角,该指令在与走纸方向平行的方向上设置绝对位置
②如果打印起始位置为右上角或者左下角,该指令在与走纸方向垂直的方向上设置绝对位置. 
·  横向和纵向移动单位是由  GS P  指令设置的. 
* Input					: 输入参数
* Output				 : 输出参数
* Return				 : 返回参数
*******************************************************************************/
void Command_1D24(void)
{
// 	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)
	{
		ReadInBuffer();
		ReadInBuffer();
	}
}


/*******************************************************************************
* Function Name	: Command_1D2841
* Description		: 执行测试打印 
*	1D	28	41	pL	pH	n	 m
*	( pL+ pH	× 256)=2	(pL=2, pH=0) 
*	0	≤	n ≤	2,		48	≤	n ≤	50 
*	1	≤	m≤	3,	49	≤	m	≤	51
*	执行测试打印,打印方式由n,m 决定,pL,pH(pL+pH ×256 )表示pH后的字节数(n和m)
*	n	决定测试纸张类型 
*		n	 	纸张类型 
*	0, 48		基本类型 ( 卷纸) 
*	1, 49 	卷纸 
*	2, 50 	卷纸 
*	m 决定打印内容 
*	m				打印内容 
*	1, 49		十六进制转存打印 
*	2, 50		打印机内部配置信息打印 
*	3, 51		循环字符打印 
*	本命令只在标准模式的行首有效 
*	本命令在页模式下无效 
*	如果在宏定义过程中接收到该命令,则停止宏定义并执行该命令 
*	执行本命令后,打印机自动复位并读取DIP开关设置. 
*	本命令执行结束时,打印机切纸 
*	执行本命令时,打印机处于忙状态,因而不接收命令
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D2841(void)
{
	uint8_t	pL;
	uint8_t	pH;
	uint8_t	n;
	uint8_t	m;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//标准模式
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//行首
		{
			pL = ReadInBuffer();
			pH = ReadInBuffer();
			n	= ReadInBuffer();
			m = ReadInBuffer();
			if (	((pL == 2) && (pH == 0)) && \
						((n <= 2) || ((n >= 48) && (n <= 50))) && \
						(((m >= 1) && (m <= 3)) || ((m >= 49) && (m <= 51)))
					)	//参数是否有效
			{
				SetComInBufBusy();		//执行本命令时,打印机处于忙状态,因而不接收命令
				//ReceiveInterrupt(DISABLE);	//关接收中断
				DisableReceiveInterrupt();
				if (g_tMacro.DefinitionFlag)	//宏定义中,退出宏定义,清除宏
				{
					g_tMacro.Number = 0;				//宏字符个数归零
					g_tMacro.GetPointer = 0;
					g_tMacro.DefinitionFlag = 0;
					g_tMacro.SetFlag = 0;				//宏填充的标志归零
					g_tMacro.RunFlag = 0;
				}
				
				if ((m == 1) || (m == 49))	//十六进制转存打印
				{
					HexMode();
				}
				else if ((m == 2) || (m == 50))	//打印机内部配置信息打印
				{
					SystemSelfTest();
				}
				else if ((m == 3) || (m == 51))	//循环字符打印
			 	{
					SystemSelfTest();
				}
				
				while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus())
				{
					;
				}
			 	InitParameter(0);				//调用系统默认值初始化全局结构
				ClearComInBufBusy();		//清除忙信号
				//ReceiveInterrupt(ENABLE);		//开接收中断
				EnableReceiveInterrupt();
			}
		}
	}
}

/*******************************************************************************
* Function Name	: Command_1D2844
* Description		: 允许,禁止实时指令 
* 1D	28	44	pL	pH	m	[a1 b1] … [ak bk] 
* 3	 ≤	(pL + pH × 256)	≤	65535	(0 ≤	pL	≤	255, 0	≤	pH	≤	255) 
* m = 20 
* a = 1, 2 
* b = 0, 1, 48, 49
* 通过 a	确定允许或禁止实时指令
* pL,pH(pL+pH ×256)表示pH后的字节数(m,[a1 b1]…[ak bk])
* a		b								功能
* 1		0, 48	DLE DC4 fn m t (fn = 1): 不处理(禁止)
* 1		1, 49	DLE DC4 fn m t (fn = 1): 处理(允许) 
* 2		0, 48	DLE DC4 fn a b (fn = 2): 不处理(禁止)
* 2		1, 49	DLE DC4 fn a b (fn = 2): 处理(允许) 
* 如果图形数据中包含了与DLE DC4 (fn = 1 or 2) 一样的数据串,建议预先使用此指令
* 来禁止实时指令
* Input					: None
* Output				 : None
* Return				 : None
* 1D 28 44 pL pH m [a1 b1]…[ak bk],通过 a 确定允许或禁止实时指令
*******************************************************************************/
void Command_1D2844(void)
{
	uint8_t	pL;
	uint8_t	pH;
	uint8_t	m;
	uint8_t	a;
	uint8_t	b;
	uint16_t	DataLen;
	uint16_t	i;
	
	pL = ReadInBuffer();
	pH = ReadInBuffer();
	//DataLen	= pL + 256 * pH;	//表示之后应该接收的字节数目
	DataLen	= pL + (uint16_t)(((uint16_t)pH) << 8);	//表示之后应该接收的字节数目
	if (DataLen < 3)
		return;
	
	m = ReadInBuffer();
	if (m != 20)
		return;
	DataLen--;								//减去m占用的字节数
	
	for (i = 0; i < DataLen; i += 2)
	{
		a = ReadInBuffer();
		b = ReadInBuffer();
		if (a == 1)
		{
			if ((b == 0) || (b == 48))	//不处理(禁止)DLE DC4 fn m t (fn = 1),10 14 fn m t 开钱箱的指令
			{
				g_tRealTimeCmd.RealTimeAllow &= (uint8_t)(~0x01);	//bit0=0;禁止实时脉冲
			}
			else if ((b == 1) || (b == 49))	//处理(允许)DLE DC4 fn m t (fn = 1)
			{
				g_tRealTimeCmd.RealTimeAllow |= 0x01;	//bit0=1;允许实时脉冲
			}
		}
		else if (a == 2)
		{
			if ((b == 0) || (b == 48))	//不处理(禁止)DLE DC4 fn a b (fn = 2),1014 fn a b执行关机序列指令 
			{
				g_tRealTimeCmd.RealTimeAllow &= (uint8_t)(~0x02);	//bit1=0;禁止关机序列执行
			}	
			else if ((b == 1) || (b == 49))	//处理(允许) DLE DC4 fn a b (fn = 2):
			{
				g_tRealTimeCmd.RealTimeAllow |= 0x02;	
			}
		}
	}
	
}

/*******************************************************************************
* Function Name	: Command_1D2845
* Description		: Set user setup commands
Controls the user setting modes. The table below explains the functions available 
in this command. Function code fn  specifies the function.
fn 	Function No. 					Function name
1		Function 1		Change into the user setting mode.
2 	Function 2		End the user setting mode session.
3 	Function 3		Change the memory switch.
4 	Function 4		Transmit the settings of the memory switch.
5 	Function 5		Set  the customized setting values.
6 	Function 6		Transmit the customized setting values.
7 	Function 7		Copy the user-defined page.
8 	Function 8		Define the data (column format ) for the character code page.
9 	Function 9		Define the data (raster format) for the character code page.
10 	Function 10		Delete the data for the character code page.
11	Function 11		Set the communication item for the serial interface.
12	Function 12		Transmit the communication item  for the serial interface.
13	Function 13		Set the configuration item for the  Bluetooth interface.
14	Function 14		Transmit the configuration item  for the Bluetooth interface.
48	Function 48		Delete the paper layout
49	Function 49		Set the paper layout
50	Function 50		Transmit the paper layout information
	pL, pH specifies ( pL + pH × 256) as the number of bytes after 
	pH ( fn and [parameters] ).
	Description of the [parameters] is described in each function.
[Notes]
■ The value of parameter fn determines the function number for this command. 
	Command operation differs, depending on the function number.
■ User setting mode is a special mode to change settings for the printer’s 
	built-in non-volatile memory.
■ To change a setting, you must change the printer into user setting mode, 
	using Function 1.
■ Execute function 2 of this command after making the setting. The setting value 
	is effective after executing the software reset of function 2.  
■ Note the rules below for the user se tting mode:
? The printer does not process character data.
? The printer does not process commands other than this command and the GS I command.
? The printer does not process real-time commands.
? Even if the ASB function is enabled, the printer does not transmit the ASB status.
■ Functions 4, 6, 12, 14 and 50, the functions used to transmit the current settings 
	to the host, can be used at any time, without changing into the user setting mode.
■ Data is written to the non-volatile memory by Functions 3, 5, 7, 11, 13, 48, and 49. 
	Note the following when using those functions:
? Do not turn off the power or reset the printer from the interface when the relevant 
	functions are being executed.
? The printer might be BUSY. In this case, be sure not to transmit a command from the 
	host because the printer will not receive the data.
? Excessive use of this function may destroy the non-volatile memory. As a guideline, 
	do not use any combination of the following commands more than 10 times per day for 
	writing data to the non-volatile memory: 
	GS(A (part of functions), GS (C (part of functions), GS(E (part of functions), 
	GS(L / GS 8 L (part of functions), GS(M (part of functions), GS g 0, FS g 1, FS q.
[Notes for transmission process]
■ Data is transmitted by Functions 1, 4, 6, 12, 14 and 50. When you use these functions, 
	obey the following rules.
? When the host PC transmits t he function data, transmit the next data after receiving 
	the corresponding data from the printer.
? With a serial interface printer, be sure to use this function when the host can receive data.
? With a parallel interface printer, data transmitted (excluding ASB status) with this command 
	(“Header to NUL”) is temporarily stored in the printer transmit buffer, as with other data. 
	When the host goes into reverse mode, the printer then transmits the data sequentially from 
	the beginning of the transmit buffer. Transmit buffer capacity is 99 bytes. Data exceeding 
	this amount is lost. Therefore, when using this command, promptly change into reverse mode 
	to start the data receive process.
? When communication with the printer uses XON/XOFF control with serial interface, 
	the XOFF code may interrupt the “Header to NUL” data string.
? The transmission information for each function can be identified to other 
	transmission data according to specific data of the transmi ssion data block. 
	When the header transmitted by the printer is [hex = 37H/decimal =55], treat 
	NUL [hex  = 00H/decimal =0] as a data group and identify it according to the 
	combination of the header and the identifier.

[Notes for ESC/POS Handshaking Protocol]
■ Use ESC/POS Handshaking Protocol below for Functions 14.
Step Host process									Printer process
	1 	Send  											GS(C < Function 14>. Start processing of Function 14. 
																	(Read specified record back to host.)
	2 	Receive data from printer.	Send device data.
	3 	Send response code.(*1)			Continue processing (*2) (*3) according to response.

(*1)
 Response code
ASCII		Hex		Decimal		Request
	ACK		06		6				Send next data.
	NAK		15		21			Resend previously sent data.
	CAN		18		24			Cancel send process.

(*2)
Processing according to response (unsent data exists, identified by send data 
set “Identification status”)
Response code		Process
	ACK						Start send processing for next data.
	NAK						Resend previously sent data.
	CAN						End processing for this command.

■ Processing the codes except for ACK, NAK, and CAN performs the same processing as CAN.
(*3) Processing according to response (no unsent data, identified by send data set 
	“Identification status”)
Response code		Process
	ACK, CAN			End processing for this command.
	NAK						Resend previously sent data.


* Input					: None
* Output				 : None
* Return				 : None
* 1D 28 44 pL pH m [a1 b1]…[ak bk],通过 a 确定允许或禁止实时指令
*******************************************************************************/
void Command_1D2845Function1(void)
{
}

void Command_1D2845Function2(void)
{
}

void Command_1D2845Function3(void)
{
}

void Command_1D2845Function4(void)
{
}

void Command_1D2845Function5(void)
{
}

void Command_1D2845Function6(void)
{
}

void Command_1D2845Function7(void)
{
}

void Command_1D2845Function8(void)
{
}

void Command_1D2845Function9(void)
{
}

void Command_1D2845Function10(void)
{
}

void Command_1D2845Function11(void)
{
}

void Command_1D2845Function12(void)
{
}

void Command_1D2845Function13(void)
{
}

void Command_1D2845Function14(void)
{
}

void Command_1D2845Function48(void)
{
}

void Command_1D2845Function50(void)
{
}

void Command_1D2845(void)
{
	uint8_t	pL, pH, fn;
		
	pL = ReadInBuffer();
	pH = ReadInBuffer();
	fn = ReadInBuffer();
	
	if ((pL == 0x03) && (pH == 0x00) && (fn == 0x01))
	{	//1D 28 45 03 00 01 49 4E
		Command_1D2845Function1();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x02))
	{	//1D 28 45 04 00 02 4F 55 54
		Command_1D2845Function2();
	}
	else if (fn == 0x03)
	{	//1D 28 45 pL  pH 03 [a1 b18 ...b11]...[ak nk8  nk1]
		Command_1D2845Function3();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x04))
	{	//1D 28 45 02 00 04 a
		Command_1D2845Function4();
	}
	else if (fn == 0x05)
	{	//1D 28 45 pL pH 05 [a1 n1L n1H] ... [ak nkL nkH]
		Command_1D2845Function5();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x06))
	{	//1D 28 45 02 00 06 a
		Command_1D2845Function6();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x07))
	{	//1D 28 45 04 00 07 a d1 d2
		Command_1D2845Function7();
	}
	else if (fn == 0x08)
	{	//1D 28 45 pL pH 08 y c1 c2 [x d1...d(y × x)]k
		Command_1D2845Function8();
	}
	else if (fn == 0x08)
	{	//1D 28 45 pL pH 08 x c1 c2 [x d1...d(x × y)]k
		Command_1D2845Function9();
	}
	else if ((pL == 0x03) && (pH == 0x00) && (fn == 0x0A))
	{	//1D 28 45 03 00 0A c1 c2
		Command_1D2845Function10();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x0B))
	{	//1D 28 45 pL pH 0B a d1...dk
		Command_1D2845Function11();
	}
	else if (fn == 0x0B)
	{	//1D 28 45 pL pH 0B a
		Command_1D2845Function12();
	}
	else if (fn == 0x0D)
	{	//1D 28 45 pL pH 0D a d1...dk
		Command_1D2845Function13();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x0E))
	{	//1D 28 45 02 00 0E a
		Command_1D2845Function14();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x30))
	{	//1D 28 45 04 00 30 d1 d2 d3
		Command_1D2845Function48();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x32))
	{	//1D 28 45 02 00 32 n
		Command_1D2845Function50();
	}
}

/*******************************************************************************
* Function Name	: Command_1D2848
* Description		: 1D28系列指令 请求应答或状态传送 
* Input					: None
* Output				 : None
* Return				 : None
*1D 28 48 pL pH fn m d1 d2 d3 d4
*******************************************************************************/
void Command_1D2848(void)
{
  uint8_t	Data[256];
	uint16_t Len, i;

	Len  = ReadInBuffer()+ 256 * ReadInBuffer();	//数据长度
	
	//接收(pL+256*pH)数据
	for(i=0; i< Len; i++)
	{
		if( i < 6 )
			Data[i]= ReadInBuffer();
		else								//多余的数据丢弃
			ReadInBuffer();
	}
	
	if(Data[0] ==48 && Len >= 6)			//fn=48,且数据量满足要求
	{
		if(Data[1] ==48 )					//m=48
		{
			Data[0] =0x37;					//返回固定格式
			Data[1] =0x22;

			for(i=2; i < 6; i++)					//ID号长度为4字节
			{
				if( Data[i] <32 || Data[i] > 126)	// 32=< dn <=126
					break;
			}
			if(i==6)							//数据有效
			{
				Data[i]=0x00;
				while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);				  //2016.06.30
 				//等待前面数据处理完成	
				UpLoadData(Data, 7);			//上传数据
			}
		}
	}
	else					//以后增加其他功能用
	{
	
	}
}

void Command_1D284B(void)
{
}

void Command_1D284C(void)
{
}

void Command_1D284E(void)
{
}


/*******************************************************************************
* Function Name	: Command_1D286B
* Description		: 1D286B系列指令  打印二维码 兼容莹普通打印机
   1D286B PL PH cn fn parameters指令打印二维码，兼容莹普通指令;
   cn: 48(0x30) 二维码PDF417，49(0x31) 二维码QR码;
   cn=48时：
   fn=65, GS ( k pL pH cn fn n,           PDF417: Set the number of columns in the data region.
   fn=66, GS ( k pL pH cn fn n            PDF417: Set the number of rows.
   fn=67, GS ( k pL pH cn fn n            PDF417: Set the width of the module.
   fn=68, GS ( k pL pH cn fn n            PDF417: Set the row height.
   fn=69, GS ( k pL pH cn fn m n          PDF417: Set the error correction level.
   fn=70, GS ( k pL pH cn fn m            PDF417: Select the options.
   fn=80, GS ( k pL pH cn fn m d1...dk    PDF417: Store the data in the symbol storage area.
   fn=81, GS ( k pL pH cn fn m            PDF417: Print the symbol data in the symbol storage area.
   fn=82, GS ( k pL pH cn fn m            PDF417: Transmit the size information of the symbol data in the symbol storage area.
   
   cn=49时：
   fn=65, GS ( k pL pH cn fn n1 n2      165 QR Code: Select the model.
   fn=67, GS ( k pL pH cn fn n          167 QR Code: Set the size of module.
   fn=69, GS ( k pL pH cn fn n          169 QR Code: Select the error correction level.
   fn=80, GS ( k pL pH cn fn m d1...dk  180 QR Code: Store the data into the symbol storage area.
   fn=81, GS ( k pL pH cn fn m          181 QR Code: Print the symbol data in the symbol storage area.
   fn=82, GS ( k pL pH cn fn m          182 QR Code: Transmit the size information of the symbol data in the symbol storage area.

* Input					 : None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D286B(void)
{	
	uint8_t PH;
	uint8_t PL;
	uint8_t	GetChar;	
	uint8_t FunType;
	uint8_t m;
  uint8_t n;
	uint16_t k;
	uint16_t DataNum;	
	
	PL = ReadInBuffer();
	PH = ReadInBuffer();
	DataNum = PL + PH*256;	 	   //数据长度
	GetChar = ReadInBuffer();		       //条码类型
	if(GetChar == 48)
	{
		g_tRamBuffer.BarCode2DStruct.BarCode2DType = 0;   //条码类型PDF417
	}	
	else if(GetChar == 49)
	{
		g_tRamBuffer.BarCode2DStruct.BarCode2DType = 2;   //条码类型QR码
	}
	else
	{
		return;
	}	
	FunType = ReadInBuffer();						//指令类型
	if(FunType == 65)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_version = ReadInBuffer();				 //PDF417 为每行字符数 ， QR为版本		
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)		//QR Code: Select the model
		{
			m = ReadInBuffer();				               
			n = ReadInBuffer();	
			if(m==49)
			{
				QR_model = 1;
			}
      else if(m==50)
			{
				QR_model = 2;
			}		
		}		
	}	
	else if(FunType == 66)						 //读取条码数据
	{		
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{	
			PDF417_Rows = ReadInBuffer();
		}	
	}
	else if(FunType == 67)
	{		
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_ModuleWidth = ReadInBuffer();				              //范围2-8 默认为3	
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)	  //QR Code: Set the size of module
		{
			QR_Rate = ReadInBuffer();				                          //放大倍数
		}	
	}
	else if(FunType == 68)
	{			
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_Rate = ReadInBuffer();				 //row height 范围2-8 默认为3	
		}		
	}	
	else if(FunType == 69)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				                   //纠错等级
			n = ReadInBuffer();	
			if(m==48)
			{
				if(n>=48 && n<=56)
				{
					PDF417_vColl = n-48;		
				}	
			}	
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();												 //纠错等级
			if(m>=48 && m <=51)
			{
				QR_vColl = m;														
			}	
		}	
	}	
	else if(FunType == 70)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_options = ReadInBuffer();				    //PDF417类型选择
		}		
	}	
	else if(FunType == 80)												  //存储条码数据
	{
		TwoCodeDataNum = DataNum -3;
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				for(k=0;k<(TwoCodeDataNum);k++)
				{
					g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[k] = ReadInBuffer();
				}
			}
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				for(k=0;k<(TwoCodeDataNum);k++)
				{
					g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[k] = ReadInBuffer();
				}
			}
		}	
	}	
	else if(FunType == 81)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				ProcessBarCode_2D_1D28();									//打印pdf417条码
			}
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				ProcessBarCode_2D_1D28();									//打印QR
			}
		}	
	}	
	else if(FunType == 82)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				;                                         //待添加
			}
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				;                                         //待添加
			}
		}
	}	
	else
	{
		return;
  }	
}

/*******************************************************************************
* Function Name	: Command_1D28
* Description		: 1D28系列指令
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D28(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x41:
		{
			Command_1D2841();		//执行测试打印
			break;
		}
		case 0x44:
		{
			Command_1D2844();		//允许,禁止实时指令
			break;
		}
		case 0x45:
		{
			Command_1D2845();		//设置用户设置指令等一系列指令,待添加
			break;
		}
		case 0x48:			
		{
			Command_1D2848();		//请求应答或状态传送 2016.06.14
			break;
		}
		case 0x4B:
		{
			Command_1D284B();		//待添加
			break;
		}
		case 0x4C:
		{
			Command_1D284C();		//待添加
			break;
		}
		case 0x4E:
		{
			Command_1D284E();		//待添加
			break;
		}
		case 0x6B:
		{
			Command_1D286B();		//打印二维码 兼容莹普通
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name	: Command_1D2A
* Description		: 定义一个下载位图,其点数由 x 和 y 指定
1D 2A x	y	d1...dk
1	≤	x	≤	255, 1 ≤ y ≤ 48
x	×	y	≤ 768
0	≤	d	≤	255
k = x ×	y ×	8
定义一个下载位图,其点数由 x 和 y 指定
·	x	指定位图的横向字节数；
·	y	指定位图的纵向字节数；
·	d	指定位图的数据.
·	位图横向上的点数为 x × 8；位图纵向上的点数为 y × 8.
·	如果 x × y	超出规定的范围,则这条指令无效. 
·	d为位图数据.数据对应位为1表示打印该点,为0表示不打印. 
·	下载的位图在下列情况下被清除: 
①	执行ESC @指令 
②	打印机电源关闭或复位. 

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
#define	DOWNLOAD_BITMAP_MAX_WIDTH					(255)
#define	DOWNLOAD_BITMAP_MAX_HEIGHT				(48)

#define	DOWNLOAD_BITMAP_MAX_DATA_BYTES		(768)		//最大768
#define	DOWNLOAD_BITMAP_MAX_DATA_LENGTH		(BMP_MAX_NUM * BMP_LENGTH)
void Command_1D2A(void)
{
	uint8_t	X;
	uint8_t	Y;
//	uint16_t i;
	uint16_t RAMLength;
	uint8_t	* pbData = NULL;
	
	//说明RAMBuffer中只保留1幅位图的内容
	X = ReadInBuffer();										//横向字节数
	Y = ReadInBuffer();										//纵向字节数
	//if (Y > DOWNLOAD_BITMAP_MAX_HEIGHT)		//最大高度值,字节计算
	//	return;
	
	RAMLength = (uint16_t)(((uint16_t)(X) * Y) << 3);			//总数据量
	if (RAMLength > DOWNLOAD_BITMAP_MAX_DATA_LENGTH)
		return;
	
	g_tRamBuffer.BitImageStruct.BitImageFlag = 2;		//下载位图标志
	//g_tRamBuffer.BitImageStruct.BitImageData[0] = X;
	//g_tRamBuffer.BitImageStruct.BitImageData[1] = Y;
	//pbData = g_tRamBuffer.BitImageStruct.BitImageData;
	pbData = g_tRamBuffer.BitImageStruct.BitImageData;
	*pbData = X;
	pbData++;
	*pbData = Y;
	pbData++;

	while (RAMLength)
	{
		*pbData++ = ReadInBuffer();
		RAMLength--;
	}
	
	pbData = NULL;
}

/*******************************************************************************
* Function Name	: Command_1D2F
* Description		: 打印下载位图,打印模式由 m	指定
1D	 2F	 m
0	≤	 m ≤	 3, 48 ≤	 m ≤	 51
	m		 模式					纵向分辨率 (DPI)	 横向分辨率 (DPI)	
0, 48	正常					203									203 
1, 49	倍宽					203									101 
2, 50	倍高					101									203 
3, 51	倍宽,倍高			101									101 
如果下载的位图没有被定义,这条指令被忽略. 
在标准模式下,只有打印缓冲区没有数据时,此指令才有效. 
除倒置打印模式外,其它打印模式对该指令无效(包括加粗,双重打印,下划线,
字体放大以及反显打印等). 
如果下载的位图超出了打印区域,则超出的部分不打印. 
该命令打印位图为下载到RAM中的位图,不是下载到FLASH中的位图. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D2F(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	ratiox;
	uint8_t	ratioy;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t X_Length;
	uint16_t Y_Length;
	uint16_t x;
	uint16_t y;
	uint8_t	* pbByteStart = NULL;
	uint8_t	* pbTemp = NULL;
	uint8_t	* pbBuf = NULL;
	uint8_t	* p = NULL;
	uint16_t  Index;
	s16	Lines;
	
//在标准模式中定义了位图且在行首
// 	if ((g_tRamBuffer.BitImageStruct.BitImageFlag == 2) && \
// 			(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit) && \
// 			(g_tSystemCtrl.PrtModeFlag == STD_MODE))

	if ((g_tRamBuffer.BitImageStruct.BitImageFlag == 2) && (g_tSystemCtrl.PrtModeFlag == STD_MODE))             //2017.02.09 去掉行首限制			
	{
		GetChar = ReadInBuffer();
		if ((GetChar <= 3) || ((GetChar >= 48) && (GetChar <= 51)))	//确保参数有效
		{
			RatioX = (GetChar & 0x01) + 1;					//横向放大倍数
			RatioY = ((GetChar & 0x02) >> 1) + 1;		//纵向放大倍数
			//X_Length = g_tRamBuffer.BitImageStruct.BitImageData[0] * 8;	//横向单双倍密度,按点计算
			X_Length = (uint16_t)(((uint16_t)g_tRamBuffer.BitImageStruct.BitImageData[0]) << 3);	//横向单双倍密度,按点计算
			Y_Length = g_tRamBuffer.BitImageStruct.BitImageData[1];			//纵向单双倍密度,按字节计算
			//超出打印区域的不打印
			if ((X_Length * RatioX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit))
			{
				X_Length = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit) / RatioX;
			}
			
			//WaitPrintAndFeedPaperFinish();		//先等待之前的打印完毕
			
// 			if (MAX_PRT_HEIGHT_BYTE >= Y_Length * RatioY)	//确保不过超过编辑缓冲区大小,一次打印完成
// 			{
// 				for (y = 0; y < Y_Length; y++)		//纵向高度,以字节为单位
// 				{	//指向当前编辑数据缓冲区起始字节行
// 					pbByteStart = g_tLineEditBuf + \
// 						(MAX_PRT_HEIGHT_BYTE - (Y_Length - y) * RatioY) * PRT_DOT_NUMBERS + \
// 						g_tPrtCtrlData.PrtLeftLimit;
// 					for (x = 0; x < X_Length; x++)	//横向宽度,以点为单位
// 					{
// 						pbTemp = pbByteStart + (x * RatioX);	//指向当前编辑数据缓冲区列
// 						GetChar = g_tRamBuffer.BitImageStruct.BitImageData[2 + Y_Length * x + y];	//按照正常打印顺序填充
// 						if (GetChar)
// 						{
// 							if (RatioY > 1)		//需要放大
// 							{
// 								memset(TempBuf, 0x00, MAX_RATIO_Y);
// 								//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
// 								ByteZoomCtrl(RatioY, GetChar, TempBuf);
// 							}
// 							else		//纵向放大倍数为1
// 							{
// 								TempBuf[0] = GetChar;
// 								TempBuf[1] = GetChar;
// 							}
// 							for (ratiox = 0; ratiox < RatioX; ratiox++)
// 							{
// 								for (ratioy = 0; ratioy < RatioY; ratioy++)
// 								{
// 									*(pbTemp + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
// 								}
// 								pbTemp++;
// 							}	//一个字节填充完毕
// 						}	//该字符不为0才处理填充,否则直接跳过
// 					}	//一行字节横向填充完毕
// 				}	//纵向按列全部填充完毕
// 				
// 				g_tPrtCtrlData.PrtDataDotIndex += (X_Length * RatioX);
// 				g_tPrtCtrlData.BitImageFlag = 1; //位图打印标志
// 				g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((Y_Length * RatioY) << 3);	//纵向总点数
//         //打印该编辑缓冲区
// 				PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
// 			}	//一次打印编辑完成
// 			else	//高度上超过了可编辑缓冲区大小,需要分为多次打印,每次处理1字节行

		for (y = 0; y < Y_Length; y++)		//纵向高度,以字节为单位
		{	//指向当前编辑数据缓冲区起始字节行  (Y_Length - y) * RatioY 改为RatioY 2016.05 
			pbByteStart = g_tLineEditBuf + \
				(MAX_PRT_HEIGHT_BYTE - RatioY ) * PRT_DOT_NUMBERS + \
				g_tPrtCtrlData.PrtLeftLimit;
			for (x = 0; x < X_Length; x++)	//横向宽度,以点为单位
			{
				pbTemp = pbByteStart + (x * RatioX);		//指向当前编辑数据缓冲区列
				if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)	//反向打印
				{
					GetChar = g_tRamBuffer.BitImageStruct.BitImageData[2 + Y_Length * x + (Y_Length - 1 - y)];
				}
				else		//按照正常打印顺序填充
				{
					GetChar = g_tRamBuffer.BitImageStruct.BitImageData[2 + Y_Length * x + y];
				}
				if (GetChar)
				{
					if (RatioY > 1)	//需要放大
					{
						memset(TempBuf, 0x00, MAX_RATIO_Y);
						//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
						ByteZoomCtrl(RatioY, GetChar, TempBuf);
					}
					else		//纵向放大倍数为1
					{
						TempBuf[0] = GetChar;
						TempBuf[1] = GetChar;
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
			g_tPrtCtrlData.PrtDataDotIndex += (X_Length * RatioX);
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
	 }	//参数有效处理完成
	}	//在行首且定义了下载位图处理完成
	
	g_tTab.TabIndex = 0;		//2016.09.06
	
	pbByteStart = NULL;
	pbTemp = NULL;
}

/*******************************************************************************
* Function Name	: Command_1D3A
* Description		: 设置/结束宏定义
在正常操作时,打印机接收到该指令开始宏定义.在宏定义时,打印机接收到该指令
结束宏定义. 
·	当打印机在定义宏时接收到	GS ^指令,则结束宏定义并且清除宏定义. 
·	打印机上电时,没有宏定义. 
·	ESC @不能清除宏定义,因此宏定义内容可以包括ESC @	指令. 
·	如果打印在刚接收完GS :指令后立刻又收到GS :时,打印机仍然没有宏定义. 
·	宏定义的内容可以达到2048 字节.如果宏定义的内容超出2048 字节,超出的数据 
被当作普通数据处理. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D3A(void)
{
	if (g_tMacro.DefinitionFlag == 0)
	{
		g_tMacro.DefinitionFlag = 1;	//宏定义开始标志
		g_tMacro.Number = 0;					//个数清零
		g_tMacro.GetPointer = 0;			//起始位置清零
		g_tMacro.RunFlag = 0;					//非运行状态
		g_tMacro.SetFlag = 0;					//宏定义完成标志清零
	}
	else
	{
		g_tMacro.DefinitionFlag = 0;	//宏定义结束标志
		if (g_tMacro.Number >= 2)
			g_tMacro.Number -= 2;				//除去结束的1D3A
		
		if (g_tMacro.Number == 0)
			g_tMacro.SetFlag = 0;				//清除宏已设置标志
		else
			g_tMacro.SetFlag = 1;				//置宏已设置标志
	}
}

/*******************************************************************************
* Function Name	: Command_1D42
* Description		: 选择/取消反显模式
*	当 n	的最低位为 0时,取消反显打印.
*	当 n	的最低位为 1时,选择反显打印.
*	n 只有最低位有效.
*	这条命令对所有字符(除过HRI字符)有效. 
·	选择反显打印后,由ESC SP 指令设置的字符间距也反显. 
·	这条指令不影响位图,自定义位图,条码,HRI字符以及由HT, ESC $, and ESC \
设定的空白. 
·	这条指令不影响行与行之间的空白. 
·	黑白反显打印模式比下划线模式优先级高.在黑白反显打印模式选择时,下划线模
式不起作用,取消黑白反显模式后,设定的下划线模式会起作用. 
[ 默认值]		n = 0 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D42(void)
{
	g_tPrtCtrlData.CharNotFlag = ReadInBuffer() & 0x01;
}

/*******************************************************************************
* Function Name	: Command_1D44
* Description		: 选择/取消驱动模式
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D44(void)
{	//进入PC机的驱动模式,,=0为正常打印模式,=1为SP驱动打印模式,=2为WH驱动打印模式
	uint8_t	GetChar;
	
	GetChar =  ReadInBuffer() & 0x01;
	g_tPrtCtrlData.PCDriverMode =	GetChar;
}

/*******************************************************************************
* Function Name	: Command_1D45
* Description		: 设置打印浓度,兼容驱动程序
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D45(void)
{
	ReadInBuffer();
}

/*******************************************************************************
* Function Name	: Command_1D48
* Description		: 设置HRI打印位置
1D	 48	 n
0	≤	 n	≤	 3, 48 ≤	 n	≤	 51 
打印条码时,为HRI字符选择打印位置 
n指定 HRI	打印位置: 
	n		 打印位置 
0, 48	不打印
1, 49	条码上方
2, 50	条码下方
3, 51	条码上,下方都打印 
·	HRI	是对条码内容注释的字符. 
·	HRI	字符的字体是由	GS f 指令来指定.
[ 默认值]		n = 0
* Input					: None
* Output			 : None
* Return				 : None
*******************************************************************************/
void Command_1D48(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if ((GetChar <= 3) || ((GetChar >= 48) && (GetChar <= 51)))
	{
		g_tBarCodeCtrl.HRIPosition = GetChar & 0x03;
	}	
}

/*******************************************************************************
* Function Name	: Command_1D49
* Description		: 查询打印机ID
1D	49	n 
1	≤	n ≤	3, 49	≤	n ≤	51, 65	≤	n ≤	69
查询打印机 ID 号,ID号类型由n 决定: 
	n		打印机 ID 号		返回参数 		 		ID (hexadecimal) 
1,49	打印机模型ID	 	SP-POS88 系列		20 
2,50	打印机类型ID		见下表 
3,51	ROM版本ID				与打印机型号及批次有关 
65		Firmware版本ID	与打印机型号及批次有关 
66		厂家						WHKJ 
67		打印机名称			POS88V 
68		打印机串号			与打印机型号及批次有关 
69		支持汉字类型		简体中文: CHINA GB2312 
											简体中文(三级字库): CHINA GB18030 
											繁体中文:TAIWAN BIG-5

n = 2,	打印机类型 ID 
Bit	Off/On	Hex	Decimal 		Function 
0		OFF			00	0						不支持双字节字符编码
0		ON			01	1						支持双字节字符编码 
1		ON			02	2						有切刀(固定) 
2		OFF			00	0						不用 
3		OFF			00	0						不用 
4		OFF			00	0						不用,固定为0 
5		_				_		_						未定义 
6		_				_		_						未定义 
7		OFF			00	0						不用,固定为0 

·	当1	≤	n ≤ 3	或49 ≤	n ≤	51 时, 打印机返回单字节ID 
·	当65 ≤	n ≤	69,	返回格式如下: 
头信息: Hexadecimal = 5FH / Decimal = 95 (1 byte) 
数据:	打印机信息 
NUL: Hexadecimal = 00H / Decimal = 0 (1 byte)

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D49(void)
{
	uint8_t	ID[2];
	uint8_t	DummyData[1];
	uint8_t	i;
	uint8_t * p = NULL;
	
	i = ReadInBuffer();
	if(( i<=4 )|| ( 48<= i && i <=52)||(65 <= i && i <=69) || (0x6F <= i && i <=0x71))	
	{
		ID[0] = 0x5F;
		ID[1] = 0x00;
		DummyData[0] = '\0';	//防止p指向错误地方
		p = DummyData;
		switch (i)
		{
			case 0:					//返回0x00
		  case 48:
			  ID[0] = 0x00;		  
				break;
			case 1:					//打印机型号ID	0x20
			case 49:
				ID[0] = 0x20;
				break;
			case 2:					//bit7-bit2=0,bit1-切刀,bit0=0-不支持双字节字符编码,=1-支持双字节字符编码
			case 50:
			{
				ID[0] = 0;
				if (g_tSysConfig.SysLanguage > 0)
				 	ID[0] |= 0x01;
				if (g_tSysConfig.CutType)
				 	ID[0] |= 0x02;
				break;
			}
			case 3:										
			case 51:				//返回ROM 版本 ID	兼容爱普生
				ID[0] = ROM_VERSION;
				break;
			case 4:					//返回0x00，兼容爱普生
		  case 52:
			  ID[0] = 0x00;		  
				break;
			case 65:				//返回固件版本
				p = FIRMWARE_STRING;
				break;
		 	case 66:				//返回厂家	WHKJ
			{
				if(ManufacturerType ==1)
					p = "EPSON";
				else
					p = g_tSysConfig.Manufacturer;
				break;
		  }
			case 67:				//返回打印机名称
				p = g_tSysConfig.Model;
				break;
			case 68:				//返回打印机序列号与打印机型号及批次有关
				p = g_tSysConfig.SerialNumber;
				break;
			case 69:				//返回打印机支持汉字类型, CHINA GB18030
				p = LanguageString[g_tSysConfig.SysLanguage];
				break;
			case 0x6f:	           	//返回@FDH，EPSON OPOS用到
				p = "@FDH";    
				break;		 
		  case 0x70:	           	//返回0，EPSON OPOS用到
				p = "0";    
				break;		 
		  case 0x71:	           	//返回1，EPSON OPOS用到
				p = "1";
				break;		
			default:
				break;
		}
		
		UpLoadData(ID, 1);			//上传数据
		i = strlen((const char *)p);
		if (i)
			UpLoadData(p, (i + 1));
		
		p = NULL;
	}
}

/*******************************************************************************
* Function Name	: Command_1D4C
* Description		: 设置左边距,在标准模式时,只在行首有效,
页模式时只设置内部标志,不影响打印效果
1D	 4C	 nL	nH
左边距设置为	[(nL + nH	× 256) × 横向移动单位)]
·	在标准模式下,该命令只有在行首才有效. 
·	在页模式下,此命令无效,打印机将该命令当作作普通字符处理. 
·	此命令不影响在页模式下的打印. 
·	如果设置超出了最大可用打印宽度,则取最大可用打印宽度 
·	横向和纵向移动单位是由	GS P命令设置的,改变纵向和横向移动单位不影响当前
的左边距.
[ 默认值]		nL = 0, nH = 0
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D4C(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	uint16_t	MiniCharWidth;
	
	GetChar = ReadInBuffer();
	//Temp = (GetChar + 256 * ReadInBuffer()) * g_tPrtCtrlData.StepHoriUnit;
	Temp = ((uint16_t)GetChar + (((uint16_t)(ReadInBuffer())) << 8)) * g_tPrtCtrlData.StepHoriUnit;
	
	if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//行首
	{
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
* Function Name	: Command_1D50
* Description		: 设置横向,纵向移动单位
1D	 50	 x	y
·	分别将横向移动单位近似设置成25.4/x mm(1/x英寸)纵向移动单位设置成25.4/y mm(1/y英寸). 
·	当 x 和 y 为0时, x 和 y被设置成默认值.
·	垂直于进纸方向为横向,进纸方向为纵向. 
·	在标准模式下,下列命令用x或者y,即使字符旋转(倒置或者顺时针旋转90°也不改变； 
		① 用 x 的命令:	ESC SP, ESC $, ESC \, FS S, GS L, GS W 
		② 用 y 的命令:ESC 3, ESC J, GS V 
·	页模式下,用 x 或者 y 要根据区域方向和打印起始位置来定: 
		①	当打印起始位置用ESC T 命令设置成左上角(打印方向从左到右)或者右下角
(打印方向从右到左)时: 
		用 x 的命令:	ESC SP, ESC $, ESC W, ESC \, FS S 
		用 y 的命令:	ESC 3, ESC J, ESC W, GS $, GS \, GS V 
		②	当打印起始位置用ESC T 命令设置成右上角(打印方向从上到下)或者左下角
(打印方向从下到上)时:	
		用 x 的命令:	ESC 3, ESC J, ESC W, GS $, GS \
		用 y 的命令:	ESC SP, ESC $, ESC W, ESC \,FS S, GS V 
·	此命令不影响以前前设定的其他设置. 
·	最小移动单位的是由该命令和其它命令综合作用的结果. 
·	一英寸等于25.4mm. .
[ 默认值]		x = 203, y = 203 ,此时一个移动单位就是一个打印点.横向距离为1/8mm,
纵向距离为1/8mm. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D50(void)
{
	uint8_t	GetChar;
	double	dTemp;
	
	//设置横向移动单位
	GetChar = ReadInBuffer();
	if (GetChar == 0)
	{
		g_tPrtCtrlData.StepHoriUnit = DEFAULT_STEP_HORI_UNIT;
	}
	else
	{
		dTemp = 25.4 / MIN_STEP_UNIT_X / GetChar;
		if (dTemp < 1.0)
		{
			g_tPrtCtrlData.StepHoriUnit = 1;
		}
		else
		{
			g_tPrtCtrlData.StepHoriUnit = (uint16_t)dTemp;
			if ((dTemp - (uint16_t)dTemp) >= 0.5)
			{
				g_tPrtCtrlData.StepHoriUnit++;
			}
		}
	}
	
	//设置纵向移动单位
	GetChar = ReadInBuffer();
	if (GetChar == 0)
	{
		g_tPrtCtrlData.StepVerUnit = DEFAULT_STEP_VER_UNIT;
	}
	else
	{
		dTemp = 25.4 / MIN_STEP_UNIT_Y / GetChar;
		if (dTemp < 1.0)
		{
			g_tPrtCtrlData.StepVerUnit = 1;
		}
		else
		{
			g_tPrtCtrlData.StepVerUnit = (uint16_t)dTemp;
			if ((dTemp - (uint16_t)dTemp) >= 0.5)
			{
				g_tPrtCtrlData.StepVerUnit++;
			}
		}
	}
}

#if	0
/*******************************************************************************
* Function Name	: Command_1D51
* Description		: 设置条码从一行开始到打印位置的距离为N个水平点距,默认为0,
*	该指令为荣达定义
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D51(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	
	giCodebarPositionSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
}
#else
/*******************************************************************************
* Function Name	: Command_1D51
* Description		: 打印可变高度位图,该指令为ESC/POS定义
*	Print variable vertical size bit image
	Hex 1D 51 30 m xL xH yL yH d1...dk
	0  ≤   m  ≤  3, 48 ≤   m  ≤  51
	1  ≤  (xL + xH × 256)  ≤  4256 (0 ≤ xL ≤ 255, 0 ≤ xH ≤ 12)
	1  ≤  (yL + yH × 256)  ≤  16 (1 ≤ yL ≤ 16, yH = 0)
	0  ≤   d  ≤  255
	k = (xL + xH ×  256)  ×  (yL + yH ×  256)
Prints a variable vertical size bit image using the  mode specified by m, as follows:
[ 描述]	 根据m 确定的模式打印位图.
	m					模式		纵向分辨率 (DPI)	横向分辨率 (DPI)
0, 48		正常						203									203
1, 49		倍宽						203									101
2, 50		倍高						101									203
3, 51		倍宽,倍高			101									101
xL,xH specifies(xL + xH × 256) dots in horizontal direction for the bit image.
yL,yH specifies(yL + yH × 256) bytes in vertical direction for the bit image.
d  specifies the bit image data (column format).
k indicates the number  of the bit image data. k is an explanation parameter ; 
therefore, it does not need to be transmitted.
· xL, xH 表示横向字点数为(xL + xH × 256) .
· yL, yH 表示纵向字节数为(yL + yH × 256) .
· d 表示指定的数据.
Y  = (yL + yH  ×  256)
The relationship between bit	image data and the print re sult is as follows:
d1	dY + 1	...		:				MSB
													LSB
d2	dY + 2	...		dk - 2	MSB
													LSB
:		:				...		dk - 1	MSB
													LSB
dY	dY * 2	...		dk			MSB
													LSB
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
#define	CMD_1D5130_DOT_XH_PAR_MAX			(12)
#define	CMD_1D5130_BYTE_YL_PAR_MIN		(1)
#define	CMD_1D5130_BYTE_YL_PAR_MAX		(16)
#define	CMD_1D5130_BYTE_YL_PAR_RATIOY_MAX	(12)
void Command_1D51(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX, RatioY;
	uint8_t	ratiox, ratioy;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint8_t	* pbByteStart = NULL;
	uint8_t	* pbTemp = NULL;
	uint8_t	* pbColumn = NULL;
	uint16_t X_Length, Y_Length, MaxX_width;
	uint16_t	x, y;
	uint16_t	SpaceDot;
	
	if (ReadInBuffer() != 0x30)
		return;
	
	if ((g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit) || \
			(g_tSystemCtrl.PrtModeFlag != STD_MODE)) //只在标准模式中且在行首才有效
		return;
	
	GetChar = ReadInBuffer();
	if (((GetChar > 3) && (GetChar < 48)) || (GetChar > 51))	//m参数无效返回
		return;
	
	RatioX = (GetChar & 0x01) + 1;					//宽度倍数,最大为2
	RatioY = ((GetChar & 0x02) >> 1) + 1;		//高度倍数,最大为2
	
	GetChar = ReadInBuffer();
	X_Length = GetChar;					//横向字点数低位
	GetChar = ReadInBuffer();		//横向字点数高位值
	if (GetChar <= CMD_1D5130_DOT_XH_PAR_MAX)
		X_Length += (uint16_t)(((uint16_t)GetChar) << 8);		//横向宽度点数值
	else
		return;
	
	GetChar = ReadInBuffer();
	Y_Length = GetChar;					//纵向字节数低位
	GetChar = ReadInBuffer();		//纵向字节数高位值
	if ((Y_Length >= CMD_1D5130_BYTE_YL_PAR_MIN) && \
			(Y_Length <= CMD_1D5130_BYTE_YL_PAR_MAX) && \
			(GetChar == 0x00))
		Y_Length += (uint16_t)(((uint16_t)GetChar) << 8);		//纵向字节数目
	else
		return;
	//因为数据是连续的一次性的,目前的打印缓冲区最大为24字节高,
	//所以在放大两倍的情况下超过12的无法处理.
	//如果最大值为超过12,那么只能限制纵向放大倍数.
	if ((Y_Length > CMD_1D5130_BYTE_YL_PAR_RATIOY_MAX) && (RatioY > 1))
		RatioY = 1;
	
	x = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit);	//横向可打印最大点数
	if (X_Length * RatioX > x)
		MaxX_width = x / RatioX;
	else
	 	MaxX_width = X_Length;
	
	SpaceDot = g_tPrtCtrlData.PrtLeftLimit;
	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)			//字符对齐模式,居中
		SpaceDot += ((x - MaxX_width * RatioX) >> 1);
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//居右
		SpaceDot += (x - MaxX_width * RatioX);
	
	//确保不过超过编辑缓冲区大小,一次打印完成
	if (MAX_PRT_HEIGHT_BYTE >= Y_Length * RatioY)
	{	//按列计算的填充起始指针,每处理完1列,改变其值
		pbByteStart = g_tLineEditBuf + SpaceDot + \
				(MAX_PRT_HEIGHT_BYTE - (Y_Length * RatioY)) * PRT_DOT_NUMBERS;
		for (x = 0; x < MaxX_width; x++)	//横向宽度,以点为单位,列值
		{
			//指向当前编辑数据缓冲区起始字节行,每处理完纵向1个字节,改变其值
			pbColumn = pbByteStart;
			for (y = 0; y < Y_Length; y++)	//纵向高度,以字节为单位
			{
				GetChar = ReadInBuffer();
				if (GetChar)				//数据不为0时全处理
				{
					if (RatioY > 1)		//需要放大
					{
						memset(TempBuf, 0x00, MAX_RATIO_Y);
						//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
						ByteZoomCtrl(RatioY, GetChar, TempBuf);
					}
					else		//纵向放大倍数为1
					{
						TempBuf[0] = GetChar;
					}
					pbTemp = pbColumn;
					for (ratiox = 0; ratiox < RatioX; ratiox++)
					{
						for (ratioy = 0; ratioy < RatioY; ratioy++)
						{
							*(pbTemp + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
						}
						pbTemp++;
					}
				}	//一个不为0字节填充完毕,数据为0时直接修改指针
				pbColumn += (RatioY * PRT_DOT_NUMBERS);
			}	//1列纵向高度字节填充完毕
			pbByteStart += RatioX;		//横向起始指针增加一个宽度值
		}		//可打印范围内的打印内容填充完毕
		
		g_tPrtCtrlData.PrtDataDotIndex += (SpaceDot + MaxX_width * RatioX);
		g_tPrtCtrlData.BitImageFlag = 1; 	//位图打印标志
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((Y_Length * RatioY) << 3);	//纵向总点数
		GetChar = g_tPrtCtrlData.LineDirection;
		g_tPrtCtrlData.LineDirection = PRINT_DIRECTION_NORMAL;	//倒置模式对其无效
// 		PrintOneLine();										//打印该编辑缓冲区
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		//读取可能的多余的数据
		for (x = MaxX_width; x < X_Length; x++)	//横向宽度,以点为单位,列值
		{
			for (y = 0; y < Y_Length; y++)	//纵向高度,以字节为单位
			{
				ReadInBuffer();
			}
		}
		if (GetChar)
		{
			WaitPreviousPrintFinish();
			g_tPrtCtrlData.LineDirection = GetChar;	//恢复倒置打印控制模式变量
		}
	}	//一次打印编辑完成
}
#endif

/*******************************************************************************
* Function Name	: Command_1D53
* Description		: 为了兼容GP驱动  执行切刀功能同1D5642
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D53(void)
{
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
		return;
	GoDotLine(CUT_OFFSET);									//走到切纸位置 2016.08.04
	DriveCutPaper(CUT_TYPE_PART);
	#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
		GoDotLine(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
	#endif

}
/*******************************************************************************
* Function Name	: Command_1D55
* Description		: 为了兼容GP驱动
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D55(void)
{
	ReadInBuffer();
	ReadInBuffer();
}

/*******************************************************************************
* Function Name	: Command_1D56
* Description		: 设置切刀和执行切纸
1D	 56	 m 
① m = 0,48,1 ,	49

1D	56	 m	n 
② m = 65,66, 0 ≤ n	≤ 255
根据m 的值选择切纸模式,如下所示: 
//此处命令定义有错误,需要根据EPSON指令进行修改
		M	 				切纸模式 
0,1,48,49		半切
65,66					进纸(切纸位置+[n	× (纵向移动单位)英寸] )并且半切纸

·	这条命令只有在行首有效.
·	m = 0,48,1 ,	49,打印机直接切纸. 
·	当m = 65,66,打印机进纸[打印位置到切刀之间距离 + n × 纵向移动单位]然后切纸. 
·	横向移动单位和纵向移动单位是由GS P 命令设置的.
·	进纸量用纵向移动单位来计算.

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D56(void)
{
	static uint8_t CutPaperMode = 4;	//=0,不切不走纸,＝3不切,＝4半切,＝5 全切
	uint8_t	GetChar;
	uint32_t i;
	
	GetChar = ReadInBuffer();
	
	switch (GetChar)
	{
		case 0x63:			 					//驱动程序,设置切纸方式
		{	//2016-8-9添加,该命令格式为1D 56 63 m n
			CutPaperMode = ReadInBuffer();
			ReadInBuffer();					//读取设置的走纸长度
			return;
		}
		case 0x6D:								//驱动程序,执行切纸
		{
			if (CutPaperMode == 3)
			{
					GoDotLine(CUT_OFFSET);		//走到切纸位置
			}
			else if (CutPaperMode == 4)
			{
				GoDotLine(CUT_OFFSET);		//走到切纸位置
				DriveCutPaper(CUT_TYPE_PART);			//半切
				#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
					KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸 2016.08.02
				#endif
			}
			else if (CutPaperMode == 5)
			{			
				GoDotLine(CUT_OFFSET);		//走到切纸位置
				DriveCutPaper(CUT_TYPE_FULL);			//全切
				#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
					KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
				#endif
			}
			break;
		}
		case 0:			 						//EPSON 全切纸方式
		case 48:
		{	//切刀指令只有在行首才有效
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_FULL);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
			#endif
			break;
		}
		case 1:			 						//EPSON 半切纸方式
		case 49:		 
		{	//切刀指令只有在行首才有效
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_PART);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
			#endif
			break;
		}
		case 65:			 					//EPSON 走纸后全切纸方式
		{
			GetChar = ReadInBuffer();
			//切刀指令只有在行首才有效
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			GoDotLine(CUT_OFFSET + GetChar * g_tPrtCtrlData.StepVerUnit);	//走到切纸位置			
			DriveCutPaper(CUT_TYPE_FULL);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
			#endif
			break;
		}
		case 66:			 					//EPSON 走纸后半切纸方式
		{
			GetChar = ReadInBuffer();
			//切刀指令只有在行首才有效
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			GoDotLine(CUT_OFFSET + GetChar * g_tPrtCtrlData.StepVerUnit);	//走到切纸位置
			DriveCutPaper(CUT_TYPE_PART);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//自动上纸
			#endif
			break;
		}
		default:
		{
			return;
		}
	}
	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
	if (g_tSysConfig.PaperOutReprint)
	{
		ClearInBuf();				//清已经打印完成的单据数据
	}
	#endif
}

/*******************************************************************************
* Function Name	: Command_1D57
* Description		: 设置打印区域宽度,在标准模式时,只在行首有效,
页模式时只设置内部标志,不影响打印效果
1D	57	nL	nH 
将打印区域宽度设置为[(nL + nH	×	256) × 横向移动单位)] 英寸
·在标准模式下,此命令仅在行首有效. 
·在页模式下此命令无效,命令数据被当成普通字符处理. 
·此命令不影响页模式下的打印. 
·如果[左边距+打印区宽度]超出可打印区域,则打印区域宽度为可打印区域宽度减去左边距.
·横向和纵向移动单位由GS P命令设置.改变横向和纵向移动单位不影响当前左边距和区域宽度.
·计算打印区宽度使用横向移动单位.
[ 默认值]		nL = 64, nH = 2 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D57(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	uint16_t	MiniCharWidth;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		GetChar = ReadInBuffer();
		//Temp = GetChar + 256 * ReadInBuffer();
		Temp = (uint16_t)GetChar + (((uint16_t)(ReadInBuffer())) << 8);
		Temp *= g_tPrtCtrlData.StepHoriUnit;
		
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//行首
		{
			if (g_tPrtCtrlData.PrtLeftLimit > g_tSystemCtrl.LineWidth)
			{
				g_tPrtCtrlData.PrtLeftLimit = g_tSystemCtrl.LineWidth;
				g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
			}
			
			//如果超过最大可打印区域
			if ((Temp + g_tPrtCtrlData.PrtLeftLimit) > g_tSystemCtrl.LineWidth)
			{
				Temp = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtLeftLimit;
			}
			
			//检查宽度是否能够放下一个字符, 否则扩展可打印区域值
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
			
			if (Temp < MiniCharWidth)	//确保能放下一个字符
			{
				Temp = MiniCharWidth;
			}
			
			g_tPrtCtrlData.PrtWidth = Temp;		//最终设置的打印区域宽度
			//检查调整左边距防止越界, 不能超越设置的最大行宽
			if ((g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtWidth) > g_tSystemCtrl.LineWidth)
			{
				g_tPrtCtrlData.PrtLeftLimit = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtWidth;
				g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
			}
			g_tPrtCtrlData.PrtLength = g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtWidth;
		}
	}
}

/*******************************************************************************
* Function Name	: Command_1D5A
* Description		: 选择二维条码类型
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D5A(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();

	if(GetChar <3)	
    {    
		g_tRamBuffer.BarCode2DStruct.BarCode2DType = GetChar;
    }
}

/*******************************************************************************
* Function Name	: Command_1D5E
* Description		: 执行宏定义 
1D	 5E	 r t	m 
0	≤	 r	≤	 255 
0	≤	 t	≤	 255 
m =	0, 1
执行宏命令. 
·	r	指定执行宏的次数. 
·	t	指定执行宏的等待时间. 
·	m 指定执行宏的模式. 
	当 m	的最低位为0时: 
	宏以t×100ms为间隔时间,连续执行r次. 
	当 m	的最低位为1时
	打印机等待t×100ms时间后,指示灯闪烁,直到用户按下进纸键,打印机才执行宏.如此循环r次. 
·	每次执行宏的等待时间为	t	×	100 ms. 
·	如果在宏定义时接收到这条命令则宏定义停止,正在定义的宏被清除.	
·	如果宏没有定义或者 r 是0 ,则该命令无效. 
·	当宏执行时( m = 1) ,不能用进纸键进纸. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D5E(void)
{
	uint8_t	i;
	uint8_t	j;
	uint8_t	MacroRunCount;
	uint8_t	MacroRunDelayRatio;
	uint8_t	MacroRunMode;
	
	if (g_tMacro.DefinitionFlag)	//宏定义中,退出宏定义,清除宏
	{
		g_tMacro.Number = 0;				//宏字符个数归零
		g_tMacro.GetPointer = 0;
		g_tMacro.DefinitionFlag = 0;
		g_tMacro.SetFlag = 0;				//宏填充的标志归零
		g_tMacro.RunFlag = 0;
	}
	
	MacroRunCount				= ReadInBuffer();		//运行宏的次数
	MacroRunDelayRatio	= ReadInBuffer();		//运行宏的延时时间倍数
	MacroRunMode				= ReadInBuffer();		//宏模式		
	if ((g_tMacro.SetFlag == 1) && (MacroRunCount != 0))	//如果没有宏定义,或参数错误,本指令无效
	{
		g_tMacro.RunFlag = 1;									//开始执行宏定义标识
		for (i = 0; i < MacroRunCount; i++)		//循环次数
		{
			DelayMs(MacroRunDelayRatio * 100);	//等待时间
			if (MacroRunMode & 0x01)	//需要按下按键时
			{
				j = 0;
				while (FEED_BUTTON_STATUS_OPEN == ReadFeedButtonStatus())	//等待按键按下
				{
					DelayMs(10);
					j++;
					if (j < 50)					//指示灯闪烁
					{
						PaperEndLedOn();
					}
					else
					{
						PaperEndLedOff();
						if (j > 100)
							j = 0;
					}
					
					g_tFeedButton.KeyStatus = 0;	//按键无效
					g_tFeedButton.KeyCount = 0;		//清空之前所有的计数
				}
				PaperEndLedOff();
			}
			
			g_tMacro.GetPointer = 0;	//执行宏
			while (g_tMacro.GetPointer < g_tMacro.Number)
			{
				ProcessCharMaskReadBuf();
			}
		}
		
		g_tMacro.RunFlag = 0;		//宏执行完毕
	}	
}

/*******************************************************************************
* Function Name	: Command_1D61
* Description		: 使能-失能自动状态返回
1D	 61	 n
确定自动状态返回的内容,参数n 含义如下:
Bit		Off/On	Hex	 Decimal	 ASB相关状态 
0			off 	 	00	 	0					钱箱打开状态使能 
0			on			01	 	1					钱箱打开状态无效
1			off 	 	00	 	0					在线/离线状态使能 
1			on 	 		02	 	2					在线/离线状态无效 
2			off 	 	00	 	0					错误状态使能 
2			on 	 		04	 	4					错误状态无效 
3			off 	 	00	 	0					纸状态使能 
3			on 	 		08	 	8					纸状态无效 
4-7		-				-			-					未定义

·	如果上表中任何一项内容处于使能状态,当该状态改变时打印机自动返回四个 
		 字节的打印机状态 
·	如果所有项目都处于无效状态,则ASB功能无效 
·	打印机自动返回状态字节时,不确认主计算机是否准备好 
·	本命令与其他命令一起按顺序执行,所以从发送该命令到自动状态返回设置有效 
有一定时间延迟 
·	即使打印机被ESC = 命令设置为无效状态,	打印机仍按设置进行自动状态返回 
·	自动返回的状态信息如下: 
第一字节(打印机信息) 

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
//低半字节任何一项内容处于使能状态,当该状态改变时,打印机自动返回四个字节的打印机状态
void Command_1D61(void)
{
	g_tError.ASBAllowFlag = ReadInBuffer() & 0x0F;
	if (g_tError.ASBAllowFlag)							//自动状态返回使能
	{
		UpLoadData(g_tError.ASBStatus, 4); 		//返回状态字
	}
}

/*******************************************************************************
* Function Name	: Command_1D66
* Description		: 设置HRI打印字体
1D	 66	 n
n	= 0,	1, 48, 49
打印条码时,为 HRI	字符选择一种字体 
用 n	来选择字体如下: 
	n		字体 
0,48	标准ASCII字符 (12	 ×	24) 
1,49	压缩ASCII字符 (9	×	17) 

·	HRI	字符是对条码内容注释的字符. 
·	HRI	字符的打印位置由	GS H	命令指定.
[ 默认值]		n = 0
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D66(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	if ((GetChar <= 1) || ((GetChar >= 48) && (GetChar <= 49)))
	{
		g_tBarCodeCtrl.HRIFont = GetChar & 0x01;
	}
}

/*******************************************************************************
* Function Name	: Command_1D6730
* Description		: 初始化维护计数器
1D	67	 30	m nL nH
m = 0 
(nL + nH × 256) = 20, 21, 50, 70		(nL = 20, 21, 50, 70, nH = 0)
设置由(nL+nH × 256 )指定的可清零维护计数器到0
nL + nH × 256 
十六进制	十进制 	维护计数器［单位］
	14			20			走纸行数［行］ 
	15			21			打印头加热次数［次］ 
	32			50			切刀操作次数［次］ 
	46			70			打印机运行时间［小时］
·	频繁写执行写NV存贮器指令可能会损坏NV存贮器,因此建议每天写NV存贮器不要超过10次. 
·	如果在执行该指令时,关闭打印机电源或通过接口复位打印机可能使打印机进入不正常状态,
	因此在执行该指令时不要关闭打印机电源或从接口复位打印机. 
·	打印机在执行该指令向NV存贮器写入数据时,打印机会变为“忙”并且停止接收数据,因此
	当打印机“忙”时,主机不要再发送数据. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D6730(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	
	GetChar = ReadInBuffer();
	if (GetChar != 0)
		return;
	GetChar = ReadInBuffer();
	Temp = GetChar + (uint8_t)(((uint16_t)(ReadInBuffer())) << 8);
	//标准模式下只有在行首有效
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
		{
		#ifdef	MAINTAIN_ENABLE
			switch (Temp)
			{
				case 0x14:						//初始化行计数器
				{
					R_Counter.LineCounter = 0;
					break;
				}
				case 0x15:						//加热次数
				{
					R_Counter.HeatCounter = 0;
					break;
				}
				case 0x32:						//切刀次数
				{
					R_Counter.CutCounter = 0;
					break;
				}
				case 0x46:						//打印机允许时间,小时
				{
					R_Counter.TimeCounter = 0;
					break;
				}
				default:
				{
					return;
				}
			}
		#endif
			
			SetComInBufBusy();
		
		#ifdef	MAINTAIN_ENABLE
			WriteFlashParameter(2);				//写CPU_FLASH维护计数器
		#endif
			ClearComInBufBusy();
		}	//在行首才执行
	}	//标准模式下
}

/*******************************************************************************
* Function Name	: Command_1D67
* Description		: 维护计数器操作指令
1D	67	32	m	nL nH
m = 0
(nL + nH × 256) = 20, 21, 50, 70 , 148 , 149, 178, 198 
(nL = 20, 21, 50, 70, , 148 , 149, 178, 198 , nH = 0) 
传送由(nL+nH × 256 )指定的维护计数器的值
nL + nH × 256 
十六进制	十进制	维护计数器［单位］			计数器类型
	14			20			走纸行数［行］					
	15			21			打印头加热次数［次］		可重置的
	32			50			切刀操作次数［次］			(可清零)
	46			70			打印机运行时间［小时］	
	94			148			走纸行数［行］					
	95			149			打印头加热次数［次］ 		累积的
	B2			178			切刀操作次数［次］			
	C6			198			打印机运行时间［小时］	
·	维护计数器的值是一测量值,因此这个值是受定时误差,怎样关电源和什么时候关电源影响的. 
·	在执行该指令时,在主机接收到返回数据前,不要发送其他数据.
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D6732(void)
{
	uint8_t	GetChar, DataBuf[16];
	uint32_t	iTemp;
	uint8_t	* p = NULL;
		
	DataBuf[1] = '\0';
	
	GetChar = ReadInBuffer();
	if (GetChar != 0)
		return;
	
	p = DataBuf;
	*p++ = 0x5F;
	GetChar = ReadInBuffer();
	//switch (GetChar + 256 * ReadInBuffer())
	#ifdef	MAINTAIN_ENABLE
	switch (GetChar + (uint8_t)(((uint16_t)(ReadInBuffer())) << 8))
	{
		case 0x14:						 //传送行计数器
		{
			iTemp = R_Counter.LineCounter;
			break;
		}
		case 0x15:						 //传送加热计数器
		{
			iTemp = R_Counter.HeatCounter;
			break;
		}
		case 0x32:						 //传送切刀计数器
		{
			iTemp = R_Counter.CutCounter;
			break;
		}
		case 0x46:						 //传送时间计数器,存放时间以分钟为单位,传送给主机为小时
		{
			iTemp = R_Counter.TimeCounter / 60;
			break;
		}
		case 0x94:						 //传送行计数器
		{
			iTemp = C_Counter.LineCounter;
			break;
		}
		case 0x95:						 //传送加热计数器
		{
			iTemp = C_Counter.HeatCounter;
			break;
		}
		case 0xB2:						 //传送切刀计数器
		{
			iTemp = C_Counter.CutCounter;
			break;
		}
		case 0xC6:						 //传送时间计数器
		{
			iTemp = C_Counter.TimeCounter / 60;
			break;
		}
		default:
		{
			return;				
		}
	}
	#endif
	
	LongToStr(iTemp, p);
	SetComInBufBusy();
	UpLoadData(DataBuf, strlen((const char *)DataBuf) + 1);
	ClearComInBufBusy();
}

/*******************************************************************************
* Function Name	: Command_1D67
* Description		: 维护计数器操作指令
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D67(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar == 0x30)			//1D 67 30,初始化维护计数器值
	{
		Command_1D6730();
	}
	else if (GetChar == 0x32)	//1D 67 32, 传送维护计数器
	{
		Command_1D6732();
	}
}

/*******************************************************************************
* Function Name	: Command_1D68
* Description		: 设置条码高度
1D	 68	 n 
1	≤	 n	≤	 255
条码高度为 n	点
[ 默认值]		n = 162
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D68(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar)
		g_tBarCodeCtrl.BarCodeHigh = GetChar;
}

/*******************************************************************************
* Function Name	: Command_1D6B
* Description		: 打印条码
1D	 6B	 m	d1...d k	00
①0	≤	 m ≤	 6	(k 和d的取值范围是由条码类型来决定)

1D	 6B	 m	n d1... dn
②65 ≤	 m ≤	 73	(k 和d 的取值范围是由条码类型来决定)

选择一种条码类型并打印条码 
m 用来选择条码类型,如下所示: 
m	条码类型							字符个数								d
0	UPC-A					11 ≤ k ≤ 12 			48 ≤ d ≤ 57
1	UPC-E					11 ≤ k ≤ 12 			48 ≤ d ≤ 57
2	JAN13 (EAN13)	12 ≤ k ≤ 13 			48 ≤ d ≤ 57 
3	JAN 8 (EAN8)		7	≤ k ≤ 8				48 ≤ d ≤ 57 
4	CODE39	 			1	≤ k ≤ 255			45 ≤ d ≤ 57, 65 ≤ d ≤ 90,32,36,37,43 
5	ITF	 					1	≤ k ≤ 255			48 ≤ d ≤ 57 
6	CODABAR	 			1	≤ k ≤ 255			48 ≤ d ≤ 57, 65 ≤ d ≤ 68,36,43,45,46,47,58

65	UPC-A					11 ≤ n ≤ 12 			48 ≤ d ≤ 57 
66	UPC-E					11 ≤ n ≤ 12 			48 ≤ d ≤ 57 
67	JAN13 (EAN13) 12 ≤ n ≤ 13				48 ≤ d ≤ 57 
68	JAN 8 (EAN8) 	7	≤ n ≤ 8				48 ≤ d ≤ 57 
69	CODE39	 			1	≤ n ≤ 255			45 ≤ d ≤ 57, 65 ≤ d ≤ 90,32,36,37,43,d1=dk=42
70	ITF	 				1	≤ n ≤ 255			48 ≤ d ≤ 57 
71	CODABAR	 		1	≤ n ≤ 255	 		48 ≤ d ≤ 57, 65 ≤ d ≤ 68,36,43,45,46,47,58
72	CODE93	 			1	≤ n ≤ 255	 		0	≤ d ≤ 127	
73	CODE128				2	≤ n ≤ 255	 		0	≤ d ≤ 127 

·	该命令在这种格式下以 NULL	结束. 
·	当选择UPC-A 或者UPC-E码时,打印机接收到12字节条码数据后,剩余的字符被当作普通字符处理. 
·	当选择JAN13 (EAN13)类型时,打印机接收到13字节条码数据后,剩余的字符被当作普通字符处理. 
·	当选择JAN8 (EAN8),类型时,打印机接收到8字节条码数据后,剩余的字符被当作普通字符处理.
·	ITF码数据个数必须是偶数.如果输入奇数个条码数据,则最后一个数据被忽略.
·	n用来指示条码数据的个数,打印机将其后边n字节数据作为条码数据处理.
·	如果n超出了规定的范围,打印机不处理这条命令,将其后的数据作为普通数据处理.

(标准模式):
·	如果条码数据d 超出了规定的范围,该命令无效. 
·	如果条码横向超出了打印区域,无效. 
·	这条命令不管由ESC 2 或ESC 3命令设置的行高是多少,走纸距离都与设定的条码高度相等. 
·	这条命令只有在打印缓冲区没有数据时才有效,如果打印缓冲区有数据,该命令被忽略. 
·	打印条码后,将打印位置设置在行首. 
·	打印模式设置(如加粗,双重打印,下划线,字符大小,反色以及字符顺时针旋转90°等)
不影响这条命令,但是倒置模式对条码打印有影响. 

(页模式):
·	这条命令只将条码图形生成到打印缓冲区,但是并不打印.处理完条码数据后将打
印位置移到条码的右边. 
·	如果 d	超出了规定的范围,该命令将被忽略. 
·	如果条码宽度超出了打印区域,该命令被忽略. 


* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
#define		VELO_COMMAND

void Command_1D6B(void)
{
	uint8_t	GetChar;
	uint8_t	Number;
	uint8_t	Temp;
	uint8_t	MaxDataLen;
	uint8_t	Inbuf[255];
	uint8_t	Hribuf[255];
	uint16_t Width;
	
	//if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)	//不在行首 2016.08.10  去掉
	//	return;
	
	GetChar = ReadInBuffer();		//模式m
	if ((GetChar >= 32 && GetChar <= 34) || (GetChar >= 97 && GetChar <= 99))
	{
		ProcessBarCode_2D(GetChar); 	//二维条码处理 
		return;
	}
	
	Width = 0;
	switch (GetChar)
	{
		case	0:				//UPC-A
			MaxDataLen = 12;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if ((Number == 11) || (Number == 12))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				//Width = FormatCodeUpc_A( Inbuf );
				Width = FormatCodeUPCA(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	65:				//UPC-A
		{
			Number = ReadInBuffer();
			if ((Number == 11) || (Number == 12))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					//Width = FormatCodeUpc_A( Inbuf );
					Width = FormatCodeUPCA(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	1:		//UPC-E
			MaxDataLen = 12;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if (((Number >= 6) && (Number <= 8)) || (Number == 11) || (Number == 12))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				Width = FormatCodeUpc_E( Inbuf );
				//Width = FormatCodeUPCE(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	66:		//UPC-E
		{
			Number = ReadInBuffer();
			if (((Number >= 6) && (Number <= 8)) || (Number == 11) || (Number == 12))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCodeUpc_E( Inbuf );
					//Width = FormatCodeUPCE(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	2:		//EAN13
			MaxDataLen = 13;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if ((Number == 12) || (Number == 13))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				//Width = FormatCodeEan13( Inbuf );
				Width = FormatCodeEAN13(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	67:		//EAN13
		{
			Number = ReadInBuffer();
			if ((Number == 12) || (Number == 13))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					//Width = FormatCodeEan13( Inbuf );
					Width = FormatCodeEAN13(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	3:		//EAN8
			MaxDataLen = 8;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if ((Number == 7) || (Number == 8))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				//Width = FormatCodeEan8( Inbuf );
				Width = FormatCodeEAN8(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	68:		//EAN8
		{
			Number = ReadInBuffer();
			if ((Number == 7) || (Number == 8))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					//Width = FormatCodeEan8( Inbuf );
					Width = FormatCodeEAN8(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	4:							 //Code39
			MaxDataLen = 0xFF;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if (Number >= 1)
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				Width = FormatCode39( Inbuf );
			}
			else
			{
				return;
			}
			break;
		case	69:
		{
			Number = ReadInBuffer();
			if (Number >= 1)
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCode39( Inbuf );
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	5:
			MaxDataLen = 0xFE;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if (Number >= 1)	//为1时无意义
			{
				if ((Number % 2) == 1)	//数据为奇数
				{
					Number -= 1;	//确保参数为偶数,如果不为偶数则忽略最后一个字符
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatItf25( Inbuf );
					ReadInBuffer();		//忽略最后的一个字符
				}
				else	//数据为偶数
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatItf25( Inbuf );
				}
			}
			else
			{
				return;
			}
			break;
		case	70:		//ITF,数据个数必须为偶数
		{
			Number = ReadInBuffer();
			if (Number >= 1)	//为1时无意义
			{
				if ((Number % 2) == 1)	//数据为奇数
				{
					Number -= 1;	//确保参数为偶数,如果不为偶数则忽略最后一个字符
					if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
					{
						//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
						Width = FormatItf25( Inbuf );
					}
					ReadInBuffer();		//忽略最后的一个字符
				}
				else	//数据为偶数
				{
					if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
					{
						//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
						Width = FormatItf25( Inbuf );
					}
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	6:
			MaxDataLen = 0xFF;	//按照最长的可能寻找,实际未必如此长
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//到NUL为止的实际数据长度
			if (Number >= 1)
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				Width = FormatCodabar( Inbuf );
			}
			else
			{
				return;
			}
			break;
		case	71:
		{
			Number = ReadInBuffer();
			if (Number >= 1)
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCodabar( Inbuf );
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	72:
		{
			Number = ReadInBuffer();
			if (Number >= 1)
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCode93(Inbuf, Number);
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	73:
		{
			Number = ReadInBuffer();
			if (Number >= 2)
			{
				Temp = GetCode128Str(Inbuf, Hribuf, Number);
				if (Temp != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCode128(Inbuf, Temp);
					strcpy((char *)Inbuf, (const char *)Hribuf);
				}
			}
			else
			{
				return;
			}
			break;
		}
		default:
			return;
	}
	
	//计算错误或者无内容,或者超过最大打印区域
	if ((Width == g_tPrtCtrlData.PrtDataDotIndex) || (Width > g_tPrtCtrlData.PrtLength))
	{
		GoDotLine((uint32_t)g_tBarCodeCtrl.BarCodeHigh);	//如果有错误则只走纸
		memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS);	 //清缓冲区
		//memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
	}
	else
	{	//返回值为从0开始的到打印图形结束的总宽度,所以需要调整为只包含实际图形的宽度值
		Width -= g_tPrtCtrlData.PrtDataDotIndex;		//得到只包含实际图形的宽度值
		PrintBar(Inbuf, Width);	//打印条码并清除下划线缓冲区
	}
}

/**********************
*********************************************************
* Function Name	: Command_1D72 n
* Description		: 返回由n值指定的状态,n=1返回纸状态,n=2返回钱箱状态
n = 1, 2, 49, 50
返回由n值指定的状态: 
	n					功能 
1,49		返回纸传感器状态 
2,50		返回钱箱状态
4,52		返回墨水状态
.	 该指令只对串口打印机有效. 
·	接收缓冲器中此命令前的数据被处理完之后,才执行这条指令,因此,发送该指令
与接收到返回状态有一定的时间滞后. 
·	返回状态字节各位对应关系如下所示:
* Input					: None
* Output				: None
* Return				: None
*******************************************************************************/
void Command_1D72(void)
{
	uint8_t	GetChar, Status;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar == 1) || (GetChar == 49))
	{
		Status = 0;
		if (g_tError.PaperSensorStatusFlag & 0x01)		//纸尽状态
			Status |= 0x0C;
		if (g_tError.PaperSensorStatusFlag & 0x02)		//纸将尽状态
			Status |= 0x03;
		
		UpLoadData(&Status, 1);							//上传数据
	}
	else if ((GetChar == 2) || (GetChar == 50))
	{
		Status = READ_MONEYBOX_STATUS;		//钱箱接口3脚电平 2016.06.19
		Status &= 0x01;
		UpLoadData(&Status, 1);							//上传数据
	}
}

/*******************************************************************************
* Function Name	: Command_1D7630
* Description		: 打印光栅位图.1D 76 30 m xL xH yL yH d1...dk
*					该指令受当前打印位置,居中影响.而不受反向打印影响
Hex	 1D	 76	30 m xL xH yL yH d1 … dk
[范围] 0	≤	m	≤	3, 48	≤	m	≤	51
1	≤	(xL + xH × 256) ≤	256 (0 ≤	xL	≤	255, xH = 0, 1)
1	≤	(yL + yH × 256) ≤	2303 (0	≤	yL	≤	255, 0 ≤	yH	≤	8)
0	≤	d	≤	255
k = (xL + xH × 256) × (yL + yH × 256)
[ 描述]	 根据m 确定的模式打印光栅位图.
	m					模式		纵向分辨率 (DPI)	横向分辨率 (DPI)
0, 48		正常						203									203
1, 49		倍宽						203									101
2, 50		倍高						101									203
3, 51		倍宽,倍高			101									101
· xL, xH 表示横向字节数为(xL + xH × 256) .
· yL, yH 表示纵向字点数为(yL + yH × 256) .
· d 表示指定的数据(光栅格式).
The relationship between bit	image data and the print re sult is as follows:
d1 			d2 			... 		dX
dX+1 		dX+2 		... 		dX*2
: 			: 			... 		:
... 		dk-2 		dk-1		dk			X = (xL + xH	×	256)
MSB	LSB MSB	LSB	MSB LSB	MSB	LSB
* Input					: None
* Output				: None
* Return				: None
*******************************************************************************/
#define	CMD_1D7630_BYTE_XH_PAR_MAX			(2)
#define	CMD_1D7630_DOT_YH_PAR_MAX				(8)
void Command_1D76(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	SpaceByte;
	uint8_t	k;
	uint8_t	l;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint8_t	DataBuf[PRT_DOT_NUMBERS / 8];		//横向总加热字节数
	uint16_t	X_Length;
	uint16_t	Y_Length;
	uint16_t	MaxX_width;
	uint16_t	i;
	uint16_t	j;
	
	if (ReadInBuffer() != 0x30)
		return;
	
	if (g_tMacro.DefinitionFlag)	//宏定义中,退出宏定义,清除宏
	{
		g_tMacro.Number = 0;				//宏字符个数归零
		g_tMacro.GetPointer = 0;
		g_tMacro.DefinitionFlag = 0;
		g_tMacro.SetFlag = 0;				//宏填充的标志归零
		g_tMacro.RunFlag = 0;
	}
	
	GetChar = ReadInBuffer();
	if (((GetChar > 3) && (GetChar < 48)) || (GetChar > 51))	//m参数无效返回
		return;
	
	//只在标准模式中且在行首才有效 2016.09.18  去掉限制 && \(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit))
	if (!(g_tSystemCtrl.PrtModeFlag == STD_MODE))
		return;
	
	RatioX = (GetChar & 0x01) + 1;					//宽度倍数,最大为2
	RatioY = ((GetChar & 0x02) >> 1) + 1;		//高度倍数,最大为2
	
	GetChar = ReadInBuffer();
	X_Length = GetChar;					//横向字节数低位
	GetChar = ReadInBuffer();		//横向字节数高位值
	if (GetChar < CMD_1D7630_BYTE_XH_PAR_MAX)
	{	
		X_Length += (uint16_t)(((uint16_t)GetChar) << 8);
	}	
	else
	{	
		return;
	}
	GetChar = ReadInBuffer();
	Y_Length = GetChar;					//纵向字点数低位
	GetChar = ReadInBuffer();		//纵向字点数高位值
	Y_Length += (uint16_t)(((uint16_t)GetChar) << 8);
// 	if (GetChar <= CMD_1D7630_DOT_YH_PAR_MAX)    //2017.04.24  去掉限制
// 	{	
// 		Y_Length += (uint16_t)(((uint16_t)GetChar) << 8);
// 	}	
// 	else
// 	{	
// 		return;
// 	}
	
	
	i = ((g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit) >> 3);	//横向可打印最大字节数
	if (X_Length * RatioX > i)
	{	
		MaxX_width = i / RatioX;
	}	
	else
	{	
	 	MaxX_width = X_Length;
	}
	SpaceByte = (g_tPrtCtrlData.PrtLeftLimit >> 3);				//不考虑不是8整数倍情况
	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)		//字符对齐模式,居中
	{	
		SpaceByte += ((i - MaxX_width * RatioX) >> 1);
	}	
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//居右
	{	
		SpaceByte += (i - MaxX_width * RatioX);
	}
	if (SpaceByte > (sizeof(DataBuf) / sizeof(DataBuf[0])))
	{	
		SpaceByte = sizeof(DataBuf) / sizeof(DataBuf[0]);
	}
	memset(DataBuf, 0x00, SpaceByte);		//开始左边清零横向总加热字节数

	for (i = 0; i < Y_Length; i++)				//纵向点数高度
	{
		l = SpaceByte;
		for (j = 0; j < MaxX_width; j++)		//横向字节数
		{
			GetChar = ReadInBuffer();
			if (RatioX == 2)									//倍宽
			{
				memset(TempBuf, 0x00, MAX_RATIO_Y);
				//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
				ByteZoomCtrl(RatioX, GetChar, TempBuf);	//数据放大处理
			}
			else
			{
				TempBuf[0] = GetChar;
			}
			for (k = 0; k < RatioX; k++)			//横向倍宽处理
			{
				DataBuf[l] = TempBuf[k];
				l++;
			}
		}
		
		for (; l < (sizeof(DataBuf) / sizeof(DataBuf[0])); l++)	 //不满一行的清零
		{
			DataBuf[l] = 0;									  //要防止下标越界
		}
		for (; j < X_Length; j++)						//丢弃超过打印区域的数
		{
			ReadInBuffer();										//否则下一行开始读取的不是需要的值		
		}
		for (k = 0; k < RatioY; k++)				//纵向倍高处理
		{
			DotLinePutDrvBuf( DataBuf );			//2016.06.16 送一行数据到驱动打印缓冲区
		}	
	}
	g_tPrtCtrlData.BMPBitNum = Y_Length *RatioY;	//2016.08.04
	g_tTab.TabIndex = 0;		//2016.09.06
}

/*******************************************************************************
* Function Name	: Command_1D77
* Description		: 设置条码宽度
1D	 77	n
2	≤	 n	≤	 6 
设置条码横向模块宽度	
用 n	来指定条码的横向模块宽度: 
n		单基本模块宽度						双基本模块宽度
				(mm)				窄基本模块(mm)	宽基本模块(mm)
2				0.25								0.25						0.625 
3				0.375								0.375						1.0 
4				0.5									0.5							1.25 
5				0.625								0.625						1.625 
6				0.75								0.75						1.875 
·	单基本模块条码如下: 
	UPC-A, UPC-E, JAN13 (EAN13), JAN8 (EAN8), CODE93, CODE128 
·	双基本模块条码如下: 
	CODE39, ITF, CODABAR 
[ 默认值]		n = 2 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D77(void)
{
	uint8_t	GetChar;
	const uint8_t	ThickWidth[5] = {5, 8, 10, 13, 15};
	//有待进一步确定,与编程说明不符
	GetChar = ReadInBuffer();
	if ((GetChar >= 2) && (GetChar <= 6))
	{
		//窄条宽,每个单位表示一个加热点宽度0.125mm,n表示n*0.125mm宽
		g_tBarCodeCtrl.BarCodeThinWidth = GetChar;	 								//窄条宽
		g_tBarCodeCtrl.BarCodeThickWidth = ThickWidth[GetChar - 2];	//宽条宽
	}
}

/*******************************************************************************
* Function Name	: GSCommand
* Description		: GS命令集的处理
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void GS_Command(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x21:			
		{
			Command_1D21();		//设置放大倍数
			break;
		}
		case 0x23:			
		{
			Command_1D23();		//定义下载到RAM中的位图图号,下载位图和打印位图根据此图号进行
			break;
		}
		case 0x24:			
		{
			Command_1D24();		//页模式下设置纵向绝对位置
			break;
		}
		case 0x28:
		{
			Command_1D28();		//执行测试打印,打印方式由n,m决定
			break;
		}
		case 0x2A:			
		{
			Command_1D2A();		//下载位图
			break;
		}
		case 0x2F:
		{
			Command_1D2F();		//打印下载位图
			break;
		}
		case 0x3A:
		{
			Command_1D3A();		//定义宏
			break;
		}
		case 0x42:
		{
			Command_1D42();		//选择/取消反白打印
			break;
		}
		case 0x44:
		{
			Command_1D44();		//选择驱动模式
			break;
		}
		case 0x45:
		{
			Command_1D45();		//设置打印浓度
			break;
		}
		case 0x48:
		{
			Command_1D48();		//设置HRI打印位置
			break;
		}
		case 0x49:
		{
			Command_1D49();		//返回打印机ID
			break;
		}
		case 0x4C:
		{
			Command_1D4C();		//设置左边距
			break;
		}
		case 0x50:
		{
			Command_1D50();		//设置移动单位
			break;
		}
		case 0x51:
		{
			Command_1D51();		//设置条形码打印移动位置/下载位图打印
			break;
		}
		case 0x53:
		{
			Command_1D53();		//切刀 2016.08.04
			break;
		}
		case 0x55:
		{
			Command_1D55();		//为了兼容GP驱动
			break;
		}
		case 0x56:			
		{
			Command_1D56();		//切刀指令
			break;
		}
		case 0x57:			
		{
			Command_1D57();		//设置打印区域
			break;
		}
		case 0x5A:			
		{
			Command_1D5A();		//选择二维条码类型
			break;
		}
		case 0x5E:
		{
			Command_1D5E();		//执行宏
			break;
		}
		case 0x61:
		{
			Command_1D61();		//使能/禁止自动状态返回
			break;
		}
		case 0x66:
		{
			Command_1D66();		//选择HRI字体
			break;
		}
		case 0x67:
		{
			Command_1D67();		//维护读数计数
			break;
		}
		case 0x68:
		{
			Command_1D68();		//设置条码高度
			break;
		}
		case 0x6B:			
		{
			Command_1D6B();		//打印条码
			break;
		}
		case 0x72:
		{
			Command_1D72();		//返回打印机状态
			break;
		}
		case 0x76:
		{
			Command_1D76();		//打印光栅位图
			break;
		}
		case 0x77:
		{
			Command_1D77();		//设置条码宽度
			break;
		}
		default:
			break;
	}			 	
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
