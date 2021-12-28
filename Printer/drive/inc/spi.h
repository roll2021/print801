/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： spi.h
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年07月29日
  * @摘要       ： SPI头文件
  ******************************************************************************/

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _SPI_H
#define _SPI_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"


/* 宏定义 --------------------------------------------------------------------*/
#define PORT_SPI_CS               GPIOE
#define PORT_SPI_SCK              GPIOD
#define PORT_SPI_MISO            GPIOC //GPIOD
#define PORT_SPI_MOSI            GPIOC //GPIOD

// #define PORT_SPI_SCK              GPIOA
// #define PORT_SPI_MISO             GPIOA
// #define PORT_SPI_MOSI             GPIOA

#define PIN_SPI_CS                GPIO_Pin_15
#define PIN_SPI_SCK               GPIO_Pin_13//GPIO_Pin_7
#define PIN_SPI_MISO             GPIO_Pin_14// GPIO_Pin_4
#define PIN_SPI_MOSI              GPIO_Pin_15//GPIO_Pin_1

// #define PIN_SPI_SCK               GPIO_Pin_5
// #define PIN_SPI_MISO              GPIO_Pin_6
// #define PIN_SPI_MOSI              GPIO_Pin_7

#define SPI_CS_ENABLE             GPIO_ResetBits(PORT_SPI_CS, PIN_SPI_CS)
#define SPI_CS_DISABLE            GPIO_SetBits(PORT_SPI_CS, PIN_SPI_CS)

#define SPI_SCK_LOW               GPIO_ResetBits(PORT_SPI_SCK, PIN_SPI_SCK)
#define SPI_SCK_HIGH              GPIO_SetBits(PORT_SPI_SCK, PIN_SPI_SCK)

#define SPI_MISO_READ             GPIO_ReadInputDataBit(PORT_SPI_MISO,PIN_SPI_MISO)

#define SPI_MOSI_LOW              GPIO_ResetBits(PORT_SPI_MOSI, PIN_SPI_MOSI)
#define SPI_MOSI_HIGH             GPIO_SetBits(PORT_SPI_MOSI, PIN_SPI_MOSI)


/* 函数申明 ------------------------------------------------------------------*/
void SPI_Initializes(void);
void SPI_WriteByte(uint8_t TxData);
uint8_t SPI_ReadByte(void);


#endif /* _SPI_H */

/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
