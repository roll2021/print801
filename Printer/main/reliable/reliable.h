/**
******************************************************************************
  * @file    x.h
  * @author  Application Team
  * @version V0.0.0
  * @date    2012-April-10
  * @brief   This file contains all the functions prototypes for the ADC firmware 
  *          library.
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
#ifndef RELIABLE_H
#define RELIABLE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#define	RELIABILITY_PARA_BASE_ADDR						(0x3F2000)	//可靠性参数存放地址

//以下5中测试程序只能有一个有效,不能同时进行或者组合进行
//而且只能在寿命测试时才能开启宏定义,否则全部关闭
//#define	TEST_2MINUTE
//#define	TEST_5MINUTE
//#define	TEST_20MINUTE
//#define	TEST_50KM
//#define	TEST_CUTTER_LIFE

//#define	DBG_QUICK_TEST_ENABLE

#if defined	TEST_2MINUTE || defined	TEST_5MINUTE || defined	TEST_20MINUTE || defined TEST_50KM
#define	ONE_SELFTEST_LEN											(135)
#define	DEFAULT_PRT_SPEED											(200)
#endif

#if defined	TEST_2MINUTE
#define	RELIABLE_TEST_INTERVAL_S							(2 * 60)
#define	RELIABLE_TEST_INTERVAL_MS							(RELIABLE_TEST_INTERVAL_S * 1000)
#define	RELIABLE_TEST_PARA_BASE_ADDRESS				(RELIABILITY_PARA_BASE_ADDR)
#define	RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM	(10)
#define	ADDITIONAL_TITLE_LEN									(23)

#elif	defined	TEST_5MINUTE
#define	RELIABLE_TEST_INTERVAL_S							(5 * 60)
#define	RELIABLE_TEST_INTERVAL_MS							(RELIABLE_TEST_INTERVAL_S * 1000)
#define	RELIABLE_TEST_PARA_BASE_ADDRESS				(RELIABILITY_PARA_BASE_ADDR + 0x1000)
#define	RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM	(4)
#define	ADDITIONAL_TITLE_LEN									(23)

#elif	defined	TEST_20MINUTE
#define	RELIABLE_TEST_INTERVAL_S							(20 * 60)
#define	RELIABLE_TEST_INTERVAL_MS							(RELIABLE_TEST_INTERVAL_S * 1000)
#define	RELIABLE_TEST_PARA_BASE_ADDRESS				(RELIABILITY_PARA_BASE_ADDR + 0x2000)
#define	RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM	(1)
#define	ADDITIONAL_TITLE_LEN									(23)

#elif	defined	TEST_50KM
#define	RELIABLE_TEST_INTERVAL_S							(5)
#define	RELIABLE_TEST_INTERVAL_MS							(RELIABLE_TEST_INTERVAL_S * 1000)
#define	RELIABLE_TEST_PARA_BASE_ADDRESS				(RELIABILITY_PARA_BASE_ADDR + 0x3000)
#define	RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM	(20)
#define	RELIABLE_TEST_50KM_SELFTEST_CYCLE			(2)
#define	ADDITIONAL_TITLE_LEN									(25)

#elif	defined	TEST_CUTTER_LIFE
#define	RELIABLE_TEST_INTERVAL_S							(2)
#define	RELIABLE_TEST_INTERVAL_MS							(RELIABLE_TEST_INTERVAL_S * 1000)
#define	RELIABLE_TEST_PARA_BASE_ADDRESS				(RELIABILITY_PARA_BASE_ADDR + 0x4000)
#define	RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM	(100)

#define	RELIABLE_TEST_FEED_PAPER_LEN					(15)	//每刀上纸mm
#define	RELIABLE_TEST_FEED_PAPER_DOTLINE			(RELIABLE_TEST_FEED_PAPER_LEN / MIN_STEP_UNIT_X)
#define	ADDITIONAL_TITLE_LEN									(40)
#endif

#define	SECOND_PER_MINUTE											(60)
#define	SECOND_PER_HOUR												(60 * SECOND_PER_MINUTE)
#define	SECOND_PER_DAY												(24 * SECOND_PER_HOUR)

#define	MM_PER_MM															(1)
#define	MM_PER_CM															(10 * MM_PER_MM)
#define	MM_PER_M															(100 * MM_PER_CM)
#define	MM_PER_KM															(1000 * MM_PER_M)

#define	ReliableStoreWarnOn()			PaperEndLedOn()
#define	ReliableStoreWarnOff()		PaperEndLedOff()

/* Funtion Declare -----------------------------------------------------------*/
#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE)
static	void ReliabilityTestMinute(void);
#endif

#ifdef	TEST_50KM
static	void	ReliabilityTestKm50(void);
#endif

#ifdef	TEST_CUTTER_LIFE
static	void	ReliabilityTestCutterLife(void);
#endif

#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
static	void	ReliableTestSaveParameter(void);			//处理存储次数值到字库
static	void	PrintReliabilityTitle(void);					//题头
static	void	PrintReliabitityResultTotalNumber(void);					//打印总测试次数
static	void	PrintReliabitityResultTotalTime(void);						//打印总测试时间
static	void	PrintReliabitityResultTotalLen(uint32_t	iMicrometer);	//打印总测试长度
static	void	InitReliableTest(void);								//初始化测试变量和定时器
static	void	ReliableTest(void);										//测试程序
static	uint32_t	GetReliableTestParaStoreIndex(void);		//得到当前有效最先存储序列号
static	uint32_t	GetReliableTestStoreValue(uint32_t iIndex);	//得到当前序号的存储数据
#endif

#ifdef __cplusplus
}
#endif

#endif /*RELIABLE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
