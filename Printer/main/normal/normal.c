/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   正常打印功能的程序
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
/* Includes ------------------------------------------------------------------*/
#include	"normal.h"
#include	"reliable.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"
#include	"gvar.h"		//全局外部变量声明
#include	"extgvar.h"
#include	"apptype.h"
#include	"mode.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern	void LongToStr(uint32_t num, uint8_t *Str);
extern	void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);
extern  void PrintNetParameterReal(uint8_t cIndex);
void assert_failed(uint8_t * file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  uint8_t	bBuf[16];

	UpLoadData(file, strlen((const char *)file));
	LongToStr(line, bBuf);
	UpLoadData(bBuf, strlen((const char *)bBuf));
	
	/* Infinite loop */
  while (1)		
  {
		EndHeat();						//关加热2016.06.04
		EndFeedMotor();				//关上纸电机
		ToggleErrorLed();
		DelayMs(1000);
  }
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测上电时系统处于何种工作模式并处理
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	ProcessModeType(void)
{	
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//系统参数
	
	StopTimer(ROUTINE_TIMER);			//先关闭TIM2，后面根据情况再开启
	EnableReceiveInterrupt();     //开启中断 防止关闭并口中断对按键计数的影响，两者公共中断函数;
	SysModeType = GetModeType();			//判断当前的模式
	InitKeyCtrlStructParameter();	//清除按键数据
	StartTimer(ETH_DATA_TIMER);		//2016.07.07
	switch (SysModeType)							//检测上电后的工作模式
	{
		case	MODE_NORMAL:					    //正常模式
			StartTimer(ROUTINE_TIMER);
			break;
		case	MODE_SELFTEST_PRT:		    //自检模式
			DelayUs(1000*500);
			StartTimer(ROUTINE_TIMER);
			SystemSelfTest();
			break;
		case	MODE_HEX_PRT:					    //16进制模式
			DelayUs(1000*500);
			StartTimer(ROUTINE_TIMER);
			ClearComInBufBusy();
			HexMode();
			break;
		case	MODE_SET_PARAS:				   //设置参数模式
			DelayUs(1000*500);
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_SET_PARAS\n");
			#endif
			//进入设置参数程序  设置wifi
				WIFI_ELINK_CLR;   //进入WIFI设置
				DelayMs(8000);
				WIFI_ELINK_SET;
				NVIC_SystemReset();			  				//复位
			break;
		case	MODE_NORMAL_UPDATE:		   //正常升级模式
			ClearComInBufBusy();
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_NORMAL_UPDATE\n");
			#endif
			Update();
			break;
		case	MODE_RESTORE_DEFAULT_PARAS:	//恢复默认参数模式
			DelayUs(1000*500);
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_RESTORE_DEFAULT_PARAS\n");
			#endif
			//进入恢复默认参数程序
			StartTimer(ROUTINE_TIMER);
      RestoreDefaultParas();
			ClearComInBufBusy();
      GoDotLine(20);		        //走到切纸位置
      PrintString("Entered MODE_RESTORE_DEFAULT_PARAS\n");
      GoDotLine(CUT_OFFSET);		//走到切纸位置
      while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 );	//等待前点行加热完成
      DelayMs(200);						  //延时
			WriteFlashParameter(2);		        // 将当前内容保存，防止掉电写入时错误 2016.05.31  
      NVIC_SystemReset();			  				//复位
			break;
		case	MODE_PRINT_TEST:		           //打印测试模式 2016.05.31
			DelayMs(1500);					           //2016.07.02
			StartTimer(ROUTINE_TIMER);
      PrtTestMode();
			break;
		case	MODE_RESTORE_NET_PARAS:		    //2016.07.14 按键恢复默认IP参数
			DelayMs(100);			
      g_tSysConfig.DHCPEnable = 0;			//2017.02.07  恢复默认ip 关闭DHCP	
      SysConfigUnionData.SysConfigStruct = g_tSysConfig;
	    WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
      DelayMs(300);						          //延时		
			InitNetworkAddr();		            //恢复IP为默认参数
		  DelayMs(1500);
			NVIC_SystemReset();			  				//复位
			break;
		case	MODE_ERASE_APP:				        //擦除程序模式
			DelayUs(1000*500);
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_ERASE_APP\n");
			#endif
			//进入擦除程序
			break;
		case	MODE_RELEASE_JTAG:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);	//SWJ,JTAG全部可用
			FLASH_Unlock();		/* Unlock the internal flash */
			break;
		default:
			break;
	}
	
	if (SysModeType == MODE_NORMAL)
	{
		DelayMs(10);    //让前面的定时器等初始化完成
		if( g_bMotorRunFlag == 0 && g_tError.DoorOpen ==0 				//电机没有启动,上盖没打开 
		&& (g_tError.PaperSensorStatusFlag & 0x01)==0	//没有缺纸
		&& (g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag)==0)	//没有错误
		{
			if ((g_tSysConfig.BlackMarkEnable))	   //找黑标开启后	上电先找黑标	2016.08.04
			{		
// 				while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);  //2016.06.30			
// 				FindMarkPoint();   //去掉开机找黑标 2016.11.17
			}
			else
			{	
				GoDotLine(FEED_STEPS_AFTER_CLOSE_SHELL);    //2016.05	上电后走纸24点 3mm
			}	
		}	
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
#ifdef	VERSION_NORMAL
int main(void)
{
	Sys_Init();		//初始化时钟,GPIO端口,全局变量,外部设备
	
#ifdef WITHBOOT
	CheckBootLegality();	//检验BOOTLOAD程序合法性
#endif
	
	ProcessModeType();
	ClearComInBufBusy();
	
	while (1)
	{
		CheckFeedButton();				//进行检查按键等工作
#ifdef	MAINTAIN_ENABLE
		CheckMaintainRoutin();		//检查周期性维护参数存储
#endif
		ProcessCharMaskReadBuf();	//处理字符点阵读取缓冲区
	}
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
