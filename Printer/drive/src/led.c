/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   led相关的程序.
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
#include	"led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 缺纸LED指示灯端口初始化
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void PELedPortInit(void)
{	//控制管脚设置为输出,初始化为1
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(PE_LED_RCC_APB2Periph_GPIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin		= PE_LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(PE_LED_GPIO_PORT, &GPIO_InitStructure);
	PELedOff();
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 错误LED指示灯端口初始化
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void ErrorLedPortInit(void)
{	//控制管脚设置为输出,初始化为1  
	GPIO_InitTypeDef GPIO_InitStructure;
	//错误灯
	RCC_APB2PeriphClockCmd(ERROR_LED_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= ERROR_LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(ERROR_LED_GPIO_PORT, &GPIO_InitStructure);
	ErrorLedOff();
	//LED灯带
	RCC_APB2PeriphClockCmd(LINE_LED_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= LINE_LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(LINE_LED_GPIO_PORT, &GPIO_InitStructure);
	LineLedOn();
	
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将指定端口的指定管脚的输出状态取反
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if (GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin))
	{
		GPIO_ResetBits(GPIOx, GPIO_Pin);
	}
	else
	{
		GPIO_SetBits(GPIOx, GPIO_Pin);
	}	
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 缺纸LED指示灯取反
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	TogglePELed(void)
{
	TogglePin(PE_LED_GPIO_PORT, PE_LED_GPIO_PIN);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 错误状态LED指示灯取反
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	ToggleErrorLed(void)
{
	TogglePin(ERROR_LED_GPIO_PORT, ERROR_LED_GPIO_PIN);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
