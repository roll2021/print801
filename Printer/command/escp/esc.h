/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   指令相关程序.
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
#ifndef ESC_H
#define ESC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"debug.h"

/* extern functions *---------------------------------------------------------*/
extern	void SetCommand_1B09(void);

/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/
#define	MM_PER_DOT									(0.125)

#define	MAX_RIGHT_SPACE_HORIZONTAL	((uint8_t)(255.0 / 203.0 * 25.4 / MM_PER_DOT))
#define	MAX_RIGHT_SPACE_VERTICAL		((uint8_t)(255.0 / 203.0 * 25.4 / MM_PER_DOT))

#define	MAX_LINE_SPACE_HORIZONTAL		((uint16_t)(1016 / MM_PER_DOT))
#define	MAX_LINE_SPACE_VERTICAL			((uint16_t)(1016 / MM_PER_DOT))

#define	DEFAULT_LINE_SPACE					(8)			//默认行间距
#define	DEFAULT_CODE_LINE_HEIGHT		(32)		//默认行高
#define	DEFAULT_CODE_LINE_DOT   		(1)		//走步距离

#define	DEFAULT_TAB_WIDTH				(DEFAULT_HT_CHAR_NUM * DEFAULT_ASCII_FONT_WIDTH)

/* Private	Funtion Declare --------------------------------------------------*/
void Command_1B0C(void);
void Command_1B20(void);
void Command_1B21(void);
void Command_1B24(void);
void Command_1B25(void);
void Command_1B26(void);
void Command_1B2A(void);
void Command_1B2D(void);
void Command_1B31(void);
void Command_1B32(void);
void Command_1B33(void);
void Command_1B3D(void);
void Command_1B3F(void);
void Command_1B40(void);
void Command_1B44(void);
void Command_1B45(void);
void Command_1B47(void);
void Command_1B4A(void);
void Command_1B4B(void);
void Command_1B4C(void);
void Command_1B4D(void);
void Command_1B51(void);
void Command_1B52(void);
void Command_1B53(void);	//未写
void Command_1B54(void);	//未写
void Command_1B55(void);
void Command_1B56(void);
void Command_1B57(void);
void Command_1B58(void);
void Command_1B5A(void);
void Command_1B5C(void);
void Command_1B61(void);
void Command_1B63(void);
void Command_1B64(void);
void Command_1B66(void);
void Command_1B69(void);
void Command_1B6C(void);
void Command_1B70(void);
void Command_1B74(void);
void Command_1B75(void);
void Command_1B76(void);
void Command_1B7B(void);
void Command_1B6D(void);

#ifdef	WH_SELFDEF_CUT_CMD_ENABLE
void	Command_1B6B(void);
#endif

void ESC_Command(void);
void PrintBarCode_2D(uint16_t X_Length, uint16_t Y_Length, uint8_t RatioX, uint8_t RatioY);
void ProcessBarCode_2D(uint8_t CommandType);
void DriveDrawer(uint8_t DrwID, uint8_t Status); 
void ProcessBarCode_2D_1D28(void);

#ifdef __cplusplus
}
#endif

#endif /* ESC_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
