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
#ifndef USART_H
#define USART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#define USART_PORT										GPIOC
#define USART_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOC
#define USART_RCC_APBPeriph						RCC_APB1Periph_UART4
#define USART_SOURCE									UART4
#define USART_TXD_PIN									GPIO_Pin_10
#define USART_RXD_PIN									GPIO_Pin_11
#define	USART_IRQ_NUMBER							UART4_IRQn

#define USART_BUSY_PORT								GPIOE
#define USART_BUSY_PIN								GPIO_Pin_14
#define USART_BUSY_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE

#define	SetUsartHardBusy()		GPIO_SetBits(USART_BUSY_PORT, USART_BUSY_PIN)
#define	ClearUsartHardBusy()	GPIO_ResetBits(USART_BUSY_PORT, USART_BUSY_PIN)

//代表数据可以进入打印机
#define	XON			(0x11)

//代表数据不可以进入打印机
#define	XOFF		(0x13)

//D7校验错误, =0时,忽略; =1,用'?'表示
#define	USART_ERR_FLAG_BIT			(7)
#define	USART_ERR_FLAG_VALUE		(1 << USART_ERR_FLAG_BIT)
#define	USART_ERR_IGNORE_VALUE	(0x00)
#define	USART_ERR_MARK_VALUE		(USART_ERR_FLAG_VALUE)

//D2～D0,表示波特率
#define	USART_BAUD_FLAG_VALUE		(0x07)

//串口的数据协议,
//D3表示握手协议, =0,XON/XOFF; =1,硬件
#define	USART_HANDSHAKE_FLAG_BIT		(2)
#define	USART_HANDSHAKE_FLAG_VALUE	(1 << USART_HANDSHAKE_FLAG_BIT)
#define	USART_HANDSHAKE_SOFT_VALUE	(USART_HANDSHAKE_FLAG_VALUE)
#define	USART_HANDSHAKE_HARD_VALUE	(0x00)

//D4表示位数, =0,7; =1,8位
#define	USART_BITS_FLAG_BIT					(4)
#define	USART_BITS_FLAG_VALUE				(1 << USART_BITS_FLAG_BIT)
#define	USART_BITS_7_VALUE					(0x00)
#define	USART_BITS_8_VALUE					(USART_BITS_FLAG_VALUE)

//D5表示有无校验, =0,有校验; =1无校验
#define	USART_CRC_YES_FLAG_BIT			(5)
#define	USART_CRC_YES_FLAG_VALUE		(1 << USART_CRC_YES_FLAG_BIT)
#define	USART_CRC_YES_VALUE					(0x00)
#define	USART_CRC_NONE_VALUE				(USART_CRC_YES_FLAG_VALUE)

//D6表示奇偶校验, =0,偶校验EVEN; =1奇校验ODD
#define	USART_CRC_TYPE_FLAG_BIT			(6)
#define	USART_CRC_TYPE_FLAG_VALUE		(1 << USART_CRC_TYPE_FLAG_BIT)
#define	USART_CRC_EVEN_VALUE				(0x00)
#define	USART_CRC_ODD_VALUE					(USART_CRC_TYPE_FLAG_VALUE)

/* Funtion Declare -----------------------------------------------------------*/
void	InitUsartPort(void);
void	ConfigUsart(void);
void	UsartSendStr(USART_TypeDef * USARTx, uint8_t * Str);
void	SetUsartBusy(void);
void	ClearUsartBusy(void);

#ifdef __cplusplus
}
#endif

#endif /* USART_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
