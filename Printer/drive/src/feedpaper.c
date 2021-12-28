/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-19
  * @brief   上纸及上纸电机相关的程序.
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
#include	"feedpaper.h"
#include	"blackmark.h"
#include	"heat.h"
#include	"print.h"
#include	"extgvar.h"
#include	"timconfig.h"
#include	"esc.h"
#include	"debug.h"
#include	"power.h"
extern	TypeDef_StructHeatCtrl		g_tHeatCtrl;
extern	TypeDef_StructSystemCtrl	g_tSystemCtrl;
//extern	TypeDef_StructLinePrtBuf	g_tLinePrtBuf;

extern	void	CutMotorStandby(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef	FEED_MOTOR_MONITOR_ENABLE
	uint8_t	m_bDriveCycleStepNumber;		//不同驱动模式时驱动芯片循环一周的电机步数
#endif

#ifdef	FEED_MOTOR_MONITOR_ENABLE
	uint8_t	g_bFeedMotorStepCount;
#endif

/*
	上纸电机相位定义
							相位0		相位1		相位2		相位3
	PHASE_1(A)		1				1				0				0
	PHASE_2(B)		1				0				0				1
	初始化为相位0
*/
/* m_bFeedPhaseCount
	标志上纸电机控制步态的相位，全步驱动时4步1点行，
	范围[0~3]，初始化为0，初始化电机相位控制管脚时设置为与0状态相同。
	电机停止后无需清零。停止后再次开始时也无需清零。
	正向上纸时每走一步电机加1，满4清0.走之前先加1，再根据其值确定相位控制管脚的输出。
	为0时执行0的相位，为1时执行1的相位，依次类推。
						m_bFeedPhaseCount	相位A		相位B
	初始化						0						1				1
	走1步							1						1				0
	走2步							2						0				0
	走3步							3						0				1
	走4步							4(0)				1				1
	
	反向退纸时每走一步电机减1，为0时减到3.根据其值确定相位控制管脚的输出。
	为0时执行3的相位，为3时执行2的相位，依次递减，为1时执行0的相位。
*/
__IO uint8_t		m_bFeedPhaseCount = 0;		//2016.06.16

uint8_t		g_bMaxRushStep = FEED_RUSH_STEPS;
uint32_t	g_iFeedMotorStartStep;

//电机加速时间表
const uint16_t g_tFeedMotorRushTable[] = 
{
	
	16000,
	16000,8000,	5000,	3500,	2800,	2500,	2300,	2144,	2035,	1941,		//精工347机芯  
	1859,	1786,	1721,	1663,	1610,	1561,	1517,	1477,	1439,	1404,
	1372,	1342,	1313,	1287,	1261,	1238,	1215,	1194,	1174,	1155,
	1136,	1119,	1102,	1086,	1071,	1056,	1042,	1029,	1016,	1003,
	991, 	979, 	968, 	957,	947, 	936,	927,	917,	908,	899,	
	890,	882,	873,	865,	857,	850,	842,	835,	828,	821,	
	815,	808,	802,	796,	789,	784,  778,	772,	766,	761,	
	756,	750,	745,	740,	735,	731,	726,	721,	717,	712,	//6
	708,	704,	699,	695,	691,	687,	683,	679,	675,	672,	//5
	668,	664,	661,	657,	654,	651,	647,	644,	641,	637,	//5
	634,	631,	628,	625,	622,	619,  616,	614,	611,	608,	//4
	605,	603,	600,	597,	595,	592,	590,	587,	585,	582,	//3
	580,	577,	575,	573,	570,	568
	

// 	16000,
// 	16000,8000,	5000,	3500,	2800,	2500,	2300,	2144,	2035,	1941,
// 	1859,	1786,	1721,	1663,	1610,	1561,	1517,	1477,	1439,	1404,
// 	1372,	1342,	1313,	1287,	1261,	1238,	1215,	1194,	1174,	1155,
// 	1136,	1119,	1102,	1086,	1071,	1056,	1042,	1029,	1016,	1003,
// 	991, 	979, 	968, 	957,	947, 	936,	927,	917,	908,	899,	
// 	890,	882,	873,	865,	857,	850,	842,	835,	828,	820,	//	
// 	813,	806,	799,	792,	785,	778,	771,	764,	757,	750,	//7
// 	744,	738,	732,	726,	720,	714,	708,	702,	696,	690,	//6
// 	685,	680,	675,	670,	665,	660,	655,	650,	645,	640,	//5
// 	635,	630,	625,	620,	615,	610,	605,	600,	595,	590,	//5
// 	586,	582,	578,	574,	570,	566,	562,	558,	554,	550,	//4
// 	547,	544,	541,	538,	535,	532,	529,	526,	523,	520,	//3
// 	522,	519,	516,	513,	510,	508,	506,	504,	502,	500,	//2
// 	498

	#if	0
	//20000,14000,8000, 6000, 4800,
	4100, 4000, 3898, 3700, 3600,
	3516, 3350, 3150, 3100, 3018, 
	2800, 2518, 2204, 1844, 1477,
	1250, 1100, 1020, 945,  884,
	833
	#endif
	#if	0
	20000,20000,18000,16000,14000,
	12000,10000,8000, 7000, 6000,
	5000, 4500, 4100, 3898, 3600,
	3516, 3350, 3150, 3100, 3018, 
	2800, 2518, 2204, 1844, 1477,
	1250, 1100, 1020, 945,  884,
	#endif
	#if	0
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  828,
	820,	813,	806,	799,  792,
	785,	778,	771,	764,	757,
	750,	744,	738,	732,  726,
	720,	714,	708,	702,	696,
	690,	685,	680,	675,  670,
	665,	660,	655,	650,	645,
	640,	635,	630,	625,  620,
	615,	610,	605,	600,	595,
	590,	586,	582,	578,  574,
	570,	566,	562,	558,	554,
	550,	547,	544,	541,  538,
	535,	532,	529,	526,	523,
	520,	522,	519,	516,  513,
	510,	508,	506,	504,	502,
	500,  498
	#endif
};

const uint16_t	m_tFeedMotorBackRushTable[ ] =         //退纸加速表
{
//	20000,4000,3898,3016,2518,2218,2004,2000,2000,2000,2000,   //机芯手册加速表
    20000,4050,3840,3518,3020,2520,2210,1850,1480,1252
};

//计算电机加速或者减速时间表的长度,注意前面的sizeof不能为sizeof(FEEDMotorRushTable[])						   
#define FEED_MOTOR_RUSH_STEPS	(sizeof(g_tFeedMotorRushTable) / sizeof(g_tFeedMotorRushTable[0]))

//包括起始擎住阶段——>慢启动阶段——>慢停止阶段——>结束擎住阶段
//快速上纸时完成恒速阶段以外全部操作需要的最小步数,
//快速上纸时如果最小步数小于FEED_MIN_FULL_STEPS,则调用单步上纸
#define	FEED_MIN_FULL_STEPS		(FEED_MOTOR_RUSH_STEPS * 2)


/* Private function prototypes -----------------------------------------------*/
#ifdef	BLACKMARK_ADC_USE_PE_ADC
//测纸和测黑标共用同一个传感器
static void	FindBlackMarkSensorShared(void);
static void	JudgeBlackMarkPosition(void);
#else
//以下为测纸和测黑标相互独立
static void	FindBlackMarkSensorIndependent(void);
#endif

//以下的函数全部为本地函数,只给FeedPaper()调用.
//上纸电机动作需要4个相位抽头一系列的状态切换组合,共4个步态,上纸电机走1步,走完1点行距离
static void FeedMoveMinStep(uint32_t direction, uint32_t step);		//最小步进上纸或者退纸
static void FeedMoveFast(uint32_t direction, uint32_t step);			//快速进纸或者退纸

static uint16_t	GetMaxAccelerateStepsOfTemperature(void);
static uint16_t	GetMaxAccelerateStepsOfDymaticHeat(void);
static uint16_t	GetMaxAccelerateStepsOfDrivePrint(void);
static uint16_t	GetMaxAccelerateStepsOfLineSpace(void);
static uint16_t	GetMaxAccelerateStepsOfBitmapPrint(void);
static uint16_t	GetMaxAccelerateStepsOfEmphasizePrint(void);

#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
static uint16_t	GetMaxAccelerateStepsOfDifferentMagnification(void);
#endif

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化上纸电机控制管脚
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitFeedPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(FEED_EN_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= FEED_EN_CTRL_GPIO_PIN_1 | FEED_EN_CTRL_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(FEED_EN_CTRL_GPIO_PORT, &GPIO_InitStructure);
	FeedMotorDisable();				//先不上电
	
	RCC_APB2PeriphClockCmd(FEED_PHASE_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= FEED_PHASE_CTRL_GPIO_PIN_1 | FEED_PHASE_CTRL_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(FEED_PHASE_CTRL_GPIO_PORT, &GPIO_InitStructure);
	FeedPhaseAllSet();			//起始状态00
	
	m_bFeedPhaseCount = 0;
	#ifdef	FEED_MOTOR_MONITOR_ENABLE
		g_bFeedMotorStepCount = 0;
	#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 电机上纸操作,根据给定值确定走纸多少,当前按照总转数使用,
*   包括起始擎住阶段——>慢启动阶段——>恒速阶段——>慢停止阶段——>结束擎住阶段,
*   快速上纸时如果最小步数小于FEED_MIN_FULL_STEPS,则调用单步上纸
*		注意：对上纸电机控制和检测之前必须先初始化管脚
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void FeedPaper(uint32_t direction, uint32_t step)
{
	if (step == 0)
	{
		return;
	}
	
	if (direction == FEED_BACKWARD)		//如果为退纸需要检查最大退纸量
	{
		if (step > FEED_BACK_MAX_STEPS)	//最大退纸值不能超过极限
		{
			return;
		}
	}
	
	if (step > FEED_MIN_FULL_STEPS)		 //上纸量超过一定限度
	{
		FeedMoveFast(direction, step);
	}
	else
	{
		FeedMoveMinStep(direction, step);
	}
}

/*******************************************************************************
* Function Name  : FindMarkPoint
* Description    : 走纸到黑标处,走过黑标停止,如果在300mm内没有找到则自动停止.
	修改找黑标方法. 分为测纸和黑标传感器独立和共用两种情况.
	a)测纸和黑标传感器独立时, 根据黑标传感器标志处理.
	b)二者共用时, 因为采用同一个测纸传感器测黑标, 在黑标位置和只有胶辊时, ADC结果
	相同, 所以无法简单区分黑标和胶辊. 
	找黑标时:
	1.如果当前位置为白纸(非黑标位置), 开始走纸不超过300mm, 直到找到黑标. 进行判断黑标
	处理. 如果找不到黑标, 说明还是白纸, 放弃寻找.
	2.如果当前位置为黑标(其实也可能只是胶辊), 进行判断黑标处理.
	判断黑标处理: 
	继续走纸不超过黑标高度尺寸值. 
	1.如果该过程中发现白纸, 说明之前位置为黑标. 则后退走纸, 直到重新进入黑标位置, 再
	继续后退走纸2mm使黑标停在中心位置.
	2.如果该过程中没有发现白纸, 说明之前位置为胶辊, 停止走纸, 报缺纸.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	FIND_BLACKMARK_MIN_LEN		(1 / MIN_STEP_UNIT_X)		//寻找时最少精度1mm,按点计算
#define	FIND_BLACKMARK_MAX_LEN		(300 / MIN_STEP_UNIT_X)	//最大寻找长度300mm,按点计算
#define	FIND_BLACKMARK_HEIGHT_LEN	(10 / MIN_STEP_UNIT_X)		//黑标高度10mm,按点计算

void	FindMarkPoint(void)
{
	#ifdef	BLACKMARK_ADC_USE_PE_ADC
		FindBlackMarkSensorShared();			//测纸和测黑标共用同一个传感器
	#else
		FindBlackMarkSensorIndependent();	//测纸和测黑标相互独立  
	#endif
}

#ifdef	BLACKMARK_ADC_USE_PE_ADC
//测纸和测黑标共用同一个传感器
/*******************************************************************************
* Function Name  : FindBlackMarkSensorShared
* Description    : 走纸到黑标处,走过黑标停止,如果在300mm内没有找到则自动停止.
	b)二者共用时, 因为采用同一个测纸传感器测黑标, 在黑标位置和只有胶辊时, ADC结果
	相同, 所以无法简单区分黑标和胶辊. 只能确定是黑标(或者胶辊或者无纸)及白纸两种情况.
	找黑标时:
	1.如果当前位置为白纸(非黑标位置), 开始走纸不超过300mm, 直到找到黑标. 进行判断黑标
	处理. 如果找不到黑标, 说明还是白纸, 放弃寻找.
	2.如果当前位置为黑标(其实也可能只是胶辊), 进行判断黑标处理.
	判断黑标处理: 
	继续走纸不超过黑标高度尺寸值. 
	1.如果该过程中发现白纸, 说明之前位置为黑标. 则后退走纸, 直到重新进入黑标位置, 再
	继续后退走纸2mm使黑标停在中心位置.
	2.如果该过程中没有发现白纸, 说明之前位置为胶辊, 停止走纸, 报缺纸.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	FindBlackMarkSensorShared(void)
{
	uint32_t	PaperLenth;
	
	PaperLenth = 0;
	
	//等待之前的走纸完成
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)				//2016.06.30
	{
		;
	}
	
	//1.如果当前位置为白纸(非黑标位置), 开始走纸不超过300mm, 直到找到黑标. 进行判断黑标处理.
	//如果找不到黑标, 说明还是白纸, 放弃寻找.
	if (g_tSystemCtrl.MarkPositionFlag == 0)
	{
		while (g_tSystemCtrl.MarkPositionFlag == 0)	//一直走到黑标处
		{
			GoDotLine(FIND_BLACKMARK_MIN_LEN);				//走1mm
			PaperLenth++;															//统计走纸长度
			if (PaperLenth > FIND_BLACKMARK_MAX_LEN)	//超过最大可能长度还没找到黑标前沿
			{
				break;
			}
		}
		
		//找到黑标. 进行判断黑标处理.
		if (g_tSystemCtrl.MarkPositionFlag)
		{
			JudgeBlackMarkPosition();
		}
		//如果找不到黑标, 说明还是白纸, 放弃寻找.
		else
		{
			return;
		}
	}
	//2.如果当前位置为黑标(其实也可能只是胶辊或者全空白), 进行判断黑标处理.
	else
	{
		JudgeBlackMarkPosition();
	}
}

/*******************************************************************************
* Function Name  : JudgeBlackMarkPosition
* Description    : 	判断黑标处理: 
	继续走纸不超过黑标高度尺寸值. 
	1.如果该过程中发现白纸, 说明之前位置为黑标. 则后退走纸, 直到重新进入黑标位置, 再
	继续后退走纸2mm使黑标停在中心位置.
	2.如果该过程中没有发现白纸, 说明之前位置为胶辊, 停止走纸, 报缺纸.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	JudgeBlackMarkPosition(void)
{
	uint32_t	PaperLenth;
	
	PaperLenth = 0;
	
	//等待之前的走纸完成
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)			//2016.06.30
	{
		;
	}
	
	//在黑标位置(也可能是胶辊或者全空白)下继续走纸不超过黑标高度尺寸值
	g_tFeedButton.KeyDownFlag = 1;			//开始找黑标
	g_tFeedButton.StepCount = FIND_BLACKMARK_MAX_LEN;
	while (g_tSystemCtrl.MarkPositionFlag ==0 )
	{
		if(0 ==g_tFeedButton.StepCount)					//没找到黑标前沿
		{
			g_tFeedButton.KeyDownFlag =0;					  //停止走纸
			return;
	 	}
	}	
	//1.如果该过程中发现白纸, 说明之前位置为黑标. 
	//再继续后退走纸10mm使黑标停在中心位置.
	g_tFeedButton.KeyDownFlag =1;						            	//按键按下标志
	g_tFeedButton.StepCount = FIND_BLACKMARK_HEIGHT_LEN;	//设置黑标走纸方向长度最大为10mm，
	while(g_tSystemCtrl.MarkPositionFlag !=0) 				    //走过黑标
	{
		if(0 ==g_tFeedButton.StepCount )				            //超过黑标宽度，则停止，可能是缺纸
			break;
	}
	
	if (g_tFeedButton.StepCount != 0)        
	{
		g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 不缺纸
	}
	//2.如果该过程中没有发现白纸, 说明之前位置为胶辊, 停止走纸, 报缺纸.
	else
	{
		g_tError.PaperSensorStatusFlag |= 0x01;	  //没有走过黑标测报缺纸
	}
	g_tFeedButton.KeyDownFlag =0;							  //停止走纸
	g_tFeedButton.StepCount =0;			
}
#else
//以下为测纸和测黑标相互独立
/*******************************************************************************
* Function Name  : FindBlackMarkSensorIndependent
* Description    : 走纸到黑标处,走过黑标停止,如果在300mm内没有找到则自动停止.
	a)测纸和黑标传感器独立时, 根据黑标传感器标志处理.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	FindBlackMarkSensorIndependent(void)
{
	uint32_t	PaperLenth;
	
	PaperLenth = 0;
	
	//等待之前的走纸完成
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)			//2016.06.30
	{
		;
	}
	
	//在黑标位置(也可能是胶辊或者全空白)下继续走纸不超过黑标高度尺寸值
	g_tFeedButton.KeyDownFlag = 1;			//开始找黑标
	g_tFeedButton.StepCount = FIND_BLACKMARK_MAX_LEN;
	while (g_tSystemCtrl.MarkPositionFlag ==0 )
	{
		if(0 ==g_tFeedButton.StepCount)					//没找到黑标前沿
		{
			g_tFeedButton.KeyDownFlag =0;					  //停止走纸
			return;
	 	}
	}	
	//1.如果该过程中发现白纸, 说明之前位置为黑标. 
	//再继续后退走纸10mm使黑标停在中心位置.
	g_tFeedButton.KeyDownFlag =1;						            	//按键按下标志
	g_tFeedButton.StepCount = FIND_BLACKMARK_HEIGHT_LEN;	//设置黑标走纸方向长度最大为10mm，
	while(g_tSystemCtrl.MarkPositionFlag !=0) 				    //走过黑标
	{
		if(0 ==g_tFeedButton.StepCount )				            //超过黑标宽度，则停止，可能是缺纸
			break;
	}
	
	if (g_tFeedButton.StepCount != 0)        
	{
		g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 不缺纸
	}
	//2.如果该过程中没有发现白纸, 说明之前位置为胶辊, 停止走纸, 报缺纸.
	else
	{
		g_tError.PaperSensorStatusFlag |= 0x01;	  //没有走过黑标测报缺纸
	}
	g_tFeedButton.KeyDownFlag =0;							  //停止走纸
	g_tFeedButton.StepCount =0;			
}
#endif

void	FeedMotorSetPhase(uint8_t	bPhase)
{
	if (bPhase == 0)
	{
		FeedPhaseAllSet();
	}
	else if (bPhase == 1)
	{
		FeedPhaseASet();
		FeedPhaseBReset();
	}
	else if (bPhase == 2)
	{
		FeedPhaseAllReset();
	}
	else if (bPhase == 3)
	{
		FeedPhaseAReset();
		FeedPhaseBSet();
	}
	else
	{
		FeedPhaseAllSet();
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 上纸电机向前转过1个步距，控制电机相位,按照相位0-1-2-3-0顺序循环
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	FeedMotorForward(void)
{
	m_bFeedPhaseCount++;
	m_bFeedPhaseCount %= 4;
	switch (m_bFeedPhaseCount)
	{
		case 0:
			FeedMotorSetPhase(0);
			break;
		case 1:
			FeedMotorSetPhase(1);
			break;
		case 2:
			FeedMotorSetPhase(2);
			break;
		case 3:
			FeedMotorSetPhase(3);
			break;
		default:
			//FeedMotorSetPhase(0);
			//m_bFeedPhaseCount = 0;
			break;
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 上纸电机向后转过1个步距，控制电机相位,按照相位0-3-2-1-0顺序循环
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	FeedMotorBackward(void)
{
	switch (m_bFeedPhaseCount)
	{
		case 0:
			FeedMotorSetPhase(3);
			m_bFeedPhaseCount = 3;
			break;
		case 1:
			FeedMotorSetPhase(0);
			m_bFeedPhaseCount--;
			break;
		case 2:
			FeedMotorSetPhase(1);
			m_bFeedPhaseCount--;
			break;
		case 3:
			FeedMotorSetPhase(2);
			m_bFeedPhaseCount--;
			break;
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 退纸n点行
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void FeedMotorBackStep(uint32_t BackNum)
{
	uint8_t i;
    uint8_t StepID;
	
	if(BackNum == 0)
    {    
		return;
    }
    g_bMotorRunFlag = 1;
    FeedMotorEnable();			//使能芯片,允许输出
    
	StepID = 0;
	BackNum += 10; 				//根据机芯手册 需要加上前进的10步 
      
    m_bFeedPhaseCount--;
	while(BackNum)
	{	
		BackNum--; 
        m_bFeedPhaseCount++;
        m_bFeedPhaseCount %= 4;
        switch (m_bFeedPhaseCount)
        {
            case 0:
			FeedMotorSetPhase(2);
			break;
            case 1:
			FeedMotorSetPhase(1);
			break;
            case 2:
			FeedMotorSetPhase(0);
			break;
            case 3:
			FeedMotorSetPhase(3);
			break;
        }   

		DelayUs(m_tFeedMotorBackRushTable[StepID]);	
		if (StepID <  9)	         //根据机芯手册的加速表 取最大步数
        {    
			StepID++;	
        }    
	}
	DelayUs(20000);				     //根据机芯手册 延迟20ms
	
    m_bFeedPhaseCount--;
	StepID = 0;
	for(i = 0 ; i < 10 ; i++)       //根据机芯手册 在进纸10步
	{
        m_bFeedPhaseCount++;
        m_bFeedPhaseCount %= 4;
        switch (m_bFeedPhaseCount)
        {
            case 0:
			FeedMotorSetPhase(0);
			break;
            case 1:
			FeedMotorSetPhase(1);
			break;
            case 2:
			FeedMotorSetPhase(2);
			break;
            case 3:
			FeedMotorSetPhase(3);
			break;
        }    
		DelayUs(m_tFeedMotorBackRushTable[StepID]);	
		if (StepID <  9)	//取最大步数
        {    
			StepID++;
        }   
	}
	DelayUs(20000);				//延迟20ms
	
	FeedMotorDisable();		    //关闭电机
	g_bMotorRunFlag = 0;
	
}


/*******************************************************************************
* Function Name  : 函数名
* Description    : 上纸电机向某个方向转过1个步距
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	StepFeedMotor(uint32_t direction)
{
	#ifdef	MAINTAIN_ENABLE
		static uint8_t	cMaintainCount = 0;
	#endif

	#ifdef	ERROR_EMERGENCY_PROTECT_ENABLE
		if (ErrorDetect())				//判断是否有错误及报警
		{
			SetEmergencyProtect();	//保护
			g_bFeedTaskFlag = 1;		//任务未完成标志
			while (ErrorDetect());
			return;
		}
	#endif
		
	#ifdef	DBG_DUMMY_FEED

	#else
		FeedMotorEnable();			//使能芯片,允许输出，此处是通过控制电流大小的方式
		if (direction == FEED_FORWARD)
		{
			FeedMotorForward();		//方向控制
		}
		else
		{
			FeedMotorBackward();	//方向控制
		}
		
		#ifdef	MAINTAIN_ENABLE
			cMaintainCount++;
			if (cMaintainCount == DEFAULT_CODE_LINE_HEIGHT * STEP_NUMBERS_PER_DOT_HEIGHT)
			{	
				cMaintainCount = 0;
				R_Counter.LineCounter++;	//行计数器＋1,以行为单位
				C_Counter.LineCounter++;
			}
		#endif
		
		#ifdef	FEED_MOTOR_MONITOR_ENABLE
			if (direction == FEED_FORWARD)
			{
				g_bFeedMotorStepCount++;
				if (g_bFeedMotorStepCount >= m_bDriveCycleStepNumber)
				{
					g_bFeedMotorStepCount = 0;
				}
				if (GetFeedMotorDriveCycleHomeStatus() == 0x00)
				{
					if (g_bFeedMotorStepCount)	//出现了不一致
					{
						InitFeedPort();
					}
				}
			}
			else
			{
				if (g_bFeedMotorStepCount)
				{
					g_bFeedMotorStepCount--;
				}
				else
				{
					g_bFeedMotorStepCount = (m_bDriveCycleStepNumber - 1);
				}
				if (GetFeedMotorDriveCycleHomeStatus() == 0x00)
				{
					if (g_bFeedMotorStepCount)	//出现了不一致
					{
						InitFeedPort();
					}
				}
			}
		#endif	//FEED_MOTOR_MONITOR_ENABLE 处理完毕
	#endif	//真实走纸处理完毕
	g_iFeedMotorStartStep++;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 上纸电机开始加电,保持之前的相位值,维持一段时间,但是没有新步进
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	StartFeedMotor(uint32_t direction)	//起始擎住阶段
{
	#ifdef	DBG_DUMMY_FEED
		
	#else
		FeedMotorEnable();			//使能芯片,允许输出
		if (direction == FEED_FORWARD)
		{
			//FeedMotorForward();		//方向控制
		}
		else
		{
			//FeedMotorBackward();	//方向控制
		}
	#endif
	
	g_bMotorRunFlag = 1;
	g_StepCount = 0;
	g_iFeedMotorStartStep = 0;
	MotoFeedInc = 1;	
}

void	StopFeedMotor(uint32_t direction)	//结束擎住阶段
{
	#ifdef	DBG_DUMMY_FEED
		
	#else
		FeedMotorEnable();			//使能芯片,允许输出
		if (direction == FEED_FORWARD)
		{
			//FeedMotorForward();		//方向控制
		}
		else
		{
			//FeedMotorBackward();	//方向控制
		}
	#endif
}

void EndFeedMotor(void)	//全部开路阶段
{
	HeatPowerOff();
	FeedMotorDisable();		//禁止芯片,H桥不允许输出
	g_bMotorRunFlag = 0;
	g_StepCount = 0;
	g_bMaxRushStep = MAX_RUSH_STEPS;
	if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_QT)	//WH驱动模式标志
	{
		g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_NONE;	//退出WH驱动模式
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 上纸电机以恒速最小步步进,可以实现一次一个最小步.
* Input          : direction决定移动的方向,step决定移动的步进数,参数值 >= 0
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void FeedMoveMinStep(uint32_t direction, uint32_t step)	//最小步进上纸或者退纸
{
	if ((direction == FEED_FORWARD) || (direction == FEED_BACKWARD))
	{
		StartFeedMotor(direction);			//起始擎住阶段
		DelayUs(FEED_START_US);					//延时us
		while (step--)
		{
			StepFeedMotor(direction);			//改变一个相位
			DelayUs(FEED_SINGLE_STEP_SPEED_US);	//延时us
		}
		
		StopFeedMotor(direction);				//结束擎住阶段
		DelayUs(FEED_STOP_US);					//延时us
		EndFeedMotor();									//全部开路阶段
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 上纸电机以高速步进,可以实现快速进纸或者退纸
* Input          : direction决定移动的方向,step决定移动的步进数,参数值 >= 0
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void FeedMoveFast(uint32_t direction, uint32_t step)
{
	uint32_t	i;
	uc16 * ptime;
	
	if ((direction == FEED_FORWARD) || (direction == FEED_BACKWARD))
	{
		if (step > FEED_MIN_FULL_STEPS)					//确保步数足够多
		{
			StartFeedMotor(direction);						//起始擎住阶段
			DelayUs(FEED_START_US);								//延时us
			
			i = 0;
			ptime = g_tFeedMotorRushTable;				//指向加速时间表起始
			while (i < FEED_MOTOR_RUSH_STEPS)			//加速阶段
			{
				StepFeedMotor(direction); //转过一步
				DelayUs(*ptime++);									//启动加速延时
				i++;
			}
			
			i = step - FEED_MIN_FULL_STEPS;				//计算恒速步数
			ptime = g_tFeedMotorRushTable;				//指向加速时间表起始
			ptime += (FEED_MOTOR_RUSH_STEPS - 1); //指向加速时间表末尾
			while (i)															//恒速阶段
			{
				StepFeedMotor(direction);	//转过一步
				DelayUs(*ptime);										//恒速延时
				--i;
			}
			
			i = FEED_MOTOR_RUSH_STEPS;						//减速步数
			ptime = g_tFeedMotorRushTable;				//指向加速时间表起始
			ptime += (FEED_MOTOR_RUSH_STEPS - 1);	//指向加速时间表末尾
			while (i)
			{
				StepFeedMotor(direction); //转过一步
				DelayUs(*ptime--);									//减速延时
				--i;
			}
			
			StopFeedMotor(direction);							//结束擎住阶段
			DelayUs(FEED_STOP_US);								//延时us
			
			EndFeedMotor();												//全部开路阶段
		}
	}
}


static uint16_t	GetMaxAccelerateStepsOfTemperature(void)
{
	int16_t	temperature;
	
	temperature = GetTemperature();					//计算机芯温度值
	if (temperature < 10)
	{
		return (MAX_RUSH_STEPS_UNDER_10_CENTIGRADE);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}

static uint16_t	GetMaxAccelerateStepsOfDymaticHeat(void)
{
	if (g_tHeatCtrl.HeaterGroupNumberCpy > 1)
	{
		return (MAX_RUSH_STEPS_ON_DYNAMIC_HEAT);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}

static uint16_t	GetMaxAccelerateStepsOfLineSpace(void)
{
	uint16_t	LineSpaceDotNumber;
	uint16_t	maxstep;
	uint16_t	tempstep;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	//计算行间距点数
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.CodeLineHigh > ptLPB->PrtMode.MaxLineHigh)
		LineSpaceDotNumber = (ptLPB->PrtMode.CodeLineHigh - ptLPB->PrtMode.MaxLineHigh);
	else
		LineSpaceDotNumber = 0;
	
	//测试时使用标准1:1汉字整行批量打印,内容相同,只修改行间距,电压24V
	//所有测试效果都是以每个区间的下限进行的结果,数值表示加速步数
	if (LineSpaceDotNumber > 2)
	{	//126(220mm/s)时打印正常,纵向横向放大8倍打印测试顺畅,偶尔会出现白道
		maxstep = MAX_RUSH_STEPS;
	}
	else
	{
		maxstep = 90 + LineSpaceDotNumber * 10;
	}
	
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
		tempstep = GetMaxAccelerateStepsOfDifferentMagnification();
		if (tempstep < maxstep)
		{
			maxstep = tempstep;
		}
	#endif
	
	return (maxstep);
}

#if	0
static uint16_t	GetMaxAccelerateStepsOfBitmapPrint(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))	//图形打印
	{
		return (MAX_RUSH_STEPS_ON_PRT_BITMAP);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}
#endif

#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
static uint16_t	GetMaxAccelerateStepsOfEmphasizePrint(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.EmphaSizeMode)		//加重打印
	{
		return (MAX_RUSH_STEPS_ON_EMPHASIZE_PRT);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}
#endif

#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
//横向放大倍数1,纵向放大倍数2,3时,串口压缩;
//横向放大倍数1,纵向放大倍数2时,E,U口压缩;
static uint16_t	GetMaxAccelerateStepsOfDifferentMagnification(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	//if ((ptLPB->PrtMode.MaxRatioX == 1) && \
	//		((ptLPB->PrtMode.MaxRatioY == 2) || (ptLPB->PrtMode.MaxRatioY == 3)))
	if ((ptLPB->PrtMode.MaxRatioX > 1) || (ptLPB->PrtMode.MaxRatioY > 1))
	{
		return (MAX_RUSH_STEPS_ON_DIFFERENT_MAGNIFICATION_PRT);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}
#endif

static uint16_t	GetMaxAccelerateStepsOfDrivePrint(void)
{
	uint16_t	StepNum;
	
	StepNum = MAX_RUSH_STEPS;
	if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)
	{
		if (g_tSystemCtrl.PrtSourceIndex == COM)						//串口
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_COM;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == USB)				//U口
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_USB;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == ETHERNET)	//E口
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_ETHERNET;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == LPT)				//L口
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_LPT;
		}
		else
		{
		}
	}
	else
	{
		if (g_tSystemCtrl.PrtSourceIndex == COM)						//串口
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_COM;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == USB)				//U口
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_USB;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == ETHERNET)	//E口
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_ETHERNET;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == LPT)				//L口
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_LPT;
		}
		else
		{
		}
	}
	
	return (StepNum);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 确定电机最高加速步数,根据温度,加热次数,是否驱动打印综合确定
*	温度在0~10℃时,打印时最高速度不超过1200PPS,即加速步不超过68
*	动态加热时不超过1200PPS
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
uint16_t	GetMaxAccelerateSteps(void)
{
	uint16_t	maxstep;
	uint16_t	temp;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	//2016.06.11 根据打印模式赋值
	
	MAX_RUSH_STEPS = g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode];		//2016.06.11
	maxstep = MAX_RUSH_STEPS;		//默认值
#if	0
	temp = GetMaxAccelerateStepsOfTemperature();	//检查温度
	if (temp < maxstep)
	{
		maxstep = temp;
	}
	
	temp = GetMaxAccelerateStepsOfDymaticHeat();	//检查加热控制
	if (temp < maxstep)
	{
		maxstep = temp;
	}
	
	if (g_tPrtCtrlData.PCDriverMode)	//驱动打印,驱动模式时不使用打印缓冲区
	{
		temp = GetMaxAccelerateStepsOfDrivePrint();
		if (temp < maxstep)
		{
			maxstep = temp;
		}
	}
	else
	{
		ptLPB = &U_PrtBuf.g_tLinePrtBuf;
		if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))	//图形打印
		{
			temp = MAX_RUSH_STEPS_ON_PRT_BITMAP;
			if (temp < maxstep)
			{
				maxstep = temp;
			}
		}
		else	//非图形打印
		{
			#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
			if (ptLPB->PrtMode.EmphaSizeMode)		//加重打印
			{
				temp = MAX_RUSH_STEPS_ON_EMPHASIZE_PRT;
				if (temp < maxstep)
				{
					maxstep = temp;
				}
			}
			#endif
			
			temp = GetMaxAccelerateStepsOfLineSpace();	//检查打印缓冲区
			if (temp < maxstep)
			{
				maxstep = temp;
			}
			if (ptLPB->PrtMode.CharCycle90)			//旋转90度打印
			{
				temp = MAX_RUSH_STEPS_ON_CYCLE90_PRT;
				if (temp < maxstep)
				{
					maxstep = temp;
				}
			}
			
			#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
			if (ptLPB->PrtMode.CharNotFlag)		//反显打印
			{
				temp = MAX_RUSH_STEPS_ON_CHAR_NOT_PRT;
				if (temp < maxstep)
				{
					maxstep = temp;
				}
			}
			#endif
		}
	}
#endif	
	return (maxstep);
}

/*******************************************************************************
* Function Name  : void AdjustMotorStep(uint16_t minsteptime)
* Description    : 根据传入的最小单步时间与当前的延时比较找出新的适合的步态顺序号
* 根据全局变量g_StepCount对应顺序时间，如果minsteptime<当前时间，不改变当前顺序，
* 结束。否则顺序递减后再比较直到满足条件或者到记录头部
* 注意需要判断从当前g_StepCount - 4开始找起,因为必须满足最后一步也要大于minsteptime
* 2016-7-13修改，根据是否驱动打印限制最高速度.
* Autor			 : 
* Date			 : 2016-7
* Input          : 一步电机最短时间
* Output         : 
* Return         : 
*******************************************************************************/
void	AdjustMotorStep(uint16_t minsteptime)
{
	uint16_t	maxstep;
	
	//得到最大加速步数,决定本次打印缓冲区中所有内容的最高限度速度
	maxstep = GetMaxAccelerateSteps();
	if (g_StepCount > maxstep)	//判断目前速度是否超过限定最高速度
	{
		g_StepCount = maxstep;		//如果超过,以限定速度为标准进行调整
	}
	g_bMaxRushStep = maxstep;
	
	//判断最大加速步数的时间是否会影响加热,如果不影响直接返回,否则需调整
	//最高速度时会影响加热,需要减速,找出加速表中加速时间大于一点行总加热时间的步数
	while (1)		//2016.06.14 保证电机走步时间要大于加热时间
	{
		if(g_tFeedMotorRushTable[g_StepCount] > (minsteptime+20))
		{
			break;
		}			
		g_StepCount--;
	}
}
/*******************************************************************************
* Function Name  : KeyFeedPaper(uint16_t DotLine)
* Description    : 走纸n点行，按键走纸方式 不占用打印缓冲区
* Input          : None
* CONput         : None
* Return         : None
*******************************************************************************/
void KeyFeedPaper(uint16_t DotLine)
{
  g_tFeedButton.KeyDownFlag = 1;			
	g_tFeedButton.StepCount = DotLine;	
}
/*******************************************************************************
* Function Name  : GoDotLine
* Description    : 走纸N点行
* Input          : DotLineNumber:走纸的点行数
* Output         : 
* Return         : 
*******************************************************************************/
void	GoDotLine(uint32_t DotLineNumber)
{
	uint32_t	i;
	for (i = 0; i < DotLineNumber; i++)
	{
			PrtDrvBufFull();										//2016.06.27 走点行放入行打印缓冲区
			IncDrvPrtBufAddr(0x00, 0);
	}
}

#define	WAIT_LPB_ILDE_MS		(10)
void	WaitPreviousPrintFinish(void)
{
	while (LPB_STATUS_ILDE != GetPrintStatus());
	
	#if	0
	uint32_t	SleepCounter;    	//定时
	
	SleepCounter = g_tSystemCtrl.SleepCounter;
	while (LPB_STATUS_ILDE != GetPrintStatus())
	{
		if (g_tSystemCtrl.SleepCounter > SleepCounter)
		{
			if ((g_tSystemCtrl.SleepCounter - SleepCounter) > WAIT_LPB_ILDE_MS)
			{
				StartTimer(PRINT_CHECK_TIMER);
				SleepCounter = g_tSystemCtrl.SleepCounter;
			}
		}
		else
		{
			SleepCounter = g_tSystemCtrl.SleepCounter;
		}
	}
	#endif
}

//等待加热完毕
void	WaitHeatFinish(void)
{
	while (g_tHeatCtrl.HeatEnable == HEAT_ENABLE);
}

void	WaitFeedPaperFinish(void)
{
	while (g_tSystemCtrl.StepNumber);		//等待走纸完成
}

void	WaitHeatAndFeedFinish(void)
{
	//等待前点行走纸或者加热完毕
	while ((g_tHeatCtrl.HeatEnable == HEAT_ENABLE) || (g_tSystemCtrl.StepNumber != 0));
}

//等待加热或走纸完成
void	WaitPrintAndFeedPaperFinish(void)
{
	WaitPreviousPrintFinish();
	WaitHeatAndFeedFinish();
}

/*******************************************************************************
* Function Name  : void ErrorDetect(void)
* Description	: 报警检测。如果出现报警，则等待当前行打印完后关加热\电机，
*					并等待直到报警解除。
* Input		  : None
* Output		 : None
* Return		 : None
*******************************************************************************/
uint32_t	ErrorDetect(void)
{
	uint8_t	ErrorStatus;
	
	ErrorStatus = 0;
	if (g_tError.AR_ErrorFlag || g_tError.R_ErrorFlag || g_tError.UR_ErrorFlag)
	{
		ErrorStatus = 1;
	}
	else
	{
		if ((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)	//纸尽直接停止打印
		{
			ErrorStatus = 1;
		}
		else
		{
			if ((g_tError.PaperEndStopSensorSelect & 0x01) == 0x01)	//选择了纸将尽传感器停止打印
			{
				if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)	//纸将尽停止打印
				{	//打印机打印完当前行并进纸后进入离线状态
					if ((LPB_STATUS_ILDE == GetPrintStatus()) && \
							(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && \
							(g_tSystemCtrl.StepNumber == 0))
					{
						ErrorStatus = 1;
					}
				}
			}
		}
	}
	
	return	(ErrorStatus);
}

void	SetEmergencyProtect(void)
{
	TIM_Cmd(FEED_TIMER, DISABLE);					//关定时器
	TIM_Cmd(HEAT_TIMER, DISABLE);					//关定时器
	TIM_Cmd(PRINT_CHECK_TIMER, DISABLE);	//关定时器
	
	EndHeat();						//关加热
	EndFeedMotor();				//关上纸电机
	#if defined (CUTTER_ENABLE)
		CutMotorStandby();	//关切刀电机
	#endif
	
	g_tRealTimeStatus.PrinterStatus |= 0x08;	//先进入离线状态
}

void	ReleaseEmergencyProtect(void)
{
	g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//错误排除完毕,自动脱离离线状态
	DelayMs(1000);
	if (LPB_STATUS_ILDE != GetPrintStatus())
	{
		TIM_Cmd(PRINT_CHECK_TIMER, ENABLE);		//开定时器
	}
	
	if ((g_tHeatCtrl.HeatEnable == HEAT_ENABLE) || (g_tSystemCtrl.StepNumber != 0))
	{
		TIM_Cmd(FEED_TIMER, ENABLE);					//开定时器
		TIM_Cmd(HEAT_TIMER, ENABLE);					//开定时器
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
