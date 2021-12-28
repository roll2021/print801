/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-7-31
  * @brief   相关的程序.
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
#ifndef SYSTIMFUNC_H
#define SYSTIMFUNC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macro Definition ----------------------------------------------------------*/

/* Funtion Declare -----------------------------------------------------------*/
void	SystemTimer2Func(void);
void	SystemTimer3Func(void);
void	SystemTimer4Func(void);
void	SystemTimer5Func(void);
void	ProcessHeatAndMotor(void);
void	ProcessMotor(void);
void	SystemT5FuncDealWithPrtData(void);
void CheckCoverStatus(void);
void Command_1B42(void);	 
void	DrvBuzzerFun(void);
#ifdef __cplusplus
}
#endif

#endif /* SYSTIMFUNC_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
