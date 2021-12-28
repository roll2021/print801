/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   上纸按键以及以太网复位按键检测相关的程序.
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
#include	"button.h"
#include	"timconfig.h"
#include	"feedpaper.h"
#include	"esc.h"
#include	"extgvar.h"

extern uint32_t	GetPrintBusyStatus(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化检测上纸按键的端口设置,设置为下拉输入
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitFeedButtonPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#ifdef	FEED_BUTTON_CHECK_BY_INT
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(FEED_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= FEED_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;				//2016.06.09   
	GPIO_Init(FEED_BUTTON_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(FEED_BUTTON_GPIO_PORT_SOURCE, FEED_BUTTON_GPIO_PIN_SOURCE);	//中断
  EXTI_InitStructure.EXTI_Line = FEED_BUTTON_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//外部中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//下降沿有效
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#else
	RCC_APB2PeriphClockCmd(FEED_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= FEED_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(FEED_BUTTON_GPIO_PORT, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测上纸按键状态
* Input          : 无
* Output         : 输出参数,开或者关
* Return         : 返回参数,0/1
*******************************************************************************/
uint8_t	ReadFeedButtonStatus(void)
{	//读入管脚值
	if (FEED_BUTTON_VALUE_CLOSE == GPIO_ReadInputDataBit(FEED_BUTTON_GPIO_PORT, FEED_BUTTON_GPIO_PIN))
	{
		return	(FEED_BUTTON_STATUS_CLOSE);
	}
	else
	{
		return	(FEED_BUTTON_STATUS_OPEN);
	}
}

/*******************************************************************************
* Function Name  : CheckFeedButton
* Description    : Feed键的处理,允许按键操作时,连续纸模式按住走纸,松开停止.
*				   黑标纸模式时,走过黑标处,如果在300mm内没有找到黑标,则停止
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	CheckFeedButton(void)
{
	uint32_t Temp;
	uint32_t Feednum;
  if(g_tSysConfig.BlackMarkEnable)	
	{	
		if((FeedToGapFlag)&&((g_tError.PaperSensorStatusFlag & 0x01) == 0) && ((g_tError.AR_ErrorFlag & 0x02) == 0)
							&& (g_tError.DoorOpen |g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag)==0  )    //  2016.01 有纸并且上纸标志有置位
		{
				DelayUs(400);
				FindMarkPoint();                //用于合盖后找黑标 2016.07.02
				FeedToGapFlag = 0;
		}
  }
	
	if (g_tFeedButton.KeyEnable != 0)	//不允许按键
	{
		return;
	}	//不允许按键时检查按键处理结束
	
	if (g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE)	// 驱动打印 不判断按键 2016.06.16
	{
		return;
	}	
	
	if (g_tFeedButton.KeyStatus == 0)	//没有按键按下
	{
		return;
	}	//没有按键按下处理结束
	
	//以下为允许按键且有按键按下处理
	if (g_tFeedButton.KeyCount)	//按键次数不为0
	{
		if (g_tSystemCtrl.SelfTestMode)	//自检模式时不上纸
		{
			g_tFeedButton.KeyCount--;
			if (g_tFeedButton.KeyCount == 0)
			{
				g_tFeedButton.KeyStatus = 0;
			}
			return;
		}	//自检模式处理结束
		else if (g_tSystemCtrl.HexMode)	//16进制时不做处理
		{
			return;
		}
		else	//正常模式处理开始
		{
			if (g_tMacro.RunFlag == 0)	//不是正在处理宏定义
			{
				//检查是否可以走纸, 不可以则退出不走				
				if (PRINTER_STATUS_BUSY == GetPrintBusyStatus())
				{
					return;
				}
				
				//如果可以走纸
				g_tRealTimeStatus.PrinterStatus |= 0x08;	//先进入离线状态
				if (PAPER_TYPE_CONTINUOUS == g_tSystemCtrl.PaperType)	//连续纸
				{
					if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//按键还按下
					{	//松开按键才停止走纸
						Feednum=0;
						while (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())
						{
							if(Feednum<10)
							{	
								g_tFeedButton.KeyDownFlag = 1;			//2016.06.28 按键按下标志 置位
								g_tFeedButton.StepCount = 200;							
								DelayMs(50);
								Feednum++;	
							}		
						}
					
						g_tFeedButton.KeyStatus = 0;	//按键无效
						g_tFeedButton.KeyCount = 0;		//清空之前所有的计数
					}
					else	//按键已经松开,只按照已保存的按键次数上纸
					{
						while (g_tFeedButton.KeyCount != 0)
						{
// 							GoDotLine(DEFAULT_CODE_LINE_HEIGHT);	//走纸1行
// 							g_tFeedButton.KeyCount--;
							g_tFeedButton.KeyCount = 0;   //2020.09.28  松开按键后清零 不再走纸
						}
						g_tFeedButton.KeyStatus = 0;	//按键无效
					}
				}			//连续纸处理结束
				else	//黑标纸处理开始
				{
					FindMarkPoint();
					g_tFeedButton.KeyStatus = 0;	//按键无效
					g_tFeedButton.KeyCount = 0;		//清空之前所有的计数
				}
				
				g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//上纸完毕,自动脱离离线状态
			}	//不是正在执行宏处理结束
		}	//正常模式处理结束
	}	//按键次数不为零处理结束
	else
	{
		g_tFeedButton.KeyStatus = 0;	//无按键按下
	}	//按键次数为零处理结束
	
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化检测以太网复位按键的端口设置,设置为下拉输入
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitResetEthernetButtonPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#ifdef	RESET_ETHERNET_BUTTON_CHECK_BY_INT
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RESET_ETHERNET_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= RESET_ETHERNET_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(RESET_ETHERNET_BUTTON_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(RESET_ETHERNET_BUTTON_GPIO_PORT_SOURCE, RESET_ETHERNET_BUTTON_GPIO_PIN_SOURCE);	//中断
  EXTI_InitStructure.EXTI_Line = RESET_ETHERNET_BUTTON_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//外部中断	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//上升沿有效
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#else
	RCC_APB2PeriphClockCmd(RESET_ETHERNET_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= RESET_ETHERNET_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(RESET_ETHERNET_BUTTON_GPIO_PORT, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化打印机控制复位以太网管脚
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitPrtCtrlResetEthernetPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(PRT_CTRL_RESET_ETH_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PRT_CTRL_RESET_ETH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(PRT_CTRL_RESET_ETH_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测以太网复位按键状态
* Input          : 无
* Output         : 输出参数,开或者关
* Return         : 返回参数,0/1
*******************************************************************************/
uint8_t	ReadResetEthernetButtonStatus(void)
{	//读入管脚值
	if (RESET_ETHERNET_BUTTON_VALUE_CLOSE == 
			GPIO_ReadInputDataBit(RESET_ETHERNET_BUTTON_GPIO_PORT, RESET_ETHERNET_BUTTON_GPIO_PIN))
	{
		return	(RESET_ETHERNET_BUTTON_STATUS_CLOSE);
	}
	else
	{
		return	(RESET_ETHERNET_BUTTON_STATUS_OPEN);
	}
}

void	ResetEthernet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(PRT_CTRL_RESET_ETH_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= PRT_CTRL_RESET_ETH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(PRT_CTRL_RESET_ETH_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(PRT_CTRL_RESET_ETH_GPIO_PORT, PRT_CTRL_RESET_ETH_GPIO_PIN);
	DelayMs(RESET_ETHERNET_MS);
	GPIO_SetBits(PRT_CTRL_RESET_ETH_GPIO_PORT, PRT_CTRL_RESET_ETH_GPIO_PIN);
	
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_Init(PRT_CTRL_RESET_ETH_GPIO_PORT, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
