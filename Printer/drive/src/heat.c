/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   加热控制相关的程序.
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
#include <string.h>
#include	"heat.h"
#include	"print.h"
#include	"ad.h"
#include	"power.h"
#include	"temperature.h"
#include	"extgvar.h"
#include	"debug.h"

extern void	DelayUs(uint32_t	delaytime);

extern	uint8_t	g_tbHeatElementBuf[];	//加热机芯一点行所有的加热寄存器单元数
extern	TypeDef_StructAD	g_tADC;			//所有AD转换变量的结构
extern uint32_t	g_iFeedMotorStartStep;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
TypeDef_StructHeatCtrl	g_tHeatCtrl;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if	MAX_HEAT_SUM == 6
#elif	MAX_HEAT_SUM == 3
#elif	MAX_HEAT_SUM == 2
uc16	m_tHeatCtrlPinTable[] = {HEAT_PDST1_GPIO_PIN, HEAT_PDST2_GPIO_PIN};
#elif	MAX_HEAT_SUM == 1
uc16	m_tHeatCtrlPinTable[] = {HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN};
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化加热控制管脚
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitHeatPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(HEAT_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= HEAT_PDST1_GPIO_PIN | \
																		HEAT_PDST2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;			//管脚均为输出
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(HEAT_CTRL_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(HEAT_PLATCH_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= HEAT_PLATCH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;			//管脚均为输出
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(HEAT_PLATCH_GPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_SetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	StopHeatAll();
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 锁存机芯数据,负脉冲锁存数据
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	LatchHeatData(void)
{
	GPIO_SetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_SetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化加热控制结构
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitHeatCtrlStruct(void)
{
	g_tHeatCtrl.HeatEnable						= HEAT_DISABLE;
	g_tHeatCtrl.HeaterGroupNumber			= 0;
	g_tHeatCtrl.HeaterGroupNumberCpy	= 0;
	memset(g_tHeatCtrl.HeaterGroupElement, 0x00, MAX_HEAT_SUM);
	memset(g_tHeatCtrl.HeaterGroupElementCpy, 0x00, MAX_HEAT_SUM);
	//memset(g_tHeatCtrl.HeaterGroupElement, 0x00, sizeof(g_tHeatCtrl.HeaterGroupElement) / sizeof(g_tHeatCtrl.HeaterGroupElement[0]));
	//memset(g_tHeatCtrl.HeaterGroupElementCpy, 0x00, sizeof(g_tHeatCtrl.HeaterGroupElementCpy) / sizeof(g_tHeatCtrl.HeaterGroupElementCpy[0]));
}

/*******************************************************************************
* Function Name  : 
* Description    : 开始加热
* Autor					 : 
* Date					 : 2012-8-2
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void	StartHeat(volatile uint16_t	HeatPins)		//2016.06.11
{
	#ifdef	DBG_DUMMY_HEAT
		
	#else
		HeatPowerOn();
		GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HeatPins);		//相应的分组加热控制
		
		#ifdef	MAINTAIN_ENABLE
			R_Counter.HeatCounter++;			//加热计数器＋1
			C_Counter.HeatCounter++;
		#endif
	#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 全部停止加热
* Autor					 : 
* Date					 : 2012-8-2
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void	EndHeat(void)
{
	#ifdef	DBG_DUMMY_HEAT
		
	#else
		HeatPowerOff();
		StopHeatAll();
	#endif
}

/*******************************************************************************
* Function Name  : int16_t	GetTemperature(void)
* Description    : 计算当前机芯检测到的温度,单位℃
* Autor					 : 
* Date					 : 2012-6-11
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
int16_t		GetTemperature(void)
{
	uint16_t i;
	int16_t	Temperature;
	//机芯内部温度转换表
	const int16_t	g_tTemperatureValueTable[] = 
	{
		(uint16_t)(ADC_MAX_VALUE * RTH_20 / (RH + RTH_20)),
		(uint16_t)(ADC_MAX_VALUE * RTH_15 / (RH + RTH_15)),
		(uint16_t)(ADC_MAX_VALUE * RTH_10 / (RH + RTH_10)),
		(uint16_t)(ADC_MAX_VALUE * RTH_5 / (RH + RTH_5)),
		(uint16_t)(ADC_MAX_VALUE * RTH0 / (RH + RTH0)),
		(uint16_t)(ADC_MAX_VALUE * RTH5 / (RH + RTH5)),
		(uint16_t)(ADC_MAX_VALUE * RTH10 / (RH + RTH10)),
		(uint16_t)(ADC_MAX_VALUE * RTH15 / (RH + RTH15)),
		(uint16_t)(ADC_MAX_VALUE * RTH20 / (RH + RTH20)),
		(uint16_t)(ADC_MAX_VALUE * RTH25 / (RH + RTH25)),
		(uint16_t)(ADC_MAX_VALUE * RTH30 / (RH + RTH30)),
		(uint16_t)(ADC_MAX_VALUE * RTH35 / (RH + RTH35)),
		(uint16_t)(ADC_MAX_VALUE * RTH40 / (RH + RTH40)),
		(uint16_t)(ADC_MAX_VALUE * RTH45 / (RH + RTH45)),
		(uint16_t)(ADC_MAX_VALUE * RTH50 / (RH + RTH50)),
		(uint16_t)(ADC_MAX_VALUE * RTH55 / (RH + RTH55)),
		(uint16_t)(ADC_MAX_VALUE * RTH60 / (RH + RTH60)),
		(uint16_t)(ADC_MAX_VALUE * RTH65 / (RH + RTH65)),
		(uint16_t)(ADC_MAX_VALUE * RTH70 / (RH + RTH70)),
		(uint16_t)(ADC_MAX_VALUE * RTH75 / (RH + RTH75)),
		(uint16_t)(ADC_MAX_VALUE * RTH80 / (RH + RTH80)),
		(uint16_t)(ADC_MAX_VALUE * RTH85 / (RH + RTH85)),
		(uint16_t)(ADC_MAX_VALUE * RTH90 / (RH + RTH90)),
		(uint16_t)(ADC_MAX_VALUE * RTH95 / (RH + RTH95)),
		(uint16_t)(ADC_MAX_VALUE * RTH100 / (RH + RTH100)),
	};
	
	for (i = 0; i < TEMPERATURE_TABLE_VALUE_TOTAL_NUMBER; i++)
	{
		if (g_tADC.Temperature_ADC_Value > g_tTemperatureValueTable[i])
		{
			break;
		}
	}
	Temperature = (TEMPERATURE_TABLE_VALUE_LOW + 5 * i);	//计算当前的温度值，从-20℃算起，每隔5℃一个档次

	return (Temperature);
}

/*******************************************************************************
* Function Name  : 
* Description    : 根据全局变量g_tbHeatElementBuf中1的个数统计,比较左右两个加热
*	控制区的点数,确定是否一次加热.分别统计两侧的点数,如果两侧点数之和超过1次加热
*	最大值,则设置为两次加热,否则设置为1次加热.
* Autor					 : 
* Date					 : 2016-3
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void	SetHeatNumber(void)
{
	volatile uint8_t	* p = NULL;
	uint32_t	i;
	uint32_t	Count;
	
#ifdef	HEAT_ONE_TIME_ENABLE		//测试用,无论内容多少统统一次加热
	g_tHeatCtrl.HeaterGroupNumber = 1;		//一行加热次数
	#if	MAX_HEAT_SUM == 2
		g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0] | m_tHeatCtrlPinTable[1];	//加热分组控制
	#elif	MAX_HEAT_SUM == 1
		g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//加热分组控制
	#endif
#else		//以下为正式程序
	p = g_tbHeatElementBuf;
	p += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;			//左侧有空白字节不能打印
	Count = 0;
	
	for (i = 0; i < DST1_HEAT_BYTE; i++)		//左侧控制区的字节数
	{
		if (*p)		//如果该值为0就不必统计直接跳过
		{
			Count += GetHighBitNumberInByte( *p );	//统计每个字节中的1的个数
		}
		p++;
	}
	
	if (Count == 0)		//如果左侧皆为0,不必再统计右侧了
	{
		g_tHeatCtrl.HeaterGroupNumber = 1;		//一行加热次数
		#if	MAX_HEAT_SUM == 2
			g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0] | m_tHeatCtrlPinTable[1];	//加热分组控制
		#elif	MAX_HEAT_SUM == 1
			g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//加热分组控制
		#endif
	}
	else		//否则继续统计右侧
	{
		if (Count == MAX_HEAT_DOT_ONE_TIME)	//达到最大加热点数，直接设置两次加热
		{
			g_tHeatCtrl.HeaterGroupNumber = 2;									//一行加热次数
			#if	MAX_HEAT_SUM == 2
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//加热分组控制
				g_tHeatCtrl.HeaterGroupElement[1] = m_tHeatCtrlPinTable[1];	//加热分组控制
			#elif	MAX_HEAT_SUM == 1
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//加热分组控制
			#endif
		}
		else		//否则继续统计另一个加热组剩下的点数
		{
			for (i = 0; i < DST2_HEAT_BYTE; i++)	//右侧控制区的字节数
			{
				if (*p)		//如果该值为0就不必统计直接跳过
				{
					Count += GetHighBitNumberInByte( *p );	//统计每个字节中的1的个数
				}
				p++;
			}
			
			if (Count <= MAX_HEAT_DOT_ONE_TIME)			//左右两侧和不超过最大一次加热点数,一次加热
			{
				g_tHeatCtrl.HeaterGroupNumber = 1;		//一行加热次数
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0] | m_tHeatCtrlPinTable[1];	//加热分组控制
			}
			else
			{
				g_tHeatCtrl.HeaterGroupNumber = 2;									//一行加热次数
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//加热分组控制
				g_tHeatCtrl.HeaterGroupElement[1] = m_tHeatCtrlPinTable[1];	//加热分组控制
			}
		}		//剩余加热组统计完毕
	}		//继续统计处理完毕
#endif
}

/*******************************************************************************
* Function Name  : uint16_t	GetHeatTime(void)
* Description    : 确定加热时间，根据命令或者驱动或者曲线打印模式的不同，
*				   及当前机芯检测温度的不同进行加热时间调整
* Autor			 : 
* Date			 : 2016-3
* Input          : 
* Output         : 
* Return         : 确定的下一点行的一次加热时间,us
*******************************************************************************/
uint16_t	GetHeatTime(void)
{
	uint16_t	heattime;
	uint16_t	adjusttime;
	int32_t	Temperature;
	uint8_t	temp;
	
	const uint16_t	cHeatTimeTable[] = 
	{
		HEAT_TIME_LOW,			//低浓度
		HEAT_TIME_MIDDLE,		//中等浓度
		HEAT_TIME_HIGH,			//高浓度
	};
#define	PAPER_KIND	(sizeof(cHeatTimeTable) / sizeof(cHeatTimeTable[0]))
	
#define	HEAT_ADJUST_TIME_US_LEVEL_1			(500)	//500
#define	HEAT_ADJUST_TIME_US_LEVEL_2			(400)	//400
#define	HEAT_ADJUST_TIME_US_LEVEL_3			(300)	//300
#define	HEAT_ADJUST_TIME_US_LEVEL_4			(200)	//200
#define	HEAT_ADJUST_TIME_US_LEVEL_5			(100)	//100
#define	HEAT_ADJUST_TIME_US_LEVEL_6			(50)		//50
#define	HEAT_ADJUST_TIME_US_LEVEL_7			(30)		//30
#define	HEAT_ADJUST_TIME_US_LEVEL_8			(10)		//10
#define	HATUL_1													(HEAT_ADJUST_TIME_US_LEVEL_1)
#define	HATUL_2													(HEAT_ADJUST_TIME_US_LEVEL_2)
#define	HATUL_3													(HEAT_ADJUST_TIME_US_LEVEL_3)
#define	HATUL_4													(HEAT_ADJUST_TIME_US_LEVEL_4)
#define	HATUL_5													(HEAT_ADJUST_TIME_US_LEVEL_5)
#define	HATUL_6													(HEAT_ADJUST_TIME_US_LEVEL_6)
#define	HATUL_7													(HEAT_ADJUST_TIME_US_LEVEL_7)
#define	HATUL_8													(HEAT_ADJUST_TIME_US_LEVEL_8)
	const uint16_t	cStartHeatTimeCompensateTable[] = 
	{
		HATUL_1,  HATUL_1,	HATUL_1,	HATUL_1,	HATUL_1,
		HATUL_2,  HATUL_2,	HATUL_2,	HATUL_2,	HATUL_2,
		HATUL_3,  HATUL_3,	HATUL_3,	HATUL_3,	HATUL_3,
		HATUL_4,  HATUL_4,	HATUL_4,	HATUL_4,	HATUL_4,
		HATUL_5,  HATUL_5,	HATUL_5,	HATUL_5,	HATUL_5,
		HATUL_6,  HATUL_6,	HATUL_6,	HATUL_6,	HATUL_6,
		HATUL_7,  HATUL_7,	HATUL_7,	HATUL_7,	HATUL_7,
		HATUL_8,  HATUL_8,	HATUL_8,	HATUL_8,	HATUL_8,
	};

#define	START_STEP_COMPENSATE_TABLE_LEN	\
	(sizeof(cStartHeatTimeCompensateTable) / sizeof(cStartHeatTimeCompensateTable[0]))

	temp = g_tSysConfig.Density;		//打印浓度档次设置
	if (temp >= PAPER_KIND)					//超过预定值时以免下标越界
		temp = 1;
	heattime = cHeatTimeTable[temp];	//标准加热时间
	
	#ifdef	HEAT_TIME_BY_POWER_ADJUST_ENABLE
		heattime = GetAdjustHeatTimeOfPowerVoltage(heattime);	//得到根据电源电压补偿的基准加热时间
	#endif
	
	#ifdef	HEAT_TIME_START_STEP_ADJUST_ENABLE
		//添加初始步阶段增加加热时间  2016.06.20  调整起步的加热时间 现在程序是否合理
		if (g_iFeedMotorStartStep < START_STEP_COMPENSATE_TABLE_LEN)
		{	//heattime = GetAdjustHeatTimeOfFirstLineHeat(heattime);
			heattime += cStartHeatTimeCompensateTable[g_iFeedMotorStartStep];
		}
	#endif
	
	#ifdef	HEAT_TIME_BY_TEMPERATURE_ADJUST_ENABLE
		Temperature = GetTemperature();	//计算当前的温度值
		adjusttime = GetAdjustHeatTimeOfTemperature(Temperature);	//得到根据温度补偿的附加加热时间
		if (Temperature > ROOM_TEMPERATURE)
		{
			if (heattime > adjusttime)
			{
				heattime -= adjusttime;			//2017.03.16  最少有280的加热时间
				if(heattime < 280)
				{
					heattime = 280;
				}	
			}
		}
		else
		{
			heattime += adjusttime;
		}
	#endif
	
	if (heattime < MIN_HEAT_TIME)
	{	
		heattime = MIN_HEAT_TIME;
	}
	if (heattime > MAX_HEAT_TIME)
	{
		heattime = MAX_HEAT_TIME;
	}
	
	heattime /= STEP_NUMBERS_PER_DOT_HEIGHT;
	
	return (heattime);
}


/*******************************************************************************
* Function Name  : uint8_t GetHighBitNumberInByte(uint8_t DataIn)
* Description    : 获得1字节数据内 bit =1 的个数
* Input          : 字节
* Output         : 
* Return         : 字节内 1 的个数
*******************************************************************************/
static uint8_t	GetHighBitNumberInByte(uint8_t DataIn)
{
	uint32_t	i;
	uint32_t	Result;
	
	Result = 0;	
	for (i = 0; i < 8; i++)
	{
		if ((DataIn & 0x01) != 0)
		{
			Result++;
		}
		DataIn >>= 1;
	}
	
	return (Result);
}

#if	0
/*******************************************************************************
* Function Name  : uint16_t	GetPowerVoltage(void)
* Description    : 计算当前检测到的供电电压值,单位V
* Autor					 : 
* Date					 : 2013-3-21
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
static double	GetPowerVoltage(void)		//计算电源电压值
{
	double	voltage;
	
	//得到当前测到的ADC换算后的供电电压值，注意电压分压比由外接分压电阻决定,单位V
	voltage = VREF * g_tADC.Power_ADC_Value / POWER_DIVIDE_RATIO / ADC_MAX_VALUE;
	
	return (voltage);
}
#endif

/*******************************************************************************
* Function Name  : static uint16_t	GetAdjustHeatTimeOfPowerVoltage(uint16_t htime)
* Description    : 计算补偿,根据当前机芯供电电压的不同进行加热时间调整.
* 以标准电压为基准,按照电压除值平方计算
*	Ti = Tstd * (Ustd / Ui)^2
* Autor			 : 
* Date			 : 2013-3-19
* Input          : 无
* Output         : 调整基准加热
* Return         : 调整后的下一点行的根据供电电压补偿的一次加热时间调整,us
*******************************************************************************/
static uint16_t	GetAdjustHeatTimeOfPowerVoltage(uint16_t htime)
{
	double	heattime;
	uint32_t	StdPowerVoltageADC;
	uint32_t	CurrentPowerVoltageADC;
	volatile uint32_t Power_ADC_Value;
	
	StdPowerVoltageADC = ((uint32_t)POWER_VALUE_STD) * ((uint32_t)POWER_VALUE_STD);
	//CurrentPowerVoltageADC = ((uint32_t)g_tADC.Power_ADC_Value) * ((uint32_t)g_tADC.Power_ADC_Value);
	Power_ADC_Value = (uint32_t)g_tADC.Power_ADC_Value;
	CurrentPowerVoltageADC = Power_ADC_Value;
	CurrentPowerVoltageADC *= Power_ADC_Value;
	if (CurrentPowerVoltageADC)			//防止除数为0
		heattime = (uint16_t)(((double)(StdPowerVoltageADC * htime)) / ((double)(CurrentPowerVoltageADC)));
	else
		heattime = htime;
	
	return ((uint16_t)heattime);
}

//#ifdef	HEAT_TIME_START_STEP_ADJUST_ENABLE
#if	0
/*******************************************************************************
* Function Name  : static uint16_t	GetAdjustHeatTimeOfFirstLineHeat(uint16_t htime)
* Description    : 计算补偿,对于从静止开始打印的部分点行进行加热时间调整.
* Autor			 : 
* Date			 : 2013-3-21
* Input          : 
* Output         : 调整基准加热
* Return         : 调整后的下一点行的根据加速步数进行补偿的一次加热时间调整,us
*******************************************************************************/
static uint16_t	GetAdjustHeatTimeOfFirstLineHeat(uint16_t htime)
{
	uint16_t	CurrentStep;
	
	CurrentStep = g_iFeedMotorStartStep;
	htime += cStartHeatTimeCompensateTable[CurrentStep];
	
	return (htime);
}
#endif

/*******************************************************************************
* Function Name  : static uint16_t	GetAdjustHeatTimeOfTemperature(int16_t	Temperature)
* Description    : 计算温度补偿,根据当前机芯检测温度的不同进行加热时间调整.
* 以25℃为基准,温度每增长1℃,加热时间减少若干us;温度每减少1℃,
* 加热时间增长若干us.按照温度值划分为几个档次,每个档次的温度补偿值不同.				   
* Autor			 : 
* Date			 : 2012-1-11
* Input          : 机芯温度
* Output         : 附加调整加热时间
* Return         : 调整后的下一点行的根据温度补偿的附加一次加热时间调整,us
*******************************************************************************/
static uint16_t	GetAdjustHeatTimeOfTemperature(int32_t	Temperature)
{
	uint16_t	adjusttime;
	
	adjusttime = 0;	
	if (Temperature < -10)
	{
		adjusttime = (uint16_t)((ROOM_TEMPERATURE - Temperature) * 5);
	}
	else if ( (Temperature >= -10) && (Temperature < 0) )
	{
		adjusttime = (uint16_t)((ROOM_TEMPERATURE - Temperature) * 4);
	}
	else if ( (Temperature >= 0) && (Temperature < (int16_t)25) )
	{
		adjusttime = (uint16_t)((ROOM_TEMPERATURE - Temperature) * 5);    //2016.07.02 3 改为5
	}
	else if ( (Temperature >= 25) && (Temperature < 60) )
	{
		adjusttime = (uint16_t)((Temperature - ROOM_TEMPERATURE) * 9);		//5,6,7,4降低过多  2016.07.02 2改为6 2017.03.16 7改为9
	}
	else
	{
		adjusttime = (uint16_t)((Temperature - ROOM_TEMPERATURE) * 7);		//5,6,7,4降低过多  2016.07.02 3改为7
	}
	
	return (adjusttime);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
