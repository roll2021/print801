/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-28
  * @brief   设置参数指令相关的程序.
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
#include	"setcommand.h"
#include	"spiflash.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"charmaskbuf.h"
#include	"defconstant.h"
#include	"uip.h"
#include  "cutter.h"
#include  "speaker.h"
#include "spi.h"
#include "sflash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t	m_bComBaudRate;
static uint8_t	m_bComProtocol;
extern void WriteFlashParameter(uint8_t Flag);
extern void	Update(void);

extern u8  *PrtModeString[];

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
* Function Name  : ReadSetByte(uint8_t *Parameter,uint8_t MaxValue)
* Description    : 读取设置的参数值，并判断是否有效，如有效存入相应的地址，
	否则，上传0Xff，表示错误
* Input          : Parameter：参数地址， MaxValue：参数的最大值
* Output         : None
* Return         : None
*******************************************************************************/
void ReadSetByte(uint8_t *Parameter, uint8_t MaxValue)
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if ((GetChar < MaxValue) || ((GetChar >= 0x30) && (GetChar < (MaxValue + 0x30))))
	{
		*Parameter = GetChar & 0xCF;	//允许输入的为ASCII代码的数字，如0x01，或0x31
	}
	else
	{
		GetChar = 0xFF;								//参数错误,上传0Xff
	}
	
	UpLoadData(&GetChar, 1);				//上传数据
}

/*******************************************************************************
* Function Name  : ReceiverString(uint8_t *Str ,uint8_t Length)
* Description    : 接收字符串
* Input          : Str:接收字符串地址，Length：字符串长度
* Output         : None
* Return         : None
*******************************************************************************/
void ReceiverString(uint8_t *Str, uint8_t Length)
{
 	uint8_t i;

	for (i = 0; i < Length; i++)
	{
		*Str = ReadInBuffer();
		Str++;
	}
	*Str = 0;
}


/*******************************************************************************
* Function Name  : SetCommand_1F12
* Description    : 设置测试打印模式
* Input          : None
* Output         : None
* Return         : None  
*******************************************************************************/
void SetCommand_1F12( void )
{
	  ReadSetByte(&g_tSysConfig.PrintMode, 6);
    PrintString("Print Test Mode：");   
    PrintString(PrtModeString[g_tSysConfig.PrintMode]);    
    PrintString("\r\n"); 
	  GoDotLine(CUT_OFFSET);			//走到撕纸位置 2016.05.31
}
//保存wifi设置
void CloseWiFICommand(void)
{
	uint8_t DataBuf[40]={0},Status=0,i=0;
	DelayMs(200);
	for(i=0;i<3;i++)
	{
		Status = WIFI_ReadCommand("AT+SAVE\r", DataBuf);
		if(Status == 0)
			break;
	}
	DelayMs(100);
	Status = WIFI_ReadCommand("AT+REBOOT\r", DataBuf);
	DelayMs(300);
}
/*******************************************************************************
* Function Name  : SetCommand_1F1F
* Description    : 退出设置模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F1F(void)
{
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//系统参数
	
//	if (g_tSysConfig.ComBaudRate != m_bComBaudRate)	//检查是否改变了
//		g_tSysConfig.ComBaudRate = m_bComBaudRate;
//	if (g_tSysConfig.ComProtocol != m_bComProtocol)
//		g_tSysConfig.ComProtocol = m_bComProtocol;
	
	if( g_tInterface.BT_Type)		//有蓝牙接口板
 	{
		CloseSetupBluetooth();				
	}
	else if(g_tInterface.WIFI_Type) 	  //2017.10.25
	{
		CLI();
		CloseWiFICommand();
		SEI();
	}
	if(1 == Set_BTWFflg)
	{
		Set_BTWFflg = 0;
		DriveCutPaper(CUT_TYPE_DEFAULT);	//半切纸
		
	}
	SysConfigUnionData.SysConfigStruct = g_tSysConfig;
	WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
	g_tSystemCtrl.SetMode = 0;
  DelayMs(200);						//延时
	WriteFlashParameter(2);		        // 将当前内容保存，防止掉电写入时错误 2016.05.31
  NVIC_SystemReset();			  				//复位
}

/*******************************************************************************
* Function Name  : SetCommand_1F18
* Description    : 设置语言
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F18(void)
{
	ReadSetByte(&g_tSysConfig.SysLanguage, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F28
* Description    : 设置指令集
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F28(void)
{
	ReadSetByte(&g_tSysConfig.CommandSystem, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F39
* Description    : 设置切刀、机芯类型,高4位设置切刀，00表示列切刀，低4位表示机芯类型
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F39(void)
{
	uint8_t GetChar, Temp;

	GetChar = ReadInBuffer();
	Temp = (GetChar & 0xf0) >> 4;
	if (Temp < 2)									//允许设置2种切刀
		g_tSysConfig.CutType = Temp;
	else
		GetChar = 0xFF;
	UpLoadData(&GetChar, 1);					//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F13
* Description    : 设置制造商名称，最长16字符
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F13(void)
{
	uint8_t GetChar;

	GetChar = ReadInBuffer();
	if (GetChar > 16)
		GetChar = 16;	
	ReceiverString(g_tSysConfig.Manufacturer, GetChar);
	UpLoadData(g_tSysConfig.Manufacturer, GetChar);		//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F14
* Description    : 设置打印机型号，最长16字符
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F14(void)
{
	uint8_t GetChar;

	GetChar = ReadInBuffer();
	if (GetChar > 16)
		GetChar = 16;	
	ReceiverString(g_tSysConfig.Model, GetChar);
	UpLoadData(g_tSysConfig.Model, GetChar);			//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F2F
* Description    : 设置打印机序列号，最长16字符
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F2F(void)
{
	uint8_t GetChar;

	GetChar = ReadInBuffer();
	if (GetChar > 16)
		GetChar = 16;	
	ReceiverString(g_tSysConfig.SerialNumber, GetChar);
	UpLoadData(g_tSysConfig.SerialNumber, GetChar);		//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F15
* Description    : 设置打印速度
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F15(void)
{
	ReadSetByte(&g_tSysConfig.MaxSpeed, 4);
}

/*******************************************************************************
* Function Name  : SetCommand_1F2E
* Description    : 设置蜂鸣器使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F2E(void)
{
	ReadSetByte(&g_tSysConfig.BuzzerEnable, 4);
}

/*******************************************************************************
* Function Name  : SetCommand_1F1A
* Description    : 设置打印宽度
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F1A(void)
{
	ReadSetByte(&g_tSysConfig.PrintWidthSelect, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F19
* Description    : 设置字体,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F19(void)
{
	ReadSetByte(&g_tSysConfig.SysFontSize, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F44
* Description    : 设置黑标模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F44(void)
{
	ReadSetByte(&g_tSysConfig.BlackMarkEnable, 3);			//0普通纸 1黑标纸 2标签纸
}
/*******************************************************************************
* Function Name  : SetCommand_1F49
* Description    : 设置取纸检测
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F49(void)
{
	ReadSetByte(&g_tSysConfig.PaperTakeAwaySensorEnable, 3);			//0关闭功能  1开启功能未取单 2开启切刀来单提示 
}
/*******************************************************************************
* Function Name  : SetCommand_1F4A
* Description    : 设置取纸检测灵敏度
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F4A(void)
{
	ReadSetByte(&g_tSysConfig.PaperTakeAwaySensorSensitive, 3);			//0 1 2为低中高
}
/*******************************************************************************
* Function Name  : SetCommand_1F2C
* Description    : 设置串口参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F2C(void)
{
	uint8_t GetChar, Temp;
	
	GetChar = ReadInBuffer();
	
	g_tSysConfig.ComBaudRate = GetChar & 0x87;
	Temp = (~GetChar & 0x18) >> 1;
	Temp |= ((~GetChar & 0x60) >> 5);
	g_tSysConfig.ComProtocol = Temp;
	
//	m_bComBaudRate = GetChar & 0x87;
//	m_bComProtocol = Temp;
	
	UpLoadData(&GetChar, 1);					//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F43
* Description    : 设置纸传感器灵敏度
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F43(void)
{
	ReadSetByte(&g_tSysConfig.PaperSensorSensitive, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F27
* Description    : 设置缺纸重打
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F27(void)
{
	ReadSetByte(&g_tSysConfig.PaperOutReprint, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F3B
* Description    : 设置0D回车指令是否有效
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F3B(void)
{
	ReadSetByte(&g_tSysConfig.CREnable, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F30
* Description    : 设置USB ID使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F30(void)
{
	ReadSetByte(&g_tSysConfig.USBIDEnable, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F3E
* Description    : 设置出厂日期
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F3E(void)		//2016.05.31
{
  uint8_t GetChar;

	GetChar = ReadInBuffer();
	if(GetChar > 3)		         // 四个三次 第一个为时间位数 默认为3， 年最后一位，月份两位 如：505
	{	
		GetChar = 3;
	}	
	ReceiverString(g_tSysConfig.ProductiveData,GetChar);
	UpLoadData(g_tSysConfig.ProductiveData, GetChar);		//上传数据
  PrintString("Production Date:");	
  PrintString(g_tSysConfig.ProductiveData);
  PrintString("\r\n");
  GoDotLine(CUT_OFFSET);			//走到撕纸位置
}

/*******************************************************************************
* Function Name  : SetCommand_1F17
* Description    : 设置打印方向
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F17(void)
{
	ReadSetByte(&g_tSysConfig.PrintDir, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F55
* Description    : 设置代码页
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F55(void)
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar < MAX_CODEPAGE_SET_NUM)
		g_tSysConfig.DefaultCodePage = GetChar;
	else
		GetChar = 0xFF;
	UpLoadData(&GetChar, 1);					//上传数据
}
/*******************************************************************************
* Function Name  : SetCommand_1F62
* Description    : 设置纸类型
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F62(void)   //2017.03.29
{
	uint8_t GetChar;
	
	ReadSetByte(&g_tSysConfig.PaperWith, 2);
}
/*******************************************************************************
* Function Name  : SetCommand_1FA1
* Description    : 恢复所有默认参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1FA1(void)   //2018.08.16
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if(GetChar == 0x01)
	{
		  RestoreDefaultParas();
			ClearComInBufBusy();
      GoDotLine(20);		        //走到切纸位置
      PrintString("Entered MODE_RESTORE_DEFAULT_PARAS\n\n\n\n\n\n\n\n\n");
      while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 );	//等待前点行加热完成
		  DelayMs(200);
			WriteFlashParameter(2);		        // 将当前内容保存，防止掉电写入时错误 2016.05.31  
      NVIC_SystemReset();			  				//复位
	}	
}

/*******************************************************************************
* Function Name  : SetCommand_1F42
* Description    : 升级程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F42( void )       
{
    uint8_t Temp[7];
    uint8_t i,PrintInterface;
    
    for(i=0; i<7; i++)
    {
        Temp[i] =  ReadInBuffer();
    }
    if( memcmp(Temp,"upgrade",7) == 0  )
    {    
        //PrintInterface = g_tInterface.COM_Type;
        Update();
    }
    else
    {

    }    
} 
/*******************************************************************************
* Function Name  : SetCommand_1F16
* Description    : 设置打印浓度
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F16(void)
{
	ReadSetByte(&g_tSysConfig.Density, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F60
* Description    : 设置DHCP功能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F60( void )
{
	ReadSetByte(&g_tSysConfig.DHCPEnable, 2);
}
/*******************************************************************************
* Function Name  : SetCommand_1F63
* Description    : 设置语音模块音量
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F63(void)
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if(GetChar > 0x03)
		return;
	
	if(0 == GetChar)
	{
		Line_2A_WTN588H_Dly(SPEAKERLOW);
	}
	else if(1 == GetChar)
	{
		Line_2A_WTN588H_Dly(SPEAKERMIDDLE);
	}
	else if(2 == GetChar)
	{
		Line_2A_WTN588H_Dly(SPEAKERHIGHT);
	}
	else if(SPEAKECLOSED == GetChar)  //关闭语音电路
	{
		g_tSpeaker.SpeakCtrl = 0;  //关闭语音
	}

	g_tSysConfig.SpeakerVolume = GetChar;
	if((g_tSpeaker.SpeakElecCtrl == 1)&&(GetChar<=2))
	{
		g_tSpeaker.SpeakCtrl = 1;
	}
		UpLoadData(&GetChar, 1);					//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F64
* Description    : 设置语音音律
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F64(void)
{
	uint8_t GetChar;
	uint16_t TempNum;
	
	GetChar = ReadInBuffer();
	if(GetChar >= 0 && GetChar <40)	
	{
		g_tSysConfig.AlarmNumber = 0;     //(GetChar+11);  //2018.06.23需要测试   打印取单音律选择
    g_tSysConfig.AlarmNumber = g_tSysConfig.AlarmNumber|GetChar;
		Line_2A_WTN588H_Dly(g_tSysConfig.AlarmNumber);

		UpLoadData(&GetChar, 1);					      //上传数据
	}	
}


/*******************************************************************************
* Function Name  : SetCommand_1F75
* Description    : 设置缺纸语音音律
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F75(void) 
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	
	if(GetChar >= 0 && GetChar <40)						      //缺纸音律在语音芯片
	{
		g_tSysConfig.POutVoiceNumber = 0;     //2018.06.23需要测试    放在了11的位置开始 也可以改为1开始 更具具体情况
    g_tSysConfig.POutVoiceNumber = g_tSysConfig.POutVoiceNumber|GetChar;
		Line_2A_WTN588H_Dly(g_tSysConfig.POutVoiceNumber);

		UpLoadData(&GetChar, 1);					//上传数据
	}	
}
/*******************************************************************************
* Function Name  : SetCommand_1F74
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//向588H内置flash中写语音内容
void SetCommand_1F74(void)
{
	uint8_t BuffCRC[2]={0},ceshi[256],cshi2[256];
	u32 size_1 =0,SectorAddress=0,i;
  uint8_t TempFlag;
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//系统参数
  TempFlag = 0;
	for(i=0; i<2; i++)
	{
		BuffCRC[i] = ReadInBuffer();
	}
	if((BuffCRC[0] == 0x55)&&(BuffCRC[1] == 0x31))
	{
		g_tSpeaker.SpeakWrite = 1;  //控制语音flash		
		Line_2A_WTN588H_Dly(0xFFFD);  //断开语音芯片内部flash
	}
	
	
	SPI_Initializes();      //2018.06.22  语音芯片的spi初始化则影响语音播放 只在下载语音时多io口初始化

  SFLASH_EraseChip();     //2018.06.22 擦除整片

	DelayMs(300);          //延时
	
  g_tSpeaker.SpeakWriteHandle = WRITE_START;
	while (1)
	{
		for (i=0; i < 256; i++)
		{
			cshi2[i] = ReadInBuffer();
			if (g_tSpeaker.SpeakWriteHandle == WRITE_END)
			{
				break;
			}
		}
		SFLASH_WriteNByte(cshi2, SectorAddress, i);
		DelayMs(1);
		SFLASH_ReadNByte(ceshi,SectorAddress,i);
		if(ceshi[0] !=cshi2[0] )
		{
			TempFlag = 1;
			break;
		}
		SectorAddress += 256;
		i = 0;
		
		if(g_tSpeaker.SpeakWriteHandle == WRITE_END)
			break;
	}
	if(TempFlag == 1)
	{
		UpLoadData("ERROR", 4);					//上传数据 2018.06.23 错误返回 上位机判断
	}
  else
  {		
		UpLoadData("OK", 2);					  //上传数据
		//来单地址回复到00H，缺纸地址恢复到01H
		g_tSysConfig.POutVoiceNumber = 1; 
		g_tSysConfig.AlarmNumber = 0;
		SysConfigUnionData.SysConfigStruct = g_tSysConfig;
		WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
	}	
	DelayMs(400);
	g_tSpeaker.SpeakWrite = 0;	//释放语音flash
	g_tSpeaker.SpeakWriteHandle = 0;
	g_tSystemCtrl.SetMode = 0;
	NVIC_SystemReset();			  				//复位
	
}
/*******************************************************************************
* Function Name  : SetCommand_1F34
* Description    : 下载位图
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	MAX_FLASH_BIT_BMP_BYTE		(64*1024 - 5)
void SetCommand_1F34(void)
{
	uint8_t	FlashBitBmpNum;
	uint32_t FlashBitBmpByte, SectorAddress, i;
	uint32_t	iMaxBitBmpByte;
	
	FlashBitBmpNum = ReadInBuffer();		//位图号
	g_tLineEditBuf[0] = FlashBitBmpNum;
	SectorAddress = BIT_BMP_DATA_BASE_ADDR + 0x10000 * FlashBitBmpNum;

	for (i = 1; i < 5; i++)						 //读取位图大小
		g_tLineEditBuf[i] = ReadInBuffer();
	
 	iMaxBitBmpByte = \
		(g_tLineEditBuf[1] + 256 * g_tLineEditBuf[2]) * (g_tLineEditBuf[3] + 256 * g_tLineEditBuf[4]);
	if (iMaxBitBmpByte > MAX_FLASH_BIT_BMP_BYTE)
		FlashBitBmpByte = MAX_FLASH_BIT_BMP_BYTE;
	else
		FlashBitBmpByte = iMaxBitBmpByte;
	
	//if (FlashBitBmpByte == 0 || FlashBitBmpNum > 7)	//无效位图参数
	if (FlashBitBmpNum > 7)		//无效位图参数
	{
		UpLoadData("ERROR", 5);					//上传数据
		return;
	}
	//SPI_FLASH_BlockErase( SectorAddress ); 	//删除对应扇区
	sFLASH_EraseBlock( SectorAddress ); 	//删除对应扇区
	
	while (FlashBitBmpByte > 0)
	{
		for (; i < 256; i++)
		{
			g_tLineEditBuf[i] = ReadInBuffer();
			FlashBitBmpByte--;
			if (FlashBitBmpByte == 0)
			{
				i++;
				break;
			}
		}
		sFLASH_WritePage(g_tLineEditBuf, SectorAddress, i);
		SectorAddress += 256;
		i = 0;
	}
	
	if (iMaxBitBmpByte > MAX_FLASH_BIT_BMP_BYTE)	//读取多余的数据丢弃
	{
		for (i = 0; i < (iMaxBitBmpByte - MAX_FLASH_BIT_BMP_BYTE); i++)
			ReadInBuffer();
	}
	
	UpLoadData("OK", 2);					//上传数据
}

/*******************************************************************************
* Function Name  : SetCommandFun
* Description    : 选择设置模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommandFun(void)
{
	uint8_t GetChar;
	uint16_t Len;
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//系统参数
	
	//读取FLASH中的参数
	Len = sizeof( g_tSysConfig );	
	sFLASH_ReadBuffer(SysConfigUnionData.bSysParaBuf, SYS_PARA_BASE_ADDR, Len);
	g_tSysConfig = SysConfigUnionData.SysConfigStruct;
	
	m_bComBaudRate = g_tSysConfig.ComBaudRate;
	m_bComProtocol = g_tSysConfig.ComProtocol;
	
	g_tSystemCtrl.SetMode = 1;
	if(g_tInterface.BT_Type)  		//蓝牙接口
 	{
		OpenSetupBluetooth();				
	}
	if(g_tInterface.WIFI_Type)				//2017.10.25
	{
			USART_ITConfig(WIFI_COM, USART_IT_RXNE, DISABLE);	
			WIFI_TranS_Config();                        //从透传——>指令模式
			USART_ITConfig(WIFI_COM, USART_IT_RXNE, ENABLE);	
	}   
	
	while (g_tSystemCtrl.SetMode)
	{
		GetChar = ReadInBuffer();
		if (GetChar == 0x1B)				//设置用ESC指令集 兼容的设置指令
		{
			GetChar = ReadInBuffer();
			if(g_tInterface.BT_Type)	  //为带蓝牙型机器
			{
				SetBluetooth(0X1B,GetChar);
			}
			else if(g_tInterface.WIFI_Type)	  //2017.10.25
			{
				SetWiFICommand(0X1B,GetChar);
			}
			switch( GetChar )
			{
				case 0x0A:			
				{
					SetCommand_1F12();		//设置测试程序类型 
					break;
				}
				case 0x15:			
				{
					SetCommand_1F1F();		//退出设置模式
					break;
				}
				case 0x16:			
				{
					SetCommand_1F18();		//设置语言
					break;
				}
				case 0x18:
				{
					SetCommand_1F28();		//设置指令集
					break;
				}
				case 0x19:			
				{
					SetCommand_1F39();		//设置切刀
					break;
				}
				case 0x1B:
				{
					SetCommand_1F13();		//设置制造商名
					break;
				}
				case 0x1C:
				{
					SetCommand_1F14();		//设置打印机型号
					break;
				}
				case 0x1D:
				{
					SetCommand_1F2F();		//设置打印机序列号
					break;
				}
				case 0x1E:
				{
					SetCommand_1F15();		//设置打印速度
					break;
				}
				case 0x1F:
				{
					SetCommand_1F2E();		//设置蜂鸣器
					break;
				}
				case 0x20:
				{
					SetCommand_1F1A();		//设置打印
					break;
				}
				case 0x21:
				{
					SetCommand_1F19();		//设置字体
					break;
				}
				case 0x22:
				{
					SetCommand_1F44();		//设置黑标模式
					break;
				}
				case 0x23:
				{
					SetCommand_1F2C();		//设置串口参数
					break;
				}
				case 0x28:
				{
					SetCommand_1F43();		//设置纸传感器灵敏度
					break;
				}
				case 0x2A:
				{
					SetCommand_1F27();		//设置缺纸重打
					break;
				}
				case 0x2B:
				{
					SetCommand_1F3B();		//设置0D指令使能
					break;
				}
				case 0x2C:
				{
					SetCommand_1F30();		//设置USBID使能
					break;
				}
				case 0x3C:
				{
					SetCommand_1F3E();		//设置 出厂日期 2016.05.31
					break;
				}
				case 0x63:
				{
					SetCommand_1F17();		//设置打印方向
					break;
				}
				case 0x74:
				{
					SetCommand_1F55();		//设置默认代码页
					break;
				}            
        case 0xFA:
				{
					SetCommand_1F42();		//进入升级模式
					break;
				}
				case 0xFD:
				{
					SetCommand_1F16();		//设置打印浓度
					break;
				}
				case 0xFE:
				{
					SetCommand_1F34();		//下载位图
					break;
				}

				default:
					break;
			}		
		}	
		else if (GetChar == 0x1F)						   //1F设置指令
		{
			GetChar = ReadInBuffer();
			if(g_tInterface.BT_Type)	  //为带蓝牙型机器
			{
				SetBluetooth(0X1F,GetChar);
			}
			else if(g_tInterface.WIFI_Type)	  //2017.10.25
			{
				SetWiFICommand(0X1F,GetChar);
			}
			switch( GetChar )
			{
				case 0x12:			
				{
					SetCommand_1F12();		//设置测试程序类型 
					break;
				}
				case 0x13:
				{
					SetCommand_1F13();		//设置制造商名
					break;
				}
				case 0x14:
				{
					SetCommand_1F14();		//设置打印机型号
					break;
				}
				case 0x15:
				{
					SetCommand_1F15();		//设置打印速度
					break;
				}
				case 0x16:
				{
					SetCommand_1F16();		//设置打印浓度
					break;
				}
				case 0x17:
				{
					SetCommand_1F17();		//设置打印方向
					break;
				}
				case 0x18:			
				{
					SetCommand_1F18();		//设置语言
					break;
				}
				case 0x19:
				{
					SetCommand_1F19();		//设置字体
					break;
				}
				case 0x1A:
				{
					SetCommand_1F1A();		//设置打印
					break;
				}
				case 0x1F:			
				{
					SetCommand_1F1F();		//退出设置模式
					break;
				}
				case 0x27:
				{
					SetCommand_1F27();		//设置缺纸重打
					break;
				}
				case 0x28:
				{
					SetCommand_1F28();		//设置指令集
					break;
				}
				case 0x2C:
				{
					SetCommand_1F2C();		//设置串口参数
					break;
				}
				case 0x2E:
				{
					SetCommand_1F2E();		//设置蜂鸣器
					break;
				}
				case 0x2F:
				{
					SetCommand_1F2F();		//设置打印机序列号
					break;
				}
				case 0x30:
				{
					SetCommand_1F30();		//设置USBID使能
					break;
				}
				case 0x34:
				{
					SetCommand_1F34();		//下载位图
					break;
				}
				case 0x39:			
				{
					SetCommand_1F39();		//设置切刀
					break;
				}
				case 0x3B:
				{
					SetCommand_1F3B();		//设置0D指令使能
					break;
				}				
				case 0x3E:
				{
					SetCommand_1F3E();		//设置 出厂日期 2016.05.31
					break;
				}
				case 0x42:
				{
					SetCommand_1F42();		//进入升级模式
					break;
				}
				case 0x43:
				{
					SetCommand_1F43();		//设置纸传感器灵敏度
					break;
				}
				case 0x44:
				{
					SetCommand_1F44();		//设置黑标模式
					break;
				}
				case 0x49:
				{
					SetCommand_1F49();		//设置取纸检测功能
					break;
				}
				case 0x4A:
				{
					SetCommand_1F4A();		//设置取纸检测功能
					break;
				}
				case 0x55:
				{
					SetCommand_1F55();		//设置默认代码页
					break;
				}    
				case 0x60:
				{
					SetCommand_1F60();		//设置默认代码页
					break;
				} 
				case 0x62:
				{
					SetCommand_1F62();		//设置默认打印纸类型 2017.03.29
					break;
				} 	
				case 0x63:
				{
					SetCommand_1F63();		//设置语音模块音量
					break;
				}
				case 0x64:
				{
					SetCommand_1F64();		//2018.06.23 设置打印音律种类
					break;
				}
				case 0x74:
				{
					SetCommand_1F74();		//写语音内容
					break;
				}
				case 0x75:
				{
					SetCommand_1F75();		//2018.06.23 设置缺纸音律种类
					break;
				}						
				default:
					break;
			}
		}
	}		
}

/*******************************************************************************
* Function Name  : SetCommandTcp
* Description    : 选择设置模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommandTcp(void)
{
	uint16_t Newipaddr[2];
	uint8_t GetChar;
	uint8_t Temp1,Temp2,Temp3,Temp4;
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//系统参数
	
	struct uip_eth_addr macaddr;
	uip_ipaddr_t ipaddr[3];
	char tmp[64];
	
	GetChar = ReadInBuffer();
	if(GetChar == 0x1f)
	{
		GetChar = ReadInBuffer();
		if(GetChar == 0x91)
		{
			GetChar = ReadInBuffer();
			if(GetChar == 0x00)
			{
				GetChar = ReadInBuffer();
				if(GetChar == 0x49)
				{
					GetChar = ReadInBuffer();
					if(GetChar == 0x50)
					{
						Temp1 = ReadInBuffer();   //新的ip地址 2017.02.07
						Temp2 = ReadInBuffer();
						Temp3 = ReadInBuffer();
						Temp4 = ReadInBuffer();								
						 //读入IP参数
						ReadNetworkAddr(ipaddr, &macaddr);            
						uip_ipaddr(Newipaddr, Temp1,Temp2,Temp3,Temp4);   //新ip写入uip_hostaddr
						uip_sethostaddr(Newipaddr);	
						uip_ipaddr(Newipaddr, Temp1,Temp2,Temp3,1);			  //新网关写入uip_draddr
						uip_setdraddr(Newipaddr);
						ipaddr[0][0] = uip_hostaddr[0];
						ipaddr[0][1] = uip_hostaddr[1];
						ipaddr[1][0] = uip_netmask[0];
						ipaddr[1][1] = uip_netmask[1];
						ipaddr[2][0] = uip_draddr[0];
						ipaddr[2][1] = uip_draddr[1];	
						//新的ip地址写入flash						
						WriteNetworkAddr(ipaddr, macaddr);
						//打印新的TCP地址
            PrintString(" Network Setting:\n");
					
						sprintf(tmp,"  IP Address:  %d.%d.%d.%d\n",
								(HTONS(ipaddr[0][0]) >>8) &0xff, HTONS(ipaddr[0][0]) &0xff,
								(HTONS(ipaddr[0][1]) >>8) &0xff, HTONS(ipaddr[0][1]) &0xff);
						PrintString((uint8_t*)tmp);

						sprintf(tmp,"  SubneMask:   %d.%d.%d.%d\n",
								(HTONS(ipaddr[1][0]) >>8) &0xff, HTONS(ipaddr[1][0]) &0xff,
								(HTONS(ipaddr[1][1]) >>8) &0xff, HTONS(ipaddr[1][1]) &0xff);
						PrintString((uint8_t*)tmp);

						sprintf(tmp,"  Gateway:     %d.%d.%d.%d\n\n",
								(HTONS(ipaddr[2][0]) >>8) &0xff, HTONS(ipaddr[2][0]) &0xff,
								(HTONS(ipaddr[2][1]) >>8) &0xff, HTONS(ipaddr[2][1]) &0xff);
						PrintString((uint8_t*)tmp);
						GoDotLine(CUT_OFFSETNUM);		//走到切纸位置  2017.02.07 
            DriveCutPaper(CUT_TYPE_PART);
						DelayMs(200);						//延时
            //关闭DHCP功能并写入flash参数区
            g_tSysConfig.DHCPEnable = 0;
						SysConfigUnionData.SysConfigStruct = g_tSysConfig;
						WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
						g_tSystemCtrl.SetMode = 0;
						DelayMs(200);						//延时
						NVIC_SystemReset();			//复位									
					}		
				}	
			}		
		}		
	}	
}	
/*******************************************************************************
* Function Name  : US_Command
* Description    : IP设置指令集
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void US_Command(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x11:
		{
			SetCommandFun();	      //进入设置系统参数
			break;
		}
		case 0x1b:								//1F1B设置IP地址
		{
			SetCommandTcp();	      //进入设置系统参数
			break;
		}
		default:
			break;
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
