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
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
	 
/* Funtion Declare -----------------------------------------------------------*/
void Init_BT (void);
void InitBluetoothPort(void);
void InitBTUsart(void);
void Read_NByte(uint32_t Length, uint8_t *DataBuf);
uint8_t Judge_BT_Link(void);
void SetBluetooth(u8 FCommand,u8 SCommand);
uint8_t OpenSetupBluetooth(void);
uint8_t CloseSetupBluetooth(void);
/* Macro Definition ----------------------------------------------------------*/
/******* 蓝牙模块类型 *************/
//1,JOBT4 4.0模块；2,GCJO 3.0模块；3,DMBT,I482ES双模模块；4,DLBT,一拖多BT模块
enum {JOBT4=1,GCJO=2,DMBT=3,DLBT=4};
 
#define BT_STR_LEN_NAME       30        //蓝牙模块名称字符串的最大长度 2017.03.21  23改为30
#define BT_STR_LEN_ADDR       20        //蓝牙模块地址字符串的最大长度
#define BT_STR_LEN_PWD        20        //蓝牙模块密码字符串的最大长度
#define BT_STR_LEN_VERSION    50        //蓝牙模块版本字符串的最大长度 2017.03.21  20改为30
#define BT_STR_LEN_CLASS      10        //蓝牙设备类型最大长度

//定义蓝牙所用串口
#define BT_UART         USART2               //蓝牙串口为串口3
#define BT_RCC_APB1Periph_USART	RCC_APB1Periph_USART2			//时钟
#define BT_UART_IRQn    USART2_IRQn					//中断源	
//定义蓝牙IO端口引脚
#define BT_UART_PORT    GPIOD               //蓝牙串口为串口3
#define BT_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOD
#define BT_TYPE_PORT           GPIOD              //蓝牙串口为串口3
#define	BT_WIFI_TYPE		GPIO_Pin_15     //蓝牙或者wifi识别  T801M
#define	BT_SET		      GPIO_Pin_1     //蓝牙设置模式控制
#define	BT_UART_TX		  GPIO_Pin_5    //串口发送
#define	BT_UART_RX		  GPIO_Pin_6    //串口接收
#define	BT_LINK		      GPIO_Pin_4	   //蓝牙链接状态	 
#define	BT_UART_CTS		  GPIO_Pin_2    //串口接收忙
#define	BT_UART_RTS		  GPIO_Pin_3    //串口发送忙
#define	BT_RST		      GPIO_Pin_7	   //蓝牙模块复位控制


/****** 定义蓝牙设置的控制****/
#define	BT_SET_LOW		GPIO_ResetBits(BT_UART_PORT,BT_SET)
#define	BT_SET_HIGH		GPIO_SetBits(BT_UART_PORT,BT_SET)
/****** 定义蓝牙复位的控制****/
#define	BT_RST_LOW		GPIO_ResetBits(BT_UART_PORT,BT_RST)
#define	BT_RST_HIGH		GPIO_SetBits(BT_UART_PORT,BT_RST)
/****** 定义蓝牙RTS（忙）的控制****/
#define	BT_BUSY_CLR		GPIO_ResetBits(BT_UART_PORT, BT_UART_RTS)			//对蓝牙模块清忙
#define	BT_BUSY_SET		GPIO_SetBits(BT_UART_PORT, BT_UART_RTS)		//对蓝牙模块置忙

/****** 定义蓝牙链接状态****/
#define READ_BT_LINK		GPIO_ReadInputDataBit(BT_UART_PORT,BT_LINK)
#define READ_BT_CTS			GPIO_ReadInputDataBit(BT_UART_PORT,BT_UART_CTS)
#define READ_BT_WIFI		GPIO_ReadInputDataBit(BT_UART_PORT,BT_WIFI_TYPE)


/******* 定义蓝牙模块的信息结构 *************/
typedef struct
{
    u8 Type;                        //蓝牙模块类型＝0 BTM162，＝1 金瓯BCO4	  =2 BM57(SPP03)  =3 BM57(SPP05)    
    u8 InitFlag;                    //蓝牙初始化完成标志
    u8 reInitFlag;                  //蓝牙模块重新初始化标志
    u8 Bind;                        //蓝牙模块绑定状态
    u8 Auth;			                  //蓝牙模块密码使能
    u8 Name[BT_STR_LEN_NAME];	      //蓝牙模块名称
    u8 Laddr[BT_STR_LEN_ADDR];	    //蓝牙模块地址
    u8 Password[BT_STR_LEN_PWD];	  //蓝牙模块密码
    u8 Version[BT_STR_LEN_VERSION]; //蓝牙模块版本    
    u8 Class[BT_STR_LEN_CLASS];     //蓝牙设备类型 
}Bluetooth_Struct; 


#ifdef __cplusplus
}
#endif

#endif /* USART_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
