#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f10x.h"

//定义HPY地址
#define PHY_ADDRESS       0x01 /* Relative to STM3210C-EVAL Board */

//定义MII接口类型
#define MII_MODE          // MII mode 
//#define RMII_MODE       // RMII mode

#define MII_REMAPPED      //重定位

//定义MII接口
#define MII_PORTA	GPIOA
#define MII_PORTB	GPIOB
#define MII_PORTC	GPIOC
#define MII_PORTD	GPIOD

#define MII_CRS		GPIO_Pin_0
#define MII_RX_CLK	GPIO_Pin_1
#define MII_MDIO	GPIO_Pin_2
#define MII_COL		GPIO_Pin_3
#define MII_MCO		GPIO_Pin_8

#define MII_TXD0	GPIO_Pin_12
#define MII_TXD1	GPIO_Pin_13
#define MII_TXD3	GPIO_Pin_8
#define MII_TXEN	GPIO_Pin_11
#define MII_RX_ER	GPIO_Pin_10

#define MII_MDC		  GPIO_Pin_1
#define MII_TXD2  	GPIO_Pin_2
#define MII_TX_CLK	GPIO_Pin_3

#ifndef MII_REMAPPED
	#define MII_CRS_DV	GPIO_Pin_7
	#define MII_RXD0	GPIO_Pin_4
	#define MII_RXD1	GPIO_Pin_5
	#define MII_RXD2	GPIO_Pin_2
	#define MII_RXD3	GPIO_Pin_3
#else

	#define MII_CRS_DV	GPIO_Pin_8
	#define MII_RXD0	  GPIO_Pin_9
	#define MII_RXD1	  GPIO_Pin_10
	#define MII_RXD2	  GPIO_Pin_11
	#define MII_RXD3	  GPIO_Pin_12
#endif

#define	PHY_EXTI_LINE		EXTI_Line10
#define	PHY_INT_PORT		GPIO_PortSourceGPIOB	//HPY中断源端口
#define	PHY_INT_LINE		GPIO_PinSource10 		//PHY中断源引脚	

#define IP_FLASH_ADDR		  (ETH_FLASH_ADDR)
#define MASK_FLASH_ADDR		(IP_FLASH_ADDR+4)
#define GATE_FLASH_ADDR		(MASK_FLASH_ADDR+4)
#define EMAC_FLASH_ADDR		(GATE_FLASH_ADDR+4)

#define HTTP_PORT			  80		   //网页端口
#define PRINT_PORT			9100	   //数据端口
#define STATUS_PORT			4000	   //状态端口

#define BOOTLOAD_VER		BootVerStr			        //引导程序版本 2016.07.14
#define SOFT_VER			  FIRMWARE_STRING	        //应用软件版本
#define ReadFlash(addr)		(*(u32*)(addr))			  //定义读CPUFLASH宏

#define USERID  			SysConfigStruct.Manufacturer
#define PASSWORD 			SysConfigStruct.Model
#define LOGIN_SIZE   		(18 + sizeof(USERID) + sizeof(PASSWORD))

//#if defined   (__GNUC__)        /* GNU Compiler */
//  #define __ALIGN_END    __attribute__ ((aligned (4)))
//  #define __ALIGN_BEGIN         
//#else                           
//  #define __ALIGN_END
//  #if defined   (__CC_ARM)      /* ARM Compiler */
//    #define __ALIGN_BEGIN    __align(4)  
//  #elif defined (__ICCARM__)    /* IAR Compiler */
//    #define __ALIGN_BEGIN 
//  #elif defined  (__TASKING__)  /* TASKING Compiler */
//    #define __ALIGN_BEGIN    __align(4) 
//  #endif /* __CC_ARM */  
//#endif /* __GNUC__ */ 

#endif
