/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : GlobalVariable.h
* Author             : 
* Version            : V1.0
* Date               : 2016-4-3
* Description        : Header for module
********************************************************************************
* 
*******************************************************************************/
#ifndef DEFCONSTANT_H
#define DEFCONSTANT_H

#include	"stm32f10x.h"
#include	"debug.h"
#include	"custom.h"
#include "wifi.h"
/**************************    宏定义区    **************************************/
/*********************************************************************************
		全部片内（字节）内存分配:
		1.行打印缓冲区:最大倍高×（行高/3）×行宽,g_tPrtCtrlData.PrtDataBuf[PRTBUFLENGTH]
		  片内:最多支持 8倍高 x24点高×576点。地址下对齐
		  最大:     		13824B 8×（24/8）×576
		2.下划线缓冲区:	576B   行宽
		3.用户自定义字符:	6175B 95×(2+36+27),分别记录95个12x24/9×17点阵的用户自定义字符
		4.下载位图缓存:	12KB
		5.用户定义宏:   	2KB(2048字节)
		6.垂直制表:		256B
		7.水平制表:	 	144B （576/8）×2
		8.用户定义光栅:	2K(2048字节)
		9.指令接收缓存:	4K（4096字节）,g_tPrtCtrlData.PrtCodeBuf[MAXCODELENGTH]
		10.页模式打印缓冲区:128K,片外地址
**********************************************************************************/
#define	MODAL_NAME_STRING			"T801"			
#define	FIRMWARE_STRING				"T801M-V0.53"
#define	COMPANY_NAME_STRING		"AYHL"

#define	PARA_CHECK_STRING			"IPRT"		//系统参数写入flash校验字符串

#if defined	STM32F10X_CL
	#define	DOWNLOAD_BITMAP_IN_RAM_ENABLE
#endif

//#define	MAINTAIN_ENABLE         // 维护计数器
//#define	BARCODE_2D_ENABLE       

#define	RX_UNFINISHED			(0)
#define	RX_FINISHED				(1)
#define	RX_FLAG_TIM				(TIM5)
#define	RX_DELAY_MS				(50)

#define	ROM_VERSION				(0x64)   
#define	FIRMWARE_VERSION	(0x01)

#define	DRV_PRT_TYPE_NONE	(0x00)
#define	DRV_PRT_TYPE_IP		(0x01)
#define	DRV_PRT_TYPE_QT		(0x02)

#define	LED_FLASH_MS											(160)
#define	LED_FLASH_TOTAL_INTERVAL_MS				(2560)

#define	UNDERLINE_ENABLE_MASK							(1 << 3)
#define	UNDERLINE_DISABLE_MASK						(~(1 << 3))
#define	UNDERLINE_ENABLE_ONE_DOT_MASK			(0x09)
#define	UNDERLINE_ENABLE_TWO_DOT_MASK			(0x0B)
#define	UNDERLINE_ENABLE_THREE_DOT_MASK		(0x0F)

/********* 定义各缓冲区大大小 **********/
/****************************/
#define	MAX_RATIO_X				(8)					//横向最大倍数
#define	MAX_RATIO_Y				(8)					//纵向最大倍数

#define	DrvPrtBufDeep			(MAX_RATIO_Y*24)

#define	PRT_DOT_NUMBERS		(576)
#define PRT_BUF_LENGTH		(PRT_DOT_NUMBERS * MAX_RATIO_Y * 24 / 8)  //行打印缓冲区大小

#define	PAGE_HEIGHT_DOT_MAX	(1024 * 6)	//相当于纵向长度128mm * x(占内存72K * x) 未用
#define PAGE_BUF_LENGTH			(PRT_DOT_NUMBERS / 8 * PAGE_HEIGHT_DOT_MAX)  //页打印缓冲区大小

//定义下载位图的 X*Y最大值 ,如果机芯宽为576点,则下载缓冲就少（打印缓冲与下划线缓冲）
#ifdef	DOWNLOAD_BITMAP_IN_RAM_ENABLE
	#define	BMP_LENGTH			(48 * 256)	//一副位图最大1536字节
	#define	BMP_MAX_NUM			(1)
	#define	BMP_SIZE				((2 * BMP_MAX_NUM) + (BMP_MAX_NUM * BMP_LENGTH))	//定义下载位图最大缓冲区
#else
	#define	BMP_LENGTH			(8 * 1024)	//一副位图最大7K字节
	#define	BMP_MAX_NUM			(1)
	#define	BMP_SIZE				((2 * BMP_MAX_NUM) + (BMP_MAX_NUM * BMP_LENGTH))	//定义下载位图最大缓冲区 参数加点阵数据大小
#endif

#define	RASTER_SIZE				BMP_SIZE		//定义打印光栅缓存,与下载图形共用
#define	BARCODE2D_SIZE		BMP_SIZE		//定义二维条码缓存,与下载图形共用
#define	MACRO_SIZE					(1)		//定义宏的缓存  2015.06.16

#define	USER_HZ_SIZE				(10)			//用户自定义汉字数,最多10个
#define	USER_ZF_SIZE				(95)			//用户自定义字符数,最多95个,自定义区与下载图形区共用。

#define	COMMAND_ESCP				(0)
#define	COMMAND_IPRT				(1)
#define	COMMAND_WHKJ				(2)
#define	COMMAND_OTHER				(2)

/******* 定义语言代码 *************/
#define	LANGUAGE_ENGLISH		(0)
#define	LANGUAGE_CHINESE		(1)
#define	LANGUAGE_GB18030		(1)
#define LANGUAGE_BIG5				(2)

#define	LANGUAGE_NUM				(3)
#define	DEFAULT_CHINESE_LANGUAGE	(LANGUAGE_CHINESE)

#ifdef	DBG_SPI_ZIKU
#define LANGUAGE_DEFAULT		(LANGUAGE_ENGLISH)
#else
#define LANGUAGE_DEFAULT		(LANGUAGE_CHINESE)
#endif

#define	INTERNATIONAL_CHAR_SET_INDEX_ENGLISH		(0)
#define	INTERNATIONAL_CHAR_SET_INDEX_CHINESE		(15)

/*********** 切刀与打印位置参数 ******************/
/* 切刀和打印位置的值随不同打印头及按照方式不同而需要调整 */
#define CUT_OFFSETNUM					(230)	  //2017.02.07  230 走纸到出纸口
#define CUT_OFFSET						(128)	  // X mm * 8dot/mm, 打印位置与切纸位置的偏移值,与机型、切刀相关 2016.06.19 33 * 8改为128
#define NO_CUT_OFFSET					(112)	  // X mm * 8dot/mm, 切刀与撕纸位置的偏移值,与机型、切刀相关   14*8 改为112
#define CUT_STATUS_ON_TIME		(150)		//切刀检测开关最大闭合时间(在原位）,单位ms 
#define CUT_STATUS_OFF_TIME		(600)		//切刀检测开关最大打开时间 

/********** 机芯电机参数 ****************/
#define STEP_NUMBERS_PER_DOT_HEIGHT	(1)		//每点行电机步数

/********** 机芯参数 ****************/
#define EPSON_DOTNUMBER		(PRT_DOT_NUMBERS - 512)		//两种机芯宽度差（EPSON兼容）
#define CUSTOM_57MM_PRT_DOT_NUMBERS	(384)		//57mm纸宽设置的点数调整点数

#define	MIN_STEP_UNIT_X		(0.125)		//横向最小移动单位
#define	MIN_STEP_UNIT_Y		(0.125)	 	//纵向最小移动单位
#define TEMP_COEFFICIENT	(4)				//定义温度补偿系数

/********* 定义四种速度对应的加速步数 **********/
#define	SPEED_LOW							58						//150mm/s 每步约835uS  
#define	SPEED_MIDDLE 					94						//190mm/s 每步约657uS		94
#define	SPEED_HIGH 						126						//220mm/s每步约568uS		
#define	SPEED_MAX							126						//250mm/s 每步约568uS		
#define LPT_Drver_MaxStep     74            //170mm/s 每步约744uS  74 并口驱动最大速度  
#define USB_Drver_MaxStep     94            //150mm/s 每步约835uS 58 usb口驱动最大速度

/********* 定义休眠关机时间 **********/
#define SLEEP_TIME_MS			(500)		//自动进入休眠时间,单位为mS, 5000mS

#define	PRINTABLE_START_CODE								(0x20)
#define	CODEPAGE_START_CODE									(0x80)

/*************FLASH字库控制区*************/
//#define	BIG5_ZK_INDEPENDENT		//字库是独立的还是查表的
#define	ZK_ENCRYPTION		//字库是独立的还是查表的

//GB18030 24*24 汉字基地址,长度 28576*72=0x1F6500, 结束 0x3A64FF
#define	GB18030_24X24_BASE_ADDR							(0x180000)
#define	GB18030_16X16_BASE_ADDR							(0x180000)

//BIG5 24*24 汉字基地址，？需要通过转换表转换，待修改
// 13973*2 = 0x6D2A, 0x125B00-0x12C829
#define	BIG5_BASE_ADDR											(0x125B00)

//12*24国际字符基地址, (12*16)*36=0x1B00, 结束 0xE277F
#define	INTERNATIONAL_CODE_12_24_BASE_ADDR	(0xE0C80)

//12*24ASCII字符基地址, 96*36=0xD80, 结束 0xE34FF
#define	ASCII_12_24_BASE_ADDR								(0xE2780)

//12*24代码页字符基地址, 59*128*36=0x42600, 结束 0x125AFF
#define	ASCII_12_24_CODEPAGE_BASE_ADDR			(0xE3500)

//9×17国际字符基地址, (12*16)*27=0x1440, 结束 0x16E43F
#define	INTERNATIONAL_CODE_09_17_BASE_ADDR	(0x13D000)
#define	INTERNATIONAL_CODE_08_16_BASE_ADDR	(0x13D000)

//9*17ASCII字符基地址, 96*27=0XA20, 结束 0x16EE5F
#define	ASCII_09_17_BASE_ADDR								(0x13E440)
#define	ASCII_08_16_BASE_ADDR								(0x13E440)

//9*17代码页字符基地址, 59*128*27=0x31C80, 结束 0x1A0ADF
#define	ASCII_09_17_CODEPAGE_BASE_ADDR			(0x13EE60)
#define	ASCII_08_16_CODEPAGE_BASE_ADDR			(0x13EE60)

//字库版本地址
#define	ZK_Ver_ADDR		    	(0x13CFFB)

//预存位图基地址,必须是64K为单位的起始地址, 5*64K=320K, 结束 0x3FFFFF
#define	BIT_BMP_DATA_BASE_ADDR							(0x380000)

//预存位图控制参数存放地址 ?
#define	BIT_BMP_PARA_BASE_ADDR							(0x3F0000)

//使用寿命等参数存放地址 ?
#define	MAINTAIN_PARA_BASE_ADDR							(0x3FC000)

//系统参数存放地址, 4K, 结束 0x166FFF 
#define	SYS_PARA_BASE_ADDR									(0x136000)

#define	NV_FLASH_SPACE_MAX									(0x400000)	//字库最后地址
#define	NV_FLASH_END_ADDR										(0x400000)	//字库最后地址
#define	BIT_BMP_MAX_STORE_SPACE							(320*1024)	//下载位图预留的最大存储地址512K  5个64K的块

/*************定义外扩SRAM地址*************/
//外扩sram分为两部分 1为页模式所用缓存256K，剩余作为收数缓冲区
#define ExtSRamBaseAddr							(0x60000000)	                    //外部RAM的基地址
#define PageModeBaseAddr			      ExtSRamBaseAddr	                  //外部页模式缓冲区基地址
#define PageModeBufNum			        0x40000			                      //外部页模式缓冲区大小
#define ExtInBufBaseAddr				    (PageModeBaseAddr+PageModeBufNum) //外部接收缓冲区基地址
                        
/*************定义CODEPAGE*************/
#define	MAX_CODEPAGE_SET_NUM				(63)	//代码页最大选择项目,59

/*************打印控制区*************/
#define	PageX0						g_tPrtCtrlData.PageArea[0]	   //页打印区域X向起始点
#define	PageY0						g_tPrtCtrlData.PageArea[1]	   //页打印区域y向起始点
#define	PageDx						g_tPrtCtrlData.PageArea[2]	   //页打印区域X向结束点
#define	PageDy						g_tPrtCtrlData.PageArea[3]	   //页打印区域X向结束点

#define PageDotIndex			g_tPrtCtrlData.PageDataDotIndex   //页模式下当前点位置
#define	PageLineIndex			g_tPrtCtrlData.PageDataLineIndex  //页模式下当前行位置

#define	STD_MODE										(0)		//标准打印模式
#define	PAGE_MODE										(1)		//页打印模式

#define	ASCII_FONT_A								(0)		//字符点阵种类,12*24点阵
#define	ASCII_FONT_B								(1)		//字符点阵种类,9*17点阵
#define	HZ_FONT_A										(2)		//汉字点阵种类,24*24点阵
#define	ASCII_FONT_C								(3)		//字符点阵种类,8*16点阵
#define	HZ_FONT_B										(4)		//汉字点阵种类,16*16点阵

#define	ASCII_FONT_DEFAULT					(ASCII_FONT_A)	//默认字符点阵种类
#define	HZ_FONT_DEFAULT							(HZ_FONT_A)

#define	ASCII_FONT_A_WIDTH					(12)
#define	ASCII_FONT_B_WIDTH					(9)
#define	ASCII_FONT_C_WIDTH					(8)
#define	HZ_FONT_A_WIDTH							(24)
#define	HZ_FONT_B_WIDTH							(16)
#define	MAX_ASCII_FONT_WIDTH				(12)
#define	MAX_FONT_WIDTH							(24)

#define	ASCII_FONT_A_HEIGHT					(24)
#define	ASCII_FONT_B_HEIGHT					(17)
#define	ASCII_FONT_C_HEIGHT					(16)
#define	HZ_FONT_A_HEIGHT						(24)
#define	HZ_FONT_B_HEIGHT						(16)
#define	MAX_ASCII_FONT_HEIGHT				(24)
#define	MAX_FONT_HEIGHT							(24)

#define	DEFAULT_ASCII_FONT_WIDTH		(ASCII_FONT_A_WIDTH)
#define	DEFAULT_ASCII_FONT_HEIGHT		(ASCII_FONT_A_HEIGHT)
#define	DEFAULT_HZ_FONT_WIDTH				(HZ_FONT_A_WIDTH)
#define	DEFAULT_HZ_FONT_HEIGHT			(HZ_FONT_A_HEIGHT)

#define	ASCII_FONT_WIDTH_DEFAULT		(ASCII_FONT_A_WIDTH)
#define	ASCII_FONT_HEIGHT_DEFAULT		(ASCII_FONT_A_HEIGHT)
#define	HZ_FONT_WIDTH_DEFAULT				(HZ_FONT_A_WIDTH)
#define	HZ_FONT_HEIGHT_DEFAULT			(HZ_FONT_A_HEIGHT)

/******* 定义各字体占用字节数 *************/
#define BYTES_OF_ASCII_FONT_A				(36)	//12 X 24 点阵字符的点阵数据字节数
#define BYTES_OF_ASCII_FONT_B				(27)	//9 X 17 点阵字符的点阵数据字节数
#define BYTES_OF_ASCII_FONT_C				(16)	//8 X 16 点阵字符的点阵数据字节数
#define BYTES_OF_HZ_FONT_A					(72)	//24 X 24 点阵字符的点阵数据字节数
#define BYTES_OF_HZ_FONT_B					(32)	//16 X 16 点阵字符的点阵数据字节数
#define	MAX_BYTES_OF_CHAR						(BYTES_OF_HZ_FONT_A)
#define	MAX_BYTES_OF_HZ							(BYTES_OF_HZ_FONT_A)

//对齐方式
#define	AIM_MODE_LEFT								(0)
#define	AIM_MODE_MIDDLE							(1)
#define	AIM_MODE_RIGHT							(2)

#define	PRINT_DIRECTION_NORMAL			(0)		//默认正向打印
#define	PRINT_DIRECTION_REVERSE			(1)		//反向打印

#define	MAX_HT_INDEX_NUM						(32)	//最多设置跳格数目
#define	DEFAULT_HT_CHAR_NUM					(8)		//默认跳格字符设置宽度
#define	DEFAULT_HT_WIDTH						(DEFAULT_HT_CHAR_NUM * MAX_ASCII_FONT_WIDTH)	//默认设置跳格宽度

#define	PAPER_TYPE_CONTINUOUS				(0)		//连续纸
#define	PAPER_TYPE_BLACKMARK				(1)		//不连续纸,黑标纸
#define	PAPER_TYPE_LABLE				    (2)		//标签纸

#define CMD_POS                             (0)     //指令类型
#define CMD_CPCL                            (1)
#define CMD_CPCLU                           (2)

#define	PRINTER_STATUS_ILDE					(0)
#define	PRINTER_STATUS_BUSY					(1)

#define	PRINT_PROCESS_STATUS_ILDE		(0)
#define	PRINT_PROCESS_STATUS_BUSY		(1)

#define	DEFAULT_STEP_HORI_UNIT			(1)		//默认横向最小移动单位
#define	DEFAULT_STEP_VER_UNIT				(1)		//默认纵向最小移动单位

//2016.07.29 定义数据长度
#define SETSTRINGLEN	32	           //设置字符串长

//2017-5-26  定义语音模块音调
#define SPEAKERLOW				0xFFE2
#define SPEAKERMIDDLE			0xFFE5
#define SPEAKERHIGHT			0xFFED
#define SPEAKECLOSED      0x03

typedef struct
{
	uint16_t 	size;		  //输入指针
	uint16_t 	ptr_in;		//输入指针
	uint16_t	ptr_out;	//输出指针
	uint16_t	length;		//数据长度
	uint8_t		State;		//状态标志 
	uint8_t		*Buffer;	//数据缓冲区地址
}APP_Tx_Struct;


#ifdef	MAINTAIN_ENABLE
/******* 定义维护计数器类型结构 *************/
typedef struct
{
	u32	LineCounter;		//走纸行计数器
	u32	HeatCounter;		//加热次数计数器
	u32	CutCounter;			//切刀计数器
	u32	TimeCounter;		//运行时间计数器
}Maintenance_Counter;
#endif

/********** 定义系统参数全局变量结构 ***********/
typedef struct 
{
	uint8_t	ComBaudRate;			//串口波特率 ,D7校验错误＝1,用'?'表示,＝0时,忽略,
														//D2～D0,表示波特率
	uint8_t	ComProtocol;			//串口的数据协议,D3表示位数,＝0,8位,＝1,7位,
														//D2表示握手协议,＝0,硬件,＝1,XON/XOFF,
														//D1～D0表示校验,00,10无校验,11偶校验,01奇校验
 	uint8_t	SysLanguage;			//系统语言（使用索引,0=English;1=GB;2=BIG5）
 	uint8_t	SysFontSize;			//系统西文字体打大小（使用索引,0=12x24;1=9x17）
	uint8_t PrintWidthSelect; //最大宽度设置,为了兼容EPSON TM-88的宽度
 	uint8_t	Density;					//打印浓度设置(0=低,1=中,2=高)
	uint8_t	MaxSpeed;					//最高速度
 	uint8_t	CutType;					//切刀标志
	uint8_t	BuzzerEnable;			//蜂鸣器使能,=0禁止;=1,缺纸,错误时允许;=2,切纸时允许;=3,缺纸,错误,切纸时允许
	uint8_t	PaperOutReprint;	//缺纸重新打印
  uint8_t	BlackMarkEnable;	//黑标设置(0=关,1=开)
	uint8_t	BlackMarkPosition;	//黑标传感器位置,=0右边,=1左边,默认右边
 	uint8_t	PaperSensorSensitive;	//测纸传感器灵敏度设置(0=低,1=中,2=高)
 	uint8_t	BlackMarkSensitive;	//黑标传感器灵敏度设置(0=低,1=中,2=高)
	uint8_t	SysShutDownMode;	//系统关机模式（使用索引,0=从不;1=10分钟;2=20分钟;3=30分钟）
	uint8_t	PrintDir;					//打印方向(0=正向,1=反向)
	uint8_t	CommandSystem;		//命令集,0＝EPSON ESC/POS命令集,1＝IP微打命令集,2＝炜煌微打命令集
	uint8_t DefaultCodePage; 	//默认代码页
	uint8_t	CREnable;					//0D回车指令使能
	
	uint8_t	Manufacturer[21];	//制造商
	uint8_t	Model[21];				//型号
	uint8_t	SerialNumber[21];	//序列号
	
	uint8_t	USBIDEnable;			//使能USBID
	uint8_t USB_Class;			 //USB类型，0＝打印机类，1＝虚拟串口类（VCP类）
	
	uint8_t DHCPEnable;       //网卡DHCP协议 自动获取IP功能使能字节
	uint8_t	PaperWith;        //2017.03.2
	
	uint8_t PaperNearEndSensorEnable;			  //纸将近使能开关
	uint8_t PaperNearEndSensorSensitive;		//纸将近传感器灵敏度
	uint8_t PaperTakeAwaySensorEnable;			//取纸使能开关
	uint8_t PaperTakeAwaySensorSensitive;		//取纸传感器灵敏度
  
  uint8_t ProductiveData[4];							//产品出厂日期 2016.05.31
	uint8_t PrintMode;											//打印测试模式 2016.05.31
	uint8_t SpeakerVolume;
	uint16_t AlarmNumber;										//音律选择  两字节
	uint16_t POutVoiceNumber;								//音律选择
	uint8_t	ConfigFlag[7];		             //配置有效标志,有效值为“IPRT"

}TypeDef_StructSysConfig;

/********** 定义系统参数全局变量联合 ***********/
typedef union
{
	TypeDef_StructSysConfig	SysConfigStruct;
	uint8_t bSysParaBuf[sizeof(TypeDef_StructSysConfig)];     //2016.04  128改为sizeof函数
}TypeDef_UnionSysConfig;

/******* 定义接口类型 *************/
enum g_eComPortType{ COM = 0, LPT, USB, ETHERNET,BLUETOOTH};

/******* 定义指令集类型 *************/
enum g_eCmdSystemType{CMD_SYS_ESCPOS = 0, CMD_SYS_IPMP, CMD_SYS_WHMP, CMD_SYS_OTHER};

/******* 定义接口类型结构 *************/
typedef struct
{
	uint8_t	COM_Type;
	uint8_t	LPT_Type;
	uint8_t	USB_Type;
	uint8_t	ETH_Type;
  uint8_t	BT_Type;
	uint8_t	WIFI_Type;
}TypeDef_StructInterfaceType;

/******* 定义钱箱控制结构 *************/
typedef struct
{
	volatile uint8_t	Status;			//钱箱状态,0钱箱未启动,1钱箱已启动
	volatile uint8_t	DrwID;			//钱箱编号
	volatile uint16_t	ON_Time;		//钱箱开启时间,从上位机来
 	volatile uint16_t	OFF_Time;		//钱箱关闭时间,从上位机来
}TypeDef_StructDrawer;

/********** 定义宏定义全局变量结构 ***********/
typedef struct
{
	uint16_t	Number;						//宏字符个数
	uint16_t	GetPointer;				//宏取数指针
	uint8_t		DefinitionFlag;		//正定义宏标志
	uint8_t		SetFlag;					//已定义宏的标志
	uint8_t		RunFlag;					//正运行宏内容标志
	uint8_t		Buffer[MACRO_SIZE];	//宏数据缓存区
}TypeDef_StructMacro;

/********** 定义一维条码参数全局变量结构 ***********/
typedef struct
{
	uint8_t		HRIPosition;				//HRI的打印位置,0=不打印,1=条码上,2=条码下,3=条码上下, 
	uint8_t		HRIFont;						//HRI字体 		0=12*24,1=9*17
	uint8_t		BarCodeThinWidth;		//条码窄条宽度,以0.125mm为基本单位的倍数
	uint8_t		BarCodeThickWidth;	//条码宽条宽度,以0.125mm为基本单位的倍数
	uint8_t		BarCodeHigh;				//条码打印高度,以0.125mm为基本单位的倍数
}TypeDef_StructBarCtrl;

/********** 定义字符排版参数全局变量结构 ***********/
typedef struct
{
 	uint8_t		WidthMagnification;		//横向放大倍数Ratio
	uint8_t		HeightMagnification;	//纵向放大倍数Ratio
	uint8_t		EmphaSizeMode;				//加粗模式
	uint8_t		Underline;						//字符下划线控制,bit3 = on/off; bit1~0 = 下划线样式（单或双线）
	uint8_t		CharWidth;						//字符或汉字宽
	uint8_t		CharHigh;							//字符或汉字高
	uint16_t	LeftSpace; 						//左间距
	uint16_t	RightSpace;						//右间距
}TypeDef_StructPrintMode;

/********** 定义页模式打印控制全局变量结构 ***********/
typedef struct
{
	uint32_t	PageLineHigh;					//页模式下的行高控制,按点计算
	uint32_t	PageCurrentLineHigh;	//页准模式下的当前行高

  uint32_t	PageArea[4];					//页模式下打印区域,依次为X0,Y0,DX,DY
	uint32_t	PageVerLoc;						//页模式下的相对位置

	uint32_t	PageLeftLimit; 				//打印区域左边距,与标准模式采用不同的值
	uint32_t	PageRightLimit;				//打印区域右边距,与标准模式采用不同的值

	uint32_t	PageLineStartLoc;			//有效起始打印行位置,固定结束行位置
	uint32_t	PageLineWidth;				//有效打印行的数量
	uint32_t	PageDataLineIndex;		//打印进程,行计数器索引。一次字符打印的打印行数索引（一次打印24行）
	uint32_t	PageDataDotIndex;			//打印进程,列计数器索引。一次行打印打印点阵计数器

	uint32_t	PageNumber;						//页模式下的总页数控制
	uint32_t	PageWritePoint;				//页模式下的页填充控制计数器
	uint32_t	PageReadPoint;				//页模式下的页打印计数器

	uint8_t		AscRightSpace; 				//ASCII字符的右间距	
	uint8_t		HZRightSpace;					//汉字字符的右间距
	uint8_t		HZLeftSpace;					//汉字字符的左间距
	uint8_t		PageFeedDir;					//页模式下的打印方向,0 = 左到右;1 = 下到上; 2 = 右到左; 3 = 上到下
	uint8_t		PageToStandMode;			//从页模式返回到标准模式标志  //1208
	uint8_t		PagePrted;						//已经打印了页缓冲中的内容	  //1207
	uint8_t		PagePrtOe;						//页模式下,打印允许控制
}TypeDef_StructPageMode;

/********** 定义按键控制全局变量结构 ***********/
typedef struct
{
	uint32_t StepCount;      //按键走步步数
	uint8_t	KeyDownFlag;		//1 = 按下, 0= 松开  按键按下标志
	uint8_t	KeyEnable;			//0 = 允许按键, 1= 禁止按键
	uint8_t	KeyStatus;			//按键有效标志,高有效
	uint8_t	KeyCount;				//按键计数
}TypeDef_StructKeyCtrl;

/********** 定义系统进程控制使用的全局变量结构 ***********/
typedef volatile struct
{
	volatile  uint32_t	SleepCounter;    	//系统进入休眠计数器
	uint32_t	ShutDownCounter;	//系统进入关机的时间
	
	uint16_t	HeatTime;					//默认加热时间
	uint16_t	LineWidth;				//行宽
	uint16_t	StepNumber;
	
	uint8_t		HexMode;			    //十六进制打印模式
	uint8_t		SetMode;			    //设置模式
	uint8_t		SelfTestMode;		  //自检模式
  uint8_t		AgingTestMode;		//老化测试模式          //2016
  uint8_t		ParamRecoverMode;	//参数恢复模式 
	uint8_t		UserSelectFlag;		//选择了自定字符 1B 25 指令
	uint8_t		CharacterSet;		  //国际字符集
	uint8_t		PaperType;			  //纸类型,0 = 连续纸; 1 = 标记纸  2 = 标签纸 
	uint8_t		PrtDeviceDisable;	//打印机允许、禁止控制,高有效,=1,被禁止;=0被允许
	uint8_t		PrtModeFlag;		  //打印模式控制 ,0 = 普通模式 and 1 = 页模式
	uint8_t		PrtSourceIndex;		//打印数据源索引, 0 = RS232 and 1 = BLUETOOTH, 2 = USB 3 = ETH
	uint8_t		MarkPositionFlag;	//黑标状态,高有效
	uint8_t		CutRunFlag;			  //切刀正运行
  uint8_t 	MaxStepPrtNum[2];	//最大加速步数表，［0］＝指令模式，［1］＝驱动模式  
	uint8_t   CutLedFlag;				//切刀运行 灯带指示标志
	uint8_t   BillIDCutFlag;		//切刀运行 每一单切一次作为单据结束标志
  volatile uint8_t CMDType;    //指令类型  
  volatile uint8_t BillPrtFlag; //票据打印状态
	uint8_t   CutLedFlagBuf1;
	uint8_t   CutLedFlagBuf2;
}TypeDef_StructSystemCtrl;

/********* 定义打印机状态全局变量结构 *************/
typedef struct
{
 	uint8_t		PrinterStatus;		//打印机状态, n = 1
	uint8_t		OffLineStatus;		//打印机脱机状态, n = 2
	uint8_t		ErrorStatus;			//打印机错误状态, n = 3
	uint8_t		PaperStatus;			//打印机传送纸状态, n = 4
}TypeDef_StructStatus;

/********* 定义打印机错误全局变量结构 *************/
typedef struct
{
	volatile uint8_t	PaperSensorStatusFlag;	//纸传感器状态标志,位操作,高有效,bit.0=缺纸;bit.1= 纸将尽;bit.2=未取纸
	volatile uint8_t	AR_ErrorFlag;		//可自动恢复错误标志,位操作,高有效,bit.0=打印头过热;bit.1=上盖
	volatile uint8_t	R_ErrorFlag;		//可恢复错误标志,位操作,高有效,bit.0=切刀位置错误;bit.1=打印头原位错误（只有9针式有）,bit.2=卡纸，
	volatile uint8_t	UR_ErrorFlag;		//不可恢复错误标志,位操作,高有效,bit.0=内存读写错误;bit.1=过电压;bit.2=欠电压;bit.3=CPU执行错误;bit.4=UIB错误(EPSON);bit.7=温度检测电路错误
	
	volatile uint8_t	PaperEndStopSensorSelect;	//缺纸停止打印传感器选择,bit0=1,PNE有效;bit1=1,PE有效
	volatile uint8_t	LPTPaperEndSensorSelect;	//并口缺纸信号输出传感器选择,bit0=1,PNE有效;bit1=1,PE有效
	volatile uint8_t	R_ErrorRecoversFlag;	//解除可恢复错误标志
	
	volatile uint8_t	ASBAllowFlag;		        //自动状态返回允许标志,Bit0对应钱箱,Bit1离线,Bit2错误,Bit3纸状态,
	volatile uint8_t	ASBStatus[4];		        //自动返回状态字节
    
  volatile uint8_t    DoorOpen;                  //纸舱盖状态
    
}TypeDef_StructError;

/********* 定义跳格控制使用的全局变量结构 *************/
typedef struct
{
	uint8_t		TabSetNumber;	//系统跳格数量,即设置的有效跳格设置数,最大不超过32
	uint8_t		TabIndex;			//系统跳格索引,即目前的跳格位置,每执行一次跳格,值增加1
	uint16_t	TabPositionIndex[MAX_HT_INDEX_NUM];	//跳格位置表,跳格位置与字符宽度,右间距,横向放大倍数有关
}TypeDef_StructTab;

/********* 定义打印控制使用的全局变量结构 *************/
typedef struct
{
	uint16_t	PrtDataDotIndex;		//字符字模存入缓冲区指针
	uint16_t	CodeLineHigh;				//字符模式下的行高控制,按点计算
	uint16_t	PrtLeftLimit; 			//打印区域左边距,与页模式采用不同的值,以横向移动单位计算
	uint16_t	PrtRightLimit; 			//打印区域右边距,与页模式采用不同的值,以横向移动单位计算
	uint16_t	PrtWidth; 					//打印宽度,实际使用的打印区域
	uint16_t	PrtLength; 					//总打印宽度= 左边距 + 打印宽度,包括左边的空白和规定的打印区域
	uint16_t	MaxPrtDotHigh;			//编辑/打印缓冲区内容以基线为基准算起,所有打印内容中高度最高的点数值,
																//初始化为0,编辑字符及汉字,图形,光栅,条形码等过程中实时更新此值
	uint16_t	StepHoriUnit;				//打印横向移动单位,按点计算,暂时不用
	uint16_t	StepVerUnit;				//打印纵向移动单位,按步计算,影响打印一行后的走纸
	uint16_t	BMPBitNum;				  //1D763000指令运行参数 2016.08.04
	
	uint8_t		PCDriverMode;				//进入PC机的驱动模式 0:没进入 1:进入
	uint8_t		DriverOffset;				//驱动时设置的绝对位置
	uint8_t		BitImageFlag;				//位图打印模式标志
	uint8_t		VariableBitImageFlag;	//位图打印模式标志,可以通过放大倍数变大
	uint8_t		CharWidth;					//ASCII字符宽度,12 或 9
	uint8_t		CharHigh;						//ASCII字符高	24或17
	uint8_t		LineHigh;						//一行中最高字符
	uint8_t		MaxRatioX;					//一行内最大横向放大倍数
	uint8_t		MaxRatioY;					//一行内最大纵向放大倍数
	uint8_t		CodeAimMode;				//字符对齐模式,0 = 左对齐, 1 = 中间对齐; 2 = 右对齐
	uint8_t		LineDirection;			//反向打印（180°）
	uint8_t		CharNotFlag;				//字符反显的控制,高有效
	uint8_t		DoUnderlineFlag;		//需打印下划线标志	
	uint8_t		CharCycle90;				//字符旋转90°控制,高有效
	uint8_t		UnderlineMaxPrtHigh;	    //一行编辑过程中出现的下划线最高的点数,范围0~3,据此确定打印时的取值,
											//初始化为0,编辑字符及汉字过程中实时更新此值
	uint8_t		OneLineWidthFlag;		    //行内倍宽标志
}TypeDef_StructPrtCtrl;

/********* 定义实时指令使用的全局变量结构 *************/
typedef struct
{
	uint8_t		RealTimeEnable;			//是否使能实时指令功能,0禁止,1使能
	uint8_t		RealTimeFlag; 			//实时指令有效标志,高有效
	uint8_t		RealTimeCntr;				//实时指令数据计数器
	uint8_t		RealTimeAllow;			//实时指令10 14 的fn功能使能标志,Bit0对应fn1,Bit2fn2,...
	uint8_t		RealTimeBuf[16];		//实时指令缓存
}TypeDef_StructRealTime;

/********** 定义用户自定义字符全局变量 ***********/
typedef struct
{
	uint8_t		UserdefineFlag;		//定义了自定义字符,1B 26指令,BIT0=1表示定义了字符;
															//1C 32指令, BIT1=1表示定义了汉字
	
	uint8_t		UserCode_12[ USER_ZF_SIZE ];		//12×24用户定义字符代码表
	uint8_t		Buffer_12[ USER_ZF_SIZE * BYTES_OF_ASCII_FONT_A ]; //12*24用户定义字符的存储BUFFER
	
	uint8_t		UserCode_09[ USER_ZF_SIZE ];		//9×17用户定义字符代码表
	uint8_t		Buffer_09[ USER_ZF_SIZE * BYTES_OF_ASCII_FONT_B ];	//9*17用户定义字符的存储BUFFER
#ifdef	CPCL_CMD_ENABLE	
	uint8_t		UserCode_08[ USER_ZF_SIZE ];		//8×16用户定义字符代码表
	uint8_t		Buffer_08[ USER_ZF_SIZE * BYTES_OF_ASCII_FONT_C ];	//8*16用户定义字符的存储BUFFER
#endif	
	uint8_t		UserCode_HZ[ USER_HZ_SIZE ];		//汉字用户定义字符代码表
	uint8_t		Buffer_HZ[ USER_HZ_SIZE * BYTES_OF_HZ_FONT_A ];	//汉字用户定义字符的存储BUFFER
#ifdef	CPCL_CMD_ENABLE	
	uint8_t		UserCode_HZ16[ USER_HZ_SIZE ];		//汉字用户定义字符代码表
	uint8_t		Buffer_HZ16[ USER_HZ_SIZE * BYTES_OF_HZ_FONT_B ];	//16*16汉字用户定义字符的存储BUFFER
#endif
}TypeDef_StructUserChar;

/********** 定义用户下载位图全局变量 ***********/
typedef struct
{
	uint8_t		BitImageFlag;							//下载位图标志,=2表示定义了下载RAM位图
	uint8_t		BitImageData[ BMP_SIZE ];	//下载位图存贮区,头两个字节表示位图的长度和宽度
}TypeDef_StructBitImage;


typedef struct							//定义二维条码结构
{
	uint8_t	BarCode2DFlag;	                //打印二维条码标志
	uint8_t	BarCode2DType;	                //二维条码类型,0表示选择PDF417,1选择Data Matrix,2选择QR Code
    uint8_t	BarCode2DBuf[BARCODE2D_SIZE];	//二维条码缓冲区
}TypeDef_StructBarCode2D;

/********** 定义用户下载位图、自定义字符变量的联合 ***********/
typedef union							//位图、自定义字符的联合
{
	TypeDef_StructBitImage	BitImageStruct;		//下载位图结构
	TypeDef_StructUserChar	UserCharStruct;		//自定义字符存贮区
 	TypeDef_StructBarCode2D	BarCode2DStruct;	//二维条码缓冲区  
}TypeDef_UnionRAMBuffer;

//行数据宏定义
#define	BYTES_OF_ALL_ELEMENT						(76)			//2016.06.16 80改为76,608点
#define	BYTES_OF_HEAT_ELEMENT						(72)
#define	BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT			(0)				//2016.06.16 4改为0 
#define	BYTES_OF_RIGHT_EMPTY_HEAT_ELEMENT			(4)

#define	MAX_PRT_DOT_HEIGHT			(MAX_RATIO_Y * MAX_FONT_HEIGHT)	//最大打印高度点数,最大倍高×24点
#define	MAX_PRT_HEIGHT_BYTE			(MAX_PRT_DOT_HEIGHT / 8)				//最大打印高度字节数,如不能整除需要调整
#define BYTES_OF_LINE_PRT_BUF		(PRT_DOT_NUMBERS * MAX_PRT_HEIGHT_BYTE)	//行打印缓冲区大小
#define BYTES_OF_LPB				 BYTES_OF_LINE_PRT_BUF

/******* 定义一点行打印缓冲区结构 *************/
typedef struct
{
	uint16_t			ActiveDots;						        //需要加热点数 2016.06.15
	uint8_t				Buf[BYTES_OF_ALL_ELEMENT];	  //数据缓冲区 驱动一行字节数
} DotLineStruct;

/********** 定义驱动打印缓冲区 ***********/
typedef	struct
{
	uint32_t	Size;										          //缓冲区大小	2016.06.15
	DotLineStruct	DdotLine[DrvPrtBufDeep];			//缓冲区地址	   8*24点行  一行为
}PrintBuf_Struct;


typedef	struct
{
	volatile uint32_t	IdxWR;				//写打印缓冲索引/
	volatile uint32_t	BufHeight;		//缓冲中数据点行数											
	volatile uint32_t	IdxRD;				//读打印缓冲索引/
}PrintBufCtrl_Struct;


typedef struct
{
	uint16_t	PrtDataDotIndex;				//列计数索引
	uint16_t	CodeLineHigh;						//字符模式下的行高控制,按点计算,影响走行间距	
	uint16_t	PrtLeftLimit;						//打印区域左边距,与页模式采用不同的值,以横向移动单位计算
	uint16_t	PrtRightLimit;					//打印区域右边距,与页模式采用不同的值,以横向移动单位计算
	uint16_t	PrtWidth; 							//打印宽度,实际使用的打印区域
	uint16_t	PrtLength;							//总打印宽度= 左边距 + 打印宽度,包括左边的空白和规定的打印区域
	uint16_t	MaxPrtDotHigh;					//编辑/打印缓冲区内容以基线为基准算起,所有打印内容中高度最高的点数值
	
	uint8_t	PrtModeFlag;							//打印模式控制,	0 = 普通 and 1 = 页模式
	uint8_t	BitImageFlag;							//位图打印模式标志
	uint8_t	VariableBitImageFlag;			//可放大位图打印模式标志
	uint8_t	MaxLineHigh;							//一行中最高的字符/位图尺寸,一行字符等于字符宽度+字符右间距
#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	uint8_t	MaxRatioX;								//一行内最大横向放大倍数
#endif
	uint8_t	MaxRatioY;								//一行内最大纵向放大倍数	
	uint8_t	CodeAimMode;							//对齐方式,0=左对齐,1=中间对齐,2=右对齐
	uint8_t	PrintDirection;						//打印方向
#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
	uint8_t	CharNotFlag;							//字符反显的控制,高有效
#endif
#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
	uint8_t	EmphaSizeMode;
#endif
	uint8_t	CharCycle90;							//旋转90度打印标志,=1表示旋转90度打印,=0表示不旋转打印
	uint8_t	CharHeightMagnification;	//字符纵向放大倍数
	uint8_t	HZHeightMagnification;		//汉字纵向放大倍数
	uint8_t	PrintLineIndex;						//当前打印点行索引数
	uint8_t	DoUnderlineFlag;					//字符下划线控制,1=on; 0= off
	uint8_t	UnderlineMaxPrtHigh;			//一行编辑过程中出现的下划线最高的点数,范围0~3,据此确定打印时的取值	
	uint8_t	UnderlinePrintIndex;			//标志下划线点行数处理过程的变量,正向时从0到最高;反向时从最高到0
	uint8_t	UnderLinePrtOverFlag;
}TypeDef_StructPrtMode;

#define	LINE_PRT_BUF_LEN		(BYTES_OF_LINE_PRT_BUF + PRT_DOT_NUMBERS)
typedef	struct
{
	__IO uint8_t	Status;											//行打印缓冲区的状态
	TypeDef_StructPrtMode			PrtMode;	      //行打印控制参数
	//将下划线缓存与打印内容缓冲区合并,最后PRT_DOT_NUMBERS个字节作为下划线使用
	uint8_t	Buf[LINE_PRT_BUF_LEN];
}TypeDef_StructLinePrtBuf;

//定义打印缓冲区联合体 驱动和字符打印缓冲区
typedef union							//
{
	TypeDef_StructLinePrtBuf	g_tLinePrtBuf;	  	//字符打印缓冲区
	PrintBuf_Struct           DrivePrintBuf;	    //驱动打印缓冲区
}Union_PrtBuf;


//CPCL页模式 结构定义
#ifdef	CPCL_CMD_ENABLE
typedef struct
{
	uint8_t		FontType;						//字体号
	uint8_t		SizeNumber;						//字体大小
	uint8_t		Width;								//字符或汉字宽
	uint8_t		Height;								//字符或汉字高
	uint16_t	LeftSpace; 						//左间距
	uint16_t	RightSpace;						//右间距
 	uint8_t		WidthMagnification;		//横向放大倍数Ratio
	uint8_t		HeightMagnification;	//纵向放大倍数Ratio
	uint8_t		EmphaSizeMode;				//加粗模式
	uint8_t		Underline;						//字符下划线控制,bit3 = on/off; bit1~0 = 下划线样式（单或双线）
	uint16_t	RotateType;						//文字旋转角度设置，0：正常 1:90°2:180° 3:270°
}TypeDef_StructHZCharPara;

typedef struct
{
	uint8_t * PageBuf;
}TypeDef_StructPageEditPrtBuf;
#endif

#define	 MODEL_ID 				0x20				//80纸宽系列
#define  WRITE_START       1
#define  WRITE_END   			 2

typedef struct
{
	uint16_t   SpeakCnt;
	uint16_t SpeakBuf;	//1D f0指令用
	uint8_t SpeakBufFlg; //使用1D F0 指令标志
	uint8_t SpeakTempCntBuf; //时钟计数器，防止语音地址没有发送完时，退出指令，造成下次指令不正确
  uint8_t	 SpeakFlag;
	uint8_t  SpeakEnable;
	uint8_t  SpeakCtrl;
	uint8_t  SpeakElecCtrl;	
	uint8_t  SpeakPEnable;
	uint8_t  SpeakDoorOpen;
	uint8_t  SpeakWrite;
	uint8_t  SpeakWriteHandle;
	uint32_t SpeakWriteTime;
}TypeDef_StructSpeaker;


#endif /* DEFCONSTANT_H */

