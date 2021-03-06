/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   胶辊（纸仓盖）检测相关的程序.
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
#include	"platen.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化检测胶辊（纸仓盖）的端口设置，设置为上拉输入
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
#ifdef	PLATEN_CHECK_BY_INT
#else
	void	InitPlatenPort(void)
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(PLATEN_RCC_APB2Periph_GPIO, ENABLE);
		GPIO_InitStructure.GPIO_Pin			= PLATEN_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
		GPIO_Init(PLATEN_GPIO_PORT, &GPIO_InitStructure);		
	}
#endif

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测胶辊（纸仓盖）状态
* Input          : 无
* Output         : 输出参数，开或者关
* Return         : 返回参数，0/1
*******************************************************************************/
#ifdef	PLATEN_CHECK_BY_INT
#else
	uint8_t	ReadCoverStatus(void)
	{	//读入管脚值
		if (PLATEN_VALUE_CLOSE == GPIO_ReadInputDataBit(PLATEN_GPIO_PORT, PLATEN_GPIO_PIN))
		{
			return	(PLATEN_STATUS_CLOSE);
		}
		else
		{
			return	(PLATEN_STATUS_OPEN);
		}
	}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
