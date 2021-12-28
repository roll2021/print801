/**
******************************************************************************
	* @file		x.h
	* @author	Application Team  
	* @version V0.0.1
	* @date		2016-3-18
	* @brief	 SPI字库相关的程序.
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
#ifndef SPIFLASH_H
#define SPIFLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "speaker.h"

/* SPI使用DMA发送一批无关字节,使用RX中断接收传回的数据 */

/* Macro Definition ----------------------------------------------------------*/
/**
	* @brief	M25P SPI Flash supported commands
	*	W25X32系列FLASH大部分指令与之相同,但是扇区擦除和块擦除的指令不同,待确定
	*/
	/*
	W25Q32BV支持SPI模式0(0,0),模式3(1,1).
	模式0时CLK信号正常时为低.模式1时CLK信号正常时为高.
	W25Q32BV每个最小可编程区域为256字节,为1页page.
	每个扇区sector为4K,每一块block为32K/64K.
	其1024个可擦除扇区/64个可擦除块是相互独立的.
	擦除时可以以扇区为单位,也可以以块为单位.
	4K扇区的编程为数据和参数的存储提供了方便.
	*/
//以下为擦除/编程指令
#define sFLASH_CMD_WREN					0x06	/*!< Write enable instruction */
#define sFLASH_CMD_WRVSREN			0x50	/*!< Write enable for volatile status register instruction */
#define sFLASH_CMD_WRDISEN			0x04	/*!< Write disable instruction */

#define sFLASH_CMD_RDSR					0x05	/*!< Read Status Register 1 instruction,S7~S0	*/
																			/*!< 状态寄存器内容S7~S0将持续不断的送出直到/CS中断该指令 */
#define sFLASH_CMD_RDSR1				0x05	/*!< Read Status Register 2 instruction,S7~S0	*/
																			/*!< 状态寄存器内容S7~S0将持续不断的送出直到/CS中断该指令 */
#define sFLASH_CMD_RDSR2				0x35	/*!< Read Status Register 2 instruction,S15~S8 */
																			/*!< 状态寄存器内容S18~S8将持续不断的送出直到/CS中断该指令 */
#define sFLASH_CMD_WRSR					0x01	/*!< Write Status Register instruction */

#define sFLASH_CMD_WRITE				0x02	/*!< Write to Memory instruction, */
																			/*!<页编程,后面跟3个字节的地址(和1字节数据), */
																			/*!< A23~A16,A15~A8,A7~A0,(D7~D0) */
#define sFLASH_CMD_PAGEWRITE		0x02	/*!< 页写 instruction,同sFLASH_CMD_WRITE */
#define sFLASH_CMD_SE						0x20	/*!< Sector Erase instruction,4K */
																			/*!< 扇区擦除,4K,后面跟3个字节的地址 */
																			/*!< A23~A16,A15~A8,A7~A0 */
#define sFLASH_CMD_BE1					0x52	/*!< Block Erase instruction,32K */
																			/*!< 块擦除,32K,后面跟3个字节的地址 */
																			/*!< A23~A16,A15~A8,A7~A0 */
#define sFLASH_CMD_BE						0xD8	/*!< Block Erase instruction,64K */
																			/*!< 块擦除,64K,后面跟3个字节的地址 */
																			/*!< A23~A16,A15~A8,A7~A0 */
#define sFLASH_CMD_EE1					0xC7	/*!< 全片 Erase instruction */
#define sFLASH_CMD_EE2					0x60	/*!< 全片 Erase instruction */
#define sFLASH_CMD_EPS					0x75	/*!< Erase/Program Suspend instruction */
#define sFLASH_CMD_EPR					0x7A	/*!< Erase/Program Resume instruction */
#define sFLASH_CMD_PD						0xB9	/*!< Power down instruction */

//以下为读操作指令
#define sFLASH_CMD_READ					0x03	/*!< Read from Memory instruction */
																			/*!< 后面跟3个字节的地址(和1字节数据), */
																			/*!< A23~A16,A15~A8,A7~A0,(D7~D0) */
#define sFLASH_CMD_FASTREAD			0x0B	/*!< fate Read from Memory instruction */
																			/*!< 后面跟3个字节的地址加1字节无关数(和1字节数据), */
																			/*!< A23~A16,A15~A8,A7~A0,dummy,(D7~D0) */
#define sFLASH_CMD_RDID					0x9F	/*!< Read identification, ID15~ID0 */

//以下为某些常量定义
#define sFLASH_WIP_FLAG					0x01	/*!< Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE				0xA5
#define sFLASH_SPI_PAGESIZE			0x100		/* 页大小需要根据手册值确定, 256字节 */
#define sFLASH_SPI_SECTORSIZE		0x1000	/* 扇区大小需要根据手册值确定, 4K */
#define sFLASH_SPI_BLOCKSIZE1		0x8000	/* 块大小1需要根据手册值确定, 32K */
#define sFLASH_SPI_BLOCKSIZE2		0x10000	/* 块大小2需要根据手册值确定, 64K */
#define sFLASH_SPI_BLOCKSIZE		0x10000	/* 块大小需要根据手册值确定, 64K */


#define sFLASH_M25P128_ID				0x202018
#define sFLASH_M25P64_ID				0x202017
#define sFLASH_W25Q32_ID				0x4016

/* 字库SPI控制端口定义 */
#define ZK_FLASH_RCC_APBPeriph_GPIO		RCC_APB2Periph_GPIOA
#define ZK_FLASH_RCC_APBPeriph_SPI		RCC_APB2Periph_SPI1
#define ZK_FLASH_SPI									SPI1
#define ZK_FLASH_PORT									GPIOA
#define ZK_FLASH_CS_PIN								GPIO_Pin_4
#define ZK_FLASH_CLK_PIN							GPIO_Pin_5
#define ZK_FLASH_MISO_PIN							GPIO_Pin_6
#define ZK_FLASH_MOSI_PIN							GPIO_Pin_7

/* Select SPI FLASH: ChipSelect pin low	*/
#define sFLASH_CS_LOW()			GPIO_ResetBits(ZK_FLASH_PORT, ZK_FLASH_CS_PIN)

/* Select SPI FLASH: ChipSelect pin high */
#define sFLASH_CS_HIGH()		GPIO_SetBits(ZK_FLASH_PORT, ZK_FLASH_CS_PIN)

#define SPI1_DR_ADDR				(SPI1_BASE + 0x0C)

#define	sFLASH_ReadBuffer		sFLASH_FastReadBuffer

/* Funtion Declare -----------------------------------------------------------*/
/**
	* @brief	High layer functions
	*/
void	sFLASH_DeInit(void);
void	sFLASH_LowLevel_Init(void);		//初始化管脚和时钟
void	sFLASH_Init(void);						//调用该函数即可完成初始化
void	sFLASH_DMAInit(void);					//初始化字库SPI的DMA设置
void	sFLASH_StartDMARx(uint32_t * Buffer_Rx);	//启动机芯SPI的DMA接收

void	sFLASH_EraseSector(uint32_t SectorAddr);	//扇区擦除,4K
void	sFLASH_EraseBlock(uint32_t BlockAddr);		//块擦除,64K
void	sFLASH_EraseAllChip(void);								//全片删除

void	sFLASH_WritePage(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void	sFLASH_WriteBuffer(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
//void	sFLASH_ReadBuffer(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void	sFLASH_FastReadBuffer(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

uint32_t	sFLASH_ReadID(void);
void	sFLASH_StartReadSequence(uint32_t ReadAddr);

/**
	* @brief	Low layer functions
	*/
uint8_t	sFLASH_ReadByte(void);
uint8_t	sFLASH_SendByte(uint8_t byte);
uint16_t	sFLASH_SendHalfWord(uint16_t HalfWord);
void	sFLASH_WriteEnable(void);
void	sFLASH_WaitForWriteEnd(void);
void	WriteSPI_FlashParameter(uint8_t *Buf, uint8_t Len);

#ifdef __cplusplus
}
#endif

#endif /* SPIFLASH_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
