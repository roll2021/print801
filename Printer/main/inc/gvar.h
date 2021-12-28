/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : .h
* @author  Application Team  
* @version V0.1
* @date    2016-3-18
* @brief   全局变量定义
********************************************************************************
* 
*******************************************************************************/
/*****	全部片内（48K字节）内存分配：
		1.行打印缓冲区：最大倍高×行高/3×行宽,g_tPrtCtrlData.PrtDataBuf[PRTBUFLENGTH]
		  片内：最多支持 8倍高 x24点高×576点。地址下对齐
		  最大：     		13824B 8×（24/8）×576
		2.下划线缓冲区：	576B   行宽
		3.用户自定义字符：	6175B   95×（2＋36＋27），分别记录95个 12 x 24、9×17 点阵的用户自定义字符
		4.下载位图缓存：	12KB
		5.用户定义宏：   	2KB(2048字节)
		6.垂直制表：		256B
		7.水平制表：	 	144B （576/8）×2
		8.用户定义光栅：	2K(2048字节)
		9.指令接收缓存：	8K（16384字节），g_tPrtCtrlData.PrtCodeBuf[MAXCODELENGTH]
		10.页模式打印缓冲区：128K，片外地址
*********/

#ifndef GVAR_H
#define GVAR_H

#include "defconstant.h"
#include "bluetooth.h"
/**********************************************************/
//	打印机系统参数设置部分：
TypeDef_StructSysConfig	g_ctSysDefaultValue = 
{
	0x87,		//uint8_t	ComBaudRate;		//串口波特率,0x83时为BPS9600,0x87时为115200
	0x00,		//uint8_t	ComProtocol;		//串口的数据协议,
	LANGUAGE_DEFAULT,		//uint8_t	SYS_Language;		//系统语言（使用索引，0=English;1=GB;2=BIG5）,默认为简体中文
 	ASCII_FONT_DEFAULT,	//uint8_t	SYS_FontSize;		//系统西文字体打大小（使用索引，0=12x24;1=9x17）
	
	#if defined	CUSTOM_LOTTERY_PAPER_ENABLE
		0x01,		//uint8_t 	PrintWidthSelect;  		//最大宽度设置,58mm固定
	#else
		0x00,		//uint8_t 	PrintWidthSelect;  		//最大宽度设置,为了兼容EPSON TM-88的宽度
	#endif
	
	0x01,		//uint8_t	Density;				//打印浓度设置(0=低,1=中,2=高)
	0x01,		//uint8_t	MaxSpeed;				//最高速度,0=150mm/s,1=190mm/s,2=220mm/s,3=250mm/s
	0x01,		//uint8_t	CutType;				//切刀标志
 	0x01,		//uint8_t	BuzzerEnable;			//蜂鸣器使能 2017.03.21
	0x00,		//uint8_t	PaperOutReprint;		//缺纸重新打印
	
	#if defined	CUSTOM_LOTTERY_PAPER_ENABLE
  0x01,		//uint8_t	BlackMarkEnable;	//黑标设置(0=关,1=开)
	#else
	0x00,		//uint8_t	BlackMarkEnable;	//黑标设置(0=关,1=开)  0普通纸 1黑标纸 2标签纸 0改为2 2016.03
	#endif
	
	0x00,		//uint8_t	RightMark;				    //黑标传感器位置，=0右边,=1左边，默认右边
 	0x01,		//uint8_t	PaperSensor;			    //测纸传感器灵敏度设置(0=低,1=中,2=高)
  0x01,		//uint8_t	BlackMarkSensor;	    //黑标传感器灵敏度设置(0=低,1=中,2=高)
 	0x00,		//uint8_t	SysShutDownMode;	    //系统关机模式（使用索引,0=从不;1=10分钟;2=20分钟;3=30分钟）
	0x00,		//uint8_t	PrintDir;				      //打印方向(0=正向,1=反向)
	0x00,		//uint8_t	CommandSystem;		    //命令集，0＝EPSON ESC/POS命令集，1＝WH 命令集，2＝IP命令集
	0x00,		//uint8_t DefaultCodePage; 	    //默认代码页
	0x00,		//uint8_t	CREnable;				      //0D回车指令使能
	
	COMPANY_NAME_STRING,	                  //uint8_t	Manufacturer[12];	//厂家
	MODAL_NAME_STRING,		                  //uint8_t	Model[12];			//型号
	"11101800002",                          //uint8_t	SerialNumber[21];	//序列号	
			
	0x00,		//uint8_t	USBIDEnable;		      //使能USBID
  0x00,		//uint8_t	USB_Class;			      //USB类型，0＝打印类，1＝VCP类
	
	0x00,   //uint8_t DHCPEnable;           //网卡DHCP协议 自动获取IP功能使能字节
	0x00,		//uint8_t	PaperWith;		        //2017.03.29 0为80纸 1为58纸
	
	0x00,		//uint8_t	PaperNearEndSensorEnable;			  //纸将近使能开关
	0x01,		//uint8_t	PaperNearEndSensorSensitive;		//纸将近传感器灵敏度
	0x02,		//uint8_t	PaperTakeAwaySensorEnable;			//取纸使能开关 默认为2 0关闭 1开启未取单报警 2来单报警
  0x01,		//uint8_t	PaperTakeAwaySensorSensitive;		//取纸传感器灵敏度
	
	"531",  //uint8_t ProductiveData[4];							//产品出厂日期 2016.05.31  
	0x01,		//uint8_t PrintMode;											//打印测试模式 2016.05.31  00为普通模式 01为2分钟一次自检打印 02为5分钟一次自检打印 03为20分钟一次自检打印 04为50KM老化试验 05 切刀测试
  0x02,   //SpeakerVolume;						//语音模块的音调设置  0，小音量   1 中音量      2大音量
	0x0000,   //AlarmNumber;							//切纸后音律选择
	0x0001,   //POutVoiceNumber           //缺纸音律选择
	PARA_CHECK_STRING,	//uint8_t	ConfigFlag[7];	//配置有效标志 
	
};

/********** 宏定义全局变量 ***********/
TypeDef_StructMacro			g_tMacro;

/********** 定义用户下载位图、自定义字符变量、二维码的联合全局变量 ***********/
TypeDef_UnionRAMBuffer	g_tRamBuffer;

/********** 一维条码参数全局变量 ***********/
TypeDef_StructBarCtrl		g_tBarCodeCtrl;

/********** 字符排版参数全局变量 ***********/
TypeDef_StructPrintMode	g_tPrtModeChar;
TypeDef_StructPrintMode	g_tPrtModeHZ;

/********** 打印机状态参数全局变量 ***********/
TypeDef_StructStatus		g_tRealTimeStatus;

/********** 打印机错误参数全局变量 ***********/
TypeDef_StructError				g_tError;
TypeDef_StructSysConfig		g_tSysConfig; 		//从Flash中读入的系统参数
TypeDef_StructPageMode 		g_tPageMode;			//页模式
TypeDef_StructKeyCtrl			g_tFeedButton;		//按键控制
TypeDef_StructSystemCtrl	g_tSystemCtrl;		//系统控制
TypeDef_StructPrtCtrl			g_tPrtCtrlData;		//打印控制
//TypeDef_StructBarCode2D		g_tBarCode2D;			//二维条码结构
TypeDef_StructDrawer  		g_tDrawer1,g_tDrawer2;	//钱箱结构
TypeDef_StructRealTime		g_tRealTimeCmd;		//实时指令结构
TypeDef_StructTab					g_tTab;						//跳格结构
TypeDef_StructInterfaceType	g_tInterface;		//接口类型结构

TypeDef_StructInBuffer volatile g_tInBuf;   //2016.07.08

uint8_t	g_tLineEditBuf[ PRT_BUF_LENGTH ];		//打印缓冲区
uint8_t	g_tUnderlineBuf[ PRT_DOT_NUMBERS ];	//下划线缓冲区
uint8_t	g_bSleepModeFlag;					   				//进入休眠标志
volatile uint8_t	g_bRoutinFlag;						//T2中断标志

PrintBufCtrl_Struct U_PrtBufCtrl;						//2016.06.15
Union_PrtBuf U_PrtBuf;                      //2016.06.16 打印缓冲区联合体

//g_StepCount上纸电机加速表中位置标志,初始化时清零,
//在加热中根据加热总时间调用 AdjustMotorStep()调整，只减不增或者不变，
//在 SystemTimer3Func()中走电机时递增,电机停止时清零。
//每个 打印行或者 GoDotLine()实现完整的加速过程，
//但是 打印行中加速位置可能随时根据加热总时间而调整。
volatile uint16_t	g_StepCount;
volatile uint16_t	g_HeatUs;							//加热一次的时间

uint8_t	g_bMotorRunFlag = 0;		//运行标志
uint8_t	g_bWriteFlashFlag = 0;	//SPIFLASH是否已经写过的标志
uint8_t	g_bRAMBitMapNumber = 1;	//RAM下载位图的图号
uint8_t	g_bFeedTaskFlag = 0;		//上纸任务完成标志

#ifdef	DBG_025
	uint8_t	g_bDbg025EnalbeFlag = 0;
	uint8_t	g_bDbg025FontFlag = 0;
#endif

#ifdef	MAINTAIN_ENABLE
	/******* 定义维护计数器*************/
	//g_tResetableCounter:可重置的可清零的计数值,
	//g_tCumulateCounter:累积的计数值,
// 	TypeDef_StructMaintenanceCounter g_tResetableCounter;
// 	TypeDef_StructMaintenanceCounter g_tCumulateCounter;
// 	TypeDef_StructMaintenanceCounter g_tResetableCounterBakeup;
  Maintenance_Counter R_Counter, C_Counter, R_CounterBK;
	volatile uint8_t	g_bMaintainRountinWriteFlag = 0;
#endif

#ifdef	CPCL_CMD_ENABLE
	TypeDef_StructHZCharPara	g_tPageModeCharPara;
	TypeDef_StructHZCharPara	g_tPageModeHZPara;
	TypeDef_StructPageEditPrtBuf	g_tPageEditPrtBuf;	//页编辑打印缓冲区
#endif

uc16	MaxStepNum[]={SPEED_LOW, SPEED_MIDDLE, SPEED_HIGH, SPEED_MAX};		//打印速度设定值 2016.05.31
//外部EXTSRAM的容量大小
uint32_t ExtSRamSize;

//标签缝隙有效标志
uint8_t LableEffectiveFlag;
//进纸标志
uint8_t FeedPaperFlag = 0;
//进纸找标志
uint8_t FeedToGapFlag = 0;
//自动上纸标志
uint8_t AutoFeedPaper = 0;
//boot程序版本
uint8_t	*BootVerStr = "NoBoot";	
//字库版本
uint8_t  ZIKUVerStr[5]; 
//最大加速步数
uint8_t  MAX_RUSH_STEPS = 94;		//默认速度190
//写flash标志
uint8_t  WriteFlashFlag = 0;
//16指令运行标志	2016.06.08
volatile uint8_t  Command16RunFlag = 0;
volatile uint32_t	FeedStepCount = 0;   //2016.06.11  T3中走纸步数记录
//加速标志	2016.06.16
volatile uint8_t  MotoFeedInc = 1;
//2016.07.02  打印机模式
uint8_t	SysModeType = 0;
//2016.07.10
volatile uint8_t T3_over =0;
volatile uint8_t SPI3_Over=0;
volatile uint8_t HeatOver;
//2016.07.15
uint8_t ManufacturerType;
//2016.08.20 二维码相关变量
uint8_t QR_version =0,PDF417_version=0;			     //版本
uint8_t QR_vColl =48 ,PDF417_vColl=0;						 //纠错等级
uint8_t PDF417_Rows =0,PDF417_ModuleWidth =3,PDF417_RowHeight=3,PDF417_options=0,PDF417_Rate=3;
uint8_t QR_model=2,QR_Rate=3;
uint16_t TwoCodeDataNum=0;

//2016.09.14 蜂鸣器控制参数
uint8_t		BuzzerRunFlag = 0;
uint8_t		BuzzerNum = 0;
uint16_t  BuzzerTime = 0;

//2017.02.07 DHCP获取标志
uint8_t		DhcpGetFlag = 0;
//2017.08.14 等待音律完成
uint8_t		VoiceOverflag = 0;
//2017.08.14 切刀允许标志 切刀后走纸
uint8_t		CutGoPaperflag = 0;
TypeDef_StructSpeaker  g_tSpeaker={0};
//2016.07.22 定义蓝牙结构体
Bluetooth_Struct    BluetoothStruct;		
uint8_t BtOnStatus = 0x55;     //2016.10.13 蓝牙存在未AA  不存在未55
USART_InitTypeDef   USART_BT_InitStructure;
uint8_t		BtTimeOut = 0;     //蓝牙超时标准
volatile  uint8_t		BtParaReloadFlag = 0;
uint8_t Set_BTWFflg = 0;
//2018.03.05 网口复位计数器
uint32_t  NetReloadCounter =0;
#endif /* GVAR_H */

/******************* (C) COPYRIGHT 20010 Sprinter *****END OF FILE****/
