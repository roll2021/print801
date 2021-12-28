/**
******************************************************************************
  * @file    x.h
  * @author  WH Application Team  
  * @version V0.0.1
  * @date    2012-5-15
  * @brief   公共类型及宏定义的程序
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
#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#if	0
/******* 定义维护计数器类型结构 *************/
typedef struct
{
	volatile uint32_t	LineCounter;		//走纸行计数器
	volatile uint32_t	HeatCounter;		//加热次数计数器
	volatile uint32_t	CutCounter;			//切刀计数器
	volatile uint32_t	TimeCounter;		//运行时间计数器
}Maintenance_Counter;
#endif

/********** 定义系统参数全局变量结构 ***********/
typedef struct 
{
//	uint8_t	COM_Para;			//串口的参数全在一个字节中表示
	uint8_t	COM_BaudRate;		//串口波特率 ，D7校验错误＝1，用'?'表示，＝0时，忽略，
							//			   D2～D0，表示波特率
	uint8_t	COM_Protocol;		//串口的数据协议，D3表示位数，＝0，8位，＝1，7位，
							//                D2表示握手协议，＝0，硬件，＝1，XON/XOFF，
							//				  D1～D0表示校验，00无校验，01偶校验，10奇校验

 	uint8_t	SysLanguage;		//系统语言（使用索引，0=English;1=GB;2=BIG5）
 	uint8_t	SysFontSize;		//系统西文字体打大小（使用索引，0=12x24;1=9x17）
	uint8_t 	PrintWidth;  		//最大宽度设置，为了兼容EPSON TM-88的宽度
 	uint8_t	Density;			//打印浓度设置(0=低,1=中,2=高)

	uint8_t	MaxSpeed;			//最高速度
 	uint8_t	CutType;			//切刀标志
	uint8_t	BuzzerEnable;		//蜂鸣器使能
	uint8_t	PaperOutReprint;		//缺纸重新打印
  uint8_t	BlackMark;			//黑标设置(0=关,1=开)
	uint8_t	RightMark;			//黑标传感器位置，=0右边,=1左边，默认右边

 	uint8_t	PaperSensor;		//测纸传感器灵敏度设置(0=低,1=中,2=高)
 	uint8_t	BlackMarkSensor;	//黑标传感器灵敏度设置(0=低,1=中,2=高)
	uint8_t	SYS_ShdnMode;		//系统关机模式（使用索引,0=从不;1=10分钟;2=20分钟;3=30分钟）

	uint8_t	PrintDir;			//打印方向(0=正向,1=反向)
	uint8_t	CommandSystem;		//命令集，0＝EPSON ESC/POS命令集，1＝SP微打命令集，2＝炜煌微打命令集
	uint8_t 	DefaultCodePage;  	//默认代码页
	uint8_t	CR_Enable;			//0D回车指令使能

	uint8_t	Manufacturer[21];	//制造商
	uint8_t	Model[21];			//型号
	uint8_t	SerialNumber[21];	//序列号
	
	uint8_t	USBIDEnable;		//使能USBID
	uint8_t	CfgFlag[7];			//配置有效标志，有效值为“WHKJ"
}Sys_Cconfig_Struct;

/********** 定义系统参数全局变量联合 ***********/
typedef union				
{
	Sys_Cconfig_Struct	SysConfigStruct;
	uint8_t Sys_Para_Buff[128];
}Sys_Config_Union;

/********* 定义打印机状态全局变量结构 *************/
typedef struct
{
	volatile uint8_t	PrinterStatus;			//打印机状态， n = 1
	volatile uint8_t	OffLineStatus;			//打印机脱机状态， n = 2
	volatile uint8_t	ErrorStatus;				//打印机错误状态， 	n = 3
	volatile uint8_t	PaperStatus;				//打印机传送纸状态， n = 4
}Status_Struct;

typedef struct
{
	volatile uint8_t	PrinterStatus;			//打印机状态， n = 1
	volatile uint8_t	OffLineStatus;			//打印机脱机状态， n = 2
	volatile uint8_t	ErrorStatus;				//打印机错误状态， 	n = 3
	volatile uint8_t	PaperStatus;				//打印机传送纸状态， n = 4
}Typedef_struStatus;

/********* 定义打印机错误全局变量结构 *************/
typedef struct
{
	volatile uint8_t	PaperSensorFlag;	//纸传感器状态标志，位操作，高有效，bit.0=缺纸；bit.1= 纸将尽
	volatile uint8_t	AR_ErrorFlag;			//可自动恢复错误标志，位操作，高有效，bit.0=打印头过热；bit.1=上盖
	volatile uint8_t	R_ErrorFlag;			//可恢复错误标志，位操作，高有效，bit.0=切刀位置错误；bit.1=打印头原位错误（只有9针式有）
	volatile uint8_t	UR_ErrorFlag;			//不可恢复错误标志，位操作，高有效，bit.0=内存读写错误；bit.1=过电压；bit.2=欠电压；bit.3=CPU执行错误；bit.4=UIB错误（EPSON）；bit.7=温度检测电路错误
	
	volatile uint8_t	R_ErrorRecoversFlag;	//可恢复错误标志
	volatile uint8_t	PaperEndSensor;				//并口缺纸信号输出传感器选择
	volatile uint8_t	PaperEndStop;					//缺纸停止打印传感器选择
	
	volatile uint8_t	ASBAllowFlag;					//自动状态返回允许标志，Bit0对应钱箱，Bit1离线，Bit2错误，Bit1纸状态，
	volatile uint8_t	ASBStatus[4];					//自动返回状态字节
}Error_Struct;

/********* 定义打印机错误全局变量结构 *************/
typedef struct
{
	volatile uint8_t	PaperSensorFlag;	//纸传感器状态标志，位操作，高有效，bit.0=缺纸；bit.1= 纸将尽
	volatile uint8_t	AR_ErrorFlag;			//可自动恢复错误标志，位操作，高有效，bit.0=打印头过热；bit.1=上盖
	volatile uint8_t	R_ErrorFlag;			//可恢复错误标志，位操作，高有效，bit.0=切刀位置错误；bit.1=打印头原位错误（只有9针式有）
	volatile uint8_t	UR_ErrorFlag;			//不可恢复错误标志，位操作，高有效，bit.0=内存读写错误；bit.1=过电压；bit.2=欠电压；bit.3=CPU执行错误；bit.4=UIB错误（EPSON）；bit.7=温度检测电路错误

	volatile uint8_t	R_ErrorRecoversFlag;	//可恢复错误标志
	volatile uint8_t	PaperEndSensor;				//并口缺纸信号输出传感器选择
	volatile uint8_t	PaperEndStop;					//缺纸停止打印传感器选择

	volatile uint8_t	ASBAllowFlag;			//自动状态返回允许标志，Bit0对应钱箱，Bit1离线，Bit2错误，Bit1纸状态，
	volatile uint8_t	ASBStatus[4];			//自动返回状态字节
}Typedef_struError;

/********* 定义跳格控制使用的全局变量结构 *************/
typedef struct
{
	uint16_t	TabIndex[32];				//跳格位置表，跳格位置与字符宽度有关
	uint8_t	TabNumber;					//系统跳格数量，
	uint8_t	TabNumIndex;				//系统跳格索引，
}Tab_Struct;

/********* 定义跳格控制使用的全局变量结构 *************/
typedef struct
{
	volatile uint8_t		TabNumber;			//系统跳格数量，
	volatile uint8_t		TabIndex;				//系统跳格索引，
	volatile uint16_t	TabTable[32];		//跳格位置表，跳格位置与字符宽度有关
}Typedef_struTab;

/********* 定义实时指令使用的全局变量结构 *************/
typedef struct
{
	uint8_t	RealTimeFlag; 				//实时指令有效标志，高有效
	uint8_t	RealTimeCntr;				//实时指令数据计数器
	uint8_t	RealTimeAllow;				//实时指令10 14 的fn功能使能标志，Bit0对应fn1，Bit2fn2，...
	uint8_t	RealTimeBuf[16];			//实时指令缓存
}RealTime_Struct;

/********* 定义实时指令使用的全局变量结构 *************/
typedef struct
{
	uint8_t	EnableFlag; 	//实时指令有效标志，高有效
	uint8_t	CmdCounter;		//实时指令数据计数器
	uint8_t	FnFlag;				//实时指令10 14 的fn功能使能标志，Bit0对应fn1，Bit2fn2，...
	uint8_t	Buf[16];			//实时指令缓存
}Typedef_struRealTimeCommand;

#if	0
#define	USERHZSIZE				10					//用户自定义汉字数，最多10个
#define	USERZFSIZE				95					//用户自定义字符数，最多95个，自定义区与下载图形区共用。
/********** 定义用户自定义字符全局变量 ***********/
typedef struct
{
	uint8_t  UserdefineFlag;					//定义了自定义字符 ，1B 26指令

	uint8_t	UserCode_12[USERZFSIZE];		//12×24用户定义字符代码表
	uint8_t	Buffer_12[USERZFSIZE*3*12];  	//12*24用户定义字符的存储BUFFER

	uint8_t	UserCode_09[USERZFSIZE];		//9×17用户定义字符代码表
	uint8_t	Buffer_09[USERZFSIZE*3*9];	 	//9*17用户定义字符的存储BUFFER
									
	uint8_t	UserCode_HZ[USERHZSIZE];		//汉字用户定义字符代码表
	uint8_t	Buffer_HZ[USERHZSIZE*3*24];		//汉字用户定义字符的存储BUFFER
}UserChar_Struct;

#define	USER_DEF_HZ_NUMBERS			(10)	//用户自定义汉字数，最多10个
#define	USER_DEF_CHAR_NUMBERS		(95)	//用户自定义字符数，最多95个，自定义区与下载图形区共用。
/********** 定义用户自定义字符全局变量 ***********/
typedef struct
{
	volatile uint8_t  UserdefineFlag;					//定义了自定义字符 ，1B 26指令	
	volatile uint8_t	UserCode917[USER_DEF_CHAR_NUMBERS];					//9×17用户定义字符代码表
	volatile uint8_t	Buffer917[USER_DEF_CHAR_NUMBERS * 9 * 3];	 	//9*17用户定义字符的存储BUFFER
	volatile uint8_t	UserCode1224[USER_DEF_CHAR_NUMBERS];				//12×24用户定义字符代码表
	volatile uint8_t	Buffer1224[USER_DEF_CHAR_NUMBERS * 12 * 3]; //12*24用户定义字符的存储BUFFER
	volatile uint8_t	UserCodeHZ[USER_DEF_HZ_NUMBERS];					//汉字用户定义字符代码表
	volatile uint8_t	BufferHZ[USER_DEF_HZ_NUMBERS * 24 * 3];		//汉字用户定义字符的存储BUFFER
}Typedef_struUserDefChar;
#endif

#if	0
/********** 定义用户下载位图全局变量 ***********/
typedef struct
{
	uint8_t  BitImageFlag;					//下载位图标志，定义了 
	uint8_t	BitImageData[ BMPSIZE ];		//下载位图存贮区
}BitImage_Struct;

/********** 定义用户下载位图、自定义字符变量的联合 ***********/
typedef union							//位图、自定义字符的联合
{
	BitImage_Struct BitImageStruct;		//下载位图结构
	UserChar_Struct	UserCharStruct;		//自定义字符存贮区
	uint8_t	BarCode2DBuf[BARCODE2D_SIZE];	//二维条码缓冲区
}RAM_BUFFER;
#endif

typedef struct							//定义二维条码结构
{
	uint8_t	BarCode2DFlag; 				   	//打印二维条码标志
	uint8_t	BarCode2DType; 					//二维条码类型，0表示选择PDF417，1选择Data Matrix，2选择QR Code
//	uint8_t	version;
//	uint8_t 	vRate;
//	uint8_t	vColl;
//	uint8_t	*map;
//	uint8_t 	*pSrc;
//	uint8_t  *pDes;
//	uint16_t	datalen;
//	uint16_t	res;
}BarCode2D_Struct;

/*----系统进程控制使用的全局变量结构----*/
typedef struct
{
	volatile uint32_t	SleepCounter;		//系统进入休眠计数器
	volatile uint32_t	ShutDownCnt;		//系统进入关机的时间
	volatile uint16_t	StepNumber;			//电机走纸结束
	int16_t		StepTime;				//电机定时时间
	
	volatile uint8_t	SystemMode;			//工作模式
	volatile uint8_t	HexMode;				//十六进制打印模式
	volatile uint8_t	SetMode;				//设置模式
	volatile uint8_t	FeedCount;			//Feed键读数
	
	volatile uint8_t	PaperCtrl;			//纸传感器的检测控制,位控制,bit.1 = 0；不检测缺纸；bit.1 = 1,检测缺纸
	volatile uint8_t UserSelectFlag;	//选择了自定字符	1B 25 指令
	volatile uint8_t BitImageFlag;		//选择位图打印		1B 2A 指令
	
	volatile uint8_t	CodePAge;				//把0X80以后的数据按选择的国家号打印出来
	volatile uint8_t CharacterSet;		//国际字符集
	
	volatile uint8_t	ucBT_Type;			//是否有蓝牙模块标志,＝0没有,＝1有
	
	volatile uint8_t	PaperType;			//纸类型,0 = 连续纸； 1 = 标记纸
	volatile uint8_t	PrtDevOe;				//打印机允许、禁止控制,高有效
	volatile uint8_t	PrtBufFlag;			//打印标志, 0 = 正常 and 1 = 错误
	volatile uint8_t	PrtSourceIndex;	//打印数据源索引, 0 = 并口 and 1 = 串口, 2 = USB
	
	volatile uint8_t	LowPowerMode;		//系统关机的控制字（1 = On）		
	volatile uint8_t	MarkFlag;				//黑标状态,高有效		
}Typedef_struSystemCtrl;	//SystemCtrl_Struct

/*----PARTC:系统打印控制使用的全局变量结构----*/
typedef struct
{
	//----打印设备控制部分,----
	volatile uint8_t	PrtBusyFlag;			//打印进程标志, 0 = 空闲 1 = 正打印

	volatile uint8_t	CharWidth;				//ASCII字符宽度,12 或 9
	volatile uint8_t	CharHigh;				//ASCII字符高	24或17
	volatile uint8_t	LineHigh;
	volatile uint8_t	MaxRatioY;				//一行内最大纵向放大倍数

	//----打印进程控制部分,----
	//--------打印数据填充及消耗控制--------
	volatile uint8_t	RealTimeFlag;			//实时指令有效标志,高有效
	volatile uint8_t	RealTimeCntr;			//实时指令数据计数器
	volatile uint8_t	RealTimeBuf[8];			//实时指令缓存

	volatile uint32_t	PaperPrtMove;		//打印缓冲区内容并走纸控制,标准模式与页模式公用

	//////////////////////////////////////////////////////////
	//----标准模式打印控制设置部分定义----
	//////////////////////////////////////////////////////////
	//--------片内打印缓冲区数据管理控制--------
//	uint32_t	PrtLineStartLoc;		//有效起始打印行位置,固定结束行位置
//	uint32_t	PrtLineWidth;			//有效打印行的数量
	volatile uint32_t	PrtDataLineIndex;	//打印进程,行计数器索引。一次字符打印的打印行数索引（一次打印24行）
	volatile uint32_t	PrtDataDotIndex;	//打印进程,列计数器索引。一次行打印打印点阵计数器

	volatile uint8_t PCDriverMode;			//进入PC机的驱动模式 0：没进入 1：进入
	volatile uint8_t	DriverOffset;			//驱动时设置的绝对位置
	volatile uint8_t PCPrtLine;				//驱动打印的行数 

	volatile uint16_t	StepHoriUnit;		//打印横向移动单位,按点计算,暂时不用
	volatile uint16_t	StepVerUnit;		//打印纵向移动单位,按步计算,影响打印一行后的走纸

	volatile uint16_t MarkFeedLen;			//设置距离黑标位置的长度。

	volatile uint8_t	TabNumber;				//系统跳格数量,
	volatile uint8_t	TabNumIndex;			//系统跳格索引,
	volatile uint16_t	TabIndex[32];		//跳格位置表,跳格位置与字符宽度有关

	volatile uint32_t	CodeLineHigh;		//字符模式下的行高控制,按点计算
	volatile uint32_t	StdCurrentLineHigh;	//标准模式下的当前行高
	volatile uint8_t	CodeAimMode;			//字符对齐模式,0 = 左对齐, 1 = 中间对齐, 2 = 右对齐
	volatile uint8_t	CodeInversion;			//倒置打印控制,0 = normal Mode, 1= Inversion Mode

	//-- 一行字符等于字符宽度+字符右间距----
	volatile uint8_t	MaxLineHigh;			//一行中最高的字符/位图尺寸

	//-- PrtLeftLimit + PrtLength + PrtRightLimit = 可打印区域
	volatile uint16_t PrtLeftLimit;			//打印区域左边距,与页模式采用不同的值
	volatile uint16_t PrtLength;			//有效打印区域,与页模式采用不同的值
	volatile uint16_t PrtRightLimit;			//打印区域右边距,与页模式采用不同的值

	volatile uint8_t	CharCycle90;			//字符旋转90°控制,高有效
	volatile uint8_t	CharRBFlag;				//字符颠倒（翻转180°）打印控制，高有效
	volatile uint8_t	CharNotFlag;			//字符反显的控制，高有效

//	uint8_t  RasterFlag;				//打印光栅的标志	1113
//	uint8_t  RasterHigh;				//光栅的高度		1113
	volatile uint8_t OneLineWidthFlag;		//行内倍宽标志
	volatile uint8_t	DoUnderlineFlag;		//需打印下划线标志

	volatile uint8_t	PrintCurveFlag;			//曲线打印标志，＝1，曲线打印
}Typedef_struPrtCtrl;		//PRTCtrl_Struct

/* Funtion Declare -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H定义结束 */

/******************* (C) COPYRIGHT 2012 WH *****END OF FILE****/
