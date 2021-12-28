/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.2
  * @date    2012-5-29
  * @brief   钱箱驱动程序.
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
#include	<string.h>
#include	"ad.h"
#include	"power.h"
#include	"ptake.h"
#include	"extgvar.h"
#include	"debug.h"
#include	"drawer.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : void	InitDrawerPort(void)
* Description    : 钱箱控制端口的初始化
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitDrawerPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(MBOX_OUT1_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= MBOX_CTRL1;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
	GPIO_Init(MBOX_OUT1_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(MBOX_OUT1_PORT, MBOX_CTRL1);		//关钱箱
	
	RCC_APB2PeriphClockCmd(MBOX_OUT2_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= MBOX_CTRL2;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
	GPIO_Init(MBOX_OUT2_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(MBOX_OUT2_PORT, MBOX_CTRL2);		//关钱箱	
	
	RCC_APB2PeriphClockCmd(MBOX_IN_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= MBOX_STATUS;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
	GPIO_Init(MBOX_IN_PORT, &GPIO_InitStructure);
			
}

/*************************************************
函数: 
功能: 
参数: 无
返回: 无
**************************************************/




/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
