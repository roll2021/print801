/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-6-4
  * @brief   时间设置相关的程序.
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
#include	"timconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	IncreaseDelayMs(void)
{
	volatile static uint32_t DelayMsCount = 0;
	
	DelayMsCount++;
}

void	DelayUs(uint32_t	delaytime)
{
	uint32_t	i;
	volatile uint8_t	j;
	
	if (delaytime > 0)
	{
		for (i = 0; i < delaytime; i++)
		{
			for (j = 0; j < 9; j++);	//需要精确调整 J 的数值,72M：j=9; 48M：j=6
		}
	}
}

void	DelayMs(uint32_t DelayNumber)
{
	volatile uint32_t	i;		//2016.06.04
	volatile uint8_t	j;
	
	if (DelayNumber > 0)
	{
		for (i = 0; i < DelayNumber * 1000; i++)
		{
			for (j = 0; j < 9; j++);	//需要精确调整 J 的数值,72M：j=9; 48M：j=6
		}
	}
#if	0
	if (DelayNumber > 0)
	{
		DelayMsCount = 0;
		while (DelayMsCount < DelayNumber)
		{
			;
		}
	}
#endif
}

/*******************************************************************************
* Function Name  : void TimerConfigPrescaler(TIM_TypeDef* TIMx, uint32_t Prescaler)
* Description    : 初始化各定时器的预分频因子
* Input          : TIMx:定时器号，Prescaler:预分频因子,PCLK1上定时器.
* Output         : None
* Return         : None
*******************************************************************************/
void TimerConfigPrescaler(TIM_TypeDef* TIMx, uint32_t Prescaler)
{
	TIM_TimeBaseInitTypeDef		TimeBaseInitStruct;
	RCC_ClocksTypeDef					RCC_ClocksStatus;
	uint32_t	TimerClockHz;
	
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	/*----STEP1:复位使用的TIMERx设备----*/
	TIM_DeInit(TIMx);
	
	//使能时钟
	if (TIMx == TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if (TIMx == TIM3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if (TIMx == TIM4)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else if (TIMx == TIM5)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	else if (TIMx == TIM6)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	else if (TIMx == TIM7)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	else if (TIMx == TIM12)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	else if (TIMx == TIM13)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	else if (TIMx == TIM14)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	if (TIMx == TIM1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if (TIMx == TIM8)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	else if (TIMx == TIM9)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	else if (TIMx == TIM10)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	else if (TIMx == TIM11)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
	else if (TIMx == TIM15)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	else if (TIMx == TIM16)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	else if (TIMx == TIM17)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	
	/*----STEP2:设置使用的TIMERx设备----*/
	RCC_GetClocksFreq(&RCC_ClocksStatus);			//读取系统时钟
	
	if ((TIMx == TIM2) || (TIMx == TIM3)  || (TIMx == TIM4) || \
			(TIMx == TIM5) || (TIMx == TIM6)  || (TIMx == TIM7) || \
			(TIMx == TIM12) || (TIMx == TIM13)  || (TIMx == TIM14))
	{
		TimerClockHz = RCC_ClocksStatus.PCLK1_Frequency;
	}
	else if ((TIMx == TIM1) || (TIMx == TIM8) || \
			(TIMx == TIM9) || (TIMx == TIM10)  || (TIMx == TIM11) || \
			(TIMx == TIM15) || (TIMx == TIM16)  || (TIMx == TIM17))
	{
		TimerClockHz = RCC_ClocksStatus.PCLK2_Frequency;
	}
	
	if (RCC_ClocksStatus.HCLK_Frequency == TimerClockHz)
	{	 //预分频值,	
		if (Prescaler < TimerClockHz)
		{
			TimeBaseInitStruct.TIM_Prescaler = TimerClockHz / Prescaler - 1;
		}	
	}
	else
	{	//预分频值
		if (Prescaler < (TimerClockHz * 2))
		{
			TimeBaseInitStruct.TIM_Prescaler = (TimerClockHz * 2) / Prescaler - 1;
		}
	}
	
	TimeBaseInitStruct.TIM_ClockDivision	= TIM_CKD_DIV1;					//时钟分频因子
	if ((TIMx != TIM6) || (TIMx != TIM7))
	{
		TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Down;	//计数模式向下
	}
	else
	{
		TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up;		//计数模式向上
	}
	TimeBaseInitStruct.TIM_Period					= 1;										//重装载值,AutoReload
	TimeBaseInitStruct.TIM_RepetitionCounter = 0; 								//T1，T8才有用
	
	TIM_TimeBaseInit(TIMx, &TimeBaseInitStruct);
	
	/*----STEP3:清除使用的TIMERx标志----*/
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	
	/*----STEP4:允许使用的TIMERx中断----*/
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	
	TIM_UpdateDisableConfig(TIMx, DISABLE);
	
	/*----STEP5:允许使用的TIMERx设备----*/
	TIM_Cmd(TIMx, DISABLE);
}

/*******************************************************************************
* Function Name  : SetTimerCounterWithAutoReload(TIM_TypeDef* TIMx, uint16_t Counter)
* Description    : 设置定时器的计数值
* Input          : TIMx:定时器号，Counter:计数值、重装载值
* Output         : None
* Return         : None
*******************************************************************************/
void  SetTimerCounterWithAutoReload(TIM_TypeDef* TIMx, uint16_t Counter)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIMx->ARR = Counter;
	TIMx->CNT = Counter;
}

/*******************************************************************************
* Function Name  : StartTimer(TIM_TypeDef* TIMx)
* Description    : 启动定时器
* Input          : TIMx:定时器号
* Output         : None
* Return         : None
*******************************************************************************/
void  StartTimer(TIM_TypeDef* TIMx)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, ENABLE);
}

/*******************************************************************************
* Function Name  : StopTimer(TIM_TypeDef* TIMx)
* Description    : 关闭定时器
* Input          : TIMx:定时器号
* Output         : None
* Return         : None
*******************************************************************************/
void  StopTimer(TIM_TypeDef* TIMx)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, DISABLE);
}

#ifdef	DBG_PROCESS_TIME
volatile uint32_t	g_iDbgProcessTimeUs;
void	InitSystick(void)
{
	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(72000000 / 1000))
	{ 
		/* Capture error */
		while (1);
	}
	SysTick_CounterCmd(SysTick_Counter_Disable);	//禁止使能
	//配置systick时钟源,72/8=9MHZ
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	//设置重装载值,定时us, =9,1us; =90,10us; =900,100us; =9000,1ms;
	SysTick_SetReload(90);
	SysTick_CounterCmd(SysTick_Counter_Clear);		//设置定时器初值
	SysTick_ITConfig(ENABLE);
	g_iDbgProcessTimeUs = 0;
}

void	StopProcessTimeCounter(void)
{
	SysTick_CounterCmd(SysTick_Counter_Disable);	//禁止使能
	SysTick_CounterCmd(SysTick_Counter_Clear);		//设置定时器初值
}
#endif

uint32_t	GetTimerEnableStatus(TIM_TypeDef* TIMx)
{
	/* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	if (TIM_CR1_CEN == (TIMx->CR1 & TIM_CR1_CEN))
		return (ENABLE);		//开启
	else
		return (DISABLE);		//关闭
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
