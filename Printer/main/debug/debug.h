/**
******************************************************************************
  * @file    x.h
  * @author  Application Team
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   This file contains all the functions prototypes for the firmware 
  *          library.
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
#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macro Definition ----------------------------------------------------------*/
#define	CUTTER_ENABLE
#define	ERROR_EMERGENCY_PROTECT_ENABLE	//是否在错误时进行紧急保护处理
#define	ERROR_STATUS_ADC_UPDATE_ENABLE	//是否进行各种ADC错误状态检查更新:黑标，缺纸,纸将尽，过温,过压,欠压,纸张取走等
#define	REALTIME_STATUS_UPDATE_ENABLE		//是否进行各种ADC错误状态检查更新:纸仓盖，是否缺纸进入离线状态

#define	PAPER_OUT_RE_PRINT_ENABLE										  //缺纸重打处理允许 2016.10.08
//#define	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE					//加重打印速度降低处理
//#define	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE	//不同放大倍数时打印调整
//#define	PRT_SPEED_LIMIT_ENABLE

//#define	HEAT_TIME_BY_POWER_ADJUST_ENABLE						//根据供电电压调整加热时间
#define	HEAT_TIME_BY_TEMPERATURE_ADJUST_ENABLE			//根据温度调整加热时间
//#define	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE						//反显打印加热时间减少处理
#define	HEAT_TIME_START_STEP_ADJUST_ENABLE					//起始打印时调整加热时间
#define	HEAT_TIME_DRV_PRT_ADJUST_ENABLE							//驱动打印加热时间调整

#define	AUTO_FEED_AFTER_CUT_ENABLE								//允许切纸后自动上纸 2016.081.02  去掉
#define	UNDER_LINE_PRT_ENABLE												//允许处理下划线
#define	MACRO_DEFINITION_ENABLE											//允许处理宏定义
#define	WH_SELFDEF_CUT_CMD_ENABLE										//允许处理炜煌自定义切刀指令
#define	MAINTAIN_ENABLE														  //允许处理统计指令

#define	SPI_PRT_TX_BY_DMA_ENABLE	//往机芯送数全部使用DMA方式发送
#define	PNE_ENABLE								//允许处理纸将尽缺纸信号
#define	PARALLEL_PNE_OUT_ENABLE		//允许处理并口纸将尽输出信号
// #define	EXT_SRAM_ENABLE				//允许外部SRAM扩展
// #define	CPCL_CMD_ENABLE

//#define	UART_MAX_BAUD							(115200)
//#define	DRIVE_PRT_MODE_RX_DELAY_ENABLE	//驱动打印时接收延时
//#define	BARCODE_PRT_AS_BITMAP_ENABLE
//#define	FEED_MOTOR_MONITOR_ENABLE
// #define	HEAT_ONE_TIME_ENABLE				  //无论内容多少统统一次加热 2016.06.09


//#define	DBG_NEW_METHOD_SELFTEST
//#define	DBG_RCV_DATA_LOSE
//#define	DBG_INT_RCV
//#define	DBG_UART_VISIBLE_ENABLE
//#define	DBG_RX_TOTAL_THEN_PRT		//测试全部收完之后再打印

//#define	DBG_MODE_SELECT

//#define	DUMMY
#ifdef	DUMMY
	#define	DBG_DUMMY_FEED	//虚拟上纸
	#define	DBG_DUMMY_HEAT	//虚拟加热
	#define	DBG_DUMMY_CUT		//虚拟切纸
#endif
//#define	DBG_DUMMY_CUT		//虚拟切纸
//#define	DBG_DUMMY_HEAT	//虚拟加热

//#define	DBG_TIM					//测试定时器
#ifdef	DBG_TIM
//	#define	DBG_TIM1
//	#define	DBG_TIM2
//	#define	DBG_TIM3
//	#define	DBG_TIM4
//	#define	DBG_TIM5
//	#define	DBG_TIM6
//	#define	DBG_TIM7
//	#define	DBG_TIM8
//	#define	DBG_TIM9
//	#define	DBG_TIM10
//	#define	DBG_TIM11
//	#define	DBG_TIM12
//	#define	DBG_TIM13
//	#define	DBG_TIM14
//	#define	DBG_TIM15
//	#define	DBG_TIM16
//	#define	DBG_TIM17
#endif

//#define	DBG_BUTTON			//上纸和以太网复位按键
//#define	DBG_BUTTON_USE_EXTI
//#define	DBG_POWER_CTRL	//加热电压输出控制
//#define	DBG_USART				//串口中断接收,查询发送
//#define	USE_USART1_AS_NET
//#define	UART_MAX_SPEED_ENABLE
//#define	UART_MAX_BAUD						(250000)
//#define	UART_MAX_BAUD						(1000000)

//#define	DBG_ADC					//ADC
#ifdef	DBG_ADC
//	#define	DBG_ADC_PE					//缺纸信号
//	#define	DBG_ADC_TEMPERATURE	//机芯温度信号
//	#define	DBG_ADC_POWER				//电源分压ADC
//	#define	DBG_ADC_PNE					//纸将尽信号
//	#define	DBG_ADC_BM					//黑标信号
#endif
//#define	DBG_ADC_PRECISION	//测量ADC的精度

//#define	DBG_INBUF_RCV		//InBuf相关的操作

//#define	DBG_SPI					//SPI
#ifdef	DBG_SPI
//	#define	DBG_SPI_ZIKU	//字库读写,查询发送,写入
//	#define	DBG_SPI_ROM		//字库读写,查询发送,写入
//	#define	DBG_SPI_HEAT	//一点行数据发送到机芯,MDA中断发送
#endif

//#define	DBG_LPB_INT_COUNT		//测试打印过程中打印控制定时器进入的次数
//#define	DBG_HEAT_INT_COUNT	//测试打印过程中加热控制定时器进入的次数

//测试各个环节耗时
//#define	DBG_PROCESS_TIME
#ifdef	DBG_PROCESS_TIME
//	#define	DBG_PROCESS_TIME_ANALYSE_AND_READ_ONE_HZ	//分析并读取一个汉字的点阵字库地址等信息耗时
//	#define	DBG_PROCESS_TIME_READ_ZIKU_ONE_ASCII		//读一个ASCII的点阵字库数据耗时
//	#define	DBG_PROCESS_TIME_READ_ZIKU_ONE_HZ				//读一个汉字的点阵字库数据耗时
//	#define	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB			//填满一个字符的编辑缓冲区耗时,只包括编辑填充时间
//	#define	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB			//复制编辑行打印缓冲区数据到打印缓冲区耗时
//	#define	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB			//清空编辑行打印缓冲区耗时
//	#define	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//发送一点行机芯数据耗时,DMA方式
//	#define	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB				//打印一行耗时
//	#define	DBG_PROCESS_TIME_WAIT_LPB_ILDE					//打印一行开始前等待状态变为空闲的耗时
//	#define	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB	//填满一行的编辑缓冲区耗时,包括读字库和编辑填充时间
//	#define	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//发送一点行机芯数据耗时,查询发送
#endif
//#define	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//打印一行过程中继续编辑字符的个数

//#define	DBG_ONE_LINE_TIME			//测试程序各个环节处理耗时
#ifdef	DBG_ONE_LINE_TIME
//	#define	DBG_DEAL_ONE_LINE_TIME	//测试填满一行打印内容耗时
//	#define	DBG_COPY_ONE_LINE_TIME	//测试复制打印缓冲区数据耗时
//	#define	DBG_COPY_AND_CLEAR_ONE_LINE_TIME	//测试复制打印缓冲区数据并清空原缓冲区耗时
//	#define	DBG_PRT_SPEED						//测试打印速度,无法测试
//	#define	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
#endif

//#define	DBG_LEAST_MEM_OPERATE
//#define	NORMAL_MEM_OPERATE
//#define	DBG_ZIKU_SPI_RX_DMA			//测试字库SPI使用DMA方式处理
//#define	DBG_LATCH								//测试机芯数据锁存操作
//#define	DBG_MOTOR_HOLD					//测试电机起始和结束擎住处理效果
//#define	DBG_SPI_HEAT_DATA_SET		//测试机芯SPI3发送接收是否正确

//#define	DBG_FEED				//按键上纸,延时方式
//#define	DBG_CUT					//切刀,延时方式
//#define	DBG_HEAT				//加热,测量管脚电平,不能直接接机芯
//#define	DBG_PRT					//打印

//#define	DBG_USB						//USB
//#define	DBG_ETH_USART			//以太网串口
//#define	DBG_ETH_SPI				//以太网SPI
//#define	DBG_UPDATE
//#define	DBG_ZIKU_SPI_WRITE
//#define	DBG_SELF_DEF_CMD_ENABLE
//#define	DBG_025
//#define	DBG_SELF_DEF_CODEPAGE
//#define	DBG_MOTOR_STOP_COUNT
//#define	DBG_REALTIME_1B76_ENABLE
//#define	DBG_GB18030_ASCII	//将字库中指定位置开始的指定数目的字符取出并打印出来
//#define	DBG_TEMPERATURE_AND_HEAT_TIME
//#define	DBG_EXT_SRAM
#define	DBG_ESC_RELEASE

//#define	DBG_CPCL_TEXT		//测试添加CPCL指令集，实现文本打印



/* Funtion Declare -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
