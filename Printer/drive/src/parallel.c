/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-23
  * @brief   并口驱动相关的程序.
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
#include	"parallel.h"
#include	"timconfig.h"
#include	"extgvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化数据端口设置,
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitParallelDataPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(PARALLEL_DATA_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_DATA0_GPIO_PIN | PARALLEL_DATA1_GPIO_PIN | \
																		PARALLEL_DATA2_GPIO_PIN | PARALLEL_DATA3_GPIO_PIN | \
																		PARALLEL_DATA4_GPIO_PIN | PARALLEL_DATA5_GPIO_PIN | \
																		PARALLEL_DATA6_GPIO_PIN | PARALLEL_DATA7_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;				//上拉输入
// // 	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;		//浮动输入 2016.06.12
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(PARALLEL_DATA_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化控制端口设置
* Input          : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void	InitParallelCtrlPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(PARALLEL_READ_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_READ_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(PARALLEL_READ_GPIO_PORT, &GPIO_InitStructure);
	ClearParallelReadData();
	SetParallelReadData();
	
	RCC_APB2PeriphClockCmd(PARALLEL_ACK_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_ACK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;		//2016.06.12
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(PARALLEL_ACK_GPIO_PORT, &GPIO_InitStructure);
	SetParallelAck();
	
	RCC_APB2PeriphClockCmd(PARALLEL_PE_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_PE_GPIO_PIN;
	GPIO_Init(PARALLEL_PE_GPIO_PORT, &GPIO_InitStructure);
	LPT_PE_Out_PaperFull();
	
	RCC_APB2PeriphClockCmd(PARALLEL_ERR_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_ERR_GPIO_PIN;
	GPIO_Init(PARALLEL_ERR_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(PARALLEL_ERR_GPIO_PORT, PARALLEL_ERR_GPIO_PIN);     //2016.06.19
	
	RCC_APB2PeriphClockCmd(PARALLEL_BUSY_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_BUSY_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;		//2016.06.12
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(PARALLEL_BUSY_GPIO_PORT, &GPIO_InitStructure);
	SetParallelBusy();
	
	RCC_APB2PeriphClockCmd(PARALLEL_STB_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PARALLEL_STB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_IPU;				//上拉输入 2016.06.12
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(PARALLEL_STB_GPIO_PORT, &GPIO_InitStructure);
	Read_LPT();													//并口清复位后的STB
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	/* Enable AFIO clock */
	GPIO_EXTILineConfig(PARALLEL_STB_GPIO_PORT_SOURCE, PARALLEL_STB_GPIO_PIN_SOURCE);	//并口中断
	EXTI_InitStructure.EXTI_Line		= PARALLEL_STB_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode		= EXTI_Mode_Interrupt;		//外部中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //	//下降沿有效 2016.06.21
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* Function Name  : ACKOut
* Description    : 输出并口ACK脉冲
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	LPT_ACK_Out(void)
{
	uint8_t i;
// 	SetParallelAck();			//置高
	ClearParallelAck();		//置低
	for(i=0; i<3; i++);		//2016.06.07 延时，满足ACK宽度要求>500ns
	SetParallelAck();			//置高
	ClearParallelBusy();	//清忙
}

/*******************************************************************************
* Function Name  : LPT_PE_Out(uint8_t State)
* Description    : 输出并口PE（缺纸）状态
* Input          : State:PE的状态,0有纸输出低
* Output         : None
* Return         : None
*******************************************************************************/
void	LPT_PE_Out(uint8_t State)
{
	if (State == DISABLE)
		ClearParallelPE();		//置低
	else
		SetParallelPE();			//置高
}

/*******************************************************************************
* Function Name  : LPT_ERR_Out(uint8_t State)
* Description    : 输出并口ERR（错误）状态
* Input          : State:ERR的状态,0无错误,输出高
* Output         : None
* Return         : None
*******************************************************************************/
void	LPT_ERR_Out(uint8_t State)
{
	if (State == DISABLE)
		;	//置高
	else
		;	//置低
}

/*******************************************************************************
* Function Name  : Read_LPT(void )
* Description    : 读并口数据
* Input          : None
* Output         : None
* Return         : 返回读取的数据
*******************************************************************************/
uint8_t	Read_LPT(void)
{
	uint8_t	Data;
	
	ClearParallelReadData();
	Data = (uint8_t)ReadParallelData();
	SetParallelReadData();
	
	return (Data);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
