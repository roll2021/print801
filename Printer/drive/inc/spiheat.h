/**
******************************************************************************
	* @file		x.h
	* @author	Application Team  
	* @version V0.0.1
	* @date		2016-3-20
	* @brief	 机芯SPI相关的程序.
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
#ifndef SPIHEAT_H
#define SPIHEAT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
/**
	* @brief	M25P SPI Flash supported commands
	*	W25X32系列FLASH大部分指令与之相同,但是扇区擦除和块擦除的指令不同,待确定
	*/
#define sHEAT_CMD_WRITE					0x02	/*!< Write to Memory instruction */
#define sHEAT_CMD_WRSR					0x01	/*!< Write Status Register instruction */
#define sHEAT_CMD_WREN					0x06	/*!< Write enable instruction */
#define sHEAT_CMD_READ					0x03	/*!< Read from Memory instruction */
#define sHEAT_CMD_RDSR					0x05	/*!< Read Status Register instruction	*/
#define sHEAT_CMD_RDID					0x9F	/*!< Read identification */
//#define sHEAT_CMD_SE						0xD8	/*!< Sector Erase instruction */
//#define sHEAT_CMD_BE						0xC7	/*!< Bulk Erase instruction */
#define sHEAT_CMD_SE						0x20	/*!< Sector Erase instruction */
#define sHEAT_CMD_BE						0xD8	/*!< Bulk Erase instruction */

#define sHEAT_WIP_FLAG					0x01	/*!< Write In Progress (WIP) flag */

#define sHEAT_DUMMY_BYTE				0xA5
#define sHEAT_SPI_PAGESIZE			0x100	/*页大小需要根据手册值确定*/

#define sHEAT_M25P128_ID				0x202018
#define sHEAT_M25P64_ID					0x202017

/* 机芯SPI控制端口定义 */
#define SPI_HEAT_RCC_APBPeriph_GPIO		RCC_APB2Periph_GPIOB
#define SPI_HEAT_RCC_APBPeriph_SPI		RCC_APB1Periph_SPI3
#define SPI_HEAT_SPI									SPI3
#define SPI_HEAT_PORT									GPIOB
#define SPI_HEAT_CLK_PIN							GPIO_Pin_3
#define SPI_HEAT_MOSI_PIN							GPIO_Pin_5
//#define SPI_HEAT_MISO_PIN							GPIO_Pin_4

//#define SPI3_DR_ADDR			0x40003C0C
#define SPI3_DR_ADDR			(SPI3_BASE + 0x0C)

#define	MAX_HEAT_DATA_BYTES		(80)		//最多640点

/* Funtion Declare -----------------------------------------------------------*/
/**
	* @brief	High layer functions
	*/
void	sHeat_DeInit(void);
void	sHeat_LowLevel_Init(void);	//初始化管脚及时钟
void	sHeat_Init(void);						//初始化机芯SPI
void	sHeat_DMAInit(void);				//初始化机芯SPI的DMA设置
void	sHeat_StartDMATx(uint32_t * Buffer_Tx);	//启动机芯SPI的DMA发送

void	sHeat_EraseSector(uint32_t SectorAddr);
void	sHeat_EraseBulk(void);
void	sHeat_WritePage(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void	sHeat_WriteBuffer(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void	sHeat_ReadBuffer(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t	sHeat_ReadID(void);
void	sHeat_StartReadSequence(uint32_t ReadAddr);
uint16_t	SPI_HeatSendByte(uint8_t Byte);

/**
	* @brief	Low layer functions
	*/
uint8_t	sHeat_ReadByte(void);
uint8_t	sHeat_SendByte(uint8_t byte);
uint16_t	sHeat_SendHalfWord(uint16_t HalfWord);
void	sHeat_WriteEnable(void);
void	sHeat_WaitForWriteEnd(void);

#ifdef __cplusplus
}
#endif

#endif /* SPIHEAT_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
