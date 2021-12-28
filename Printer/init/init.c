/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-31
  * @brief   初始化相关的程序.
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
#include "init.h"
#include "reliable.h"
#include "usb_main.h"
#include "speaker.h"
#include "bluetooth.h"
#include "spi.h"
#include "sflash.h"
//加热机芯一点行所有的加热寄存器单元数
extern uint8_t	g_tbHeatElementBuf[BYTES_OF_ALL_ELEMENT];
extern TypeDef_StructInBuffer volatile g_tInBuf;
extern uint8_t	g_bLineEditBufStatus;
extern uc16	MaxStepNum[];


#ifdef	EXT_SRAM_ENABLE

#else
	uint8_t	bInBuf[MAX_BYTES_OF_IN_BUF];
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Static Funtion Declare ----------------------------------------------------*/
static void	InitCommonGPIO(void);
static void	InitStdPeriph(void);
static void	GetConfigData(void);
static void	InitBarCodeParameter(void);
static void	InitUserDefCodeParameter(void);
static void	InitPrintModeParameter(void);
static void	InitSystemCtrlStructParameter(void);
static void	InitRealTimeStructParameter(void);
static void	InitRealTimeStatusStructParameter(void);
static void	InitPrtCtrlDataParameter(void);
static void	InitTabStructParameter(void);
static void	InitPageModeStructParameter(void);
static void	InitSystemCtrlStructParameter(void);
static void	InitDrawerParameter(void);
static void	InitInbufParameter(void);
static void	InitParaOnlyTurnOn(void);
static void	InitInterfaceParameter(void);
static void	InitMacroStructParameter(void);
static void	CheckBT_WIFI(void);
#ifdef	CPCL_CMD_ENABLE
static void	InitPageModeCharPara(void);
static void	InitPageModeHZPara(void);
#endif
static void LEDPowerOn(void);
extern void uIPMain(void);
/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化,包括时钟,GPIO端口,接口检测,外设,中断,定时器,全局变量,
*	系统参数,切刀,上纸电机,接口设置
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#define	DEBUG       //2016.06.18  st link仿真需要打开  ulink不用； 否则定时器在断点不停止 打印异常；

void	Sys_Init(void)
{
	SystemInit();					//初始化时钟
	InitCommonGPIO();			//初始化GPIO端口
	
	#ifdef DEBUG		 		  //设置调试时定时器关闭
	{
		u32 *p;
		p= (u32*)0xE0042004;
		*p= 0x00043800;
	}
  #endif
	
	sFLASH_Init();				//初始化字库SPI
  //EXT_SRAM_Init();      //外部ram的初始化和配置
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	//检查通信接口类型并赋值全局变量,应该放置在变量初始化之后,端口初始化之前
	InitStdPeriph();			//初始化MCU的其他外部设备,包括串口,ADC,SPI,EXTI,ETH,USB,TIMx
  CheckBT_WIFI();				//检测模块类型  必须在设置中断前给模块类型赋值，以便后面设置中断优先级
	InitNVIC();						//设置系统所有使用的中断配置
	InitSysTick();				//2016.05.31 初始化滴答定时器
	LEDPowerOn();
	
	if(g_tSpeaker.SpeakCtrl == 1)
	{
		DelayMs(250);
		if(g_tSysConfig.SpeakerVolume == 0)
			Line_2A_WTN588H_Dly(SPEAKERLOW);
		else if(g_tSysConfig.SpeakerVolume == 1)
			Line_2A_WTN588H_Dly(SPEAKERMIDDLE);
		else if(g_tSysConfig.SpeakerVolume == 2)
			Line_2A_WTN588H_Dly(SPEAKERHIGHT);
		else if(g_tSysConfig.SpeakerVolume == SPEAKECLOSED)
			g_tSpeaker.SpeakCtrl = 0;
	}

	Init_BT();   //2017-10-09  添加蓝牙
	
	CLI();                 //关闭总中断 
	Init_WiFi();					//初始化wifi模块
	SEI();  
	
	StartTimer(ROUTINE_TIMER);	
	
	InitCutPaper();//WK	2016.11.24 把切刀初始化移到T2开启后 防止切刀错误后没开T2不能报警显示
	CutMotorDisable();	//芯片禁用
	CutMotorSetMode();	//刹车状态	
	
	SetComInBufBusy();					//先关闭通信端口
	DisableReceiveInterrupt();
}

/*******************************************************************************
* Function Name  : InitParameter
* Description    : 设置固定的打印方式,并使用默认的打印方式打印自检字符
* Input          : initflag=1为初始化指令（1B 40）
* Output         : 初始化所有全局变量
* Return		 : None
*******************************************************************************/
void	InitParameter(uint8_t initflag)
{
	GetConfigData();		//读入系统参数
	
	InitBarCodeParameter();						//条码数据结构初始化
	InitUserDefCodeParameter();				//用户自定义数据结构
	InitPrintModeParameter();					//字符和汉字排版数据结构
	//InitErrorStructParameter();				//错误数据结构
	InitSystemCtrlStructParameter();	//系统控制数据结构
	InitKeyCtrlStructParameter();			//按键数据结构
	InitRealTimeStructParameter();		//实时指令数据结构
	InitRealTimeStatusStructParameter();	//实时状态数据结构
	InitPrtCtrlDataParameter();				//打印控制数据结构
	InitTabStructParameter();					//跳格数据结构
	InitPageModeStructParameter();		//页模式数据结构	
	
	InitCharMaskBufParameter();				//初始化字库点阵读取缓冲区结构
	InitHeatElementBuf();							//初始化加热横向转换结果缓冲区
	InitHeatCtrlStruct();							//初始化加热控制结构
	
	memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);
	memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS);
	g_bLineEditBufStatus = LEB_STATUS_ILDE;

	g_tError.PaperEndStopSensorSelect = 0x02;		//选择纸传感器停止打印,缺纸一直有效
	g_tError.LPTPaperEndSensorSelect = 0x03;	  //选择纸传感器输出缺纸信号,纸将尽或者纸尽传感器任何一个检测到缺纸则输出PE信号
	g_tSystemCtrl.BillPrtFlag = 0x01;           //票据打印状态字初始状态，表示未打印过内容 2016.10.25  02改为01
	g_tPrtCtrlData.BMPBitNum = 0;								//2016.08.04
	
	if (!(initflag))
	{
		InitLinePrtBuf();							  //初始化行打印缓冲区 2016.06.21
		#ifdef	MAINTAIN_ENABLE
			ReadFlashParameter();				  //从Flash中读取维护读数器值
		#endif
		InitInbufParameter();						//在初始化指令时初始化输入缓冲区参数
		InitMacroStructParameter();			//初始化宏结构
		InitParaOnlyTurnOn();
	}
	
	#ifdef	CPCL_CMD_ENABLE
		InitPageModeCharPara();
		InitPageModeHZPara();
		
	#endif
}

/*******************************************************************************
* Function Name  : void	InitSysTick()
* Description    : 初始化滴答定时器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	InitSysTick(void)
{
		//初始化滴答定时器
		/* Setup SysTick Timer for 1 msec interrupts  */
  	if (SysTick_Config(SystemFrequency / 1000))
  	{ 
    	/* Capture error */ 
    	while (1);
  	}
}
/*******************************************************************************
* Function Name  : void	InitNVIC()
* Description    : 设置系统所有使用的中断配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	InitNVIC(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);			//设置向量表的位置和偏移,向量表在flash中,偏移0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置优先级分组：先占优先级和从优先级,先占优先级2位,从优先级2位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	//以下抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0
	
	//----配置DMA1的通道1中断(AD转换)----//
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
	
	//以下抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级1

	if (g_tInterface.COM_Type)
	{
		//----配置USART中断----//
		NVIC_InitStructure.NVIC_IRQChannel = USART_IRQ_NUMBER;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStructure);
	}
		if((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))
	{
		NVIC_InitStructure.NVIC_IRQChannel = BT_UART_IRQn;          //BT中断等级 2016.07.22
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_Init(&NVIC_InitStructure);
	}	
	//----配置外部中断（并口）----//
	if (g_tInterface.LPT_Type)
	{																																 //经测试，并口的优先级高 则驱动打印有白道， 需要把并口的优先级调到最后，有硬件置忙 ，并口中断和按键目前硬件为公用一个中断优先级
		NVIC_InitStructure.NVIC_IRQChannel = PARALLEL_STB_INT_IRQn;		 //此程序和按键公用的中断线，实际优先级为2-2 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	//----配置TIM2中断----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//T2优先级改为1-3，比T3T5高 防止走步函数中有错误 T2不能更新开盖状态
	NVIC_Init(&NVIC_InitStructure);
	
	//以下抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	//----配置TIM3中断----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		//电机
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	//----配置TIM5中断----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;		//打印数据处理 2016.06.15
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);	

  #if	1
	//----配置DMA2的通道2中断(SPI3发送)----//
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;	//加热数据发送 2016.06.21
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_Init(&NVIC_InitStructure);
  #endif	
	
	//以下抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	/* Enable the Ethernet global Interrupt */   //2017.07.28 网口结束数据终端
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
	
	
	//----配置TIM4中断----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		//打印
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);

	#if defined	(DBG_ADC) || defined	(DBG_DEAL_SPRT_DRV_ONE_LINE_TIME) || defined	(DBG_RX_TOTAL_THEN_PRT)
	//----配置TIM5中断----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	#endif
	#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
	//----配置TIM5中断----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	#endif
	
			
	#if defined	(FEED_BUTTON_CHECK_BY_INT) || defined (RESET_ETHERNET_BUTTON_CHECK_BY_INT)
	//----配置EXTI中断,包括上纸,以太网复位和纸仓盖----//
	NVIC_InitStructure.NVIC_IRQChannel = FEED_BUTTON_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	#endif	
	
	if (g_tInterface.USB_Type)
	{
			//----USB数据端点中断----//
		#ifdef USE_USB_OTG_HS   
			NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
		#else
			NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
		#endif

			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);  
	}
	
  //滴答定时器优先级最低为15优先级 即为3-3 ;为了冲突 3-3在此处不设置优先级;

}

void	InitErrorStructParameter(void)
{
	g_tError.PaperSensorStatusFlag = 0;		//纸传感器状态标志,位操作,高有效,bit.0=缺纸；bit.1= 纸将尽
	g_tError.AR_ErrorFlag = 0;			//可自动恢复错误标志,位操作,高有效,bit.0=打印头过热；bit.1=上盖
	g_tError.R_ErrorFlag = 0;				//可恢复错误标志,位操作,高有效,bit.0=切刀位置错误；bit.1=打印头原位错误（只有9针式有）
	g_tError.UR_ErrorFlag = 0;			//不可恢复错误标志,位操作,高有效,bit.0=内存读写错误；bit.1=过电压；bit.2=欠电压；bit.3=CPU执行错误；bit.4=UIB错误（EPSON）；bit.7=温度检测电路错误
	g_tError.R_ErrorRecoversFlag = 0;	//退出可恢复错误标志
}

void	InitKeyCtrlStructParameter(void)
{
	//按键数据结构
	g_tFeedButton.KeyEnable = 0;	//0 = 允许按键, 1= 禁止按键
	g_tFeedButton.KeyStatus = 0;	//按键有效标志,高有效
	g_tFeedButton.KeyCount = 0;		//FEED键读数
}

uint8_t	GetFeedButtonCount(void)
{
	return	(g_tFeedButton.KeyCount);
}

/*******************************************************************************
*	以下函数为本地函数,只包含所有共用的端口初始化内容.
*	包括:电源控制,机芯加热,钱箱,切刀,上纸,蜂鸣器,指示灯,纸仓盖,上纸按键,端口检测
*******************************************************************************/
static void	InitCommonGPIO(void)
{
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	#ifdef	WITHBOOT
		//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);		//关闭JTAG/SWJ功能
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	#else
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	#endif
	
	InitPowerCtrlPort();
	InitHeatPort();
	InitCutPort();
	InitFeedPort();
	InitBeepPort();
	PELedPortInit();
	ErrorLedPortInit();
	InitPlatenPort();
	InitFeedButtonPort();
	InitDrawerPort();
	InitSpeakerPort();
// 	CheckBT_WIF();
}

//SRAM的配置和初始化
static void	EXT_SRAM_Init(void)
{	
    InitExtSRAM();   
    
    #ifndef	DBG_EXT_SRAM
        Test_Ext_Sram();           //外部ram的测试等
    #endif
}


static void	Test_Ext_Sram(void)
{	
    uint8_t	    Result;
	
    Result = 0;
    DelayMs(100);
				
    ExtSRamSize = GetExtSRAMLen();          //测试得到外扩sram大小
    if(ExtSRamSize)
    {    
        Result = TestReadWriteExtSRAM();
	
    }
    if (Result == 0)
    {
        g_tError.UR_ErrorFlag &= ~0x01;   	//bit.0 = 0
    }
    else if (Result == 1)
    {
        g_tError.UR_ErrorFlag |= 0x01;   	//bit.0 = 1, 扩展SRAM错,高有效 
    }	
    
	DelayMs(100); 
}

static void	InitStdPeriph(void)
{
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	sHeat_Init();						//初始化机芯SPI,注意机芯SPI初始化要放到机芯DMA初始化之前,否则错误
	sHeat_DMAInit();				//设置机芯SPI传送数据MDA
	
	CheckInterface();			  //检查通信接口类型并赋值全局变量,应该放置在变量初始化之后,端口初始化之前 2016.07.12

	if (g_tInterface.LPT_Type)	//并口配置
	{
		InitParallelDataPort();
		InitParallelCtrlPort();
	}
	
	if (g_tInterface.COM_Type)	//异步串口配置
	{
		ConfigUsart();
		InitUsartPort();
	}

				
	if (g_tInterface.USB_Type)	//初始化USB接口 2016.09.14 
	{
    USB_Port_Init();				  //初始化USB的IO口USB_DISCONNECT_PIN
		USB_Main();
		DelayUs(1000000);		      //延时10ms
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);		//2016.07.21  初始化完USB后 链接D+上拉电阻
	}
	
	if(g_tInterface.ETH_Type)  //初始化ETH接口
	{							
		uIPMain();
	}
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//设置T4为系统时标,100uS,处理行打印缓冲区的打印,一直运行,周期性检查网口信息 周期1ms
	TimerConfigPrescaler(ETH_DATA_TIMER, 10000);
	SetTimerCounterWithAutoReload(ETH_DATA_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);  //2016.07.07
		
	#if defined	DBG_ADC || defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME || defined	DBG_RX_TOTAL_THEN_PRT
		//设置T5为备用定时器,预分频为1000000,1uS,备用
		TimerConfigPrescaler(BACKUP_TIMER, 1000000);
	#endif
	
	#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
		//设置T5为备用定时器,预分频为1000000,100uS,可靠性测试使用
		TimerConfigPrescaler(BACKUP_TIMER, 10000);
	#endif
	
	//设置T5为us定时器,预分频为1000000,1uS, 2016.06.14 用于送机芯前数据处理  保证处理数据时不受其他中断干扰造成的驱动打印时处理时间过长电机停止、打印卡顿问题
	TimerConfigPrescaler(DealPrtData_TIMER, 1000000);	//1000000为1us 2016.06.16 改为1us
	
	
// 	#ifdef	DBG_PROCESS_TIME
// 		InitSystick();
// 	#endif
}
/*******************************************************************************
* Function Name  : void CheckBT_WIFI(void)
* Description    : 检测无线模块类型	 和模块是否存在            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	CheckBT_WIFI(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t Temp;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		GPIO_InitStructure.GPIO_Pin		=	BT_WIFI_TYPE;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  		
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(BT_TYPE_PORT, &GPIO_InitStructure);	
		Temp = GPIO_ReadInputDataBit(BT_TYPE_PORT, BT_WIFI_TYPE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   			//下拉输入
		GPIO_Init(BT_TYPE_PORT, &GPIO_InitStructure);
		DelayUs(40);
		if(Temp != GPIO_ReadInputDataBit(BT_TYPE_PORT, BT_WIFI_TYPE))     //不相等为串口，外部悬空 没接无线模块 
		{
	    g_tInterface.BT_Type  = 0;
			g_tInterface.WIFI_Type  = 0;
		}
		else  //有接上拉或下拉
		{	
			GPIO_InitStructure.GPIO_Pin		=	BT_WIFI_TYPE;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  		
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(BT_TYPE_PORT, &GPIO_InitStructure);
			Temp = GPIO_ReadInputDataBit(BT_TYPE_PORT, BT_WIFI_TYPE);
			if(Temp)										//2016.09.12 模块上BT_wifi 引脚上拉为BT模块 下拉为WIFI模块 
			{
				g_tInterface.BT_Type  = 1;
			}	
			else
			{
				g_tInterface.WIFI_Type  = 1;
			}		
		}		
		if(g_tInterface.BT_Type  == 1)
		{	
			InitBluetoothPort();					                       //初始化蓝牙或wifi引脚
			BT_RST_HIGH;									//复位蓝牙模块
			BT_BUSY_CLR;				          //清蓝牙模块接口忙 
		}	
		else if(g_tInterface.WIFI_Type  == 1)
		{	
			WIFI_Config_Gpio();
		}
			
// 		Judge_BT_Type();						  //后续完善

}
/*******************************************************************************
* Function Name  : void	GetConfigData(void)
* Description    : 从串行Flash中读系统设置,如没有系统设置,则写入默认的系统设置
* Input          : 无
* Output         : 初始化系统参数数据结构
* Return         : None
*******************************************************************************/
static void	GetConfigData(void)
{
	TypeDef_UnionSysConfig	SysConfigUnionData;		//系统参数	
	uint16_t Len;
	uint8_t	i;
	volatile FLASH_Status FLASHStatus;
    
	Len = sizeof(g_tSysConfig);
  BluetoothStruct.reInitFlag = 0;      
  Flash_Inside_ReadByte(SysConfigUnionData.bSysParaBuf,FLASH_INSIDE_PARA_ADDR, Len);   
    //查看内部flash参数的合法性
	if( strcmp((char *)SysConfigUnionData.SysConfigStruct.ConfigFlag, PARA_CHECK_STRING) != 0 )	
  {							

		memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
		g_tSysConfig = SysConfigUnionData.SysConfigStruct;
		FLASH_Unlock();    
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  
		FLASHStatus = FLASH_ErasePage(FLASH_INSIDE_PARA_ADDR); 
		if (FLASHStatus == FLASH_COMPLETE)                      
		{   
				FLASHStatus = Flash_Inside_BufWrite(SysConfigUnionData.bSysParaBuf, FLASH_INSIDE_PARA_ADDR, Len);
		}
		FLASH_Lock();  
		//内部参数不对则重新内部和外部参数区      
		memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
		WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, Len);
		BluetoothStruct.reInitFlag = 1;       //重新初始化蓝牙标志
  }
  else
  {           
		for(i=0; i<8; i++)
		{
				sFLASH_ReadBuffer(SysConfigUnionData.bSysParaBuf, SYS_PARA_BASE_ADDR, Len);
				if ((strcmp((const char *)SysConfigUnionData.SysConfigStruct.ConfigFlag, PARA_CHECK_STRING)) == 0)
				{    
						break;
				}    
				DelayUs(8000);				//延时10mm
		}

		if(8 ==i)		//2012.12.10
		{
				memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
				WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, Len);
		}
  }
	g_tSysConfig = SysConfigUnionData.SysConfigStruct;
    
	sFLASH_ReadBuffer(ZIKUVerStr, ZK_Ver_ADDR, 4);
	if(ZIKUVerStr[0] == 0xFF)   
	{
			strcpy(ZIKUVerStr,"0.01");
	}    
	else
	{    
			ZIKUVerStr[4] = 0;
	}    
}

void    RestoreDefaultParas(void)   //恢复默认参数
{
	TypeDef_UnionSysConfig	SysConfigUnionData;		//系统参数	
	uint16_t Len;
	volatile FLASH_Status FLASHStatus;
    
    Len = sizeof(g_tSysConfig);
    
    //内部参数区恢复默认参数
    memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
    g_tSysConfig = SysConfigUnionData.SysConfigStruct;
    FLASH_Unlock();    
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  
    FLASHStatus = FLASH_ErasePage(FLASH_INSIDE_PARA_ADDR); 
    if (FLASHStatus == FLASH_COMPLETE)                      
    {   
        FLASHStatus = Flash_Inside_BufWrite(SysConfigUnionData.bSysParaBuf, FLASH_INSIDE_PARA_ADDR, Len);
    }
    FLASH_Lock();  
    //外部参数区 恢复默认参   
    memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
    WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, Len);
     
    g_tSysConfig = SysConfigUnionData.SysConfigStruct;
}


static void	InitBarCodeParameter(void)
{
	g_tBarCodeCtrl.HRIPosition = 0;				//默认HRI打印位置,不打印
	g_tBarCodeCtrl.HRIFont = ASCII_FONT_DEFAULT;	//默认HRI字体,12×24
	g_tBarCodeCtrl.BarCodeHigh = 60;			//默认条码高度为60个点
	g_tBarCodeCtrl.BarCodeThinWidth = 2;		//默认条码窄条模块宽度为2 0.25mm
	g_tBarCodeCtrl.BarCodeThickWidth = 5;	//默认条码宽条模块宽度为5 0.625mm
}

static void	InitUserDefCodeParameter(void)
{
	//用户自定义数据结构
	g_tSystemCtrl.UserSelectFlag = 0;			//用户自定义
	memset(g_tRamBuffer.UserCharStruct.UserCode_12, 0x0 ,USER_ZF_SIZE);	//清12×24自定义字符代码区
	memset(g_tRamBuffer.UserCharStruct.UserCode_09, 0x0 ,USER_ZF_SIZE);	//清9×17自定义字符代码区
	memset(g_tRamBuffer.UserCharStruct.UserCode_HZ, 0x0 ,USER_HZ_SIZE);	//清自定义汉字代码区
#if	0	
	memset(g_tRamBuffer.UserCharStruct.UserCode_12, 0x0,\
		sizeof(g_tRamBuffer.UserCharStruct.UserCode_12) / sizeof(g_tRamBuffer.UserCharStruct.UserCode_12[0]));	//清12×24自定义字符代码区
	memset(g_tRamBuffer.UserCharStruct.UserCode_09, 0x0,\
		sizeof(g_tRamBuffer.UserCharStruct.UserCode_09) / sizeof(g_tRamBuffer.UserCharStruct.UserCode_09[0]));	//清9×17自定义字符代码区
	memset(g_tRamBuffer.UserCharStruct.UserCode_HZ, 0x0,\
		sizeof(g_tRamBuffer.UserCharStruct.UserCode_HZ) / sizeof(g_tRamBuffer.UserCharStruct.UserCode_HZ[0]));	//清自定义汉字代码区
#endif
}

static void	InitPrintModeParameter(void)
{
	//字符排版数据结构
	g_tPrtModeChar.HeightMagnification = 1;	//纵向放大倍数Ratio
	g_tPrtModeChar.WidthMagnification = 1;	//横向放大倍数Ratio
	g_tPrtModeChar.EmphaSizeMode = 0;	//加粗模式
	g_tPrtModeChar.Underline = 1;			//字符下划线控制,bit3=on/off;bit1~0=下划线样式（单或双线）,默认1点
	g_tPrtModeChar.LeftSpace = 0;			//字符左间距
	g_tPrtModeChar.RightSpace = 0;		//字符右间距
	
	//汉字排版数据结构
	g_tPrtModeHZ.HeightMagnification = 1;	//纵向放大倍数Ratio
	g_tPrtModeHZ.WidthMagnification = 1;	//横向放大倍数Ratio
	g_tPrtModeHZ.EmphaSizeMode = 0;	//加粗模式
	g_tPrtModeHZ.Underline	= 1;		//字符下划线控制,bit3=on/off;bit1~0=下划线样式（单或双线）,默认1点
	g_tPrtModeHZ.LeftSpace	= 0;		//汉字左间距
	g_tPrtModeHZ.RightSpace	= 0;		//汉字右间距	
  g_tPrtModeHZ.CharWidth	= HZ_FONT_A_WIDTH;	//汉字宽度
	g_tPrtModeHZ.CharHigh		= HZ_FONT_A_HEIGHT;	//汉字高度
}

static void	InitSystemCtrlStructParameter(void)
{
	// 系统控制数据结构
	g_tSystemCtrl.HexMode = 0;			//十六进制打印模式
	g_tSystemCtrl.SelfTestMode = 0;	//自检打印模式
	
#if	1
	//国际字符默认设置与语言选择有关.中文机型如果选择了中文,默认值为15,否则为0.
	if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
		g_tSystemCtrl.CharacterSet = INTERNATIONAL_CHAR_SET_INDEX_CHINESE;	//默认中文
	else
		g_tSystemCtrl.CharacterSet = INTERNATIONAL_CHAR_SET_INDEX_ENGLISH;	//默认英文
#else
	g_tSystemCtrl.CharacterSet = INTERNATIONAL_CHAR_SET_INDEX_ENGLISH;	//默认英文
#endif
	
	if (g_tSysConfig.BlackMarkEnable == 1)	//确定黑标使能设置, 0=关,1=开
	{	//纸类型,0 = 连续纸 1 = 标记纸
		g_tSystemCtrl.PaperType = PAPER_TYPE_BLACKMARK;
	}
    else if (g_tSysConfig.BlackMarkEnable == 2)	//确定黑标使能设置, 0 普通纸 1黑标 2标签纸 2016.05
	{	//纸类型,0 = 连续纸 1 = 标记纸 2标签纸
		g_tSystemCtrl.PaperType = PAPER_TYPE_LABLE;
	}
	else
	{
		g_tSystemCtrl.PaperType = PAPER_TYPE_CONTINUOUS;
	}
    g_tSystemCtrl.CMDType = CMD_POS;       
    
	g_tSystemCtrl.PrtDeviceDisable = 0;				//打印机允许、禁止控制,"1“禁止
	g_tSystemCtrl.PrtModeFlag = STD_MODE;	//打印模式控制 ,0 = 普通 and 1 = 页模式
	g_tSystemCtrl.SleepCounter = 0;				//系统进入休眠计数器
	g_tSystemCtrl.ShutDownCounter = 0;		//系统进入关机的时间
	
	if(g_tSysConfig.PaperWith == 1)				//2017.03.29
	{	
		g_tSystemCtrl.LineWidth = CUSTOM_57MM_PRT_DOT_NUMBERS;
	}	
	else
	{	
		g_tSystemCtrl.LineWidth = PRT_DOT_NUMBERS - g_tSysConfig.PrintWidthSelect * EPSON_DOTNUMBER;
	}
}

static void	InitRealTimeStructParameter(void)
{
	//实时指令数据结构
	g_tRealTimeCmd.RealTimeEnable = 1;		//允许实时指令执行
// 	g_tRealTimeCmd.RealTimeFlag = 0;			//实时指令有效标志,高有效
// 	g_tRealTimeCmd.RealTimeCntr = 0;			//实时指令数据计数器
	g_tRealTimeCmd.RealTimeAllow = 0xFF;	//允许实时指令标志（10 14 fn）
}

static void	InitRealTimeStatusStructParameter(void)
{
	g_tRealTimeStatus.PrinterStatus = 0x12;
	g_tRealTimeStatus.OffLineStatus = 0x12;
	g_tRealTimeStatus.ErrorStatus = 0x12;
	g_tRealTimeStatus.PaperStatus = 0x12;
}

static void	InitPrtCtrlDataParameter(void)
{
	//打印控制数据结构
	g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_NONE;	//进入PC机的驱动模式,=0为正常打印模式,=1为SP驱动打印模式,=2为WH驱动打印模式,
	g_tPrtCtrlData.DriverOffset = 0;			//驱动打印时的绝对位置 （1B 24 n ）
	
	g_tPrtCtrlData.StepHoriUnit = DEFAULT_STEP_HORI_UNIT;	//打印横向移动单位,按点计算
	g_tPrtCtrlData.StepVerUnit = DEFAULT_STEP_VER_UNIT;		//打印纵向移动单位,按点计算

	g_tPrtCtrlData.CodeLineHigh = DEFAULT_CODE_LINE_HEIGHT;		//字符模式下的行高,按点计算
	g_tPrtCtrlData.LineHigh = 0;
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)
	{
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;			//字符宽度
		g_tPrtModeChar.CharHigh	= ASCII_FONT_A_HEIGHT;		//字符高
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)
	{
		g_tPrtModeChar.CharWidth	= ASCII_FONT_B_WIDTH;			//字符宽度		
		g_tPrtModeChar.CharHigh	= ASCII_FONT_B_HEIGHT;		//字符高
	}
	g_tPrtCtrlData.CharWidth	= 0;
	g_tPrtCtrlData.CharHigh	= 0;
	
	g_tPrtCtrlData.PrtLeftLimit = 0;		//标准模式左边距
	g_tPrtCtrlData.PrtWidth = g_tSystemCtrl.LineWidth;	//打印宽度,实际使用的打印区域,硬件可以实现的最大的打印范围
	g_tPrtCtrlData.PrtRightLimit = g_tSystemCtrl.LineWidth;	
	g_tPrtCtrlData.PrtLength = g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtWidth;	//标准模式设置打印区域	
	if (g_tPrtCtrlData.PrtLength > g_tSystemCtrl.LineWidth)
	{
		g_tPrtCtrlData.PrtLength = g_tSystemCtrl.LineWidth;
	}
	
	g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;	//标准模式,点计数器
	
	g_tPrtCtrlData.CharCycle90 = 0;			//字符旋转90°控制,高有效
	g_tPrtCtrlData.CharNotFlag = 0;			//字符反显的控制,高有效
	g_tPrtCtrlData.LineDirection = g_tSysConfig.PrintDir;	  //正反向打印	
	g_tPrtCtrlData.DoUnderlineFlag	= 0;	//下划线标志
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;	//字符对齐模式,0 = 左对齐, 1 = 中间对齐； 2 = 右对齐	
	g_tPrtCtrlData.OneLineWidthFlag = 0;	//行内倍宽标志
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	g_tPrtCtrlData.MaxRatioX = 1;				//行内最大横向放大倍数
	#endif
	g_tPrtCtrlData.MaxRatioY = 1;				//行内最大纵向放大倍数
	g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;	
	g_tPrtCtrlData.MaxPrtDotHigh = 0;
	g_tPrtCtrlData.BitImageFlag = 0;			//清位图打印标志
	g_tPrtCtrlData.VariableBitImageFlag = 0;	//清位图打印标志
}

//跳格数据结构
static void	InitTabStructParameter(void)
{
	uint32_t	i;
	
	//系统默认跳格数量
	g_tTab.TabSetNumber = g_tSystemCtrl.LineWidth / DEFAULT_HT_WIDTH;
	if (g_tTab.TabSetNumber > MAX_HT_INDEX_NUM)	//不能超过最大跳格数目
		g_tTab.TabSetNumber =	MAX_HT_INDEX_NUM;
	g_tTab.TabIndex = 0;
	for (i = 0; i < g_tTab.TabSetNumber; i++)	//默认跳格位置
		g_tTab.TabPositionIndex[i] = (i + 1) * DEFAULT_HT_WIDTH;
}

static void	InitPageModeStructParameter(void)
{
	//页模式数据结构
	g_tPageMode.PagePrtOe = 0;			//页模式下,打印允许控制
	g_tPageMode.PageNumber = 0;			//页模式下的总页数控制
	g_tPageMode.PageWritePoint = 0;		//页模式下的页填充控制计数器
	g_tPageMode.PageReadPoint = 0;		//页模式下的页打印计数器
	
	g_tPageMode.PageLineStartLoc = 0;	//有效起始打印行位置,固定结束行位置
	g_tPageMode.PageLineWidth = 0;		//有效打印行的数量
	g_tPageMode.PageDataLineIndex = 0;	//打印进程,行计数器索引。一次字符打印的打印行数索引（一次打印24行）
	g_tPageMode.PageDataDotIndex = 0;	//打印进程,列计数器索引。一次行打印打印点阵计数器

	g_tPageMode.PageFeedDir = 0;		//页模式下的打印方向,0 = 左到右；1 = 下到上； 2 = 右到左； 3 = 上到下
	g_tPageMode.PageArea[0] = 0;		//页模式下打印区域,依次为X0,Y0,DX,DY
	g_tPageMode.PageArea[1] = 0;
	g_tPageMode.PageArea[2] = g_tSystemCtrl.LineWidth;
	g_tPageMode.PageArea[3] = g_tSystemCtrl.LineWidth;
	g_tPageMode.PageVerLoc = 0;			//页模式下的相对位置
	g_tPageMode.PageLeftLimit = 0;		//打印区域左边距,与标准模式采用不同的值
	g_tPageMode.PageRightLimit= 0;		//打印区域右边距,与标准模式采用不同的值
	g_tPageMode.AscRightSpace = 0;		//页模式下ASCII字符的右间距	
	g_tPageMode.HZRightSpace = 0;		//页模式下汉字字符的右间距
	g_tPageMode.HZLeftSpace = 0;			//页模式下汉字字符的右间距
}

static void	InitDrawerParameter(void)
{
	//初始化钱箱参数
		g_tDrawer1.Status =DISABLE;
		g_tDrawer2.Status =DISABLE;
		g_tDrawer1.ON_Time	= 0;
		g_tDrawer2.ON_Time	= 0;
}

static void	InitInbufParameter(void)
{
	g_tInBuf.BytesNumber = 0;				//接收缓冲区字节数
	g_tInBuf.PutPosition = 0;				//接收缓冲区
	g_tInBuf.GetPosition = 0;				//接收缓冲区
	g_tInBuf.PE_BytesNumber = 0;		//接收缓冲区字节数
	g_tInBuf.PE_GetPosition = 0;		//接收缓冲区
	g_tInBuf.BufFullFlag = 0;
	
	if(0 == ExtSRamSize)
  {
		g_tInBuf.Buf = bInBuf;                          //无外扩sram
		g_tInBuf.BytesSize = MAX_BYTES_OF_IN_BUF;       //ram的大小
		memset(g_tInBuf.Buf, 0x00, MAX_BYTES_OF_IN_BUF);
  }
  else
  {
		g_tInBuf.Buf = (uint8_t *)(ExtInBufBaseAddr);                 //有扩展sram  需要分配 2016
		g_tInBuf.BytesSize =  (ExtSRamSize-PageModeBufNum);       //外部收数缓冲区的大小；
		memset(g_tInBuf.Buf, 0x00, sizeof(g_tInBuf.Buf) / sizeof(g_tInBuf.Buf[0]));
  }    
  
	//注意因为此处外部SRAM驱动还未进行，所以不能进行外部SRAM的清零操作！内部SRAM的可以。
	//memset(g_tInBuf.Buf, 0x00, MAX_BYTES_OF_IN_BUF);
	//memset(g_tInBuf.Buf, 0x00, sizeof(g_tInBuf.Buf) / sizeof(g_tInBuf.Buf[0]));
			
}


//2016.07.15 只在上电时初始化的变量参数
static void	InitParaOnlyTurnOn(void)
{
	//2016.06.11  上电初始化 但是1B40不初始化的变量如下 待增加
	g_tSystemCtrl.MaxStepPrtNum[0] = MaxStepNum[g_tSysConfig.MaxSpeed];		
	g_tSystemCtrl.MaxStepPrtNum[1] = MaxStepNum[g_tSysConfig.MaxSpeed];
	
	//实时指令参数 2016.07.18
	g_tRealTimeCmd.RealTimeFlag = 0;			//实时指令有效标志,高有效 
	g_tRealTimeCmd.RealTimeCntr = 0;			//实时指令数据计数器	

	g_tSystemCtrl.PrtSourceIndex =ETHERNET;	
	
	InitDrawerParameter();						    //初始化钱箱参数2016.07.18
	InitErrorStructParameter();				    //错误数据结构
	
	if(memcmp(g_tSysConfig.Manufacturer,"EPSON",5)==0)
	{	
		ManufacturerType=1;
	}	
	else
	{	
		ManufacturerType=0;
	}
	
}

static void	InitMacroStructParameter(void)
{
	g_tMacro.Number = 0;					//宏字符个数
	g_tMacro.GetPointer = 0;			//宏取数指针
	g_tMacro.DefinitionFlag = 0;	//正定义宏标志
	g_tMacro.SetFlag = 0;				//已定义宏的标志
	g_tMacro.RunFlag = 0;				//正运行宏内容标志
	memset(g_tMacro.Buffer, 0x00, MACRO_SIZE);	//宏数据缓存区
	//memset(g_tMacro.Buffer, 0x00, sizeof(g_tMacro.Buffer) / sizeof(g_tMacro.Buffer[0]));
}

#ifdef	CPCL_CMD_ENABLE
	static void	InitPageModeCharPara(void)
	{
		g_tPageModeCharPara.FontType 						= 0;
		g_tPageModeCharPara.SizeNumber 					= 0;
		g_tPageModeCharPara.Width			 					= 12;
		g_tPageModeCharPara.Height							= 24;
		g_tPageModeCharPara.LeftSpace						= 0;
		g_tPageModeCharPara.RightSpace					= 0;
		g_tPageModeCharPara.WidthMagnification	= 1;
		g_tPageModeCharPara.HeightMagnification	= 1;
		g_tPageModeCharPara.EmphaSizeMode				= 0;
		g_tPageModeCharPara.Underline						= 0;
		g_tPageModeCharPara.RotateType					= 0;
	}
	static void	InitPageModeHZPara(void)
	{
		g_tPageModeCharPara.FontType 						= 0;
		g_tPageModeCharPara.SizeNumber 					= 0;
		g_tPageModeCharPara.Width			 					= 24;
		g_tPageModeCharPara.Height							= 24;
		g_tPageModeCharPara.LeftSpace						= 0;
		g_tPageModeCharPara.RightSpace					= 0;
		g_tPageModeCharPara.WidthMagnification	= 1;
		g_tPageModeCharPara.HeightMagnification	= 1;
		g_tPageModeCharPara.EmphaSizeMode				= 0;
		g_tPageModeCharPara.Underline						= 0;
		g_tPageModeCharPara.RotateType					= 0;
	}
#endif

#if	0
static void	InitInterfaceParameter(void)
{
	g_tInterface.LPT_Type = 0;
	g_tInterface.COM_Type = 0;
	g_tInterface.USB_Type = 0;
	g_tInterface.ETH_Type = 0;
}
#endif

//程序中上电后错误和缺纸灯同时点亮0.5S后熄灭，
//指示灯再根据打印机状态或者按键状态在开始闪烁；
static void LEDPowerOn(void)
{
	PaperEndLedOn();
	ErrorLedOn();
	DelayMs(500);				
	PaperEndLedOff();
	ErrorLedOff();
	LineLedOn();			//默认灯带点亮
}


/******************************************************************************
*
* Function Name  : FLASH_BufferRead
* Description    : 内部flash参数读取
* Input          : - pBuffer : pointer to the buffer that receives the data 
read 
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_Inside_ReadByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint32_t	i;
	
	for (i = 0; i < NumByteToRead; i++)	
	{
		*pBuffer++ = *(vu8 *)(ReadAddr++);		
	}
}

/******************************************************************************
*
* Function Name  : Inner_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE 
sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to 
be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
FLASH_Status Flash_Inside_BufWrite(const u8* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{		
	uint16_t	* pData;
	volatile  FLASH_Status  FLASHStatus;
	uint32_t	i;
    
	pData = (uint16_t *)pBuffer;
	i = 0;
	FLASHStatus = FLASH_COMPLETE;
	
	while (i < NumByteToWrite)
	{
		FLASHStatus = FLASH_ProgramHalfWord(WriteAddr, *pData);
		if ((*(vu16*)WriteAddr) != *pData)
		{
  			FLASHStatus = FLASH_ERROR_PG;
			break;
		}
		WriteAddr += sizeof(u16);
		i += sizeof(u16);
		pData++;		
	}
	
	return (FLASHStatus);
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
