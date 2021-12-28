/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   usart相关的程序.
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
  * <h2><center>&copy; COPYRIGHT 2016 </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WIFI_H
#define WIFI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
	 
/* Funtion Declare -----------------------------------------------------------*/
void	Init_WiFi(void);
/* Macro Definition ----------------------------------------------------------*/

	 
/*****WIFI模块接口 ****/		 
#define WIFI_RCC_APB1Periph_USART	  RCC_APB1Periph_USART2			//时钟
#define WIFI_RCC_APB2Periph_GPIO	  RCC_APB2Periph_GPIOD
#define WIFI_COM			              USART2			//定义串口
#define WIFI_COM_IRQn              	USART2_IRQn

#define	WIFI_CON_PORT		GPIOD			//串行接口数据端口
#define	WIFI_TXD			  GPIO_Pin_5
#define	WIFI_RXD			  GPIO_Pin_6

// #define	WIFI_DTR		   	GPIO_Pin_14		//DTR输入 UART_CTS
#define	WIFI_BUSY			  ( GPIO_Pin_3)		//忙输出 UART_RTS

#define	WIFI_LINK		    GPIO_Pin_4    //
// #define	WIFI_READY  		GPIO_Pin_9	  
// #define	WIFI_RELOAD  		GPIO_Pin_12	 


#define	WIFI_ELINK_SET		GPIO_SetBits(WIFI_CON_PORT, WIFI_LINK)
#define	WIFI_ELINK_CLR		GPIO_ResetBits(WIFI_CON_PORT, WIFI_LINK)



#define	WIFI_BUSY_SET		GPIO_SetBits(WIFI_CON_PORT, WIFI_BUSY)
#define	WIFI_BUSY_CLR		GPIO_ResetBits(WIFI_CON_PORT, WIFI_BUSY)
#define READ_WIFI_CTS		GPIO_ReadInputDataBit(WIFI_CON_PORT, TYPE_IN)


//2017.10.13
#define CLI()      __set_PRIMASK(1)
#define SEI()      __set_PRIMASK(0)


#ifdef __cplusplus
}
#endif

#endif /* USART_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
