/**
******************************************************************************
  * @file    x.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FEEDPAPER_H
#define FEEDPAPER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"debug.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
#define FEED_PHASE_CTRL_GPIO_PORT						GPIOE
#define FEED_PHASE_CTRL_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define FEED_PHASE_CTRL_GPIO_PIN_1					GPIO_Pin_11
#define FEED_PHASE_CTRL_GPIO_PIN_2					GPIO_Pin_10			
#define FEED_PHASE_CTRL_GPIO_PIN_ALL				GPIO_Pin_10 | GPIO_Pin_11

#define FEED_EN_CTRL_GPIO_PORT							GPIOE
#define FEED_EN_CTRL_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE
#define FEED_EN_CTRL_GPIO_PIN_1							GPIO_Pin_8
#define FEED_EN_CTRL_GPIO_PIN_2							GPIO_Pin_9
#define FEED_EN_CTRL_GPIO_PIN_ALL						GPIO_Pin_8 | GPIO_Pin_9

#define	FeedPhaseASet()			GPIO_SetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_1)
#define	FeedPhaseAReset()		GPIO_ResetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_1)
#define	FeedPhaseBSet()			GPIO_SetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_2)
#define	FeedPhaseBReset()		GPIO_ResetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_2)
#define	FeedPhaseAllSet()		GPIO_SetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_ALL)
#define	FeedPhaseAllReset()	GPIO_ResetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_ALL)

#define	FeedMotorEnable()		GPIO_ResetBits(FEED_EN_CTRL_GPIO_PORT, FEED_EN_CTRL_GPIO_PIN_ALL)	//使能
#define	FeedMotorDisable()	GPIO_SetBits(FEED_EN_CTRL_GPIO_PORT, FEED_EN_CTRL_GPIO_PIN_ALL)		//禁用

//#define	FeedMotorForward()		GPIO_SetBits(FEED_CTRL_GPIO_PORT, FEED_DIR_GPIO_PIN)		//上纸方向控制
//#define	FeedMotorBackward()		GPIO_ResetBits(FEED_CTRL_GPIO_PORT, FEED_DIR_GPIO_PIN)	//上纸方向控制

#ifdef	FEED_MOTOR_MONITOR_ENABLE

#endif

//上纸时电机当前按照以下流程运转电机:
//起始擎住阶段——>慢启动阶段——>恒速阶段——>慢停止阶段——>结束擎住阶段
/*定义电机上纸方向,前进还是后退*/
#define	FEED_FORWARD					(0)
#define	FEED_BACKWARD					(1)

#define	FEED_VOLTAGE					(24.0)		//电机供电电压,单位V
#define	FEED_MAX_PPS					(1200)		//最大脉冲数/秒

//以上是对JX-3R-021机器的参数宏定义,以下时间和参数均是按照一步一点行驱动进行的
//注意温度在0~10℃时,打印时最高速度不超过1200PPS,即加速步不超过
//动态加热时不超过1200PPS
//如果电机持续在低于450pps下驱动会产生噪声或者卡纸
#define	FEED_RUSH_STEPS												(126)	//电机加速总步数

// #define	MAX_RUSH_STEPS												(FEED_RUSH_STEPS)	//电机加速总步数
#define	MAX_RUSH_STEPS_UNDER_10_CENTIGRADE		(MAX_RUSH_STEPS / 2)	//温度低于10度打印时的限速加速步数
#define	MAX_RUSH_STEPS_ON_DYNAMIC_HEAT				(MAX_RUSH_STEPS / 2)	//动态加热打印时的限速加速步数
#define	MAX_RUSH_STEPS_ON_EMPHASIZE_PRT				(MAX_RUSH_STEPS / 2)	//加重打印时限速值
#define	MAX_RUSH_STEPS_ON_PRT_BITMAP					(MAX_RUSH_STEPS / 2)	//图形打印时限速值
#define	MAX_RUSH_STEPS_ON_CYCLE90_PRT					(MAX_RUSH_STEPS / 2)	//旋转90度打印时限速值
#define	MAX_RUSH_STEPS_ON_CHAR_NOT_PRT				(MAX_RUSH_STEPS / 2)	//反显打印时限速值
#define	MAX_RUSH_STEPS_ON_DIFFERENT_MAGNIFICATION_PRT		(MAX_RUSH_STEPS / 2)	//纵向横向放大倍数不一致时,60OK,80OK

#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT					(MAX_RUSH_STEPS / 3)	//WH驱动打印时的限速加速步数,20
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_COM			(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//串口驱动打印限速,10,15
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_ETHERNET	(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//网口驱动打印限速,20,38,30,20
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_USB			(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//U口驱动打印限速,20,38,30,20
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_LPT			(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//并口驱动打印限速,15

#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT					(MAX_RUSH_STEPS / 2)	//SP驱动打印时的限速加速步数 15
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_COM			(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//串口驱动打印限速,10
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_ETHERNET	(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//网口驱动打印限速,20,38,30
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_USB			(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//U口驱动打印限速,20,38,30
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_LPT			(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//并口驱动打印限速,30

#define	FEED_BACK_MAX_STEPS						(118)		/*定义最大退纸步数*/
#define	FEED_ONE_TIME_STEPS						(FEED_BACK_MAX_STEPS)	/*定义按下上纸键时上纸的步数*/
#define	FEED_STEPS_AFTER_CLOSE_SHELL	(16)		/*定义每次上电初始化后或者盖上纸舱盖时上纸的步数*/
#define	FEED_STEPS_ANTI_BACKLASH			(16)		//初始化时防止电机反冲而先反向上纸再正向上纸的步数

#define	FEED_START_US									(4000)	//开始擎住阶段等待时间us
#define	FEED_STOP_US									(10000)	//结束擎住阶段等待时间us
#define	FEED_CONST_SPEED_US						(884)		//恒速阶段的各步延时us
#define	FEED_SINGLE_STEP_SPEED_US			(5000)	//单步恒速的各步延时us
//以上是对JX-3R-021机器的参数宏定义

#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
#define	AUTO_FEED_AFTER_CUT_STEPS			FEED_STEPS_AFTER_CLOSE_SHELL
#endif

/* Funtion Declare -----------------------------------------------------------*/
//此函数供给外部调用
void	InitFeedPort(void);
void	FeedPaper(uint32_t direction, uint32_t step);	//电机上纸操作,根据给定值确定走纸多少
void	FindMarkPoint(void);

//上纸时电机当前按照以下流程运转电机:
//起始擎住阶段——>慢启动阶段——>恒速阶段——>慢停止阶段——>结束擎住阶段
//只控制相位和方向,不处理延时
void	StartFeedMotor(uint32_t direction); //起始擎住阶段
void	StopFeedMotor(uint32_t direction);  //结束擎住阶段
void	StepFeedMotor(uint32_t direction);	//向某个方向转过1个步距
void	EndFeedMotor(void);   //全部开路阶段
void	AdjustMotorStep(uint16_t minsteptime);
void	GoDotLine(uint32_t DotLineNumber);

void	WaitHeatAndFeedPaperFinish(void);
void	WaitHeatFinish(void);
void	WaitFeedPaperFinish(void);
void	WaitPrintAndFeedPaperFinish(void);
void	WaitPreviousPrintFinish(void);

uint16_t	GetMaxAccelerateSteps(void);

uint32_t	ErrorDetect(void);
void	SetEmergencyProtect(void);
void	ReleaseEmergencyProtect(void);
void KeyFeedPaper(uint16_t DotLine);

#ifdef __cplusplus
}
#endif

#endif /* FEEDPAPER_H文件定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
