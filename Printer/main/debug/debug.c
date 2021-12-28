/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   调试开发阶段功能的程序
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
#include	"apptype.h"
#include	"mode.h"
#include	"debug.h"

#ifdef	DBG_RX_TOTAL_THEN_PRT
extern volatile uint8_t	g_bRxFinishFlag;	//标示串口通信中数据是否已经接收完毕
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#ifdef	VERSION_DEBUG	//逐个测试各个部件
#include	"led.h"
#include	"button.h"
#include	"extgvar.h"
#include	"update.h"

void	LedHint(void)
{
#if defined	(FEED_BUTTON_CHECK_BY_INT) || defined (RESET_ETHERNET_BUTTON_CHECK_BY_INT)
	if (g_tFeedButton.KeyStatus)
	{
		PELedOn();						//LED指示灯
		if (g_tFeedButton.KeyCount)
			g_tFeedButton.KeyCount--;
		if (!g_tFeedButton.KeyCount)
			g_tFeedButton.KeyStatus = 0;
	}
	else
	{
		PELedOff();						//LED指示灯
	}
#else
	//以查询方式测试按键
	if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
	{
		PELedOn();						//LED指示灯
	}
	else
	{
		PELedOff();						//LED指示灯
	}
#endif
}

#ifdef	DBG_TIM
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试各个定时器设置,通过LED指示灯观察定时时间是否准确,
*	所有定时器都通过这种方式测试.需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口
*	3.设置中断
*	4.初始化定时器,启动定时器,定时器中断中操作LED
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	TIM_TypeDef * TIMx;
	uint16_t	DelayUs;
	
	SystemInit();					//1.初始化时钟
	PELedPortInit();			//2.初始化LED端口
	ErrorLedPortInit();	
	InitNVIC();						//3.设置中断
	
	//4.初始化定时器,启动定时器,定时器中断中操作LED
	#ifdef	DBG_TIM1
	//预分频10us,定时1ms
	TimerConfigPrescaler(TIM1, 100000);
	TIMx = TIM1;
	DelayUs = 100;		//决定延时中断周期
	#endif
	
	#ifdef	DBG_TIM2
	//设置T2为系统时标,10uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(TIM2, 100000);
	TIMx = TIM2;
	DelayUs = 100;		//决定延时中断周期
	#endif
	
	#ifdef	DBG_TIM3
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(TIM3, 1000000);
	TIMx = TIM3;
	DelayUs = 1000;		//决定延时中断周期
	#endif
	
	#ifdef	DBG_TIM4
	TimerConfigPrescaler(TIM4, 1000000);	//设置T4为走纸定时器,预分频为1000000,1uS,备用
	TIMx = TIM4;
	DelayUs = 1000;		//决定延时中断周期
	#endif
	
	#ifdef	DBG_TIM5
	//设置T5为启动AD定时器,预分频延时1uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(TIM5, 1000000);
	TIMx = TIM5;
	DelayUs = 1000;		//决定延时中断周期
	#endif
	
	TIM_SetCounter(TIMx, DelayUs);		//设置延时并启动定时
	TIM_SetAutoreload(TIMx, DelayUs);
	StartTimer(TIMx);
	
	PELedOn();												//LED指示灯初始化
	ErrorLedOn();
	
	while (1);
}
#endif	/*所有定时器测试程序结束*/


#ifdef	DBG_BUTTON
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试按键,通过LED指示灯显示是否检测到按键
*	测试上纸按键和以太网复位按键.通过EXTI中断实现.
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,按键全局变量
*	3.设置中断
*	4.检查按键变量状态值,操作LED
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	////InitResetEthernetButtonPort();
	
	InitKeyCtrlStructParameter();			//2.按键数据结构
	
	InitNVIC();						//3.设置中断
	
	//4.检查按键变量状态值,操作LED		
	while (1)
	{
		LedHint();		//通过LED指示灯提示,可能是按键中断中处理,也可能是通过查询按键方式处理
	}
}
#endif	/*按键测试程序结束*/


#ifdef	DBG_POWER_CTRL
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"power.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试加热电源电压输出控制,通过LED指示灯显示是否输出,用示波器观察
*	通过定时器2中断实现.
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,定时器2,按键端口,电源控制端口,按键全局变量
*	3.设置中断
*	4.检查按键变量状态值,操作LED
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	TIM_TypeDef * TIMx;
	uint16_t	DelayUs;
	
	SystemInit();					//1.初始化时钟
	
	PELedPortInit();			//2.初始化LED端口,按键端口,电源控制端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitKeyCtrlStructParameter();			//2.按键数据结构
	
	//初始化定时器,启动定时器,定时器中断中操作LED
	//设置T2为时标,1uS
	TimerConfigPrescaler(TIM2, 1000000);
	TIMx = TIM2;
	DelayUs = 1000;		//决定延时中断周期1s
	
	InitNVIC();						//3.设置中断
	
	TIM_SetCounter(TIMx, DelayUs);		//设置延时并启动定时
	TIM_SetAutoreload(TIMx, DelayUs);
	StartTimer(TIMx);
	
	HeatPowerOff();												//初始化电源控制端口
	PELedOff();												//LED指示灯初始化
	ErrorLedOff();
	
	//4.中断中操作电源是否输出及操作LED
	while (1);
}
#endif	/*加热电源输出控制测试程序结束*/


#ifdef	DBG_USART
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试串口通信
*	通过串口接收中断实现.
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,按键全局变量,串口4,
*	3.设置中断
*	4.中断中接收到的内容原样发送回PC端
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();	
	InitKeyCtrlStructParameter();		//2.按键数据结构
	InitCheckInterfacePort();
	CheckInterface();				//检查通信接口类型并赋值全局变量,应该放置在变量初始化之后,端口初始化之前
	InitUsartPort();
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	//4.检查按键变量状态值,操作LED,串口中断中接收到的内容原样发送回PC端
	while (1)
	{
		LedHint();		//通过LED指示灯提示,可能是按键中断中处理,也可能是通过查询按键方式处理
	}

}
#endif	/*USART测试程序结束*/


#ifdef	DBG_ADC
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试ADC
*	通过串口发送转换结果实现.
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,按键全局变量,串口4,ADC,POWER,PE,PNE,BM,TEMP等端口,TIM2
*	3.设置中断
*	4.中断中接收到的内容原样发送回PC端,ADC转换结果转换成需要的值由中断程序发送到PC端.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint16_t	DelayUs;
	TIM_TypeDef * TIMx;
	
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();	
	InitKeyCtrlStructParameter();		//2.按键数据结构
	InitUsartPort();
	ConfigUsart();				//2.异步串口配置
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	//初始化定时器,启动定时器,定时器中断中操作LED
	//设置T5为时标,1uS
	TIMx = TIM5;
	TimerConfigPrescaler(TIMx, 1000000);
	DelayUs = 50000;		//决定延时中断周期50ms
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	TIM_SetCounter(TIMx, DelayUs);		//设置延时并启动定时
	TIM_SetAutoreload(TIMx, DelayUs);
	StartTimer(TIMx);
	
	//4.检查按键变量状态值,操作LED,串口中断中接收到的内容原样发送回PC端
	while (1)
	{
		//LedHint();		//通过LED指示灯提示,可能是按键中断中处理,也可能是通过查询按键方式处理
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			StopTimer(TIMx);
		}
	}
}
#endif	/*ADC测试程序结束*/


#ifdef	DBG_FEED
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试上纸电机控制
*	检测上纸按键,按下后即上纸若干.通过DelayUs()实现延时.
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,串口4,ADC,Feed端口
*	3.设置中断
*	4.检测上纸按键,按下后调用
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitFeedPort();
	InitPlatenPort();
	InitCheckInterfacePort();	
	InitKeyCtrlStructParameter();		//2.按键数据结构,
	InitErrorStructParameter();			//错误数据结构
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	InitUsartPort();
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	//FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	
	//4.检查按键变量状态值,操作LED,上纸,注意在一次上纸未结束之前不要再按下上纸键!
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			PELedOn();						//LED指示灯
			if ((ReadCoverStatus() == PLATEN_STATUS_CLOSE))
			{
				//FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS);			//上纸若干,此为单步恒速
				FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS * 20);	//上纸若干,此为高速
				DelayUs(1000*1000);
			}
		}
		else
		{
			PELedOff();						//LED指示灯
		}		
	}
	
}
#endif	/* 上纸FEED测试程序结束,使用延时方式上纸 */


#ifdef	DBG_CUT
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#define		CUT_NUMBER		(1)

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试切刀电机控制
*	检测上纸按键,按下后即上纸若干,然后切纸.通过DelayUs()实现延时.
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,串口4,ADC,Feed端口,切刀端口
*	3.设置中断
*	4.检测上纸按键,按下后调用
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint32_t	iCutResult;
	uint32_t	iCount;
	
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitKeyCtrlStructParameter();		//2.按键数据结构
	InitErrorStructParameter();			//错误数据结构
	InitUsartPort();
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	InitCutPaper();
	//CutPaper(0);
	
	//FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	//FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	
	//4.检查按键变量状态值,操作LED,上纸,注意在一次上纸未结束之前不要再按下上纸键!
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)	//按下按键
		{
			for (iCount = 0; iCount < CUT_NUMBER; iCount++)		//循环若干次
			{
				//if (ReadCoverStatus() == PLATEN_STATUS_CLOSE)
				{
					//FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS / 2);			//上纸若干,此为单步恒速
					//FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS * 2);	//上纸若干,此为高速
					FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS);	//上纸若干,此为高速
					iCutResult = CutPaper(1);
					FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS);	//上纸若干,此为高速
					iCutResult = CutPaper(0);
					if (iCutResult)
					{
						ErrorLedOn();						//LED指示灯告警
						//break;
					}
					else
					{
						ErrorLedOff();
					}
				}
			}
			DelayUs(1000*1000);
		}
	}
	
}
#endif	/* 切刀CUT测试程序结束,使用延时方式切纸 */


#ifdef	DBG_INBUF_RCV
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"

void	ProcessComInBuf(void)
{
	uint8_t	ReadChar;
	
	if (g_tInBuf.BytesNumber != 0)
	{
		ReadChar = ReadInBuffer();	//读取接收缓冲区
		UpLoadData(&ReadChar, 1);
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试USART接收及INBUF操作控制,INBUF内容查询方式发送到PC
*	串口中断接收内容放入INBUF,主程序将读取到的结果发送回PC端
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区,读取但是不分析指令,直接将读取结果回传PC
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint32_t	iCutResult;
	
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量	
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	ConfigUsart();				//2.异步串口配置
		
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	CutPaper(0);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	
	while (1)
	{	//进行检查按键等工作
		ProcessComInBuf();			//处理接收缓冲区		
	}
}
#endif	/* INBUF接收及发送测试程序结束 */


#ifdef	DBG_SPI_ZIKU
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试SPI控制,字库查询方式读取
*	检测字库读取按键,接收到PC发送的字符内容后,解析字符指令,进入相应程序处读取字库,
*	将读取到的结果发送回PC端
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区,分析指令
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{	
	uint32_t	iAddress;	
	uint32_t	iCount;
	uint8_t	bBuf[BYTES_OF_ASCII_FONT_A];
	
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量	
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	CheckInterface();
	
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	#if	0
	CutPaper(0);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	#endif
	//iAddress = ASCII_12_24_BASE_ADDR;
	iAddress = SYS_PARA_BASE_ADDR;
	while (1)
	{	//进行检查按键等工作
		ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
		#if	1
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			PELedOn();						//LED指示灯
			sFLASH_ReadBuffer(bBuf, iAddress, BYTES_OF_ASCII_FONT_A);
			iAddress += BYTES_OF_ASCII_FONT_A;
			#if	0
			for (iCount = 0; iCount < BYTES_OF_ASCII_FONT_A; iCount++)
			{	
				DelayUs(1000);
				UpLoadData(&bBuf[iCount], 1);
				DelayUs(1000);
			}
			#else
			UpLoadData(bBuf, BYTES_OF_ASCII_FONT_A);
			#endif
		}
		else
		{
			PELedOff();						//LED指示灯
		}
		#endif
	}
}
#endif	/* 字库查询方式读取测试程序结束 */


#ifdef	DBG_SPI_ROM
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试SPI控制字库,查询方式擦除,写入,读取
*	擦除指定地址的块,读出擦除结果并上传;写入指定内容,再读出,并将读取结果上传回PC端
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint32_t	iAddress;
	uint32_t	iCount;
	uint8_t	cGetData;
	uint8_t	bBuf[BYTES_OF_ASCII_FONT_A * 2];
	uint8_t *	cStr = "kill jiangzemin,kill hujintao,kill Japanese,kill American!";
	
	SystemInit();					//1.初始化时钟
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量	
	//InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	CheckInterface();
	
	ConfigUsart();				//2.异步串口配置
		
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	CutPaper(0);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
		
	PELedOn();						//LED指示灯
	iAddress = SYS_PARA_BASE_ADDR;
	for (iCount = 0; iCount < (strlen(cStr) + 1); iCount++)
	{
		sFLASH_ReadBuffer(&cGetData, iAddress, 1);	//读取擦除前的内容上传
		UpLoadData(&cGetData, 1);
		iAddress++;
	}
	DelayUs(1000*1000);
	PELedOff();						//LED指示灯
	
	PELedOn();						//LED指示灯
	iAddress = SYS_PARA_BASE_ADDR;
	sFLASH_EraseSector(iAddress);		//擦除最后的4K
	DelayUs(1000*1000);
	PELedOff();						//LED指示灯
	
	PELedOn();						//LED指示灯
	iAddress = SYS_PARA_BASE_ADDR;	
	sFLASH_ReadBuffer(bBuf, iAddress, (strlen(cStr) + 1));	//读取擦除后的内容上传
	UpLoadData(bBuf, (strlen(cStr) + 1));
	DelayUs(1000*1000);
	PELedOff();						//LED指示灯
	
	PELedOn();						//LED指示灯
	iAddress = SYS_PARA_BASE_ADDR;
	sFLASH_WriteBuffer(cStr, iAddress, (strlen(cStr) + 1));				//写入指定内容
	DelayUs(1000*1000);
	PELedOff();						//LED指示灯
	
	PELedOn();						//LED指示灯
	iAddress = SYS_PARA_BASE_ADDR;
	for (iCount = 0; iCount < (strlen(cStr) + 1); iCount++)
	{
		sFLASH_ReadBuffer(&cGetData, iAddress, 1);	//读取写入指定的内容上传
		UpLoadData(&cGetData, 1);
		iAddress++;
	}
	DelayUs(1000*1000);
	PELedOff();						//LED指示灯
	
	PELedOn();						//LED指示灯
	iAddress = SYS_PARA_BASE_ADDR;	
	sFLASH_ReadBuffer(bBuf, iAddress, (strlen(cStr) + 1));	//读取写入指定的内容上传
	UpLoadData(bBuf, (strlen(cStr) + 1));
	DelayUs(1000*1000);
	PELedOff();						//LED指示灯
}
#endif	/* 字库查询方式擦除,写入,读出测试程序结束 */


#ifdef	DBG_SPI_HEAT
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"

extern	uint8_t	g_tbHeatElementBuf[BYTES_OF_ALL_ELEMENT];	//加热机芯一点行所有的加热寄存器单元数

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试机芯SPI使用DMA方式向机芯发送一点行打印数据的程序
*	设定好机芯SPI总线,机芯SPI的DMA,准备好一点行数据,按下按键后,点亮LED,
*	同时启动DMA发送.在DMA发送结束的中断中关闭LED,并通过串口向上位机发送一点行打印数据
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,机芯SPI设置,机芯SPI_DMA设置
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区,准备一点行数据,检测按键并启动DMA发送
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.初始化时钟	
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	InitHeatElementBuf();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	//InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	sHeat_DMAInit();				//设置机芯SPI传送数据MDA
	sHeat_Init();						//初始化机芯SPI
	
	ConfigUsart();				//2.异步串口配置
		
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	
	PELedOff();						//LED指示灯
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			PELedOn();						//LED指示灯
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);
			DelayUs(1000*1000);
		}
	}
}
#endif	/* 机芯SPI使用DMA发送一点行打印数据测试程序结束 */

#ifdef	DBG_HEAT
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试打印
*	
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint16_t	HeatUs;
	uint8_t	cTemp[sizeof(uint16_t)];
	static uint8_t m_Count = 0;
	
	SystemInit();					//1.初始化时钟	
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	sHeat_Init();					//初始化机芯SPI
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	ConfigUsart();				//2.异步串口配置
		
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	#if	0
	DriveCutPaper(CUT_TYPE_DEFAULT);	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	#endif
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			HexMode();
			break;
		default:
			break;
	}
	
	DelayUs(1000*1000);		//等待ADC采样稳定
	while (1)
	{
		//ListenEthPort();						//侦听网络服务器端口
		//进行检查按键等工作
		//ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			m_Count++;
			if ((m_Count % 2) == 0)
			{
				PELedOn();						//LED指示灯
				HeatUs = GetHeatTime();
				cTemp[0] = (HeatUs & 0xFF00) >> 8;
				cTemp[1] = (uint8_t)(HeatUs & 0x00FF);
				UpLoadData(cTemp, sizeof(uint16_t));			//上传数据
				
				//StartHeatAll();
				StartHeat(HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN);
				DelayUs(1000*1000);
			}
			else
			{
				PELedOff();						//LED指示灯
				//StopHeatAll();
				EndHeat();
				DelayUs(1000*1000);
			}
		}
	}
}
#endif	/* 加热测试程序结束 */


#ifdef	DBG_SPI_HEAT_DATA_SET
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试打印
*	
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint32_t	iCount;
	uint32_t	i;	
	uint8_t	cData;
	uint16_t	cTemp;
	
	SystemInit();					//1.初始化时钟
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	sHeat_Init();					//初始化机芯SPI
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	#if	0
	DriveCutPaper(CUT_TYPE_DEFAULT);	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	#endif
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			HexMode();
			break;
		default:
			break;
	}
	
	//GoDotLine(2400);
	iCount = 0;
	cData = 0;
	while (1)
	{
		//ListenEthPort();						//侦听网络服务器端口
		//进行检查按键等工作
		ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
		#if	0
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			for (i = 0; i < BYTES_OF_ALL_ELEMENT; i++)
			{
				g_tbHeatElementBuf[i] = cData;
			}
			cData++;
			
			for (i = 0; i < BYTES_OF_ALL_ELEMENT; i++)
			{
				cTemp = SPI_HeatSendByte(g_tbHeatElementBuf[i]);
			}
			while ((SPI_HEAT_SPI->SR & SPI_I2S_FLAG_TXE) == 0);		//确保发送完毕
			
			DelayUs(1000*1000);
		}
		#endif
		#if	1
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			iCount++;
			if (iCount % 2)
			{
				PELedOn();						//LED指示灯
				StartHeat(m_tHeatCtrlPinTable[0]);
				//StartHeatPartOne();
				DelayUs(1000*1000);
				
				StopHeatPartOne();
				PELedOff();						//LED指示灯
				DelayUs(1000*1000);
			}
			else
			{
				PELedOn();						//LED指示灯
				StartHeat(m_tHeatCtrlPinTable[1]);
				//StartHeatPartTwo();
				DelayUs(1000*1000);
				
				StopHeatPartTwo();
				PELedOff();						//LED指示灯
				DelayUs(1000*1000);
			}
		}
		else
		{
			PELedOff();						//LED指示灯
		}
		#endif
	}
}
#endif	/* 机芯SPI3通信测试程序结束 */


#ifdef	DBG_LATCH
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试打印控制管脚
*	
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint32_t	iCount;
	uint32_t	i;	
	uint8_t	cData;
	uint16_t	cTemp;
	
	SystemInit();					//1.初始化时钟	
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	sHeat_Init();					//初始化机芯SPI
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	#if	0
	DriveCutPaper(CUT_TYPE_DEFAULT);	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	#endif
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			HexMode();
			break;
		default:
			break;
	}
	
	iCount = 0;	
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			iCount++;
			if (iCount % 2)
			{
				PELedOn();						//LED指示灯
				GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
				DelayUs(1000*1000);
			}
			else
			{
				PELedOff();						//LED指示灯
				GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
				DelayUs(1000*1000);
			}
		}
	}
}
#endif	/* 打印控制管脚测试程序结束 */


#ifdef	DBG_PRT
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试打印
*	
*	需要的操作步骤:
*	1.初始化时钟
*	2.初始化LED端口,按键端口,上纸控制端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
	uint32_t	iCount;
	uint32_t	i;
	uint8_t	cData;
	uint16_t	cTemp;
	
	SystemInit();					//1.初始化时钟
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_Init();					//初始化机芯SPI
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	ConfigUsart();				//2.异步串口配置
	
	#ifdef	DBG_USB
	if (g_tInterface.USB_Type)	//初始化USB接口
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();
		//USB_Disconnect_Config();	//与SetUSBPort()工作相同
	}
	#endif
	
	InitNVIC();						//3.设置中断
	ClearUsartBusy();			//初始化时设置为忙
	
	#if defined DBG_DEAL_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 10000);	//设置为预分频100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 100000);	//设置为预分频10us
	#elif	defined	DBG_PRT_SPEED
	TimerConfigPrescaler(DBG_TIMER, 1000);	//设置预分频为1ms
	#endif
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//收完再打
	g_bRxFinishFlag = RX_FINISHED;		//通信过程已经完成
	#endif
	
	#if	0
	//DriveCutPaper(CUT_TYPE_DEFAULT);
	//FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	//FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	#endif
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			HexMode();
			break;
		default:
			break;
	}
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//GoDotLine(24);
		//FeedPaper(FEED_FORWARD, 24);	//上纸若干,此为单步恒速
		//DelayUs(1000*1000);
		
		//ListenEthPort();						//侦听网络服务器端口
		//进行检查按键等工作
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
		#ifdef	DBG_MOTOR_HOLD			
			if ((iCount % 3) == 0)
			{
				StartFeedMotor(FEED_FORWARD);	//重复输出之前的相位,但是不改变相位变量
				PELedOn();						//LED指示灯
			}
			else if ((iCount % 3) == 1)
			{
				StopFeedMotor(FEED_FORWARD);
				PELedOn();						//LED指示灯
			}
			else if ((iCount % 3) == 2)
			{
				EndFeedMotor();				//关闭电机
				PELedOff();						//LED指示灯
			}
			
			iCount++;
			DelayUs(1000*1000);
		#else
			GoDotLine(FEED_STEPS_AFTER_CLOSE_SHELL);
		#endif
		}
		
		ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
		
		#if	0
		//前点行加热走纸完成
		if ((g_tInBuf.BytesNumber == 0) && 
				(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && 
				(g_tSystemCtrl.StepNumber == 0))
		{
			EndHeat();										//关加热控制
			EndFeedMotor();								//关闭电机
			g_StepCount = 0;
			g_bMotorRunFlag = 0;
		}
		#endif
	}
}
#endif	/* 打印测试程序结束 */


#ifdef	DBG_USB
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"
#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"usb_prop.h"
#include	"selftest.h"
#include	"hexmode.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试USB
*	
*	需要的操作步骤:
*	1.初始化时钟,包括USB时钟
*	2.初始化LED端口,按键端口,上纸控制端口,USB端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
#ifdef	DBG_MOTOR_HOLD
	uint32_t	iCount;
#endif
		
	SystemInit();					//1.初始化时钟
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_Init();					//初始化机芯SPI
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	ConfigUsart();				//2.异步串口配置
	
	InitNVIC();						//3.设置中断
	
	#if defined DBG_DEAL_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 10000);	//设置为预分频100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 100000);	//设置为预分频10us
	#elif	defined	DBG_PRT_SPEED
	TimerConfigPrescaler(DBG_TIMER, 1000);	//设置预分频为1ms
	#endif
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_USB
	if (g_tInterface.USB_Type)	//初始化USB接口
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();
		//USB_Disconnect_Config();	//与SetUSBPort()工作相同
	}	
	#endif
	
	ClearComInBufBusy();
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//收完再打
	g_bRxFinishFlag = RX_FINISHED;		//通信过程已经完成
	#endif
	
	#if	0
	//DriveCutPaper(CUT_TYPE_DEFAULT);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	#endif
	
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			DelayUs(1000*500);
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			DelayUs(1000*500);
			HexMode();
			break;
		default:
			break;
	}
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//侦听网络服务器端口
		CheckFeedButton();								//进行检查按键等工作
		ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
		
		#if	0
		//前点行加热走纸完成
		if ((g_tInBuf.BytesNumber == 0) && 
				(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && 
				(g_tSystemCtrl.StepNumber == 0))
		{
			EndHeat();										//关加热控制
			EndFeedMotor();								//关闭电机
			g_StepCount = 0;
			g_bMotorRunFlag = 0;
		}
		#endif
	}
}
#endif	/* USB测试程序结束 */


#ifdef	DBG_ETH_USART
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"
#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"usb_prop.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试USB
*	
*	需要的操作步骤:
*	1.初始化时钟,包括USB时钟
*	2.初始化LED端口,按键端口,上纸控制端口,USB端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void main(void)
{
#ifdef	DBG_MOTOR_HOLD
	uint32_t	iCount;
#endif
		
	SystemInit();					//1.初始化时钟
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();
	InitUsartPort();	
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_Init();					//初始化机芯SPI
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	ConfigUsart();				//2.异步串口配置
	if (g_tInterface.USB_Type)	//初始化USB接口
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();		
	}
	
	ClearComInBufBusy();
	
	InitNVIC();						//3.设置中断
	
#ifdef WITHBOOT
//	CheckBootLegality();	//检验BOOTLOAD程序合法性
#endif
	
	#if defined DBG_DEAL_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 10000);	//设置为预分频100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 100000);	//设置为预分频10us
	#elif	defined	DBG_PRT_SPEED
	TimerConfigPrescaler(DBG_TIMER, 1000);	//设置预分频为1ms
	#endif
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//收完再打
	g_bRxFinishFlag = RX_FINISHED;		//通信过程已经完成
	#endif
		
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			DelayUs(1000*500);
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			DelayUs(1000*500);
			HexMode();
			break;
		default:
			break;
	}
	
	#if	0
	//DriveCutPaper(CUT_TYPE_DEFAULT);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	#endif
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//侦听网络服务器端口
		//CheckFeedButton();								//进行检查按键等工作
		//ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
		SetComInBufBusy();
	}
}
#endif	/* ETH_USART测试程序结束 */


#ifdef	DBG_UPDATE
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"
#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"usb_prop.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试USB
*	
*	需要的操作步骤:
*	1.初始化时钟,包括USB时钟
*	2.初始化LED端口,按键端口,上纸控制端口,USB端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
int main(void)
{
#ifdef	DBG_MOTOR_HOLD
	uint32_t	iCount;
#endif
	
	SystemInit();					//1.初始化时钟
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG功能
	
	PELedPortInit();			//2.初始化LED端口,按键端口,上纸控制端口,CUT端口
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitHeatPort();
	
#ifdef WITHBOOT
//	CheckBootLegality();	//检验BOOTLOAD程序合法性
#endif
	
	InitADCPort();
	ADC_DMA_Config();				//DMA设置在前,ADC设置在后
	ADC_Config();						//AD配置
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//系统参数如果存储到外部字库,则必须先初始化,再初始化变量
	InitParameter(0);			//调用系统默认值初始化全局结构变量,此时会读写外部SPI字库FLASH
	
	sHeat_Init();					//初始化机芯SPI
	sHeat_DMAInit();			//设置机芯SPI传送数据MDA
	
	CheckInterface();			//应该放置在变量初始化之后,端口初始化之前
	
	if (g_tInterface.COM_Type)	//初始化串口接口
	{
		InitUsartPort();
		ConfigUsart();				//2.异步串口配置
	}
	
	if (g_tInterface.USB_Type)	//初始化USB接口
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();		
	}
	
	InitNVIC();						//3.设置中断
	
	#if defined DBG_DEAL_ONE_LINE_TIME
		TimerConfigPrescaler(DBG_TIMER, 10000);	//设置为预分频100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
		TimerConfigPrescaler(DBG_TIMER, 100000);	//设置为预分频10us
	#elif	defined	DBG_PRT_SPEED
		TimerConfigPrescaler(DBG_TIMER, 1000);	//设置预分频为1ms
	#elif	defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
		TimerConfigPrescaler(DBG_TIMER, 100000);	//设置为预分频10us
	#endif
	
	//设置T2为启动AD定时器,延时100uS,周期性启动ADC转换,一直运行,进入休眠前关闭,退出休眠前开启
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//设置T3为加热定时器,预分频为1000000,1uS,处理走上纸电机和加热,需要加热或者
	//中断中走上纸电机时由程序启动,在中断中检查是否处理完毕,如果处理完毕自己关闭
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//设置T4为系统时标,1uS,处理行打印缓冲区的打印,一直运行,周期性检查两个行打印缓冲区
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//设置T5为备用定时器,预分频为1000000,1uS,备用
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//收完再打
		TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//设置1uS,通信定时
		g_bRxFinishFlag = RX_FINISHED;		//通信过程已经完成
	#endif
	
	switch (GetModeType())				//检测上电后的工作模式
	{
		case	MODE_NORMAL:					//正常模式
			break;
		case	MODE_SELFTEST_PRT:				//自检模式
			DelayUs(1000*500);
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//复位以太网模式
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16进制模式
			DelayUs(1000*500);
			ClearComInBufBusy();
			HexMode();
			break;
		case	MODE_NORMAL_UPDATE:
			ClearComInBufBusy();
			Update();
			break;
		case	MODE_RELEASE_JTAG:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);	//SWJ,JTAG全部可用
			break;
		default:
			break;
	}
	
	ClearComInBufBusy();
	
	#if	1
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//上纸若干,此为单步恒速
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//上纸
	DriveCutPaper(CUT_TYPE_DEFAULT);
	#endif
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//侦听网络服务器端口
		CheckFeedButton();								//进行检查按键等工作
		ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
	}
}
#endif	/* DBG_UPDATE测试程序结束 */



#ifdef	DBG_EXT_SRAM

#include	"init.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	void	ProcessModeType(void);

extern	void LongToStr(uint32_t num, uint8_t *Str);

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试外部SRAM
*	
*	需要的操作步骤:
*	1.初始化时钟,包括USB时钟
*	2.初始化LED端口,按键端口,上纸控制端口,USB端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
int main(void)
{
	#ifdef	EXT_SRAM_ENABLE
		uint8_t		bBuf[16];
		uint32_t	Result;
		uint32_t	iLen;
	#endif
	
	Init();		//初始化时钟,GPIO端口,全局变量,外部设备
	
#ifdef WITHBOOT
//	CheckBootLegality();	//检验BOOTLOAD程序合法性
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT		//收完再打
	TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//设置1uS,通信定时
	g_bRxFinishFlag = RX_FINISHED;		//通信过程已经完成
#endif
	
	ProcessModeType();
	ClearComInBufBusy();
	
	#ifdef	EXT_SRAM_ENABLE
		while (1)
		{
			if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
			{
				DelayMs(100);
				
				Result = TestReadWriteExtSRAM();
				if (Result == 0)
				{
					UsartSendStr(USART1, "Read and Write extern SRAM is OK!\n");
				}
				else if (Result == 1)
				{
					UsartSendStr(USART1, "Read and Write extern SRAM is Error!\n");
				}
				else
				{
					UsartSendStr(USART1, "Read and Write extern SRAM is Unknow Error!\n");
				}
				
				if (Result == 0)
				{
					iLen = GetExtSRAMLen();
					LongToStr(iLen, bBuf);
					UsartSendStr(USART1, "The extern SRAM length is bytes of ");
					UsartSendStr(USART1, bBuf);
					UsartSendStr(USART1, "\n");
				}
				
				DelayMs(100);
			}
		}
	#endif
	
	while (1)
	{
		CheckFeedButton();				//进行检查按键等工作
		#ifdef	MAINTAIN_ENABLE
			CheckMaintainRoutin();	//检查周期性维护参数存储
		#endif
		ProcessCharMaskReadBuf();	//处理字符点阵读取缓冲区
	}
}

#endif	/* 外部SRAM测试程序结束 */

#ifdef	DBG_CPCL_TEXT

#include	"init.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	void	ProcessModeType(void);

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试正式发布版本
*	
*	需要的操作步骤:
*	1.初始化时钟,包括USB时钟
*	2.初始化LED端口,按键端口,上纸控制端口,USB端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
int main(void)
{
	Init();		//初始化时钟,GPIO端口,全局变量,外部设备
	
#ifdef BOOTLOAD
//	ConfigOptionFlash();	//检验BOOTLOAD程序合法性
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT		//收完再打
	TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//设置1uS,通信定时
	g_bRxFinishFlag = RX_FINISHED;		//通信过程已经完成
#endif
	
	ProcessModeType();
	ClearComInBufBusy();
	EnableReceiveInterrupt();
	
	while (1)
	{
		if (CMD_SYS_ESCPOS == GetCommandSystem())		//处理ESC指令集
		{
			CheckFeedButton();				//进行检查按键等工作
			#ifdef	MAINTAIN_ENABLE
				CheckMaintainRoutin();	//检查周期性维护参数存储
			#endif
			ProcessCharMaskReadBuf();	//处理字符点阵读取缓冲区
		}
		#ifdef	CPCL_CMD_ENABLE
			else if (CMD_SYS_CPCL == GetCommandSystem())		//处理CPCL指令集
			{
				//进行检查按键等工作
				CPCL_Command();		//处理CPCL指令
			}
		#endif
		else	//处理其他指令集
		{
		}
	}	//主循环结束
	
}

#endif	/* DBG_CPCL_TEXT 测试程序结束 */

#ifdef	DBG_ESC_RELEASE

#include	"init.h"
#include "speaker.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	void	ProcessModeType(void);
extern uint8_t Eth_ClearBusy;

/*******************************************************************************
* Function Name  : 函数名
* Description    : 测试正式发布版本
*	
*	需要的操作步骤:
*	1.初始化时钟,包括USB时钟
*	2.初始化LED端口,按键端口,上纸控制端口,USB端口,按键全局变量,控制环境全局变量,
*		串口4,ADC,Feed端口,切刀端口,字库SPI端口,字库SPI设置,加热端口,加热全局变量,
*	3.设置中断
*	4.串口中断接收到内容放到接收缓冲区
*	5.操作
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
int main(void)
{
	Sys_Init();								//初始化时钟,GPIO端口,全局变量,外部设备
	
#ifdef WITHBOOT
	ConfigOptionByte();   //2017.10.18  芯片读写保护
 	CheckBootLegality();	//检验BOOTLOAD程序合法性
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT										//收完再打
	TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//设置1uS,通信定时
	g_bRxFinishFlag = RX_FINISHED;								//通信过程已经完成
#endif
	
	ProcessModeType();
	Eth_ClearBusy =1;	
	
	ClearComInBufBusy();
	Eth_ClearBusy =0;		
	while (1)
	{
		CheckFeedButton();				//进行检查按键等工作
		#ifdef	MAINTAIN_ENABLE
// 	CheckMaintainRoutin();	  //检查周期性维护参数存储
		#endif
		ProcessCharMaskReadBuf();	//处理字符点阵读取缓冲区
	}
}

#endif	/* DBG_ESC_RELEASE 测试程序结束 */



#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
