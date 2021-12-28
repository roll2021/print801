/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-27
  * @brief   工作模式检测相关的程序.
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
#include	"button.h"
#include	"platen.h"
#include	"led.h"
#include	"mode.h"
#include	"timconfig.h"
#include	"defconstant.h"
#include  "extgvar.h"

extern	uint8_t	GetFeedButtonCount(void);
extern	void	InitKeyCtrlStructParameter(void);
extern  TypeDef_StructSysConfig		g_tSysConfig; 		//从flash中读出系统参数
extern   Maintenance_Counter R_Counter;

extern void PrtTestFun(void);		//2017.03.20
static void OpenDrawer(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 检测上电后工作模式，检测纸仓盖、上纸键情况,新的操作检测标准
	按键操作说明：按feed键时 每按一次 paper灯亮一次，合盖计算按键的次数；
	
	打印自检条操作：
	纸仓盖合上，按住feed键上电；按键不放开。

	进入其他模式：
	上盖打开并且按住feed键上电后，
	错误灯会以0.5S亮1s灭的频率一直闪烁，程序中记录闪烁次数，
	松开feed键后，错误灯熄灭不再闪烁；
	按feed键，缺纸灯会点亮，
	松开feed键，缺纸灯熄灭，
	并记录了feed键的按键次数；
	合上纸舱盖后，程序根据错误灯闪烁次数和Feed键的按下次数来进入不同的模式；

	进入hex模式：
	打开上盖，按住feed键上电，
	直到错误和缺纸灯同时点亮,0.5s后熄灭，（上电指示，该步对所有模式均一样，可以无视）
	错误灯闪一次后（0.5S亮1s灭的频率一直闪烁并记录闪烁次数），
	松开按键，
	合上纸舱盖，进入hex模式，并打印出Hexadeciaml dumping；
	退出为关闭电源或者按三次feed键；

	进入设置模式：
	打开上盖，按住feed键上电，
	错误灯闪烁（0.5S亮1s灭的频率一直闪烁并记录闪烁次数），数到闪烁2次后，
	松开feed键，
	再按feed键2次，
	关闭上盖则进入设置模式； 
	设置模式下，长按feed键来切换设置项目并打印出来，短按feed键来循环选择项目下的具体参数值，并打印出来；
	打开上盖，按住feed键，合上上盖后再松开feed键，则保存设置的参数值并退出设置模式；
	长按为feed键按下2s后没有松开则打印切换的设置项目；
	短按为feed键按下1S后判定按键已松开；

	进入升级模式：
	打开上盖，按住feed键上电，
	错误灯闪烁（0.5S亮1s灭的频率一直闪烁并记录闪烁次数），数到闪烁4次后，
	松开feed键（松开feed键错误和缺纸灯熄灭），
	再按feed键4次，
	关闭上盖则进入升级模式；
	升级模式下错误和缺纸灯交替闪烁；

	打印机参数初始化模式：
	打开上盖，按住feed键上电，
	错误灯闪烁（0.5S亮1s灭的频率一直闪烁并记录闪烁次数），数到闪烁5次后，
	松开feed键（松开feed键错误和缺纸灯熄灭），
	再按feed键5次，
	关闭上盖则打印机参数恢复默认值； 

	快速进入升级模式：
	打开上盖，按住feed键上电，
	打开电源后快速松开，
	缺纸灯亮后按4次，
	合上盖后升级模式，
	升级模式下错误和缺纸灯交替闪烁；

	擦除程序模式：
	打开上盖，按住feed键上电，
	打开电源后快速松开，
	缺纸灯亮后按8次后，
	合上盖则擦除程序；
* Input          : 无
* Output         : 输出参数，模式种类
* Return         : 返回参数，
*******************************************************************************/
#define		ERROR_LED_OFF_MS						(500)
#define		ERROR_LED_ON_MS							(500)

#define		FEED_KEY_RAPID_OPEN_MS			(200)	//200ms之内松开，算是快速松开

#define		HEX_MODE_ERROR_LED_FLASH_COUNT										(1)
#define		SET_PARAS_MODE_ERROR_LED_FLASH_COUNT							(2)
#define		PRT_TEST_LED_FLASH_COUNT													(3)			//打印测试模式
#define		NORMAL_UPDATE_MODE_ERROR_LED_FLASH_COUNT					(4)
#define		RESTORE_DEFAULT_PARAS_MODE_ERROR_LED_FLASH_COUNT	(5)
#define		EnterUpGrade	                                    (6)     //增加进入升级模式 2017.05.23

#define		SET_PARAS_MODE_FEED_KEY_COUNT											(2)
#define		PRT_TEST_FEED_KEY_COUNT														(3)
#define		NORMAL_UPDATE_MODE_FEED_KEY_COUNT									(4)
#define		RESTORE_DEFAULT_PARAS_MODE_FEED_KEY_COUNT					(5)

#define		HEX_MODE_FEED_KEY_COUNT														(0)
#define		DIRECT_UPDATE_MODE_FEED_KEY_COUNT									(4)
#define		ERASE_APP_MODE_FEED_KEY_COUNT											(8)

uint8_t	GetModeType(void)
{
	uint8_t		PlatenStatus;					//纸仓盖状态
	uint8_t		FeedKeyStatus;				//按键状态
	uint8_t		ModeType;							//模式
	uint8_t		ErrorLedFlashCount;		//错误指示灯闪烁次数
	uint32_t	TimeCount;						//ms时间计数
	
	PlatenStatus	 = ReadPlatenStatus();
	FeedKeyStatus	 = ReadFeedButtonStatus();
	
	//纸仓盖关闭，按键松开
	if ((PlatenStatus == PLATEN_STATUS_CLOSE) && \
			(FeedKeyStatus == FEED_BUTTON_STATUS_OPEN))
	{
		ModeType = MODE_NORMAL;					//正常模式
	}
	//纸仓盖关闭，按键按下
	else if ((PlatenStatus == PLATEN_STATUS_CLOSE) && \
					 (FeedKeyStatus == FEED_BUTTON_STATUS_CLOSE))
	{
		ModeType = MODE_SELFTEST_PRT;		//自检模式
	}
	//纸仓盖打开，按键按下，这种最复杂！
	else if ((PlatenStatus == PLATEN_STATUS_OPEN) && 
					(FeedKeyStatus == FEED_BUTTON_STATUS_CLOSE))
	{
		//快速松开的两种
		for (TimeCount = 0; TimeCount < 20; TimeCount++)
		{
			DelayMs(10);				//延时再检查上纸键
			if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_OPEN)
			{
				break;
			}
		}
		
		if (TimeCount < 20)		//200ms之内松开上纸键,进入快速升级模式及擦除程序
		{
//			PaperEndLedOn();								//缺纸灯亮
//			InitKeyCtrlStructParameter();		//先清空按键中断全局变量值
//			while (PLATEN_STATUS_OPEN == ReadPlatenStatus());	//等待合上纸仓盖
//			
//			if (GetFeedButtonCount() == DIRECT_UPDATE_MODE_FEED_KEY_COUNT)
//			{	//缺纸灯亮后再按FEED键4次，并合上纸仓盖，进入快速升级模式
//				ModeType = MODE_DIRECT_UPDATE;	//快速升级模式
//			}
//			else if (GetFeedButtonCount() == ERASE_APP_MODE_FEED_KEY_COUNT)
//			{	//缺纸灯亮后再按FEED键8次，并合上纸仓盖，进入擦除程序模式
//				ModeType = MODE_ERASE_APP;			//擦除程序模式
//			}
//			else
//			{
//				ModeType = MODE_NORMAL;				//以上两种都不是，则进入默认模式
//			}
//			
//			PaperEndLedOff();								//缺纸灯关闭
		}			//快速松开的两种处理完毕
		else	//以下为长久按下的四种
		{
			//持续按下上纸键，统计错误灯闪烁次数
			ErrorLedFlashCount = 0;
			while (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
			{	
				ErrorLedFlashCount++;
				ErrorLedOn();
				DelayMs(ERROR_LED_ON_MS);
				ErrorLedOff();
				DelayMs(ERROR_LED_OFF_MS);     

				if(ErrorLedFlashCount == EnterUpGrade)   //2017.05.23  err灯闪6次进入升级模式
				{
					ClearComInBufBusy();
					Update();
					return;
				}	
			}
			
			//再统计合上纸仓盖前上纸键按下的次数
			InitKeyCtrlStructParameter();		//先清空按键中断全局变量值
			while (PLATEN_STATUS_OPEN == ReadPlatenStatus())	//等待合上纸仓盖
			{
				if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
				{
					PaperEndLedOn();						//缺纸灯亮
				}
				else
				{
					PaperEndLedOff();						//缺纸灯关闭
				}
			}
			
			//根据错误灯闪烁次数和合上纸仓盖前上纸键按下的次数判断模式
			if ((ErrorLedFlashCount   == HEX_MODE_ERROR_LED_FLASH_COUNT) && \
					(GetFeedButtonCount() == HEX_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_HEX_PRT;		//十六进制打印
			}
			else if ((ErrorLedFlashCount   == SET_PARAS_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == SET_PARAS_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_SET_PARAS;	//设置参数模式
			}
			else if ((ErrorLedFlashCount   == NORMAL_UPDATE_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == NORMAL_UPDATE_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_NORMAL_UPDATE;	//升级程序模式
			}
			else if ((ErrorLedFlashCount   == PRT_TEST_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == PRT_TEST_FEED_KEY_COUNT))
			{
				ModeType = MODE_PRINT_TEST;	        //打印测试模式
			}
			else if ((ErrorLedFlashCount   == RESTORE_DEFAULT_PARAS_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == RESTORE_DEFAULT_PARAS_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_RESTORE_DEFAULT_PARAS;	//恢复默认参数模式
			} 
			else if ((ErrorLedFlashCount   == RESTORE_DEFAULT_PARAS_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == PRT_TEST_FEED_KEY_COUNT))		//闪5次按三次恢复默认网络参数
			{
				ModeType = MODE_RESTORE_NET_PARAS;	//恢复默认参数模式 恢复默认网络参数 
			}  
			else
			{
				ModeType = MODE_NORMAL;		//以上四种都不是，则进入默认模式
			}
		}			//长久按下的四种处理完毕
	}		//纸仓盖打开，按键按下处理完毕
	//纸仓盖打开，按键松开
	else
	{
		ModeType = MODE_NORMAL;					//正常模式
	}
	
	return	(ModeType);	
}

/*******************************************************************************
* Function Name  : PrtTestMode
* Description    : 打印测试模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrtTestMode(void)
{
  uint32_t i,j,k;
  uint8_t Temp;
	
	if(g_tSysConfig.PrintMode != 0)	
	{		 	
		  GoDotLine(24);													//先走一行防止合盖后压缩
			if(g_tSysConfig.PrintMode == 1)   			//2分钟间隔 打印测试
      {    
          i = 1;					     //间隔时间 2017.10.09 1*60改为1
				  j = 1;							 //打印测试次数
				  PrintString("Test PrintMode\r\n" );	
      } 
      if(g_tSysConfig.PrintMode == 2)   			//5分钟间隔 打印测试
      {
          i =50*60;					 //间隔时间
          j= 1;						   //打印测试次数 
          PrintString("5 Minutes PrintMode\r\n" );		 
      } 
      if(g_tSysConfig.PrintMode == 3)   		//20分钟间隔 打印测试
      {
          i =200*60;					 //间隔时间
          j= 1;						   //打印测试次数
          PrintString("20 Minutes PrintMode\r\n" );	
      } 
      if(g_tSysConfig.PrintMode == 4)  			//50Km
      {
          i =100;					 //间隔时间
          j= 6;						 //打印测试次数
          PrintString( "50 Kilometres PrintMode\r\n" );				 
      }  
      if(g_tSysConfig.PrintMode == 5)				//切刀测试
      {	
          PrintString( "CutPaper TestMode\r\n" );
          while(1)
          {
							uint8_t Buff[16];							
              if(R_Counter.CutCounter%10 ==0)
              {
									LongToStr(R_Counter.CutCounter, Buff);
                  PrintString( "打印机已切纸次数：" );
                  PrintString( Buff );
                  PrintString( "次\r\n" );
              }
              GoDotLine((u32)(18/MIN_STEP_UNIT_Y) );		//走18mm
              DriveCutPaper(0);
              DelayMs(2*1000);						//每分钟30次 
         }
      }			
      while(1)
      {
					uint8_t Buff[16];
          if(g_tSysConfig.PrintMode == 4)  	//50Km 测试
          {   				 	
							LongToStr((u32)(R_Counter.LineCounter /(1000 /MIN_STEP_UNIT_Y)), Buff);	 //计数器单位为点行，转换为米
              PrintString( "打印机已走纸：" );
              PrintString( Buff );
              PrintString( "米\r\n" );
          }
          if(g_tSysConfig.PrintMode == 2)  //TEST_20MINUTE
          { 
              PrintString( "打印机已运行时间：" );
              LongToStr(R_Counter.TimeCounter/60, Buff);	 //小时
              PrintString( Buff );
              PrintString( "小时" );
              LongToStr(R_Counter.TimeCounter%60, Buff);	 //分钟
              PrintString( Buff );
              PrintString( "分钟\r\n" );
          }				
          for(k=j; k>0; k--) 						        //打印k个自检条
					{
              //SystemSelfTest();
						  //2017.03.20  测试模式打印自检更改为打印定制的内容
						  PrtTestFun();
						  OpenDrawer();
          }   
          DelayMs( i *100);                      //2017.10.09 1000改为100
			}
	}
}

/*******************************************************************************
* Function Name  : PutStrToInbuf
* Description    : 把一行内容送入收数缓冲区
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PutStrToInbuf( const uint8_t *Str )
{    
	uint8_t	Char;
    
	while(*Str)
	{
        Char = *Str;
        PutCharToInBuffer(Char);	//存入数据
        Str++;
	}
}

static void OpenDrawer(void)
{

	uint16_t OpenTime;
		g_tDrawer1.Status = ENABLE;				  //为了防止在进行钱箱开启时间内 防止实时指令101401n 的影响
		while (g_tDrawer1.ON_Time);					//等待上次执行完成
		
		g_tDrawer1.DrwID = 0 & 0x01;
		OpenTime		= (uint16_t)(((uint16_t)(32)) << 1);
		g_tDrawer1.OFF_Time	= (uint16_t)(((uint16_t)(32)) << 1);
		if (g_tDrawer1.OFF_Time < OpenTime)
		{
			g_tDrawer1.OFF_Time = OpenTime;
		}
		OpenTime += g_tDrawer1.OFF_Time;	//总控制时间
		if(OpenTime)
		{	
			DriveDrawer(g_tDrawer1.DrwID, ENABLE);			//开钱箱
		}	
		g_tDrawer1.ON_Time = OpenTime;
		g_tDrawer1.Status = DISABLE;								//在定时器中实现定时和关闭钱箱
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
