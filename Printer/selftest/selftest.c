/**
	******************************************************************************
	* @file	  x.c
	* @author	Application Team  
	* @version V0.0.1
  * @date		2012-5-30
	* @brief   自检相关的程序.
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
#include	"selftest.h"
#include	"extgvar.h"
#include	"charmaskbuf.h"
#include	"feedpaper.h"
#include	"cutter.h"
#include	"esc.h"
#include	"debug.h"
#include	"mode.h"
#include "ETH_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//语言信息
const uint8_t * LanguageString[]	=
{
	"English", "GB18030", "BIG5"
};
u8  *PrtModeString[]= {"Normal Mode\n","Test PrintMode\n","5 Minutes PrintMode\n","20 Minutes PrintMode\n","50 Kilometres PrintMode\n","CutPaper TestMode\n"};

#define	LINE_SPLIT_BAR_STRING		"- - - - - - - - - - - - - - - - - - - - -\n"

extern void PrtAsciiToBuf(uint8_t Code);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void	PrintStartCommon(uint8_t cIndex);
static void	PrintPrinterSetting(void);
static void	PrintDirveSetting(void);
static void	PrintSpeedLevel(uint8_t cIndex);
static void	PrintPaperSetting(void);
static void	PrintPaperNearEndSet(uint8_t cIndex);
static void	PrintCodepageSetting(void);
static void	PrintBarCodeSetting(void);
static void	PrintStartTile(uint8_t cIndex);
static void	PrintModel(uint8_t cIndex);
static void	PrintSoftwareVersion(uint8_t cIndex);
static void	PrintInterfaceType(uint8_t cIndex);
static void PrintUsartParameterReal(uint8_t cIndex);
static void	PrintDensitySet(uint8_t cIndex);
static void	PrintPaperTypeSet(uint8_t cIndex);
static void	PrintTakePaperSensitiveSet(uint8_t cIndex);
static void	PrintPaperSensitiveSet(uint8_t cIndex);
static void	PrintDirectionSet(uint8_t cIndex);
static void	PrintCmdSystemSet(uint8_t cIndex);
static void	PrintCodepageSet(uint8_t cIndex);
static void	PrintBeepSet(uint8_t cIndex);
static void	PrintBlackmarkSet(uint8_t cIndex);
static void	CutPaperAfterPrint(void);
static void	PrintProductInfo(uint8_t cIndex);
void PrintNetParameterReal(uint8_t cIndex);
void PrtTestFun(void);
static void PrintBTParameterReal(uint8_t cIndex);
#ifdef	TEST_2MINUTE
static void	PrintTestMinute2(void);
#endif

#ifdef	TEST_5MINUTE
static void	PrintTestMinute5(void);
#endif

#ifdef	TEST_20MINUTE
static void	PrintTestMinute20(void);
#endif

#ifdef	TEST_50KM
static void	PrintTestKm50(void);
#endif

#ifdef	TEST_CUTTER_LIFTE
static void	PrintTestCutterLife(void);
#endif

/*******************************************************************************
* Function Name  : 函数名
* Description	  : 描述
* Input	  	  	: 输入参数
* Output  	  	 : 输出参数
* Return  	  	 : 返回参数
*******************************************************************************/

/******************************************************************
//函数名：SystemSelfTest()
//函数功能描述：按住FEED键时的打印自检输出
//输入参数：无
//输出参数：无
//补充说明：在调用本函数之前，必须对在调用GetConfigData函数之后。
*******************************************************************/
void	SystemSelfTest(void)
{
	uint8_t	cTemp;
	
	g_tRealTimeStatus.PrinterStatus |= 0x08;	//先进入离线状态
	g_tSystemCtrl.SelfTestMode = 1;	//自检打印模式
	SetComInBufBusy();
	
	DisableReceiveInterrupt();	//关接收中断
	GoDotLine((uint32_t)g_tPrtCtrlData.CodeLineHigh * 5);
	
	//以下部分为自检打印内容, 固定设置为英文打印
	cTemp = g_tPrtCtrlData.CodeAimMode;
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_MIDDLE;	//居中打印

	if(SysModeType == MODE_PRINT_TEST)		           //2016.07.15 打印测试模式
	{
		PrintString(PrtModeString[g_tSysConfig.PrintMode]);
	}		
	
	PrintStartCommon(0);		//先打印公司标示,软件版本号,机器型号,通信接口参数内容
	PrintPrinterSetting();
	PrintDirveSetting();
	PrintPaperSetting();
	PrintCodepageSetting();
	PrintBarCodeSetting();
	
	PrintString("INFO PRINT END\n");
	PrintString("\n");
	PrintString("\n");
	PrintString("\n");
	
	//自检打印结束
	#if !defined (TEST_2MINUTE) && !defined (TEST_5MINUTE) && !defined (TEST_20MINUTE) && !defined (TEST_50KM) && !defined (TEST_CUTTER_LIFE)
	if(SysModeType == MODE_PRINT_TEST && g_tSysConfig.PrintMode == 4 )		//2016.07.02  测试模式下的50公里测试不切纸
	{
		;
	}	
	else	
	{	
		CutPaperAfterPrint();				//打印完毕切纸
	}	
	#endif
	
	g_tSystemCtrl.SelfTestMode = 0;		//自检打印模式
	g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//打印完毕,自动脱离离线状态
	
	g_tFeedButton.KeyStatus = 0;	//按键无效
	g_tFeedButton.KeyCount = 0;		//清空之前所有的计数
	
	g_tPrtCtrlData.CodeAimMode = cTemp;
	
	ClearComInBufBusy();
	EnableReceiveInterrupt();		//开接收中断
}

static void	PrintStartCommon(uint8_t cIndex)
{
	PrintStartTile(cIndex);						//打印自检开头
	
	PrintModel(cIndex);								//型号
	PrintSoftwareVersion(cIndex);			//打印版本号
	
	PrintInterfaceType(cIndex);				//打印接口类型
	if (g_tInterface.COM_Type)
	{
		PrintUsartParameterReal(cIndex);	//打印串口参数
	}
	if (g_tInterface.ETH_Type)
	{
		PrintNetParameterReal(cIndex);	//打印网口参数
	}
	if(g_tInterface.BT_Type)					//2016.07.22打印蓝牙参数
	{
		PrintBTParameterReal(cIndex);
	}
	else if(g_tInterface.WIFI_Type)
	{
		Wifi_SelfTest();
		PrintString(LINE_SPLIT_BAR_STRING);
		PrintString("\n");
	}
}

static void	PrintPaperSetting(void)
{
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("PAPERSENSOR SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintPaperTypeSet(0);
	PrintPaperSensitiveSet(0);
	PrintTakePaperSensitiveSet(0);	
	PrintBlackmarkSet(0);
// 	PrintPaperNearEndSet(0);			//T801 没有纸将近 关闭此选项
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintBarCodeSetting(void)
{
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("BARCODE SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintString("BarCode1D: EAN8,EAN13,UPCA,UPCE,ITF25,\n");
	PrintString("CODE39,CODE93,CODABAR,CODE128\n");
	PrintString("BarCode2D: PDF417, QR CODE, DATA Matrix\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintCodepageSetting(void)
{
	uint8_t	i;
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("CODEPAGE SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintCodepageSet(0);
	
	for (i = 0x20; i < 0x80; i++)		//打印可打印的信息
	{
		PrintAscII(i);								//打印ASCII字符
		ProcessCharMaskReadBuf();
	}
	if(g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH )	//字符方式 2016.08.24
	{
		for(i=0x80;i<0xFF;i++)				//打印扩展ASCII字符
		{
			PrintAscII(i);
			ProcessCharMaskReadBuf();
		}
	}
	PrintString("\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintDirveSetting(void)
{
	const uint8_t	* StrCutter[] = 
	{
		"Cutter:  ",
		"Cutter:  ",
		"Cutter:  ",
	};
	
	const uint8_t * CutterStatus[]	=
	{
		"ON", "OFF"
	};
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("DRIVE SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintString(StrCutter[0]);
	PrintString(CutterStatus[0]);
	PrintString("\n");
	
	PrintBeepSet(0);
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintPrinterSetting(void)
{
	const uint8_t	* StrLanguage[] = 
	{
		"Language:  ",
		"Language:  ",
		"Language:  ",
	};
	const uint8_t	* StrFontSizeASCII[] = 
	{
		"FontDot:  ",
		"FontDot:  ",
		"FontDot:  ",
	};
	
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("PRINTER SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintCmdSystemSet(0);
	
	PrintString(StrLanguage[0]);
	if (g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH)
	{
		PrintString("English\n");
	}
	else if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
	{
		PrintString("简体中文\n");
	}
	else
	{
		PrintString("BIG5\n");
	}
	
	PrintString(StrFontSizeASCII[0]);
	if (g_tSysConfig.SysFontSize == 0)
	{
		PrintString("12*24\n");
	}
	else
	{
		PrintString("9*17\n");
	}
	
	PrintDirectionSet(0);
	PrintSpeedLevel(0);
	PrintDensitySet(0);
	PrintProductInfo(0);				//2016.05.31 打印产品信息
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

//自检打印速度等级
static void	PrintSpeedLevel(uint8_t cIndex)
{
	const uint8_t	* StrSpeedString[] = 
	{
		"Speed:  ",
		"Speed:  ",
		"Speed:  ",
	};
	//打印浓度信息
	const uint8_t * SpeedLel[] =
	{
		"250(mm/s)", "250(mm/s)", "250(mm/s)", "250(mm/s)"		//2016.07.30  在增加一项
	};
	
	PrintString(StrSpeedString[cIndex]);
	PrintString(SpeedLel[g_tSysConfig.MaxSpeed]);
	PrintString("\n");
}


static void	PrintDensitySet(uint8_t cIndex)
{
	const uint8_t	* StrPrintDensity[] = 
	{
		"Density:  ",
		"Density:  ",
		"Density:  ",
	};
	//打印浓度信息
	const uint8_t * DensityString[] =
	{
		"Low", "Medium", "High"
	};
	
	PrintString(StrPrintDensity[cIndex]);
	PrintString(DensityString[g_tSysConfig.Density]);
	PrintString("\n");
}

static void	PrintProductInfo(uint8_t cIndex)
{
	const uint8_t	* StrPrintProductInfo[] = 
	{
		"Product Info:  ",
		"Product Info:  ",
		"Product Info:  ",
	};

	PrintString(StrPrintProductInfo[cIndex]);
	PrintString(g_tSysConfig.ProductiveData);			//2016.05.31 打印产品信息
	PrintString("\n");
}


static void	PrintPaperTypeSet(uint8_t cIndex)
{
	const uint8_t	* StrPaperType[] = 
	{
		"PaperType:  ",
		"PaperType:  ",
		"PaperType:  ",
	};
	
	const uint8_t * StrPaperTypeSet[]	=
	{
		"Continuous paper", "BlackMark Paper", "Label Paper"  //2016.11.17
	};
	
	PrintString(StrPaperType[cIndex]);
	PrintString(StrPaperTypeSet[g_tSystemCtrl.PaperType]);
	PrintString("\n");
}

static void	PrintTakePaperSensitiveSet(uint8_t cIndex)
{
	const uint8_t	* StrTakePaperSensitive[] = 
	{
		"TakePaperSensor:  ",
		"TakePaperSensor:  ",
		"TakePaperSensor:  ",
	};
	//传感器灵敏度信息
	const uint8_t * SensorString[]	=
	{
		"Low", "Medium", "High"
	};
	const uint8_t	* StrTakePaperEndEnable[] = 
	{
		"TakePaperMode:  ",
		"取纸功能:  ",
		"TakePaperMode:  ",
	};
	const uint8_t * TakePaperEnableStr[]	=
	{
		"TakePaperOFF", "TakePaperON", "BillPintOverON"														//2016.07.30
	};
	PrintString(StrTakePaperEndEnable[cIndex]);
	PrintString(TakePaperEnableStr[g_tSysConfig.PaperTakeAwaySensorEnable]);    //取纸功能开关
	PrintString("\n");
	PrintString(StrTakePaperSensitive[cIndex]);
	PrintString(SensorString[g_tSysConfig.PaperTakeAwaySensorSensitive]);
	PrintString("\n");
}

static void	PrintPaperSensitiveSet(uint8_t cIndex)
{
	const uint8_t	* StrPaperSensitive[] = 
	{
		"PaperSensor:  ",
		"PaperSensor:  ",
		"PaperSensor:  ",
	};
	//传感器灵敏度信息
	const uint8_t * SensorString[]	=
	{
		"Low", "Medium", "High"
	};
	
	PrintString(StrPaperSensitive[cIndex]);
	PrintString(SensorString[g_tSysConfig.PaperSensorSensitive]);
	PrintString("\n");
}

static void	PrintPaperNearEndSet(uint8_t cIndex)
{
	//传感器使能信息
	const uint8_t	* StrPaperNearEndEnable[] = 
	{
		"PaperNearlyEnable:  ",
		"PaperNearlyEnable:  ",
		"PaperNearlyEnable:  ",
	};
	const uint8_t * EnableStr[]	=
	{
		"OFF", "ON"
	};
	
	//传感器灵敏度信息
	const uint8_t	* StrPaperNearEndSensitive[] = 
	{
		"PaperNearlySensor:  ",
		"PaperNearlySensor:  ",
		"PaperNearlySensor:  ",
	};
	const uint8_t * SensitiveStr[]	=
	{
		"Low", "Medium", "High"
	};
	
	PrintString(StrPaperNearEndEnable[cIndex]);
	PrintString(EnableStr[g_tSysConfig.PaperNearEndSensorEnable]);
	PrintString("\n");
	PrintString(StrPaperNearEndSensitive[cIndex]);
	PrintString(SensitiveStr[g_tSysConfig.PaperNearEndSensorSensitive]);
	PrintString("\n");
}

static void	PrintDirectionSet(uint8_t cIndex)
{
	//打印方向
	const uint8_t	* StrDirection[] = 
	{
		"PrintDirection:  ",
		"PrintDirection:  ",
		"PrintDirection:  ",
	};
	const uint8_t * PrintDirection[]	=
	{
		"Normal", "Reverse"
	};
	
	PrintString(StrDirection[cIndex]);
	PrintString(PrintDirection[g_tSysConfig.PrintDir]);
	PrintString("\n");
}

static void	PrintCmdSystemSet(uint8_t cIndex)
{
	const uint8_t	* StrCmd[] = 
	{
		"CommandType:  ",
		"CommandType:  ",
		"CommandType:  ",
	};
	//指令集
	const uint8_t * CommandSystemString[] =
	{
		"ESC&POS", "SPMP", "WHSP", "Other"
	};
	
	PrintString(StrCmd[cIndex]);
	PrintString(CommandSystemString[g_tSysConfig.CommandSystem]);
	PrintString("\n");
}

static void	PrintCodepageSet(uint8_t cIndex)
{
	//默认代码页,共56项
	const uint8_t	* StrCodepage[] = 
	{
		"Codepage:  ",
		"Codepage:  ",
		"Codepage:  ",
	};
	const uint8_t * CodePageString[]	=
	{
		"PC437", "KataKana", "PC850", "PC860", "PC863",
		"PC865", "WPC1251", "PC866", "MIK", "PC755", 
		"Iran", "Reserve", "Reserve", "Reserve", "Reserve", 
		"PC862", "WPC1252", "WPC1253", "PC852", "PC858", 
		"Iran II", "Latvian", "PC864", "ISO-8859-1", "PC737", 
		"WPC1257", "Thai", "PC720", "PC855", "PC857", 
		"WPC1250", "PC775", "WPC1254", "WPC1255", "WPC1256", 
		"WPC1258", "ISO-8859-2", "ISO-8859-3", "ISO-8859-4", "ISO-8859-5",
		"ISO-8859-6", "ISO-8859-7", "ISO-8859-8", "ISO-8859-9", "ISO-8859-15", 
		"Thai2", "PC856", "", "", "", 
		"", "", "", "", "", "",
	};
	
	PrintString(StrCodepage[cIndex]);
	PrintString(CodePageString[g_tSysConfig.DefaultCodePage]);
	PrintString("\n");
}

static void	PrintBeepSet(uint8_t cIndex)
{
	const uint8_t	* StrBeep[] = 
	{
		"Buzzer:  ",
		"Buzzer:  ",
		"Buzzer:  ",
	};
	const uint8_t * BuzzerString[]	=
	{
		"OFF", "Error ON", "Cut ON", "All ON"
	};
	
	PrintString(StrBeep[cIndex]);
	PrintString(BuzzerString[g_tSysConfig.BuzzerEnable]);
	PrintString("\n");
}

static void	PrintBlackmarkSet(uint8_t cIndex)
{
	//黑标模式信息
	const uint8_t	* StrBlackmarkPosition[] = 
	{
		"BlackMarkPosition:  ",
		"黑标传感器位置:  ",
		"BlackMarkPosition:  ",
	};
	const uint8_t * MarkPositionModeString[] =
	{
		"RightSide", "LeftSide"
	};
	const uint8_t	* StrBlackmarkSensitive[] = 
	{
		"BlackMarkSensor:  ",
		"黑标传感器灵敏度:  ",
		"BlackMarkSensor:  ",
	};
	const uint8_t * MarkSensitiveModeString[] =
	{
		"Low", "Medium", "High"
	};
	const uint8_t	* StrMarkEndEnable[] = 
	{
		"BlackMarkEnable:  ",
		"黑标功能:  ",
		"BlackMarkEnable:  ",
	};
	const uint8_t * MarkEnableStr[]	=
	{
		"OFF", "ON"
	};
	PrintString(StrMarkEndEnable[cIndex]);
	PrintString(MarkEnableStr[g_tSysConfig.BlackMarkEnable]);
	PrintString("\n");
	PrintString(StrBlackmarkPosition[cIndex]);
	PrintString(MarkPositionModeString[g_tSysConfig.BlackMarkPosition]);
	PrintString("\n");
	PrintString(StrBlackmarkSensitive[cIndex]);
	PrintString(MarkSensitiveModeString[g_tSysConfig.BlackMarkSensitive]);
	PrintString("\n");
}

//1. 打印自检开头
static void	PrintStartTile(uint8_t cIndex)
{
	const uint8_t	* StrSelfTestTitle[] = 
	{
		"SYSTEM INFORMATION\n",
		"SYSTEM INFORMATION\n",
		"SYSTEM INFORMATION\n",
	};
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString(StrSelfTestTitle[cIndex]);
	PrintString(LINE_SPLIT_BAR_STRING);
}

//3. 打印型号
static void	PrintModel(uint8_t cIndex)
{
	const uint8_t	* StrModel[] = 
	{
		"Model:  ",
		"Model:  ",
		"Model:  ",
	};
	
	PrintString(StrModel[cIndex]);
	PrintString(g_tSysConfig.Model);
	PrintString("\n");
}

//4. 打印版本号
static void	PrintSoftwareVersion(uint8_t cIndex)
{
	const uint8_t	* StrSoftwareVersion[] = 
	{
		"FirmWare:  ",
		"FirmWare:  ",
		"FirmWare:  ",
	};
	
	PrintString(StrSoftwareVersion[cIndex]);
	PrintString(FIRMWARE_STRING);
	PrintString("\n");
	PrintString("BFFW:  ");
  PrintString(BootVerStr);
  PrintString("-");                 //不打印字库版本
  PrintString(ZIKUVerStr);
  PrintString("\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

//5. 打印接口类型
static void	PrintInterfaceType(uint8_t cIndex)
{
	const uint8_t	* StrInterface[] = 
	{
		"INTERFACE:	 ",
		"INTERFACE:	 ",
		"INTERFACE:	 ",
	};
	const uint8_t	* StrParallel[] = 
	{
		"PARALLEL  ",
		"PARALLEL  ",
		"PARALLEL  ",
	};
	const uint8_t	* StrSerial[] = 
	{
		"RS232  ",
		"RS232  ",
		"RS232  ",
	};
	const uint8_t	* StrNone[] = 
	{
		"None",
		"None",
		"None",
	};
	
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintString(StrInterface[cIndex]);
	
	if (g_tInterface.LPT_Type)
	{
		PrintString(StrParallel[cIndex]); //打印并口
	}
	if (g_tInterface.COM_Type)
	{
		PrintString(StrSerial[cIndex]); 	//打印EIA电平RS232串口
	}
	if (g_tInterface.USB_Type)
	{
		PrintString("USB  "); 					//打印USB（CPU内带USB）
	}
	if (g_tInterface.ETH_Type)
	{
		PrintString("NET  "); 					//打印NET
	}
	if (g_tInterface.BT_Type)
	{
		PrintString("BT  "); 					//打印BT
	}
	if(g_tInterface.WIFI_Type)
	{
		PrintString("WIFI  "); 
	}
	if (!(g_tInterface.LPT_Type || g_tInterface.COM_Type || g_tInterface.USB_Type || g_tInterface.ETH_Type || g_tInterface.BT_Type))
	{
		PrintString(StrNone[cIndex]);		//打印没有安装端口
	}
	
	PrintString("\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
}

static void PrintUsartParameterReal(uint8_t cIndex)
{
	uint8_t	BitNumber;
	uint8_t	Parity;
	
	const uint8_t	* StrSerialPara[] = 
	{
		"USART Setting\n",
		"USART Setting\n",
		"USART Setting\n",
	};
	const uint8_t	* StrBuadRate[] = 
	{
		"Baud Rate:  ",
		"Baud Rate:  ",
		"Baud Rate:  ",
	};
	const uint8_t	* StrBitNumber[] = 
	{
		"Data bit:  ",
		"Data bit:  ",
		"Data bit:  ",
	};
	const uint8_t	* StrParity[] = 
	{
		" Parity:  ",
		" Parity:  ",
		" Parity:  ",
	};
	const uint8_t	* StrHandshaking[] = 
	{
		" Handshake:  ",
		" Handshake:  ",
		" Handshake:  ",
	};
	const uint8_t * USARTBaudRateString[] =
	{
		"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"
	};
	const uint8_t * USARTDataBitString[] =
	{
		"8 bit", "7 bit"
	};
	const uint8_t * USARTParityString[] = 
	{
		"None", "Odd", "None", "Even"
	};

	const uint8_t * USARTHandshakingString[] =
	{
		"RTS/CTS", "XON/XOFF"
	};
	
	BitNumber = (g_tSysConfig.ComProtocol & 0x08) >> 3;	//数据位数;=0,8位;=1,7位,
	Parity		= (g_tSysConfig.ComProtocol & 0x03);				//校验位设置;00,10无校验;11偶校验;01奇校验
	if ((BitNumber == 0x01) && ((Parity & 0x01) == 0x00))	//7位数据位+无校验
	{
		BitNumber--;	//显示8位数据位
	}
	
	PrintString(StrSerialPara[cIndex]);
	PrintString(StrBuadRate[cIndex]);			//波特率
	PrintString(USARTBaudRateString[g_tSysConfig.ComBaudRate & 0x07]);
	PrintString("\n");
	
	PrintString(StrBitNumber[cIndex]);		//打印数据位数
	PrintString(USARTDataBitString[BitNumber]);
	PrintString("\n");
	
	PrintString(StrParity[cIndex]);				//打印校验
	PrintString(USARTParityString[Parity]);
	PrintString("\n");
	
	PrintString(StrHandshaking[cIndex]);	//打印握手方式
	PrintString(USARTHandshakingString[(g_tSysConfig.ComProtocol & 0x04) >> 2]);
	PrintString("\n");
	PrintString(LINE_SPLIT_BAR_STRING);
}

void PrintNetParameterReal(uint8_t cIndex)
{
	uint32_t ultmp, ultmp1;
	char str[20];
	const uint8_t * DHCPEnableStr[]	=
	{
		"OFF  ", "ON  "
	};
	
	PrintString("NET Setting\n");
		//2017.02.07 自检中打印自动获取DHCP功能
	PrintString("DHCP:  ");		//MAC地址	
	PrintString(DHCPEnableStr[g_tSysConfig.DHCPEnable]);
	PrintString("\n");
	
	ultmp =ReadFlash(IP_FLASH_ADDR);
	sprintf(str,"%d.%d.%d.%d",
	(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
	PrintString("IP Address:");			  //IP地址
	PrintString(str);
	PrintString("\n");
	
	ultmp =ReadFlash(MASK_FLASH_ADDR);	
	sprintf(str,"%d.%d.%d.%d",
	(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
	PrintString("Sub Address:  ");		//MASK地址
	PrintString(str);
	PrintString("\n");
	
	ultmp =ReadFlash(GATE_FLASH_ADDR);
	sprintf(str,"%d.%d.%d.%d",
	(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
	PrintString("Gate Address:  ");		//GATE地址
	PrintString(str);
	PrintString("\n");
	
	ultmp =ReadFlash(EMAC_FLASH_ADDR);
	ultmp1 =ReadFlash((EMAC_FLASH_ADDR +4));
	sprintf(str,"%02x-%02x-%02x-%02x-%02x-%02x",
	ultmp &0xff, (ultmp>>8)&0xff, (ultmp>>16)&0xff, (ultmp>>24)&0xff,
	ultmp1 &0xff, (ultmp1>>8)&0xff);
	PrintString("MAC Address:  ");		//MAC地址
	PrintString(str);	
	PrintString("\n");
	PrintString(LINE_SPLIT_BAR_STRING);
// 	PrintString("\n");
}
/*******************************************************************************
* Function Name  : PrintBTParameterReal
* Description    : 打印蓝牙参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void PrintBTParameterReal(uint8_t cIndex)
{
	const uint8_t	* BTVerStr[] = 
	{
		"BT_Version:  ",
		"蓝牙版本:  ",
		"蓝牙版本:  ",
	};
	const uint8_t	* BTNameStr[] = 
	{
		"BT_Name:  ",
		"蓝牙名称:  ",
		"蓝牙名称:  ",
	};
	const uint8_t	* BTAddrStr[] = 
	{
		"BT_Addr:  ",
		"蓝牙地址:  ",
		"蓝牙地址:  ",
	};
	const uint8_t	* BTAuthStr[] = 
	{
		"BT_Auth:  ",
		"蓝牙密码使能:  ",
		"蓝牙密码使能:  ",
	};
	const uint8_t	* BTClassStr[] =   //2018.02.10
	{
		"BT_Class:  ",
		"蓝牙设备类型:  ",
		"蓝牙设备类型:  ",
	};
	const uint8_t	* BTPasswordStr[] = 
	{
		"BT_Password:  ",
		"蓝牙密码:  ",
		"蓝牙密码:  ",
	};
	const uint8_t	* EnableStr[] = 
	{
		"No",
		"否",
		"否",
		"Yes",
		"是"
		"是"
	};
	PrintString("BlueTooth Setting\n");
	/****打印蓝牙版本号****************/
	PrintString(BTVerStr[cIndex] );
	PrintString(BluetoothStruct.Version);

	/****打印蓝牙模块名称*****************/
	PrintString( BTNameStr[cIndex] );
	PrintString(BluetoothStruct.Name);
	//PrintString("\n");

	/****打印蓝牙地址*****************/
	PrintString( BTAddrStr[cIndex] );
	PrintString(BluetoothStruct.Laddr);
	PrintString("\n");

	/****打印蓝牙密码是否绑定*****************/
	PrintString( BTAuthStr[cIndex] );
	PrintString(EnableStr[3*BluetoothStruct.Auth+cIndex]); 
  PrintString("\n");
	
	if(BluetoothStruct.Auth == 1)
	{
		/****打印蓝牙配对密码*****************/
		PrintString( BTPasswordStr[cIndex] );
		PrintString(BluetoothStruct.Password);
// 	PrintString( "\n" );
	}  
		/****打印蓝牙设备类型*****************/  //2018.02.10
	PrintString( BTClassStr[cIndex] );
	PrintString(BluetoothStruct.Class);
// 	PrintString("\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}
//打印完毕切纸
static void	CutPaperAfterPrint(void)
{
	GoDotLine(CUT_OFFSET);						//走到切纸位置
	DriveCutPaper(CUT_TYPE_DEFAULT);	//半切纸
}

/*******************************************************************************
* Function Name  : WriterCPUFlash(uint32_t Addr, uint32_t *Buf, uint32_t Len,uint8_t Flag)
* Description    : 写CPU内部Flash
* Input          : Flag：0正常定时写，1关机（低压）时写，2指令写
* Output         : None
* Return         : None
*********************************************************************************/
void WriterCPUFlash(uint32_t Addr, uint32_t *Buf, uint32_t Len,uint8_t Flag)
{
	FLASH_Status status;
	uint32_t i,j;
	
	FLASH_Unlock();
	
	for(j=0; j<3; j++)
	{
		for(i=0; i<Len; i++)
		{
			status = FLASH_ProgramWord(Addr, Buf[i]);
			if(status != FLASH_COMPLETE || *((uint32_t*)Addr) != Buf[i])
			{	
				break;
			}
			Addr +=4;
		}
		if(Flag ==1 || i ==Len )			//掉电写入时不校验是否正确，由上电时校验
			break;
		else
		{
			Addr -= Addr%PAGE_SIZE;
			FLASH_ClearFlag(0x00000035);	//清除所有错误标志
			FLASH_ErasePage(Addr);
		}
	}
	FLASH_Lock();
		
	//加入错误报警
	if(j==3)
		g_tError.UR_ErrorFlag |= 0x20;		//bit5 CPU中Flash写入错误
	else
		g_tError.UR_ErrorFlag &= ~0x20;
}

/*******************************************************************************
* Function Name  : PrtTestFun
* Description    : 打印测试内容 2017.03.20
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrtTestFun(void)
{    
	uint8_t	i;
	
	Command_1C26();
	
	if(SysModeType == MODE_PRINT_TEST)		           //2016.07.15 打印测试模式
	{
		PrintString(PrtModeString[g_tSysConfig.PrintMode]);
	}	
	for(i=0;i<1;i++)
	{
		PrintString("012345678901234567890123456789012345678901234567\n");
		PrintString("abcdefghijklmnopqrstuvwxyz\n");
	}
	for(i=0;i<1;i++)
	{
		PrintString("国国国国国国国国国国国国国国国国国国国国国国国国\n");
	}
	
	Command_1C2E();        //进入英文模式
	
	for(i=0;i<48;i++)
	{
		PrtAsciiToBuf(0xC4);
	}
	PrintString("\n");
	for(i=0;i<24;i++)
	{
		PrtAsciiToBuf(0xC4);
		PrtAsciiToBuf(0x20);
	}
	PrintString("\n");

  for(i=0;i<24;i++)
	{
		PrtAsciiToBuf(0x20);
		PrtAsciiToBuf(0xDB);
	}
	PrintString("\n");
	for(i=0;i<24;i++)
	{
		PrtAsciiToBuf(0xDB);
		PrtAsciiToBuf(0x20);
	}
	PrintString("\n");
		
	GoDotLine(CUT_OFFSET);						//走到切纸位置
	DriveCutPaper(CUT_TYPE_DEFAULT);	//半切纸
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
