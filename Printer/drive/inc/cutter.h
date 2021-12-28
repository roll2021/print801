/**
******************************************************************************
  * @file    x.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CUTTER_H
#define CUTTER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"extgvar.h"
#include	"beep.h"

/******************************************************************************
* 控制注意事项：
* 1. 切刀开关信号检测时需要加去抖动处理，典型值为2ms，具体时间与具体电路有关。
* 2. 电机转动状态间变换时，不要直接变化，需要先停止1ms，然后再变化到新的转动方向。
******************************************************************************/

/* Macro Definition ----------------------------------------------------------*/
#define CUT_CTRL_GPIO_PORT						GPIOE
#define CUT_CTRL_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define CUT_PHASE_GPIO_PIN						GPIO_Pin_3
#define CUT_ENABLE_GPIO_PIN						GPIO_Pin_4
#define CUT_MODE_GPIO_PIN							GPIO_Pin_2

#define CUT_HOME_IN_GPIO_PORT							GPIOB
#define CUT_HOME_IN_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOB
#define CUT_HOME_IN_GPIO_PIN							GPIO_Pin_7

//以下功能脚无或者拉高而无法控制
//#define CUT_SLEEP_GPIO_PIN						GPIO_Pin_1
//#define CUT_FAULT_GPIO_PIN						GPIO_Pin_4
//#define CUT_MODE1_GPIO_PIN						GPIO_Pin_3
//#define CUT_MODE2_GPIO_PIN						GPIO_Pin_5

#define	CutMotorEnable()			GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_ENABLE_GPIO_PIN)		//使能
#define	CutMotorDisable()			GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_ENABLE_GPIO_PIN)	//禁用

#define	CutMotorSetPhase()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_PHASE_GPIO_PIN)		//PHASE置高
#define	CutMotorResetPhase()	GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_PHASE_GPIO_PIN)	//PHASE置低

#define	CutMotorSetMode()			GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_MODE_GPIO_PIN)		//MODE置高
#define	CutMotorResetMode()		GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_MODE_GPIO_PIN)	//MODE置低

//以下功能脚无或者拉高而无法控制
//#define	CutMotorSetMode1()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_MODE1_GPIO_PIN)		//MODE1置高
//#define	CutMotorResetMode1()	GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_MODE1_GPIO_PIN)	//MODE1置低
//#define	CutMotorSetMode2()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_MODE2_GPIO_PIN)		//MODE2置高
//#define	CutMotorResetMode2()	GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_MODE2_GPIO_PIN)	//MODE2置低
//#define	CutMotorSleepOn()			GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_SLEEP_GPIO_PIN)	//进入休眠
//#define	CutMotorSleepOff()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_SLEEP_GPIO_PIN)		//停止休眠

/* 切刀时电机当前按照以下流程运转电机: */
/* 本切刀是直流电机驱动 */
/* 定义切刀工作是否结束的标志位 */
#define		CUT_PROCESS_BUSY		(1)
#define		CUT_PROCESS_ILDE		(0)

/* 定义电机是否加电的标志位 */
#define		CUT_POWER_YES				(1)
#define		CUT_POWER_NO				(0)

/* 定义电机方向,前进还是后退 */
#define		CUT_OUTWARD					(1)
#define		CUT_HOMEWARD				(0)

/* 当前电路接上切刀在原位检测到低，没有在原位检测到高 */
#define		CUT_AT_HOME					(0)
#define		CUT_NOT_AT_HOME			(1)

/* 切刀动作种类 */
#define		CUT_TYPE_FULL				(0)
#define		CUT_TYPE_PART				(1)
#define		CUT_TYPE_DEFAULT		(CUT_TYPE_PART)

/* 切刀初始化结果 */
#define		CUT_INIT_SUCCESS		(0)		//成功
#define		CUT_INIT_ERROR1			(1)		//错误状态1，开始错误
#define		CUT_INIT_ERROR2			(2)		//错误状态2，中间错误

#define		CUT_RESULT_0				(0)
#define		CUT_RESULT_1				(1)
#define		CUT_RESULT_2				(2)
#define		CUT_RESULT_3				(3)
#define		CUT_RESULT_4				(4)

/* 切刀切纸结果 */
#define		CUT_PAPER_SUCCESS		(CUT_INIT_SUCCESS)
#define		CUT_PAPER_ERROR2		(CUT_INIT_ERROR2)

#define		CUT_FEED_PAPER_BACKWARD_STEPS   (32)

#define		FEED_PAPER_MM_AFTER_CUT					(2)		//原为8
#define		FEED_PAPER_DOT_LINE_AFTER_CUT		(FEED_PAPER_MM_AFTER_CUT * 8)

/* Funtion Declare -----------------------------------------------------------*/
/* 此函数供给外部调用 */
void	InitCutPort(void);
void	InitCutPaper(void);
uint32_t	CutPaper(uint8_t CutMode);	//实际切纸动作
void	DriveCutPaper(uint8_t CutMode);	//切纸
void	CutMotorStandby(void);					//电机停止
void	StopCut(void);

static void	CutError(void);
static void CutMotorForward(void);		//电机前转
static void CutMotorReverse(void);		//电机反转
static void CutMotorBrake(void);			//电机刹车
static void	CutMotorFastDecay(void);
static uint8_t	GetCutSwitchStatus(void);	//读取切刀位置检测开关的值

#ifdef __cplusplus
}
#endif

#endif /* CUTTER_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
