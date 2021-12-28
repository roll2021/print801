/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-7-31
  * @brief   相关的程序.
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
#include	"systimfunc.h"
#include	"ad.h"
#include	"timconfig.h"
#include	"extgvar.h"
#include	"monobyte.h"
#include	"print.h"
#include	"heat.h"
#include	"cutter.h"
#include	"feedpaper.h"
#include	"platen.h"
#include	"beep.h"
#include	"led.h"
#include	"maintain.h"
#include	"parallel.h"
#include	"power.h"
#include "speaker.h"

extern  volatile uint8_t	g_bPrtSPIDMAIntForDriveMode;
extern	TypeDef_StructHeatCtrl		g_tHeatCtrl;
//extern	TypeDef_StructLinePrtBuf	g_tLinePrtBuf;
extern	uc16	g_tFeedMotorRushTable[];
extern	uint8_t	g_bMaxRushStep;
extern	TypeDef_StructInBuffer volatile g_tInBuf;  //2016.06.08

#ifdef	PAPER_OUT_RE_PRINT_ENABLE
extern	void	PaperOutDetect(void);
#endif
extern uc16	m_tHeatCtrlPinTable[];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	LED_DELAY_PE_MS			  (100)
#define	LED_FAST_PE_MS			  (200)
#define	LED_FAST_FLASH_MS			(400)
#define	LED_SLOW_FLASH_MS			(800)
#define	LED_SLOW_DOOR_MS			(1000)
#define	LED_STATUS_OFF				(0)
#define	LED_STATUS_ON					(1)
#define	LED_PTK_ON_MS			    (200)   //未取单报警
#define	LED_PTK_OFF_MS			  (100)  //
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void	ErrorLedFlash(void);
static void	PELedFlash(void);
static void	PTKLedAndBuzzerFlash(void);
/*******************************************************************************
* Function Name  : 函数名
* Description    : 定时器处理函数,系统时标任务,正常程序时1mS中断1次,
*	在中断程序中被调用,在进入休眠后,每0.25秒中断一次
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	SystemTimer2Func(void)
{
  static uint16_t	DoorNum = 0;
	static uint16_t	CutLedNum = 0;
  static uint16_t	TempNum = 0;
	static uint16_t DataDealNum = 0;
	#ifdef	MAINTAIN_ENABLE	
		static uint16_t	TimeCntr = 0;
	#endif
	
	g_bRoutinFlag = 0;
	g_tSystemCtrl.SleepCounter++;	 //系统进入休眠的计数器,在数据来源口会清零
	//2018.12.10  
	NetReloadCounter++;	
	OpenADC(); 							//打开AD采集

	if((g_tSpeaker.SpeakEnable == 1) || (g_tSpeaker.SpeakPEnable == 1))
	{
		g_tSpeaker.SpeakCnt++;
		if(g_tSpeaker.SpeakCnt > 5000)
		{
			g_tSpeaker.SpeakCnt = 0;
			g_tSpeaker.SpeakFlag = 0;
		}
	}
	else
	{
		g_tSpeaker.SpeakCnt = 0;
	}
	
	//没有数据 并且 写语音flash已经开始
	if((g_tSpeaker.SpeakWriteHandle == WRITE_START) && (g_tInBuf.PutPosition == g_tInBuf.GetPosition))
	{
		if(g_tSpeaker.SpeakWriteTime++ >1000)
		{
			g_tSpeaker.SpeakWriteTime = 0;
			g_tSpeaker.SpeakWriteHandle = WRITE_END;
		}
	}
	
	if(g_tSpeaker.SpeakBufFlg == 1) //语音电路存在
	{
		
			Line_2A_WTN588H_Tim(g_tSpeaker.SpeakBuf);
	}
	
	
	#if	0
	//防止其意外关闭
	if (LPB_PROCESS_STATUS_UNFINISH == GetLPBProcessStatus())
	{
		StartTimer(PRINT_CHECK_TIMER);
	}
	#endif
	
	#ifdef	REALTIME_STATUS_UPDATE_ENABLE
	
	CheckCoverStatus();
		
  if((AutoFeedPaper == 2) && (g_tError.PaperSensorStatusFlag & 0x01) == 0 )  
	{
	   TempNum++;
		 if(TempNum>500)			
		 {				
		   if( g_tSystemCtrl.PaperType == PAPER_TYPE_CONTINUOUS)		
			 {
 				  //g_tFeedButton.KeyDownFlag = 1;			//2016.06.28 按键按下标志 置位
 					//g_tFeedButton.StepCount = 800;
			 }
				else if(g_tSystemCtrl.PaperType == PAPER_TYPE_LABLE)  //标签纸		
				{
					FeedToGapFlag = 1;		
					FeedPaperFlag = 1;        
				}	
			 AutoFeedPaper = 0;
			 TempNum=0;
		 }	 
	  }
		else
    {    
			TempNum=0;
    }
        
		if ((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)	//纸尽直接停止打印
		{
			g_tRealTimeStatus.PrinterStatus |= 0x08;	//进入离线状态
		}
		else	//纸尽传感器检测有纸
		{
			g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);		//不缺纸脱离离线状态
			if ((g_tError.PaperEndStopSensorSelect & 0x01) == 0x01)	//选择了纸将尽传感器停止打印
			{
				if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)	//纸将尽停止打印
				{	//打印机打印完当前行并进纸后进入离线状态
					if ((LPB_STATUS_ILDE == GetPrintStatus()) && \
							(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && \
							(g_tSystemCtrl.StepNumber == 0))
					{
						g_tRealTimeStatus.PrinterStatus |= 0x08;	//进入离线状态
					}
				}
				else	//纸将尽检测有纸
				{
					g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//不缺纸脱离离线状态
				}
			}
		}
	#endif     

		
	AutomaticStatusBack();				//自动状态返回处理 2016.07.07 每次运行 允许自动返回时再返回数据

	
	//控制钱箱打开
	if(g_tDrawer1.ON_Time)						//钱箱1控制，
	{
		if(g_tDrawer1.ON_Time <= g_tDrawer1.OFF_Time)
		{
			DriveDrawer(g_tDrawer1.DrwID, DISABLE);
		}
		g_tDrawer1.ON_Time--;
	}
	if(g_tDrawer2.ON_Time)						//钱箱1控制，
	{
		if(g_tDrawer2.ON_Time <= g_tDrawer2.OFF_Time)
		{
			DriveDrawer(g_tDrawer2.DrwID, DISABLE);
		}
		g_tDrawer2.ON_Time--;
	}
	//2016.09.14 控制蜂鸣器
	DrvBuzzerFun();
	#ifdef	PARALLEL_PNE_OUT_ENABLE
		if (g_tInterface.LPT_Type)	//并口机型输出PE信号
		{
			if (g_tError.LPTPaperEndSensorSelect)	//PE或者PNE之中至少有一个被选择来输出纸尽信号
			{	//PE或者PNE二者之中任何一个检测到缺纸且被使能即输出缺纸
				if ((g_tError.LPTPaperEndSensorSelect & 0x03) == 0x03)	//PE = E, PNE = E,二者均选中
				{
					if (g_tError.PaperSensorStatusFlag & 0x01)	//二者任何一个检测到缺纸信号则输出缺纸
						LPT_PE_Out_PaperEnd();			//输出并口缺纸信号
					else
						LPT_PE_Out_PaperFull();			//输出并口不缺纸信号,二者都没有检测到缺纸信号
				}
				else if ((g_tError.LPTPaperEndSensorSelect & 0x02) == 0x02)	//PE = E,只选择PE
				{
					if ((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)
						LPT_PE_Out_PaperEnd();			//输出并口缺纸信号
					else
						LPT_PE_Out_PaperFull();			//输出并口不缺纸信号,PE没有检测到缺纸信号
				}
				else	//PNE = E,只选择PNE
				{
					if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)
						LPT_PE_Out_PaperEnd();			//输出并口缺纸信号
					else
						LPT_PE_Out_PaperFull();			//输出并口不缺纸信号,PNE没有检测到缺纸信号
				}
			}
			else	//二者均未被选择用来输出纸尽信号
			{
				LPT_PE_Out_PaperFull();			//输出并口不缺纸信号
			}
		}
	#endif	//PARALLEL_PNE_OUT_ENABLE 结束
	
	#ifdef	MAINTAIN_ENABLE	
		//运行时间维护计数器,以分钟为单位
		if (g_bSleepModeFlag)
		{
			TimeCntr += ROUTINE_SLEEP_INTERVAL_MS;
		}
		else
		{
			TimeCntr++;
		}
		if (TimeCntr >= 60*1000)		//60秒
		{
			TimeCntr = 0;
			R_Counter.TimeCounter++;
			C_Counter.TimeCounter++;
//			if (g_tResetableCounter.TimeCounter > \
//				 (g_tResetableCounterBakeup.TimeCounter + MAINTAIN_WRITE_ROUNTIN_MINUTE))
//			{
//				g_bMaintainRountinWriteFlag = 1;
//			}
		}
	#endif
	//2016.06.16 驱动打印缓冲区内有数据则开始定时器5打印
	DataDealNum++;
  if(DataDealNum == 20)		
	{
		DataDealNum = 0;
		
		if( g_bMotorRunFlag == 0 && g_tError.DoorOpen ==0 				//电机没有启动,上盖没打开 
			&& (g_tError.PaperSensorStatusFlag & 0x01)==0	          //没有缺纸
			&& (g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag)==0)	//没有错误
		{
			if (U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD || g_tFeedButton.KeyDownFlag ==1)		//2016.07.10 打印缓冲区有数据或者按键按下
			{			
				StartFeedMotor(FEED_FORWARD);						//重复输出之前的相位,但是不改变相位变量,需加此句打印效果更好
				SetTimerCounterWithAutoReload(DealPrtData_TIMER, FEED_START_US);
				StartTimer(DealPrtData_TIMER);	        //2016.06.15 开启定时5 走步
				HeatPowerOn();
			}	
		}		
	}			
	
// 	//打印缓冲区有数据，票据打印标志位置位 2016.10.25 去掉
// 	if(U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)
// 	{	
//      g_tSystemCtrl.BillPrtFlag |= 4;                
// 	}
	
	ErrorLedFlash();	//错误指示灯处理
	PELedFlash();			//缺纸指示灯处理
	//2016.06.30  切刀后如果开启未取单功能则有声光报警 0.5s一次
	if(g_tSystemCtrl.CutLedFlag == 1 )		
	{		
		PTKLedAndBuzzerFlash();
		
	}	
	else
	{
		if(g_tSpeaker.SpeakPEnable == 0)	//不缺纸时可以清除连续播报标志位
			g_tSpeaker.SpeakFlag = 0;
  }
	
	
	#ifdef	ERROR_EMERGENCY_PROTECT_ENABLE
		if (!ErrorDetect())
		{
			if (g_bFeedTaskFlag)
			{
				ReleaseEmergencyProtect();
				g_bFeedTaskFlag = 0;
			}
		}
	#endif
	
	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
		PaperOutDetect();
	#endif
}

/*******************************************************************************
* Function Name  : 函数名DrvBuzzerFun
* Description    : 控制蜂鸣器
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	DrvBuzzerFun(void)
{
	static uint16_t	BuzzerStatus = 0;
	static uint16_t	BuzzerTemp = 0;
	
	if(BuzzerRunFlag == 1)
	{	
		 if(BuzzerNum!=0)
		 {
			 if(BuzzerStatus == 0)
			 {
					BeepOn();
			    BuzzerTemp++;
				  if(BuzzerTemp == BuzzerTime)
					{
						BuzzerTemp = 0;
						BuzzerStatus = 1;
					}	
			 }
       else
       {
					BeepOff();
					BuzzerTemp++;
					if(BuzzerTemp == BuzzerTime)
					{
						BuzzerTemp = 0;
						BuzzerStatus = 0;
						BuzzerNum--;
					}	
			 }				 			  
		 }	 
		 else
		 {
			 BeepOff();    	
			 BuzzerTemp = 0;
			 BuzzerStatus = 0;
			 BuzzerTime = 0;
			 BuzzerRunFlag = 0;
		 }	 	
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 定时器处理函数,分组加热控制定时
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
* 加热和上纸合并进行处理过程说明：
* 单纯中断走纸开始时：g_tHeatCtrl.HeatEnable = 0,g_tSystemCtrl.StepNumber > 0
* 			  结束后: g_tSystemCtrl.StepNumber = 0
* 加热并走纸开始时	: g_tHeatCtrl.HeatEnable = 1,g_tSystemCtrl.StepNumber > 0
*			  结束后: g_tHeatCtrl.HeatEnable = 0,g_tSystemCtrl.StepNumber = 0	
* 加热为逐点行进行处理,前一点行完成之前,不能设置并启动下一点行,也不能改变加热走纸的全局变量
* 本程序处理电机一步一点行
* 加热一点行分为几步次,每个步次的工作包括电机走一步,依次（最多6次）加热本步次的一整行.
*  具体为每走一步后,再按照g_HeatUs的加热时间加热g_tHeatCtrl.HeaterGroupNumberCpy次.
*  每一步行总的加热时间为g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy
*  走一步电机的时间按照加速表g_tFeedMotorRushTable[]中选择,选择原则为最后一步的时间不少于
*	一步行总的加热时间.
*  电机的加速表步数标志g_StepCount在第一次进入中断之前,由启动一点行加热的程序确定.
*  加热时间g_HeatUs由启动一点行加热的程序确定.
*  如果走一步电机的时间超过一步行总的加热时间,超出部分在走电机后设置为空延时,然后加热.
*   在走每一步电机之后都要判断是否需要设置空延时,如果需要则设置一个标志变量,
*	表示空延时已经设置过了.
*  在加热完一步点行后将空延时标志清零,准备走下一个步点行.
走电机同时加热部分的中断处理时间流程：
	开始走一步电机，此后电机在转动中，关闭加热
			设置电机空延时(此延时过程中只走电机不加热)，关闭加热
					开始加热分区1，(加热时间在启动加热中断前已设置为自动重复载入)
						 开始加热分区2，
									....
											最后分区加热完毕，此时电机也走到位。关闭加热，清空变量完毕。
*******************************************************************************/
#ifdef	DBG_HEAT_INT_COUNT
volatile uint32_t	iDbgHeatTotalCount	= 0;
volatile uint32_t	iDbgHeatStartCount	= 0;
volatile uint32_t	iDbgHeatEndCount		= 0;
#endif
#define		INT_FOR_MOTOR				(0)
#define		INT_FOR_HEAT				(1)
#define		INT_FOR_OVER				(3)		//2016.06.14
/* 逐点行走电机及加热 */
void	ProcessHeat(void)
{
	static uint8_t	mHeatCount = 0;	//统计一点行加热次数,每执行一次一个分组加热值增加1,加热完毕清零
	static uint8_t	mIntFlag = INT_FOR_MOTOR;	//标志进入本次中断是为了控制走电机还是为了控制加热
	volatile uint16_t	HeatPins;
	uint16_t	Time;
	uint32_t	LPBStatus;
	
	EndHeat();	//关加热控制
	if (mIntFlag == INT_FOR_MOTOR)	//处理走电机
	{
		if (g_bMotorRunFlag == 0)	//如果电机还处于静止状态,先准备启动
		{
			StartFeedMotor(FEED_FORWARD);	//重复输出之前的相位,但是不改变相位变量,需加此句打印效果更好
			TIM_SetCounter(FEED_TIMER, FEED_START_US);
		}	//电机静止处理结束
		else	//电机处于运动中
		{
			StepFeedMotor(FEED_FORWARD);
			if (g_StepCount > (MAX_RUSH_STEPS - 1))	//如果步数统计已经超出加速表
			{
				Time = g_tFeedMotorRushTable[MAX_RUSH_STEPS - 1];
			}
			else
			{
				Time = g_tFeedMotorRushTable[g_StepCount];
				g_StepCount++;
			}
			TIM_SetCounter(FEED_TIMER, (Time - g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy));	//设置空延时
			
			mIntFlag = INT_FOR_HEAT;	//避免下一次中断又进入走电机而无法进入加热,此处为1步一点行程序
		}
		
		return;
	}
	else if (mIntFlag == INT_FOR_HEAT)	//处理加热
	{
		if (mHeatCount < (STEP_NUMBERS_PER_DOT_HEIGHT * g_tHeatCtrl.HeaterGroupNumberCpy))	//加热未完成
		{
			HeatPins = g_tHeatCtrl.HeaterGroupElementCpy[mHeatCount % g_tHeatCtrl.HeaterGroupNumberCpy];
			StartHeat(HeatPins);	//加热定时靠自动重装载计数器设定值  2016.06.14
			mHeatCount++;
		}
		else	//加热已完成
		{
			EndHeat();															//关加热控制
			g_tHeatCtrl.HeatEnable = HEAT_DISABLE;	//加热完成标志
			mHeatCount = 0;
			mIntFlag = INT_FOR_MOTOR;
			
			LPBStatus = GetPrintStatus();
			if ((LPBStatus == LPB_STATUS_SPI_SEND_OVER) || (LPBStatus == LPB_STATUS_SET_GO_LINE_SPACE))
			{
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}
		}
		
		return;
	}
	else	//防止意外时
	{
		//EndHeat();															//关加热控制
		g_tHeatCtrl.HeatEnable = HEAT_DISABLE;	//加热完成标志
		mHeatCount = 0;
		mIntFlag = INT_FOR_MOTOR;
	}
}

#ifdef	DBG_MOTOR_STOP_COUNT
	uint32_t	giStopCount = 0;	//打印中电机停止过几次
#endif

#define		FEED_MOTOR_ILDE_NUMBER		(2)  //2016.06.14 2016.06.24 20改为1
void	ProcessMotor(void)
{
	static	uint8_t	IldeCount = 0;		//电机点数走完后继续坚持几个中断
	static	uint8_t	StopHoldFlag = 0;	//电机结束擎住处理
// 	static	uint32_t	StepCount = 0;		//统计走电机步数
	uint32_t	LPBStatus;
	
	if (FeedStepCount < g_tSystemCtrl.StepNumber)	//走纸未完成
	{
		IldeCount = 0;				//走纸过程中
		if (g_bMotorRunFlag == 0)		//如果电机还处于静止状态,先准备启动
		{	//重复输出之前的相位,但是不改变相位变量,需加此句打印效果更好
			StartFeedMotor(FEED_FORWARD);
			SetTimerCounterWithAutoReload(FEED_TIMER, FEED_START_US);	//同时设置自动重载值和计时值
		}		//电机静止处理结束
		else	//电机处于运动中
		{
			StepFeedMotor(FEED_FORWARD);
			FeedStepCount++;
			if (g_StepCount < (MAX_RUSH_STEPS - 1))	//加速阶段延时时间
			{	//同时设置自动重载值和计时值
				SetTimerCounterWithAutoReload(FEED_TIMER, g_tFeedMotorRushTable[g_StepCount]);
				g_StepCount++;
			}
			else	//恒速阶段延时时间
			{	//同时设置自动重载值和计时值
				SetTimerCounterWithAutoReload(FEED_TIMER, g_tFeedMotorRushTable[(MAX_RUSH_STEPS - 1)]);
			}
		}
	}		//走纸未完成处理结束
	else	//走纸完成等待一段时间处理开始
	{
		if (IldeCount == 0)
		{
			FeedStepCount = 0;
			g_tSystemCtrl.StepNumber = 0;
			
		  SetTimerCounterWithAutoReload(FEED_TIMER, g_tFeedMotorRushTable[g_StepCount]); //2016.06.14
			
			LPBStatus = GetPrintStatus();
			if ((LPBStatus == LPB_STATUS_SPI_SEND_OVER) || (LPBStatus == LPB_STATUS_SET_GO_LINE_SPACE))
			{
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}
		}
		
		IldeCount++;
		if (IldeCount > FEED_MOTOR_ILDE_NUMBER)
		{
			if (StopHoldFlag == 0)		//添加结束擎住处理
			{
				StopFeedMotor(FEED_FORWARD);
				SetTimerCounterWithAutoReload(FEED_TIMER, FEED_STOP_US);	//同时设置自动重载值和计时值
				StopHoldFlag = 1;
			}
			else
			{
				IldeCount = 0;
				StopHoldFlag = 0;
				EndFeedMotor();								//关闭电机
				TIM_Cmd(FEED_TIMER, DISABLE);	//关闭电机控制定时器
				#ifdef	DBG_MOTOR_STOP_COUNT
					giStopCount++;
				#endif
			}
		}
	}	//走纸完成等待一段时间处理结束
}


/*******************************************************************************
* Function Name  : 函数名T3HeatLineFunc  2016.06.16  T3驱动加热
* Description    : 定时器处理函数
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	T3HeatLineFunc(void)
{
	u8 static T3Cntr = 0;
	
	T3_over=1;			//设置加热标志
	T3Cntr++;
	if(T3Cntr ==1)
	{
		//设置加热时间
		SetTimerCounterWithAutoReload(FEED_TIMER, g_HeatUs);	    //设置加热时间
		StartTimer(FEED_TIMER);
		//启动第一次加热
		if(	g_tHeatCtrl.HeaterGroupNumber ==2 )										//需要二次加热，
		{
			StartHeatPartOne();
			StopHeatPartTwo();
		}
		else															//需要一次加热
		{
			StartHeatAll();
			T3Cntr++;
		}
	}
	else if(T3Cntr ==2)									//第二次加热
	{
		StartHeatPartTwo();
		StopHeatPartOne();
	}
	else												//结束加热
	{
		StopHeatAll();
		TIM_Cmd(TIM3, DISABLE);
		T3Cntr =0;
		T3_over=0;		//2016.07.10
		HeatOver =0;
	}
}

//以下函数为在一个函数中处理中断上纸和走电机
void SystemTimer3Func(void)
{	
	  T3HeatLineFunc();
}
/*******************************************************************************
* Function Name  : 函数名SystemT5FuncDealWithPrtData  2016.06.16  T6处理数据
* Description    : 定时器处理函数
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	SystemT5FuncDealWithPrtData(void)
{
	uint16_t	HeatTime;
	uint16_t	totalheatus;
	uint32_t	i;
	
	uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT];
	uint8_t	Width, Number;
	uint8_t	* pSrc = NULL;
  uint16_t DotLine;
	uint16_t Temp1;
	
	if(g_bMotorRunFlag == 0)
	{	
		EndFeedMotor();							            	//关闭电机
		TIM_Cmd(DealPrtData_TIMER, DISABLE);			//关闭电机控制定时器		
		return;
	}
		
	if ((U_PrtBufCtrl.IdxWR == U_PrtBufCtrl.IdxRD && g_tFeedButton.KeyDownFlag == 0 )	         //没有数据且没按键按下
		|| (g_tError.PaperSensorStatusFlag & 0x01) 			                                         //连续纸且缺纸
		|| (g_tError.DoorOpen |g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag) )	//有错误 2016.08.04
	{		 
		 StopFeedMotor(FEED_FORWARD);
		 g_bMotorRunFlag = 0;
		 SetTimerCounterWithAutoReload(DealPrtData_TIMER, FEED_STOP_US);	          //同时设置自动重载值和计时值
		 StartTimer(DealPrtData_TIMER);
	}
  else	
	{
// 		while(T3_over==1);          //2016.07.10
		StepFeedMotor(FEED_FORWARD);
		if(g_tFeedButton.KeyDownFlag == 1)
		{
			if (g_StepCount < g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode])	//取最大步数
			{
				g_StepCount++;
			}
		}	
		else
		{	
			//加减速步数调整
			
			Temp1 =U_PrtBufCtrl.IdxWR;	

			if(Temp1 >=U_PrtBufCtrl.IdxRD )
			{	
				DotLine =(Temp1 - U_PrtBufCtrl.IdxRD);
			}	
			else
			{	
				DotLine =(DrvPrtBufDeep - U_PrtBufCtrl.IdxRD + Temp1);
			}
			if(MotoFeedInc==1)								            //加速处理，	1-加速, 2-减速, 3-匀速
			{
				if (g_StepCount > DotLine)	  //若当前步数大于等于缓冲高度，转为匀速，否则加速	
				{	
					MotoFeedInc = 3;		
				}	
				else
				{	
					g_StepCount++;
				}	
			}
			else if (MotoFeedInc==2)								
			{
				if (g_StepCount < DotLine)	   //若当前步数小于缓冲高度，转为匀速，否则减速	
				{	
					MotoFeedInc = 3;
				}	
				else;
				{
					g_StepCount--;
				}	
			}
			else if (MotoFeedInc==3)						//匀速			
			{
				if (g_StepCount >= DotLine)	//若当前步数大于等于缓冲高度，转为减速		
				{
					MotoFeedInc = 2;
				}
				else if (g_StepCount < DotLine)	//若当前步数小于缓冲高度，同时小于最高速，则转为加速
				{
					MotoFeedInc = 1;
				}
			}
			else
			{
				MotoFeedInc = 3;
			}
			if (g_StepCount > g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode])	//取最大步数
			{
				g_StepCount = g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode];
			}
  	}	
	  //同时设置自动重载值和计时值
		SetTimerCounterWithAutoReload(DealPrtData_TIMER, g_tFeedMotorRushTable[g_StepCount]);
		StartTimer(DealPrtData_TIMER);
    //按键处理
		if(g_tFeedButton.KeyDownFlag ==1)	  					                              //优先按键走纸
		{
			if(g_tFeedButton.StepCount >0)
			{
				g_tFeedButton.StepCount--;
				if(g_tFeedButton.StepCount ==0)
				{	
					g_tFeedButton.KeyDownFlag =0;		
				}	
			}
			return;
		}
		//打印处理
		if ((U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxRD].ActiveDots) != 0x00)
		{	
			HeatTime = GetHeatTime();			//计算加热时间
			//SetHeatNumber();						//计算本点行加热次数,设置加热控制管脚
			//计算加热次数
			if (U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxRD].ActiveDots <= MAX_HEAT_DOT_ONE_TIME)			//左右两侧和不超过最大一次加热点数,一次加热
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
	
			g_tHeatCtrl.HeaterGroupNumberCpy = g_tHeatCtrl.HeaterGroupNumber;	//赋值控制变量
			g_tHeatCtrl.HeaterGroupNumber = 0;
			for (i = 0; i < g_tHeatCtrl.HeaterGroupNumberCpy; i++)
			{
				g_tHeatCtrl.HeaterGroupElementCpy[ i ] = g_tHeatCtrl.HeaterGroupElement[ i ];
				g_tHeatCtrl.HeaterGroupElement[ i ] = 0;
			}
	
			g_HeatUs = HeatTime;
			totalheatus = g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy;	//一点行总加热时间
  		//AdjustMotorStep(totalheatus);					//计算电机延时,调整加速步数
			while (1)		//2016.06.14 保证电机走步时间要大于加热时间
			{
				if(g_tFeedMotorRushTable[g_StepCount] > (totalheatus+200))
				{
					break;
				}			
				g_StepCount--;
			}			
			SetTimerCounterWithAutoReload(DealPrtData_TIMER, g_tFeedMotorRushTable[g_StepCount]);
		  StartTimer(DealPrtData_TIMER);
			
		  g_bPrtSPIDMAIntForDriveMode = 1;
			sHeat_StartDMATx(U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxRD].Buf);	//启动机芯SPI发送
		}
// 		U_PrtBufCtrl.BufHeight--;								  	//行高计数器-1 2016.07.10
		U_PrtBufCtrl.IdxRD++;										    //调整行地址
		if (U_PrtBufCtrl.IdxRD >=DrvPrtBufDeep)			//最大打印高度点数（8倍×24点）= 192  有外扩ram时 此处需要更改
		{	
			U_PrtBufCtrl.IdxRD = 0;
		}	
	}
}
/*******************************************************************************
* Function Name  : 函数名
* Description    : 定时器处理函数
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	SystemTimer5Func(void)
{
	
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 指示灯处理函数，
	指示灯闪烁方式说明： 蜂鸣器开启后错误灯亮时响
	POW电源灯上电长亮
	ERROR错误指示灯 闪烁频率
	不可恢复错误  快闪（200ms）以200ms为ON和200ms为OFF
	可恢复错误    慢闪（800ms）以800ms为ON和800ms为OFF
	可自动恢复错误 常亮 
	开盖 常亮
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	ErrorLedFlash(void)
{
	static uint16_t	LedTimeCount = 0;	//计数时间,代表已经为高或者低的维持时间,ms
	static uint8_t	LedStatus = LED_STATUS_OFF;	//LED灯的状态，=0为熄灭，=1为点亮
	
	if (g_tError.UR_ErrorFlag)	//有不可恢复错误,指标灯闪烁、蜂鸣器响，200ms为间隔
	{  
		if (LedStatus == LED_STATUS_OFF)		//先处理灭
		{
			ErrorLedOff();
			DriveBuzzer(DISABLE);
			
			LedTimeCount++;			//统计灭的时间
			if (LedTimeCount >= LED_FAST_FLASH_MS)	//达到后换为亮
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_ON;
			}
		}
		else if (LedStatus == LED_STATUS_ON)	//再处理亮
		{
			ErrorLedOn();
			if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
			{
				DriveBuzzer(ENABLE);
			}
			
			LedTimeCount++;			//统计亮的时间
			if (LedTimeCount >= LED_FAST_FLASH_MS)	//达到后换为灭
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_OFF;
			}
		}
		else
		{
		}
	}		//不可恢复错误处理结束
	else if (g_tError.R_ErrorFlag)	      //有可恢复错误处理,指标灯闪烁、蜂鸣器响，800ms为间隔
	{
		if (LedStatus == LED_STATUS_OFF)		//先处理灭
		{
			ErrorLedOff();
			DriveBuzzer(DISABLE);
			
			LedTimeCount++;			//统计灭的时间
			if (LedTimeCount >= LED_SLOW_FLASH_MS)	//达到后换为亮
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_ON;
			}
		}
		else if (LedStatus == LED_STATUS_ON)	    //再处理亮
		{
			ErrorLedOn();
			if (g_tSysConfig.BuzzerEnable & 0x01)	  //错误时允许
			{
				DriveBuzzer(ENABLE);
			}
			
			LedTimeCount++;			//统计亮的时间
			if (LedTimeCount >= LED_SLOW_FLASH_MS)	//达到后换为灭
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_OFF;
			}
		}
		else
		{
		}
	}		//可恢复错误提示处理结束
	else if (g_tError.DoorOpen)	           //可自动恢复错误处理开始,指示灯常亮 2016.08.04
	{
		if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
		{		
 			 	if (LedStatus == LED_STATUS_OFF)		//先处理灭 2017.05.22
				{
					ErrorLedOff(); 
					DriveBuzzer(DISABLE);					
					LedTimeCount++;			//统计灭的时间
					if (LedTimeCount >= LED_SLOW_DOOR_MS)	//达到后换为亮
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_ON;
					}
				}
				else if (LedStatus == LED_STATUS_ON)	//再处理亮
				{
					if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
					{
						DriveBuzzer(ENABLE);
					}
					ErrorLedOn();
					LedTimeCount++;			//统计亮的时间
					if (LedTimeCount >= LED_FAST_PE_MS)	//达到后换为灭
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_OFF;
					}
				}
				else
				{
				}
		}	
	}	
	else if (g_tError.AR_ErrorFlag)	        //可自动恢复错误处理开始,指示灯常亮
	{
		ErrorLedOn();
		if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
		{
 			//DriveBuzzer(ENABLE);							//2016.06.19 看恢复错误 上盖打开和过温 蜂鸣器不响
		}
	}		//可自动恢复错误处理结束
	else	//没有以上三种错误处理开始,关错误指示灯,关蜂鸣器
	{
		LedTimeCount = 0;		//清除可能的计数中间值
		ErrorLedOff();

		if (g_tError.PaperSensorStatusFlag &0x01 ==0)		//不缺纸处理
		{
			if (g_tSysConfig.BuzzerEnable & 0x01)	//缺纸,错误时允许
			{
				if(g_tSystemCtrl.CutLedFlag == 0)	//2016.07.19  没有未取单时关闭
				{	
					if(BuzzerRunFlag == 0)	//2016.09.14 防止对蜂鸣器指令的影响
					{	
						DriveBuzzer(DISABLE);
					}	
				}	
			}
		}	//关蜂鸣器结束
	}		//没有以上三种错误处理结束,各种情况中不处理缺纸信号,只在无错误关闭蜂鸣器前检查缺纸
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 缺纸指示灯处理函数
	指示灯闪烁方式说明：
	缺纸灯：PAPER灯状态
	缺纸    缺纸灯长亮  蜂鸣器长响(功能开启后)
	纸将近  缺纸灯慢闪  （800ms）
	未取纸  缺纸灯快闪  （200ms） 
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	PELedFlash(void)
{
	static uint16_t	LedTimeCount = 0;	//计数时间,代表已经为高或者低的维持时间,ms
	static uint8_t	LedStatus = LED_STATUS_OFF;	//LED灯的状态，=0为熄灭，=1为点亮
	static uint8_t	PEAlarmNum = 0;	    //缺纸报警蜂鸣器次数
	static uint8_t	PEAlarmFlag = 0;
	
	if(g_tError.DoorOpen)
	{
		return;
	}	
	//缺纸处理,位操作,高有效,bit.0=缺纸;bit.1= 纸将尽
	if (g_tMacro.RunFlag == 0)			//不执行宏定义
	{
		if (((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)	&& (g_tError.DoorOpen == 0))	//缺纸 并且合盖
		{
			//缺纸灯长亮  蜂鸣器间断鸣响
			PaperEndLedOn();
			
			if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
			{	
					if(g_tSpeaker.SpeakCtrl == 1)
					{
						if((g_tSpeaker.SpeakFlag == 0) && (g_tSpeaker.SpeakDoorOpen == 0))
						{
							g_tSpeaker.SpeakPEnable = 1;
				//			Line_2A_WTN5_Tim(1);
	            Line_2A_WTN588H_Tim(g_tSysConfig.POutVoiceNumber);
						}
					}
				
				if (LedStatus == LED_STATUS_OFF)		//先处理灭
				{
					DriveBuzzer(DISABLE);					
					LedTimeCount++;			//统计灭的时间
					if (LedTimeCount >= LED_FAST_PE_MS)	//达到后换为亮
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_ON;
					}
				}
				else if (LedStatus == LED_STATUS_ON)	//再处理亮
				{
					if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
					{
						if(g_tSpeaker.SpeakCtrl == 0)
						DriveBuzzer(ENABLE);
					}
					
					LedTimeCount++;			//统计亮的时间
					if (LedTimeCount >= LED_FAST_PE_MS)	//达到后换为灭
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_OFF;
					}
				}
				else
				{
				}
			}
			else if(g_tSysConfig.BuzzerEnable == 0)	//2016.07.15 不开启蜂鸣器时 缺纸也快响三次
			{
				if(PEAlarmNum < 0x03 && PEAlarmFlag == 0)  //检测到单据未取纸
	      {			
					if (LedStatus == LED_STATUS_OFF)		     //先处理灭
					{
						BeepOn();					
						LedTimeCount++;			//统计灭的时间
						if (LedTimeCount >= LED_DELAY_PE_MS)	   //达到后换
						{
							LedTimeCount = 0;
							LedStatus = LED_STATUS_ON;
						}
					}
					else if (LedStatus == LED_STATUS_ON)	   
					{
						BeepOff();  	                          
						LedTimeCount++;			                      //统计时间
						if (LedTimeCount >= LED_DELAY_PE_MS)	  //达到后换为灭
						{
							LedTimeCount = 0;
							LedStatus = LED_STATUS_OFF;
							PEAlarmNum++;
						}
					}
				}
				else		//单据被取走
				{
					BeepOff();
					PEAlarmFlag = 1;
					PEAlarmNum = 0;
				}	
			}			
			return;
		}		//缺纸处理完毕
		else if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)	//纸将尽
		{
			//纸将近  缺纸灯慢闪  （800ms）
			if (LedStatus == LED_STATUS_OFF)		//先处理灭
			{
				PaperEndLedOff();
				DriveBuzzer(DISABLE);
				
				LedTimeCount++;			//统计灭的时间
				if (LedTimeCount >= LED_SLOW_FLASH_MS)	//达到后换为亮
				{
					LedTimeCount = 0;
					LedStatus = LED_STATUS_ON;
				}
			}
			else if (LedStatus == LED_STATUS_ON)	//再处理亮
			{
				PaperEndLedOn();
				if (g_tSysConfig.BuzzerEnable & 0x01)	//错误时允许
				{
					DriveBuzzer(ENABLE);
				}
				
				LedTimeCount++;			//统计亮的时间
				if (LedTimeCount >= LED_SLOW_FLASH_MS)	//达到后换为灭
				{
					LedTimeCount = 0;
					LedStatus = LED_STATUS_OFF;
				}
			}
			else
			{
			}
			
			return;
		}		//纸将尽处理完毕
		else	//以上各种情况均未出现，即一切正常时
		{
			PEAlarmFlag = 0;  //不缺纸 则清除标志
			LedTimeCount = 0;
			PaperEndLedOff();
			g_tSpeaker.SpeakPEnable = 0;
			if ((g_tError.UR_ErrorFlag == 0) && \
					(g_tError.R_ErrorFlag  == 0) && \
					(g_tError.AR_ErrorFlag == 0))
			{
				if(g_tSystemCtrl.CutLedFlag == 0 && (g_tError.DoorOpen == 0))	//2016.07.19  没有未取单时关闭   //2017.05.23  增加开门的判断
				{	
					if(BuzzerRunFlag == 0)	//2016.09.14 防止对蜂鸣器指令的影响
					{		
						DriveBuzzer(DISABLE);	//没有其他的各种错误时也关闭蜂鸣器
					}	
				}
			}
		}	//无各种缺纸错误时处理完毕
	}		//各种纸张检测处理完毕
}
/*******************************************************************************
* Function Name  : 函数名CheckCoverStatus
* Description    : 检测上盖状态
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void CheckCoverStatus(void)
{
	static uint16_t TempNum = 0;
	if(++TempNum >=10)	
	{
		if (ReadCoverStatus() == PLATEN_STATUS_OPEN)	//2016.08.04
		{
			TempNum = 0;
			g_tError.DoorOpen = 1;	
			g_tSpeaker.SpeakDoorOpen = 1;
		}
		else
		{
			if(g_tError.DoorOpen == 1)
			{    
				if(TempNum>200)
				{    
					TempNum = 0;
					g_tError.DoorOpen = 0;
					g_tSpeaker.SpeakDoorOpen = 0;
					//if(TempNum>250)                                           //合盖后给ad时间来更新纸状态
					{    
						 if(g_tSystemCtrl.PaperType == PAPER_TYPE_CONTINUOUS )  //连续纸  && ((g_tError.PaperSensorStatusFlag & 0x01) == 0)
						 {
								//防止粘纸 走纸n点行
								g_tFeedButton.KeyDownFlag = 1;			//2016.06.28 按键按下标志 置位
								g_tFeedButton.StepCount = 24;
						 }   
						 else if(g_tSystemCtrl.PaperType == PAPER_TYPE_LABLE && ((g_tError.PaperSensorStatusFlag & 0x01) == 0))  //标签纸
						 {
								FeedToGapFlag = 1;                                      //合盖自动上纸找标签标志位
								AutoFeedPaper = 0;
						 } 
						 else if(g_tSystemCtrl.PaperType == PAPER_TYPE_BLACKMARK )  //黑标纸
						 {
								FeedToGapFlag = 1;  
						 }     		
					}
				}                    
			}    
			else
			{
				TempNum = 0;
			}    
		}
			
		if(U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)				//2016.08.04
		{
			//检测上盖开关
			if (g_tError.DoorOpen ==1)			
				g_tError.AR_ErrorFlag |= 0x02;	//bit1=1 打开		  
			else
				g_tError.AR_ErrorFlag &= ~0x02;
		}
	}	
}
/*******************************************************************************
* Function Name  : 函数名PTKLedAndBuzzerFlash
* Description    : 未取单声光报警
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static void	PTKLedAndBuzzerFlash(void)
{
	static uint16_t	PtkCountNum = 0;	                //计数时间,代表已经为高或者低的维持时间,ms
	static uint8_t	AlarmStatus = LED_STATUS_OFF;	   //LED灯的状态，=0为熄灭，=1为点亮
	static uint16_t	PtkCheckAdNum = 0;
	static uint16_t	FlashCountNum = 0;
	
	if(g_tError.DoorOpen == 1)				//开盖后 恢复蜂鸣器和灯带状态 清参数
	{
		PtkCountNum = 0;
		AlarmStatus = LED_STATUS_OFF;
		PtkCheckAdNum = 0;
		FlashCountNum = 0;
		g_tSystemCtrl.CutLedFlag = 0;
		BeepOff();		//关蜂鸣器
		LineLedOn();
		return;
	}	
	
	if(g_tSysConfig.PaperTakeAwaySensorEnable == 1)   //开启未取单功能时
	{
		if(g_tError.PaperSensorStatusFlag & 0x04)  //检测到单据未取纸
		{
			if(g_tSpeaker.SpeakCtrl == 1)
			{
				g_tSpeaker.SpeakEnable = 1;
				if(g_tSpeaker.SpeakFlag == 0)
				{
						if(VoiceOverflag == 0)       //需要看下这个标志
						{	
							Line_2A_WTN588H_Tim(0xFFF3);
						}	
						else
						{	
							Line_2A_WTN588H_Tim(g_tSysConfig.AlarmNumber);  //2018.06.23 音律的选择

						}	
				}
			}
			PtkCheckAdNum = 0;
			if (AlarmStatus == LED_STATUS_OFF)		   //先处理灭
			{
				BeepOff();		//关蜂鸣器
				LineLedOff();
				PtkCountNum++;			//统计灭的时间
				if (PtkCountNum >= LED_FAST_FLASH_MS)	 //达到后换为亮
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_ON;
				}
			}
			else if (AlarmStatus == LED_STATUS_ON)	//再处理亮
			{
				if(g_tSpeaker.SpeakCtrl == 0)
					BeepOn();  	//开蜂鸣器
				LineLedOn();
				
				PtkCountNum++;			                  //统计亮的时间
				if (PtkCountNum >= LED_PTK_ON_MS)	      //达到后换为灭
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_OFF;
				}
			}
			else
			{
			}
		}
		else		//单据被取走
		{
			LineLedOn();
			PtkCheckAdNum++;
			if(PtkCheckAdNum>500)					//2016.07.13 防抖
			{
				g_tSystemCtrl.CutLedFlag = 0;
				PtkCheckAdNum = 0;
				g_tSpeaker.SpeakEnable = 0;
			}		
			
		}	
	}	
	else if(g_tSysConfig.PaperTakeAwaySensorEnable == 2)   	//未开启未取单功能 则切纸后 来单报警
	{
		if((g_tSpeaker.SpeakCtrl == 1)&&(g_tSpeaker.SpeakFlag == 0)) 
		{
			Line_2A_WTN588H_Tim(g_tSysConfig.AlarmNumber);  //2018.06.23 音律的选择
		}
		else if(g_tSpeaker.SpeakCtrl == 0)
			g_tSystemCtrl.CutLedFlagBuf1 = 1;
		
		if(FlashCountNum < 0x03)  //报警三次
		{			
			if (AlarmStatus == LED_STATUS_OFF)		//先处理灭
			{
				BeepOff();					//关蜂鸣器;
				LineLedOff();
				PtkCountNum++;			//统计灭的时间
				if (PtkCountNum >= LED_PTK_OFF_MS)	 //达到后换为亮
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_ON;
				}
			}
			else if (AlarmStatus == LED_STATUS_ON)	//再处理亮
			{
				if(g_tSpeaker.SpeakCtrl == 0)
					BeepOn();  	                          //开蜂鸣器;
				LineLedOn();
				
				PtkCountNum++;			                  //统计亮的时间
				if (PtkCountNum >= LED_PTK_ON_MS)	      //达到后换为灭
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_OFF;
					FlashCountNum++;
				}
			}
		}
		else		//
		{
			LineLedOn();
			FlashCountNum = 0;
			g_tSystemCtrl.CutLedFlagBuf2 = 1;
		}	
		if((g_tSystemCtrl.CutLedFlagBuf2 == 1) && (g_tSystemCtrl.CutLedFlagBuf1 == 1))
		{
				g_tSystemCtrl.CutLedFlag = 0;
			  g_tSystemCtrl.CutLedFlagBuf2 = 0;
				g_tSystemCtrl.CutLedFlagBuf1 = 0;
		}	

	}	
	else
	{
		g_tSystemCtrl.CutLedFlag = 0;
	}	
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
