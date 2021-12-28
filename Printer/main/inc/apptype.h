/**
******************************************************************************
  * @file    apptype.h
  * @author  Application Team
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   定义程序的类别：调试版本，正常功能版本，可靠性及寿命测试版本三种
	*						根据三种定义分别编译不同的main()函数。
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
#ifndef APPTYPE_H
#define APPTYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/
#define		VERSION_DEBUG
//#define		VERSION_NORMAL
//#define		VERSION_RELIABILITY

/* Funtion Declare -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* APPTYPE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
