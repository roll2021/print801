/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   This file provides all the firmware functions.
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
#include	"beep.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化蜂鸣器控制管脚,控制管脚设置为输出,初始化为0
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void InitBeepPort(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(BEEP_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= BEEP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}

/*******************************************************************************
* Function Name  : BuzzerDrive( void )
* Description    : 开启或关闭蜂鸣器
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DriveBuzzer(uint8_t Status)
{
	if ((Status == DISABLE) && (g_tSystemCtrl.CutRunFlag == 0))
	{
		BeepOff();		//关蜂鸣器,如果正在切纸，则不关闭
	}
	else
	{
		if (g_tSysConfig.BuzzerEnable)
		{
			BeepOn();  	//开蜂鸣器
		}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
