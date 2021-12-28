/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : GlobalVariable.h
* @author  Application Team  
* @version V0.0.1
* @date    2012-5-28
* @brief   全局变量定义
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
//	全部片内（64K字节）内存分配：
//		1.指令接收缓存：16K（16384字节）,g_tPrtCtrlData.PrtCodeBuf[MAXCODELENGTH]
//
//		2.标准打印缓冲区：16K（16384字节）,g_tPrtCtrlData.PrtDataBuf[PRTBUFLENGTH]
//		  	片内：最多支持 512 x 256 点。（目前支持384 x 256）,地址下对齐
//
//		3.用户自定义字符：4K（4096字节），记录96个 12 x 24 点阵的用户自定义字符
//
//		4.下载位图缓存： 8K(8192 + 2字节)
//
//		5.用户定义宏：2K(2048字节)
//
//		6.用户定义光栅：2K(2048字节)
//
//		7.页模式打印缓冲区：128K，片外地址
//
#ifndef EXTGVAR_H
#define EXTGVAR_H

#include	"defconstant.h"
#include	"cominbuf.h"
#include	"bluetooth.h"
extern	TypeDef_StructSysConfig	g_ctSysDefaultValue;

/********** 宏定义全局变量 ***********/
extern	TypeDef_StructMacro	g_tMacro;

/********** 用户自定义字符全局变量 ***********/
extern TypeDef_UnionRAMBuffer	g_tRamBuffer;

/********** 一维条码参数全局变量 ***********/
extern	TypeDef_StructBarCtrl	g_tBarCodeCtrl;

/********** 字符排版参数全局变量 ***********/
extern TypeDef_StructPrintMode	g_tPrtModeChar;
extern TypeDef_StructPrintMode	g_tPrtModeHZ;

/********** 打印机状态参数全局变量 ***********/
extern TypeDef_StructStatus	g_tRealTimeStatus;

/********** 打印机错误参数全局变量 ***********/
extern TypeDef_StructError	g_tError;

/**********************************************************/
//	打印机系统参数设置部分：
extern	TypeDef_StructSysConfig			g_tSysConfig; 
extern	TypeDef_StructPageMode	 		g_tPageMode;
extern	TypeDef_StructKeyCtrl				g_tFeedButton;
extern	TypeDef_StructSystemCtrl		g_tSystemCtrl;
extern	TypeDef_StructPrtCtrl				g_tPrtCtrlData;
//extern	TypeDef_StructBarCode2D			g_tBarCode2D;			//二维条码结构
extern	TypeDef_StructDrawer  			g_tDrawer1,g_tDrawer2;				//钱箱结构
extern	TypeDef_StructRealTime			g_tRealTimeCmd;		//实时指令结构
extern	TypeDef_StructTab						g_tTab;						//跳格结构
extern	TypeDef_StructInterfaceType	g_tInterface;			//接口类型结构

extern	uint8_t	g_tLineEditBuf[ PRT_BUF_LENGTH ];			//打印缓冲区
extern	uint8_t	g_tUnderlineBuf[ PRT_DOT_NUMBERS ];		//下划线缓冲区
extern	uint8_t	g_bSleepModeFlag;					//进入休眠标志
extern	volatile uint8_t	g_bRoutinFlag;				   	//T2中断标志

extern	PrintBufCtrl_Struct U_PrtBufCtrl;	
extern	Union_PrtBuf U_PrtBuf;          //2016.06.16 打印缓冲区联合体

//g_StepCount上纸电机加速表中位置标志,初始化时清零,
//在加热中根据加热总时间调用AdjustMotorStep()调整，只减不增或者不变，
//在 SystemTimer3Func()中走电机时递增,电机停止时清零。
//每个打印行或者GoDotLine()实现完整的加速过程，
//但是打印行中加速位置可能随时根据加热总时间而调整。
extern	volatile uint16_t	g_StepCount;
extern	volatile uint16_t	g_HeatUs;				//加热一次的时间

extern	uint8_t	g_bMotorRunFlag;					//运行标志
extern	uint8_t	g_bWriteFlashFlag;				//SPIFLASH是否已经写过的标志
extern	uint8_t	g_bRAMBitMapNumber;
extern	uint8_t	g_bFeedTaskFlag;		//上纸任务完成标志

#ifdef	DBG_025
	extern	uint8_t	g_bDbg025EnalbeFlag;
	extern	uint8_t	g_bDbg025FontFlag;
#endif

#ifdef	MAINTAIN_ENABLE
	/******* 定义维护计数器*************/
// 	extern	TypeDef_StructMaintenanceCounter g_tResetableCounter;
// 	extern	TypeDef_StructMaintenanceCounter g_tCumulateCounter;
// 	extern	TypeDef_StructMaintenanceCounter g_tResetableCounterBakeup;
  extern	Maintenance_Counter R_Counter, C_Counter, R_CounterBK;
	extern	volatile uint8_t	g_bMaintainRountinWriteFlag;
#endif

//extern	uc16	MaxStep[];						//打印速度设定值
//extern	uc16	HeatTime[];  					//加热时间，单位为uS
//extern	uint8_t	MotorMoveCntr;				  //步距计数
//extern	uint8_t	InterfaceType;		      //接口类型 
//extern	uc16	MotorMoveSequence[8];	//步进电机控制序列
//extern	const uint32_t BpsValue[8];
//extern	uint32_t	ExtRamNumber;						//外部RAM容量
//extern	TypeDef_StructKeyCtrl		g_tResetEthButton;	//以太网按键控制
//extern	uc16 	ShutTime[4];					//自动关机时间,单位为秒，分10分钟，20分钟30分钟
/******* 定义打印缓冲区*************/
//extern	TypeDef_StructPrinter		PrintBuf;

/******* 定义输入缓冲区变量 *************/
//extern	TypeDef_StructInBuffer	gstructInBuf;
//extern	uint32_t	giCodebarPositionSpace;	//表示条码打印位置距离边沿的值.单位为横向点数.
//extern	uint8_t	CutType;					 			//是否切刀标志，0无切刀，1有切刀

#ifdef	CPCL_CMD_ENABLE
	extern TypeDef_StructHZCharPara	g_tPageModeCharPara;
	extern TypeDef_StructHZCharPara	g_tPageModeHZPara;
	extern TypeDef_StructPageEditPrtBuf	g_tPageEditPrtBuf;	//页编辑打印缓冲区;
#endif

extern	uint32_t ExtSRamSize;
extern  uint8_t LableEffectiveFlag;
extern  uint8_t FeedPaperFlag;
extern  uint8_t FeedToGapFlag;
extern  uint8_t AutoFeedPaper;
extern  uint8_t	*BootVerStr;
extern  uint8_t  ZIKUVerStr[5]; 
extern  uint8_t  MAX_RUSH_STEPS;
extern  uint8_t  WriteFlashFlag;
extern  volatile uint8_t  Command16RunFlag;
extern  volatile uint32_t	FeedStepCount; 		//2016.06.11
extern  volatile uint8_t  MotoFeedInc;
extern  uint8_t	SysModeType;
extern TypeDef_StructInBuffer volatile g_tInBuf;
//2016.07.10
extern volatile uint8_t T3_over;
extern volatile uint8_t SPI3_Over;
extern volatile uint8_t HeatOver;
extern uint8_t ManufacturerType;

//2016.08.20
extern uint8_t QR_version;
extern uint8_t PDF417_version;			     //版本
extern uint8_t QR_vColl;
extern uint8_t PDF417_vColl;						 //纠错等级
extern uint8_t PDF417_Rows;
extern uint8_t PDF417_ModuleWidth;
extern uint8_t PDF417_RowHeight;
extern uint8_t PDF417_options;
extern uint8_t PDF417_Rate;
extern uint8_t QR_model;
extern uint8_t QR_Rate;
extern uint16_t TwoCodeDataNum;

extern  uint8_t		BuzzerRunFlag;
extern  uint8_t		BuzzerNum;
extern  uint16_t  BuzzerTime;
extern  uint8_t		DhcpGetFlag;
extern  uint8_t		VoiceOverflag;
extern  uint8_t		CutGoPaperflag;
extern TypeDef_StructSpeaker  g_tSpeaker;
extern Bluetooth_Struct    BluetoothStruct;		
extern  uint8_t BtOnStatus;
extern  USART_InitTypeDef   USART_BT_InitStructure;
extern  uint8_t		BtTimeOut;
extern  volatile  uint8_t		BtParaReloadFlag;

extern uint8_t Set_BTWFflg;
extern  uint32_t  NetReloadCounter;

#endif
