/**
******************************************************************************
  * @file    x.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HEAT_H
#define HEAT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
#define HEAT_CTRL_GPIO_PORT							GPIOE
#define HEAT_CTRL_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE
#define HEAT_PDST1_GPIO_PIN							GPIO_Pin_5
#define HEAT_PDST2_GPIO_PIN							GPIO_Pin_6
	 
#define HEAT_PLATCH_GPIO_PORT						GPIOB
#define HEAT_PLATCH_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOB
#define HEAT_PLATCH_GPIO_PIN						GPIO_Pin_6 

#define StartHeatPartOne()	GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN)
#define StartHeatPartTwo()	GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST2_GPIO_PIN)
#define StartHeatAll()			GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN)
#define StopHeatPartOne()		GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN)
#define StopHeatPartTwo()		GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST2_GPIO_PIN)
#define StopHeatAll()				GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN)

#define	MAX_HEAT_SUM					(2)				//一点行最大分组加热控制次数 
#define	MAX_HEAT_DOT_ONE_TIME	(352)			//一点行最大允许可同时加热点数 352
//左右各空出32点不加热
#define	DST1_HEAT_DOT					(352)			//左侧DST1控制的加热点数
#define	DST2_HEAT_DOT					(224)			//右侧DST2控制的加热点数
#define	DST1_HEAT_BYTE				(44)			//(DST1_HEAT_DOT >> 3)
#define	DST2_HEAT_BYTE				(28)			//(DST2_HEAT_DOT >> 3)

#define	HEAT_DISABLE					(0)
#define	HEAT_ENABLE						(1)

#define	HEAT_ACTION_DISABLE		(0)
#define	HEAT_ACTION_ENABLE		(1)

#define	HEAT_TEMP_COEFFICENT	(0.0076)
#define	ROOM_TEMPERATURE			(25)			//室温下的标准温度值

#define	MIN_HEAT_TIME					(240)		//240
#define	MAX_HEAT_TIME					(1000)		//最长加热时间,us,1000

/********* 定义三种浓度对应的时间, 单位uS **********/
#define HEAT_TIME_LOW					(240)			//低浓度加热时间,单位uS
#define HEAT_TIME_MIDDLE			(360)			//中浓度加热时间, 360
#define HEAT_TIME_HIGH				(400)			//高浓度加热时间,
#define HEAT_TIME_MIN					(240)			//最小加热时间,240

typedef struct
{
	uint8_t	HeatEnable;				  			//使用定时器的加热控制标志
	uint8_t	HeaterGroupNumber;				//一行点阵数据打印加热分组个数
	uint8_t	HeaterGroupNumberCpy;			//一行点阵数据打印加热分组个数副本
	
	uint16_t	HeaterGroupElement[ MAX_HEAT_SUM ];		//一行点阵数据打印加热分组控制数据
	uint16_t	HeaterGroupElementCpy[ MAX_HEAT_SUM ];	//一行点阵数据打印加热分组控制数据副本
}TypeDef_StructHeatCtrl;

/* Funtion Declare -----------------------------------------------------------*/
void	InitHeatPort(void);
void	LatchHeatData(void);
void	InitHeatCtrlStruct(void);

void	StartHeat(uint16_t	HeatPins);
void	EndHeat(void);

int16_t	GetTemperature(void);
void	SetHeatNumber(void);
uint16_t	GetHeatTime(void);

static uint8_t	GetHighBitNumberInByte(uint8_t DataIn);
static double	GetPowerVoltage(void);
static uint16_t	GetAdjustHeatTimeOfPowerVoltage(uint16_t htime);
static uint16_t	GetAdjustHeatTimeOfFirstLineHeat(uint16_t htime);
static uint16_t	GetAdjustHeatTimeOfTemperature(int32_t	Temperature);

#ifdef __cplusplus
}
#endif

#endif /* HEAT_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
