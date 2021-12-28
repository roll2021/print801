/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   切刀及切刀电机相关的程序.
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
#include	"cutter.h"
#include	"feedpaper.h"
#include	"led.h"
extern void	DelayUs(uint32_t	delaytime);
extern void	DelayMs(uint32_t	DelayNumber);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	FORWARD_DETECT_STEP1_MAX_MS		(500)		//正转时第一阶段检测的最长时间
#define	FORWARD_DETECT_STEP2_MAX_MS		(1000)	//正转时第二阶段检测的最长时间
#define	REVERSE_DETECT_MAX_MS					(1000)	//反转时检测的最长时间
#define	BRAKE_TIME_MS									(100)		//刹车时间

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化切刀电机控制管脚
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitCutPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(CUT_HOME_IN_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= CUT_HOME_IN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;	//输入浮空
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
	GPIO_Init(CUT_HOME_IN_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(CUT_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= CUT_ENABLE_GPIO_PIN | \
																		CUT_PHASE_GPIO_PIN | \
																		CUT_MODE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;		//均为输出
	GPIO_Init(CUT_CTRL_GPIO_PORT, &GPIO_InitStructure);

	CutMotorDisable();	//芯片禁用
	CutMotorSetMode();	//刹车状态
}

/*******************************************************************************
* Function Name  : void InitCutPaper
* Description    : 初始化切刀，如果切刀不在原位，则返回原位。如果超时认为没有切刀.
*				    切刀在原位时，开关关闭状态，状态输入为低
* Input          : None
* Output         : None
* Return         : 
*注意：不同的切刀，其时序，各等待时间可能不同，需要按其调整
*******************************************************************************/
void	InitCutPaper(void)
{
		uint32_t i;
	
		if (g_tSysConfig.CutType == 0)	    //无切刀 2017.03.21
		{
			return;
		}
	
		if(GetCutSwitchStatus())					//如果为“1”，不在原位，则返回原位，如超时，则认为无切刀
		{
				CutMotorReverse();					//反转
				for(i=0; i<CUT_STATUS_OFF_TIME; i++)	//等待返回原位
				{
						DelayMs(1);
						if(!GetCutSwitchStatus())		//如果回到原位，则跳出
						{
								DelayMs(1);		//去抖
								i++;
								if(!GetCutSwitchStatus())	//如果回到原位，则跳出
									break;
						}
				}

				StopCut();						//停止
				if(i>= CUT_STATUS_OFF_TIME)		//在300ms内没有回原位
				{
						CutMotorForward();					//正转
						for(i=0; i<CUT_STATUS_OFF_TIME; i++)//等待300ms
						{
								DelayMs(1);
								if(!GetCutSwitchStatus())	//如果回到原位，则跳出
								{
										DelayMs(1);	//去抖
										i++;
										if(!GetCutSwitchStatus())//如果回到原位，则跳出
											break;
								}
						}
			
						StopCut();					//停止
						if(i>=CUT_STATUS_OFF_TIME)	//在300ms内没有回原位
						{
								CutError();				//切刀错误
						}
				}
		}
}

/******************************************************************************
* Name        : CutPaper
* Author      : 
* Version     : V0.01
* Date        : 2016-3月-20日
* Description : 切纸
* Others      : 注意：不同型号切刀具体时序不同,本程序只针对JX-3R-21型号.
*                     切纸效果是全切还是半切由具体硬件实现.
* 返回值      : 成功返回CUTTERSUCESS,失败返回CUTTERERROR
* History     : 
******************************************************************************/
uint32_t CutPaper(uint8_t CutMode)
{
	//uint8_t	status;					//切刀位置信号结果
	uint8_t	CutStatus;			//切纸结果
	uint32_t	i;
	
	#if defined	DBG_DUMMY_CUT
		return	(CUT_PAPER_SUCCESS);
	#endif
	
	if (!((CutMode == CUT_TYPE_PART) || (CutMode == CUT_TYPE_FULL)))
	{
		return	(CUT_PAPER_SUCCESS);
	}
	
	CutStatus = CUT_INIT_ERROR1;
	while (CutStatus)
	{
// 		if (!GetCutSwitchStatus())
// 		{
			CutStatus = CUT_INIT_SUCCESS;
			if (CutMode == 0)					//全切
			{
				CutMotorForward();			//正转
			}
			else
			{
				CutMotorForward();			//正转
			}
			//2017.04.22  开始出刀的前 70 ms不测试切刀状态，防止微动开关抖动造成的影响
			for (i = 0; i < 70; i++)	
			{
				DelayMs(1);
			}
			
			//等待切刀状态开关打开，切刀离开原位
			for (; i < CUT_STATUS_ON_TIME; i++)	
			{
				DelayMs(1);
				if (GetCutSwitchStatus())		//离开原位，则跳出
				{
					DelayMs(1);				//去抖
					i++;
					if(GetCutSwitchStatus())		//离开原位，则跳出
					{
						break;
					}
				}
			}
			//如果没有超时，则继续
			if (i < CUT_STATUS_ON_TIME)
			{
				//等待切刀状态开关关闭，切刀返回原位
				for (i = 0; i < CUT_STATUS_OFF_TIME; i++)
				{
					DelayMs(1);
					if (!GetCutSwitchStatus())
					{
						DelayMs(1);			//去抖
						i++;
						if (!GetCutSwitchStatus())
						{
							break;
						}
					}
				}
				StopCut();
				
				//如果超时，切刀错误，进行切刀退回操作
				if (i >= CUT_STATUS_OFF_TIME)			
				{
					if (CutMode == 0)			//全切
					{
						CutMotorReverse();	//反转
					}
					else
					{
						CutMotorReverse();	//反转  2017.03.21
						//CutMotorForward();	//正转
					}
					
					//等待切刀状态开关关闭，切刀返回原位
					for (i = 0; i < CUT_STATUS_ON_TIME; i++)	
					{
						DelayMs(1);
						if (!GetCutSwitchStatus())
						{
							DelayMs(1);		//去抖
							i++;
							if (!GetCutSwitchStatus())
							{
								break;
							}
						}
					}
					
					CutStatus = CUT_INIT_ERROR2;	//错误状态2，中间错误
				}
			}
			else
			{
				CutStatus = CUT_INIT_ERROR1;		//错误状态1，开始错误
			}
			
			if (g_tSystemCtrl.CutRunFlag)
			{
				g_tSystemCtrl.CutRunFlag = 0;
				DriveBuzzer(DISABLE);			//关闭蜂鸣器
			}
			
			if (CutStatus != CUT_PAPER_SUCCESS)     //2016.11.24
			{
				CutError();					//切刀错误处理
				if(g_tError.R_ErrorRecoversFlag !=0)
				{
					//切刀错误恢复方式，100502时，不再执行切纸动作
					if(g_tError.R_ErrorRecoversFlag ==2)
						CutStatus =CUT_INIT_SUCCESS;					
					g_tError.R_ErrorRecoversFlag =0;
				}
			}
			else
			{
				g_tSystemCtrl.CutLedFlag = 1;       //切刀运行标志 用于指示T2指示未取单中声光报警
				g_tSystemCtrl.BillPrtFlag |= 8;     //切刀完成，表示当前票据打印完成

				//2017.08.14  切刀结束后 音律报警
				VoiceOverflag = 0;
				
				#ifdef	MAINTAIN_ENABLE
					 R_Counter.CutCounter++;					//切刀计数器
					 C_Counter.CutCounter++;
				#endif
			}	
	
// 		}
// 		else
// 		{
// 			InitCutPaper();						//初始化切刀
// 		}
	}
	
	return	(CutStatus);
}

/*******************************************************************************
* Function Name  : void DriveCutPaper
* Description    : 驱动切刀 
* Input          : CutMode:切纸模式，＝0 全切，＝1 半切
* Output         : None
* Return         : 
*******************************************************************************/
void	DriveCutPaper(uint8_t CutMode)
{
	uint16_t CutStatus;
	uint16_t i;
	
 	if (g_tSysConfig.CutType == 0)	    //无切刀
	{
// 		GoDotLine(NO_CUT_OFFSET);		     //走到切纸位置 2017.03.21
		return;
	}
	
	CutStatus = CUT_PAPER_SUCCESS;
	
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);			 //等待电机停止,没有按键按下，行打印缓冲区没有数据
	
	if (g_tSysConfig.BuzzerEnable & 0x02)
	{
		g_tSystemCtrl.CutRunFlag = 1;
		if(g_tSpeaker.SpeakCtrl == 0)
			DriveBuzzer(ENABLE);			        //开启蜂鸣器
	}
	
#ifdef	CUTTER_ENABLE	
	CutStatus = CutPaper(CutMode);
#endif	

}

/*******************************************************************************
* Function Name  : void CutError
* Description    : 切刀错误,等待切刀返回原位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	CutError(void)
{
	StopCut();
	g_tError.R_ErrorFlag |= 0x01;		//发生了切刀位置错误
	
	//上盖打开后再关闭或1005n指令恢复切刀错误
	while((g_tError.R_ErrorFlag &0x01))
	{
		while( g_tError.DoorOpen ==0)
		{
			if((g_tError.R_ErrorFlag &0x01) == 0x00)			//1005指令恢复切刀错误
			{	
				break;
			}	
		}
		while( (g_tError.R_ErrorFlag &0x01))				  	//开盖在合盖后取消错误
		{
			if( g_tError.DoorOpen ==0)
			{	
				g_tError.R_ErrorFlag &= ~0x01;	
			}	
		}
	}	
}

/******************************************************************************
* Name        : CutMotorx
* Author      : 
* Version     : V0.01
* Date        : 2012-5月-17日
* Description : 控制切刀电机的四种动作,前转,反转,刹车,停止。
* Others      : 注意：  这四个程序只是改变电机的行为,不管时间等控制,
*                       实际使用时一定要注意相应动作的时间控制以保护电机
*                       这四个函数只供给InitCut()和CutPaper()调用
* History     : 
******************************************************************************/
static void CutMotorForward(void)		//电机正转
{
	//CutMotorSleepOff();		//禁止休眠
	CutMotorSetPhase();		//设置旋转方向
	CutMotorEnable();			//允许输出
}

static void CutMotorReverse(void)		//电机反转
{
	//CutMotorSleepOff();		//禁止休眠
	CutMotorResetPhase();	//设置旋转方向
	CutMotorEnable();			//允许输出
}

static void CutMotorBrake(void)			//电机刹车,慢衰减
{
	//CutMotorSleepOff();		//禁止休?
	//CutMotorSetMode1();		//MODE1 = H
	//CutMotorResetMode2();	//MODE2 = L
	CutMotorSetMode();		//MODE1 = H
	CutMotorDisable();		//禁止输出
}

void CutMotorStandby(void)		//电机停止
{
	//CutMotorSleepOn();		//进入休眠
	CutMotorSetMode();		//MODE1 = H
	CutMotorDisable();		//禁止输出
}

/*******************************************************************************
* Function Name  : void StopCut
* Description    : 停止切刀 
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void StopCut(void)
{
	CutMotorStandby();	//停止
	DelayUs(1000);
 	CutMotorBrake();		//刹车
	DelayUs(150000);
	CutMotorStandby();	//停止
}

/******************************************************************************
* Name        : GetCutSwitchStatus
* Author      : 
* Version     : V0.01
* Date        : 2012-5月-17日
* Description : 读取切刀电机开关传感器的输出值,只有两个状态。
* Output      : 返回CUTTERON或者CUTTEROFF,0为闭合，1为打开
* Others      : 
* History     : 
******************************************************************************/
static uint8_t	GetCutSwitchStatus(void)
{
	uint8_t	Status;
	
	Status = GPIO_ReadInputDataBit(CUT_HOME_IN_GPIO_PORT, CUT_HOME_IN_GPIO_PIN);
	//Status = (~Status)&0x01;
	
	return (Status);
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
