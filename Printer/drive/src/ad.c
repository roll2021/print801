/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.2
  * @date    2012-5-29
  * @brief   AD转换相关的程序.
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
#include	"ad.h"
#include	"temperature.h"
#include	"blackmark.h"
#include	"pe.h"
#include	"pne.h"
#include	"power.h"
#include	"ptake.h"
#include	"extgvar.h"
#include	"debug.h"
#include	"led.h"
#include	"maintain.h"

extern	uint8_t	g_bWriteFlashFlag;					//SPIFLASH标志
extern	volatile uint8_t	g_bRoutinFlag;		//T2中断标志

extern	void	DelayUs(uint32_t	delaytime);
extern	void	SetEmergencyProtect(void);
extern	uint32_t	GetFeedPaperStatus(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address 	(ADC1_BASE + 0x4C)

#ifdef	DBG_ADC_POWER
#define	POWER_ADC_COUNT		(1)	//调试时需要知道变换差异
#else
#define	POWER_ADC_COUNT		(8)	//检测足够次数后才取平均
#endif

#define	ADC_COUNT					(8)	//检测足够次数后才取平均

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TypeDef_StructAD	g_tADC;			//所有AD转换变量的结构

/* Private function prototypes -----------------------------------------------*/
static void UpdateErrorStatusByADCResult(void);
static void	UpdatePaperStatusByADCResult(void);
static void	UpdateBlackMarkStatusByADCResult(void);
static void	UpdatePaperNearEndStatusByADCResult(void);
static void	UpdateTemperatureStatusByADCResult(void);
static void	UpdatePowerStatusByADCResult(void);
static void	UpdatePaperAndBlackMarkStatusByADCResult(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 设置所有的ADC端口的初始化
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitADCPort(void)
{
	//InitBlackMarkPort();
	InitPaperEndPort();
	//InitPaperNearEndPort();
	InitPowerInPort();
	InitTemperaturePort();
	InitPaperTakeAwayPort();
}

/*************************************************
函数: void	OpenADC(void)
功能: 在DMA中断中关掉ADC转换，用这个函数打开
参数: 无
返回: 无
**************************************************/
#define	ADC_INTERVAL_MS		(1)
void	OpenADC(void)
{
	ADC_Cmd(ADC_SOURCE, ENABLE);		//需要两次才能开始AD转换
	ADC_Cmd(ADC_SOURCE, ENABLE);		//需要两次才能开始AD转换
}

/*************************************************
函数: void	ADC_Config(void)
功能: ADC进行配置
参数: 无
返回: 无
**************************************************/
void	ADC_Config(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	
	/* ADCCLK = PCLK2/6 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;				//独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;							//扫描方式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;				//连续转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//外部触发禁止
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		//数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUMBER;	//用于转换的通道数
  ADC_Init(ADC_SOURCE, &ADC_InitStructure);
	
	#define		ADC_SAMPLE_TIME		ADC_SampleTime_28Cycles5

	/* ADC_SOURCE regular channels configuration [规则模式通道配置]*/ 
  ADC_RegularChannelConfig(ADC_SOURCE, POWER_ADC_CHANNEL, 3, ADC_SAMPLE_TIME); 				// V24V_IN  POWER_ADC_CHANNEL  2016.04.04
	ADC_RegularChannelConfig(ADC_SOURCE, TEMPERATURE_ADC_CHANNEL, 1, ADC_SAMPLE_TIME);	//温度
  ADC_RegularChannelConfig(ADC_SOURCE, PE_ADC_CHANNEL, 2, ADC_SAMPLE_TIME); 				  //缺纸  PE_ADC_CHANNEL  也用于黑标
	ADC_RegularChannelConfig(ADC_SOURCE, PTAKE_ADC_CHANNEL, 4, ADC_SAMPLE_TIME);  			// 纸张是否取走
	#ifdef	BLACKMARK_ADC_USE_PE_ADC
	ADC_RegularChannelConfig(ADC_SOURCE, PE_ADC_CHANNEL, 5, ADC_SAMPLE_TIME); 					//黑标 黑标和取纸复用传感器时
	//#else
	//ADC_RegularChannelConfig(ADC_SOURCE, BLACKMARK_ADC_CHANNEL, 5, ADC_SAMPLE_TIME); 		//黑标 
	#endif
	//ADC_RegularChannelConfig(ADC_SOURCE, PNE_ADC_CHANNEL, 6, ADC_SAMPLE_TIME); 					//纸将尽 PNE_ADC_CHANNEL 
	
  /* Enable ADC_SOURCE DMA [使能ADC_SOURCE DMA]*/
  ADC_DMACmd(ADC_SOURCE, ENABLE);
  
  /* Enable ADC_SOURCE [使能ADC_SOURCE]*/
  ADC_Cmd(ADC_SOURCE, ENABLE);  

  /* Enable ADC_SOURCE reset calibaration register */   
  ADC_ResetCalibration(ADC_SOURCE);
  /* Check the end of ADC_SOURCE reset calibration register */
  while (ADC_GetResetCalibrationStatus(ADC_SOURCE));

  /* Start ADC_SOURCE calibaration */
  ADC_StartCalibration(ADC_SOURCE);
  /* Check the end of ADC_SOURCE calibration */
	while (ADC_GetCalibrationStatus(ADC_SOURCE));
	
	/* Start ADC_SOURCE Software Conversion */
	ADC_SoftwareStartConvCmd(ADC_SOURCE, ENABLE);
}

/*************************************************
函数: void	ADC_DMA_Configuration(void)
功能: DMA进行配置
参数: 无
返回: 无
**************************************************/
void	ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;	//定义DMA初始化结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	#ifdef	STM32F10X_CL
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	#endif
	
	/* DMA1 channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			//外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_tADC.ADBuffer;		//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//外设为源
  DMA_InitStructure.DMA_BufferSize = ADC_BUF_LEN;		//缓冲区大小,设置DMA在传输时缓冲区的长度word
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//设置DMA的外设递增模式，一个外设
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//设置DMA的内存递增模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据字长
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//内存数据字长
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;			//设置DMA的传输模式：连续不断的循环模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//设置DMA的优先级别
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;				//设置DMA的2个memory中的变量互相访问
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
  /* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);			//数据完成产生中断
}

/*******************************************************************************
* Function Name  : void	DMA1_Channel1_ADFunc
* Description    : 用于ADC的DMA中断功能，用于计算ADC转换值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	DMA1_Channel1_ADFunc(void)
{
	static uint8_t ADCount = 0;		//ADC次数统计
	
	#ifdef	BLACKMARK_ADC_USE_PE_ADC	
	static uint16_t m_tu16Buf[ADC_CHANNEL_NUMBER] = {0, 0, 0, 0, 0};
	#else	
  static uint16_t m_tu16Buf[ADC_CHANNEL_NUMBER] = {0, 0, 0, 0};
  #endif
	uint16_t	tu16TempBuf[ADC_CHANNEL_NUMBER];
	uint32_t	i;
	uint32_t	j;
	
	volatile uint16_t	* ptu16Buf = NULL;
	uint16_t * ptu16TempBuf = NULL;
	
	DMA_ClearITPendingBit(DMA1_IT_TC1);		//清除DMA传输完成中断位
	ADC_Cmd(ADC_SOURCE, DISABLE); 				//关AD模块
	ADCount++;								//采用连续若干次平均值
	g_bRoutinFlag = 0;				//T2中断标志，休眠时使用
	
	//先清零临时变量数组
	ptu16TempBuf = tu16TempBuf;				//临时加和
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
		*ptu16TempBuf = 0x00;
		ptu16TempBuf++;
	}
	//再计算本次采样总值
	ptu16Buf = g_tADC.ADBuffer;	//ADC转换结果
	for (i = 0; i < ADC_CONVERT_NUMBER; i++)
	{
		ptu16TempBuf = tu16TempBuf;			//临时加和
		for (j = 0; j < ADC_CHANNEL_NUMBER; j++)
		{
			*ptu16TempBuf += *ptu16Buf;
			ptu16TempBuf++;
			ptu16Buf++;
		}
	}
	//然后采样总值根据取样次数取平均,并将本次平均后的结果存放到静态存储区中
	ptu16Buf = m_tu16Buf;							//ADC转换结果静态存储区
	ptu16TempBuf = tu16TempBuf;				//临时加和
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
		*ptu16TempBuf >>= ADC_CONVERT_SHIFT_NUMBER;	//取平均
		*ptu16Buf += *ptu16TempBuf;	//新结果添加存储到静态存储区中
		ptu16Buf++;
		ptu16TempBuf++;
	}
	
	if (ADCount >= ADC_COUNT)			//总次数到,求平均值
	{
		ptu16Buf = m_tu16Buf;				//ADC转换结果静态存储区
		for (i = 0; i < ADC_CHANNEL_NUMBER; i++)	//取平均
		{
			*ptu16Buf /= ADCount;
			ptu16Buf++;
		}
		ptu16Buf = m_tu16Buf;				//ADC转换结果静态存储区,更新ADC变量
		g_tADC.Temperature_ADC_Value		= *ptu16Buf++;
		g_tADC.PaperEnd_ADC_Value				= *ptu16Buf++;
		g_tADC.Power_ADC_Value				  = *ptu16Buf++;				//2016.04  根据电路更改 
		g_tADC.PaperTakeAway_ADC_Value	= *ptu16Buf++; 
		#ifdef	BLACKMARK_ADC_USE_PE_ADC	
		g_tADC.BlackMark_ADC_Value			= *ptu16Buf++;
		#endif
		//g_tADC.PaperNearEnd_ADC_Value 	= *ptu16Buf++;
		
		ptu16Buf = m_tu16Buf;				//ADC转换结果静态存储区
		for (i = 0; i < ADC_CHANNEL_NUMBER; i++)	//重新清零
		{
			*ptu16Buf = 0x00;
			ptu16Buf++;
		}
		ADCount	= 0;	//转换次数清零
		
		#ifdef	ERROR_STATUS_ADC_UPDATE_ENABLE
			UpdateErrorStatusByADCResult();	//根据ADC采样结果值调整各个错误状态变量结果
		#endif
	}
	
	ADC_DMA_Config();		//重新设置一次
}

/* 以下为对ADC的封装,返回各个变量的值 */
uint16_t	GetADCValueOfPaperEnd(void)
{
	return	(g_tADC.PaperEnd_ADC_Value);
}

uint16_t	GetADCValueOfTemperature(void)
{
	return	(g_tADC.Temperature_ADC_Value);
}

uint16_t	GetADCValueOfPower(void)
{
	return	(g_tADC.Power_ADC_Value);
}

uint16_t	GetADCValueOfPaperNearEnd(void)
{
	return	(g_tADC.PaperNearEnd_ADC_Value);
}

uint16_t	GetADCValueOfBlackMark(void)
{
	return	(g_tADC.BlackMark_ADC_Value);
}

uint16_t	GetADCValueOfPaperTakeAway(void)
{
	return	(g_tADC.PaperTakeAway_ADC_Value);
}

const uint16_t	g_tPaperendADValueTable[] =
{
	(uint16_t)PE_ADC_VALUE_LOW, 
	(uint16_t)PE_ADC_VALUE_MIDDLE, 
	(uint16_t)PE_ADC_VALUE_HIGH
};	//测纸传感器灵敏度值

/* 以下为本地函数 */
static void	UpdatePaperStatusByADCResult(void)
{
	//判断打印头是否缺纸或黑标位置，无纸（黑标）时电压高，有纸时电压低
	if (g_tADC.PaperEnd_ADC_Value > g_tPaperendADValueTable[g_tSysConfig.PaperSensorSensitive])
	{		
		if (g_tSysConfig.BlackMarkEnable)	//黑标允许时才检测
		{
			g_tSystemCtrl.MarkPositionFlag = 0x01;	//黑标位置
		}	
		else
		{
			g_tError.PaperSensorStatusFlag |= 0x01;	  //bit.0 = 1 缺纸  非黑标模式下才赋值缺纸标志
		}	
	}
	else
	{
		if (g_tSysConfig.BlackMarkEnable)	//黑标允许时才检测
		{
			g_tSystemCtrl.MarkPositionFlag = 0x00;	  //黑标位置清除
			g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 不缺纸  2016.11.17  黑标模式下 有纸就清缺纸标志
		}	
		else
		{	
			g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 不缺纸
		}
	}
}

static void	UpdateBlackMarkStatusByADCResult(void)
{
	const uint16_t	g_tBlackmarkADValueTable[] =
	{
		(uint16_t)BLACKMARK_ADC_VALUE_LOW, 
		(uint16_t)BLACKMARK_ADC_VALUE_MIDDLE, 
		(uint16_t)BLACKMARK_ADC_VALUE_HIGH
	};	//测黑标传感器灵敏度值
	
	//判断打印头是否黑标位置，黑标时电压高，非黑标时电压低
	if (g_tADC.BlackMark_ADC_Value > g_tBlackmarkADValueTable[g_tSysConfig.BlackMarkSensitive])
	{
		g_tSystemCtrl.MarkPositionFlag = 0x01;	//黑标位置
	}
	else
	{
		g_tSystemCtrl.MarkPositionFlag = 0x00;	//非黑标位置
	}
}

/*******************************************************************************
* Function Name  : void	UpdatePaperStatusByADCResult()
* Description    : 根据ADC采样结果值调整缺纸错误状态变量结果. 
	按照缺纸传感器和黑标传感器是否独立分为两种情况.
	独立时各自测量缺纸和黑标, 共用时根据是否黑标模式处理缺纸和黑标结果. 此时黑标下
	无法判断是否缺纸, 只能由其他方式确定是否缺纸.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	UpdatePaperAndBlackMarkStatusByADCResult(void)
{
	//测纸和测黑标共用同一个传感器
	#ifdef	BLACKMARK_ADC_USE_PE_ADC
		if (g_tSysConfig.BlackMarkEnable)	//黑标允许时只检测黑标, 不处理缺纸, 也无从判断是否缺纸
		{
			UpdateBlackMarkStatusByADCResult();
			//判断打印头是否缺纸或黑标位置，无纸（黑标）时电压高，有纸时电压低
			if (g_tADC.PaperEnd_ADC_Value > g_tPaperendADValueTable[g_tSysConfig.PaperSensorSensitive])
			{
			}
			else
			{
				g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 不缺纸
			}
		}
		else	//非黑标模式时只测纸, 不处理黑标
		{
			UpdatePaperStatusByADCResult();
		}
	//以下为测纸和测黑标相互独立
	#else
		UpdatePaperStatusByADCResult();		//判断打印头是否缺纸，无纸时电压高，有纸时电压低
// 		if (g_tSysConfig.BlackMarkEnable)	//黑标允许时才检测 T801没有单独和黑标传感器，和测试传感器公用;
// 		{
// 			UpdateBlackMarkStatusByADCResult();
// 		}
	#endif
}

static void	UpdatePaperNearEndStatusByADCResult(void)
{
    static uint32_t PNEDelayNum = 0;       //延时采集纸将近信号 
	#ifdef	PNE_ENABLE
		const uint16_t	g_tPneADValueTable[] =
		{
			(uint16_t)PNE_ADC_VALUE_LOW, 
			(uint16_t)PNE_ADC_VALUE_MIDDLE, 
			(uint16_t)PNE_ADC_VALUE_HIGH
		};	//纸将尽传感器灵敏度值
	#else
		const uint16_t	g_tPneADValueTable[] =
		{
			(uint16_t)PE_ADC_VALUE_LOW, 
			(uint16_t)PE_ADC_VALUE_MIDDLE, 
			(uint16_t)PE_ADC_VALUE_HIGH
		};	//测纸传感器灵敏度值
	#endif
	
	#ifdef	PNE_ENABLE
		//只有当打印机静止时才更新纸将尽信号,否则不处理
		if (PRINTER_STATUS_ILDE == GetFeedPaperStatus())
		{
            
			if ((g_tADC.PaperNearEnd_ADC_Value > g_tPneADValueTable[g_tSysConfig.PaperSensorSensitive]) && (g_tSysConfig.PaperNearEndSensorEnable ==1) ) // 满足纸将近条件 并 打开了纸将近功能
			{
				PNEDelayNum++;
				if(PNEDelayNum > 200)   //2s持续判别后赋值  时间有待完善
				{
						g_tError.PaperSensorStatusFlag |= 0x02;		//bit.1 = 1 纸将近
						PNEDelayNum = 200;
				}   
				else
				{
						g_tError.PaperSensorStatusFlag &= ~0x02;    //清标志
				}    
			}
			else
			{
                PNEDelayNum = 0;
				g_tError.PaperSensorStatusFlag &= ~0x02;    //清标志
			}
		}
	#endif
}

static void	UpdateTemperatureStatusByADCResult(void)
{
	//----通过测温电路来判断打印头是否连接上,温度检测错误下限值,温度检测错误上限值
	if ((g_tADC.Temperature_ADC_Value > TEMPERATURE_ERROR_ADC_LOW) || \
			(g_tADC.Temperature_ADC_Value < TEMPERATURE_ERROR_ADC_HIGH))
	{
		g_tError.UR_ErrorFlag |= 0x80;	   		//bit7 温度检测电路错误
	}
	else
	{
		g_tError.UR_ErrorFlag &= (uint8_t)(~0x80);
	}
	
	//----判断打印头是否过热----//
	//打印头采样电压越低，打印头温度越高
	if (g_tADC.Temperature_ADC_Value < TEMPERATURE_WARN_ADC_HIGH)
	{
		g_tError.AR_ErrorFlag |= 0x01;	   		//bit.0 = 1 打印头过热
	}
	else if (g_tADC.Temperature_ADC_Value > TEMPERATURE_WARN_ADC_LOW)
	{
		g_tError.AR_ErrorFlag &= (uint8_t)(~0x01);	//bit.0 = 0 打印头正常
	}
}

static void	UpdatePowerStatusByADCResult(void)
{
	//----判断输入电源电压是否正常，过电压、你电压
	if (g_tADC.Power_ADC_Value < POWER_VALUE_LOW)	//欠电压
	{	//防止正常写时，重复写
// 		if (((g_tError.UR_ErrorFlag & 0x04) == 0) && (WriteFlashFlag == 0))  //2017.04.24
// 		{
// 			#ifdef	MAINTAIN_CMD_ENABLE
// 			  WriteFlashParameter(1);							//写CPU_FLASH维护计数器
// 			#endif
// 			g_tError.UR_ErrorFlag |= 0x04;			//bit3欠电压
// 		}
	}
	else
	{
		if (((g_tError.UR_ErrorFlag & 0x04) == 0x04) && (WriteFlashFlag == 0))	//防止正常写时，重复写
		{
			#ifdef	MAINTAIN_CMD_ENABLE
				ReadFlashParameter();								//读维护计数器，防止写入时超过页
			#endif
			g_tError.UR_ErrorFlag &= (uint8_t)(~0x04);
		}
	}
	
	if (g_tADC.Power_ADC_Value > POWER_VALUE_HIGH)	//过电压
	{
		g_tError.UR_ErrorFlag |= 0x02;	   		//bit2 过电压
	}
	else
	{
		g_tError.UR_ErrorFlag &= (uint8_t)(~0x02);
	}
	
	if (g_tADC.Power_ADC_Value > POWER_VALUE_CLOSE)	//关闭打印电压
	{
		SetEmergencyProtect();
	}
}

static void UpdatePaperTakeAwayStatusByADCResult(void)
{
	const uint16_t	m_tPaperTakeAwayADValueTable[] =
	{
		(uint16_t)PTAKE_ADC_VALUE_LOW, 
		(uint16_t)PTAKE_ADC_VALUE_MIDDLE, 
		(uint16_t)PTAKE_ADC_VALUE_HIGH
	};	//纸张取走传感器灵敏度值
	
	//判断纸张是否已经取走,高电平时为缺纸，表示纸张被取走；低电平为有纸，表示纸张未取走
	if (g_tADC.PaperTakeAway_ADC_Value > m_tPaperTakeAwayADValueTable[g_tSysConfig.PaperTakeAwaySensorSensitive])
	{
		if(g_tSysConfig.PaperTakeAwaySensorEnable ==1)         //增加打开取纸功能开关
		{	
			g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x04);	 //bit.2 = 0 纸张被取走
		}	
	}
	else 
	{
		if(g_tSysConfig.PaperTakeAwaySensorEnable ==1)         //增加打开取纸功能开关
		{	
			g_tError.PaperSensorStatusFlag |= 0x04;							 //bit.2 = 1 纸张未被取走
		}	
	}
}

/*******************************************************************************
* Function Name  : void	UpdateErrorStatusByADCResult()
* Description    : 根据ADC采样结果值调整各个错误状态变量结果
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void UpdateErrorStatusByADCResult(void)
{
  if(g_tError.DoorOpen == 0)                    //上盖打开 不更新纸状态
  {    
		UpdatePaperAndBlackMarkStatusByADCResult();
		UpdatePaperTakeAwayStatusByADCResult();
  }    
	UpdatePaperNearEndStatusByADCResult();
	UpdateTemperatureStatusByADCResult();
	UpdatePowerStatusByADCResult();
    
	AutoGoPaper();      //自动上纸更新状态
	
}
/*******************************************************************************
* Function Name  : void	AutoGoPaper()
* Descripti on   : 自动上纸更新状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void AutoGoPaper(void)
{
	static uint16_t CounterNum = 0;		 
 				  		
	if((g_tError.PaperSensorStatusFlag & 0x01) != 0)    //缺纸状态下
	{
		AutoFeedPaper = 1;                              //缺纸状态为1  缺纸到有纸为2  进纸完成后赋值为0
    CounterNum = 0;
	}	
  else
  {    
        if(AutoFeedPaper == 1)
        {
            if((g_tError.PaperSensorStatusFlag & 0x01) == 0) //有纸状态下
            {
                CounterNum++;
                if(CounterNum > 10)                            //防抖
                {
                    CounterNum = 0;
                    AutoFeedPaper = 2;					 
                }
            } 			
        }	
    }   
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
