/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   通信接收缓冲区相关的程序.
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
#ifndef COMINBUF_H
#define COMINBUF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "debug.h"

/* Macro Definition ----------------------------------------------------------*/
#ifdef	EXT_SRAM_ENABLE
	#define MAX_BYTES_OF_IN_BUF				(64*1024)			//最大通信接收缓冲区大小
	#define	MAX_RSV_BYTES_OF_IN_BUF		(256)					//保留的最大缓冲区
	#define	MIN_RSV_BYTES_OF_IN_BUF		(128)					//保留的最小缓冲区
#else
	#define MAX_BYTES_OF_IN_BUF				(6*1024)			//最大通信接收缓冲区大小 2016.07.29  8K改为6K
	#define	MAX_RSV_BYTES_OF_IN_BUF		(1500+256)		//保留的最大缓冲区 2016.06.08 1500+256
	#define	MIN_RSV_BYTES_OF_IN_BUF		(1500)			  //保留的最小缓冲区   8*1024-512 2016.07.07 1500
#endif

#define	COM_IN_BUF_NOT_FULL					(0)						//缓冲区未满
#define	COM_IN_BUF_FULL							(1)						//缓冲区已满

/******* 定义输入缓冲区变量 *************/
typedef struct 
{   
  volatile uint32_t	BytesSize;		    	//输入缓冲区字节数 2016
	volatile uint32_t	BytesNumber;			//输入缓冲区字节数
	volatile uint32_t	PutPosition;			//输入缓冲区存数位置
	volatile uint32_t	GetPosition;			//输入缓冲区取数位置
	volatile uint32_t	PE_BytesNumber;		//当前单据输入总字节数
	volatile uint32_t	PE_GetPosition;		//当前单据取数指针
	volatile uint8_t	BufFullFlag;			//缓冲区满标志
					 uint8_t * Buf;							//通信接收缓冲
}TypeDef_StructInBuffer;


extern void RealTimeCommand(void);
/* Funtion Declare -----------------------------------------------------------*/
void	SetComInBufBusy(void);
void	ClearComInBufBusy(void);
void	EnableReceiveInterrupt(void);
void	DisableReceiveInterrupt(void);
void	ReceiveInterrupt(FunctionalState NewState);
void	InitInBuf(void);
uint32_t	GetInBufBytesNumber(void);
void	SetActiveComInBusy(void);
void	ClearActiveComInBusy(void);
void	EnableActiveReceiveInterrupt(void);
void	DisableActiveReceiveInterrupt(void);


#ifdef	NO_PAPER_RE_PRINT_ENABLE
void	ClearInBuf(void);
void	PaperOutDetect(void);
#endif

void	PutCharToInBuffer(uint8_t Data);		//往接收缓冲区存数
uint8_t		GetCharFromInBuffer(void);		//从接收缓冲区读数
void	DecInBufferGetPosition(void);	//回写缓冲区，只更改读数位置值
uint8_t		ReadInBuffer(void);						//从接收缓冲区读数
void	DecInBufferGetPoint(void);				//回写缓冲区，只更改读数位置值
static void FillRealTimeCommand(uint8_t Data);

#ifdef __cplusplus
}
#endif

#endif /* COMINBUF_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
