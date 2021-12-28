/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.0.1
  * @date    2012-12-24
  * @brief   小批试制可靠性和寿命测试功能的程序
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
#include	"reliable.h"
#include	"stm32f10x.h"
#include	"apptype.h"
#include	"mode.h"
#include	"normal.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"

extern	void LongToStr(uint32_t num, uint8_t *Str);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
uint8_t	gcReliableRoutineFlag;				//标志是否可以开始一次可靠性测试
uint32_t	giReliableTimerCount;					//可靠性测试1ms延时次数

static	uint16_t	m_ReliableStoreIndex;	//表示当前可靠性测试次数值存储起始地址的下一个编号
static	uint32_t	m_iReliableTestNumber;	//已执行的可靠性测试的次数值
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE)
/*******************************************************************************
* Function Name  : 函数名
* Description    : 
	2分钟,5分钟,20分钟的每个延时时间打印一次自检,统计已打印次数并打印出来,
	到达存储次数则保存到字库中.每隔2,5,20分钟打印一次自检.并打印出总的统计时间,
	打印次数.将数据保存到外部FLASH中.使用一个定时器来计时,在主程序中进行定时器
	的初始化和启动,在定时器中判断是否到达设定时间,如果到达则设置标志,主程序中检查
	标志,调用可靠性程序生成可靠性打印及自检内容,统计次数,保存次数到字库FLASH.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static	void	ReliabilityTestMinute(void)
{
	uint32_t	iTemp;
	
	PrintReliabilityTitle();		//条件编译,可靠性测试时才打印
	
	#if !defined	DBG_QUICK_TEST_ENABLE
		SystemSelfTest();					//执行1次自检
	#endif
	//WaitPrintAndFeedPaperFinish();
	m_iReliableTestNumber++;		//已执行的可靠性测试的次数值
	
	#if defined	(TEST_2MINUTE)	//打印可靠性自检次数信息
		PrintString("2");
	#elif	defined	(TEST_5MINUTE)
		PrintString("5");
	#elif	defined	(TEST_20MINUTE)
		PrintString("20");
	#endif
	PrintString("分钟循环打印自检可靠性测试程序：");
	PrintString("\n");
	
	PrintReliabitityResultTotalNumber();		//打印累计测试次数
	PrintReliabitityResultTotalTime();			//打印累计测试时间
	
	iTemp = m_iReliableTestNumber * (ONE_SELFTEST_LEN + ADDITIONAL_TITLE_LEN);
	PrintReliabitityResultTotalLen(iTemp);	//打印累计测试长度
	
	WaitPrintAndFeedPaperFinish();
	ReliableTestSaveParameter();	//以下为处理存储次数值到字库
}
#endif	//2,5,20分钟可靠性测试处理程序结束


#ifdef	TEST_50KM
/*******************************************************************************
* Function Name  : 函数名
* Description    : 
	每隔一个设定延时时间打印规定次数的自检,统计已打印次数并打印出来,到达存储次数则
	保存到字库中.打印出总的统计时间,及换算得到的走纸长度.
	将数据保存到外部FLASH中.使用一个定时器来计时,在主程序中进行定时器的初始化和启动,
	在定时器中判断是否到达设定时间,如果到达则设置标志,主程序中检查标志,调用可靠性
	程序生成可靠性打印及自检内容,统计次数,保存次数到字库FLASH.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static	void	ReliabilityTestKm50(void)
{
	uint32_t	iTemp;
	
	PrintReliabilityTitle();	//条件编译,可靠性测试时才打印
	for (iTemp = 0; iTemp < RELIABLE_TEST_50KM_SELFTEST_CYCLE; iTemp++)
	{
	#if !defined	DBG_QUICK_TEST_ENABLE
		SystemSelfTest();				//执行若干次自检
	#endif
	}
	//WaitPrintAndFeedPaperFinish();
	m_iReliableTestNumber++;		//已执行的可靠性测试的次数值
	
	PrintString("50Km可靠性测试程序：");
	PrintString("\n");	
	PrintReliabitityResultTotalNumber();		//打印累计测试次数
	PrintReliabitityResultTotalTime();			//打印累计测试时间
		
	iTemp = m_iReliableTestNumber * \
		(RELIABLE_TEST_50KM_SELFTEST_CYCLE * ONE_SELFTEST_LEN + ADDITIONAL_TITLE_LEN);
	PrintReliabitityResultTotalLen(iTemp);	//打印累计测试长度
	
	WaitPrintAndFeedPaperFinish();
	ReliableTestSaveParameter();	//以下为处理存储次数值到字库
}
#endif	//50KM可靠性测试处理程序结束


#ifdef	TEST_CUTTER_LIFE
/*******************************************************************************
* Function Name  : 函数名
* Description    : 
	每隔一个设定延时时间上纸并切纸,统计已切纸次数并打印出来,到达存储次数则保存到字
	库中.打印出总的统计次数,及换算得到的走纸长度.
	将数据保存到外部FLASH中.使用一个定时器来计时,在主程序中进行定时器的初始化和启动,
	在定时器中判断是否到达设定时间,如果到达则设置标志,主程序中检查标志,调用可靠性
	程序生成可靠性打印及自检内容,统计次数,保存次数到字库FLASH.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static	void	ReliabilityTestCutterLife(void)
{
	uint32_t	iTemp;
	
	#if !defined	DBG_QUICK_TEST_ENABLE
		GoDotLine(RELIABLE_TEST_FEED_PAPER_DOTLINE);	//先上纸
		DriveCutPaper(CUT_TYPE_DEFAULT);							//再切纸
		while ((g_tError.R_ErrorFlag & 0x01));			//如果有错误
	#endif
	//WaitPrintAndFeedPaperFinish();
	m_iReliableTestNumber++;			//如果没有错误,已执行的可靠性测试的次数值
	
	if (!(m_iReliableTestNumber % RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM))	//达到预定次数
	{
		PrintReliabilityTitle();	//条件编译,可靠性测试时才打印
		
		PrintReliabitityResultTotalNumber();		//打印累计测试次数
		PrintReliabitityResultTotalTime();			//打印累计测试时间
		
		//计算走纸长度,以下为纯上纸和每次切纸后自动上纸距离值
		iTemp = m_iReliableTestNumber * (RELIABLE_TEST_FEED_PAPER_LEN + FEED_PAPER_MM_AFTER_CUT);
		//再加上每隔固定存储次数时打印的内容的长度
		iTemp += ((m_iReliableTestNumber / RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM) * ADDITIONAL_TITLE_LEN);
		PrintReliabitityResultTotalLen(iTemp);	//打印累计测试长度
		
		WaitPrintAndFeedPaperFinish();
		ReliableTestSaveParameter();	//以下为处理存储次数值到字库
	}
}
#endif	//切刀可靠性测试处理程序结束


#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
//可靠性测试时才打印
static	void	PrintReliabilityTitle(void)
{
#ifdef TEST_2MINUTE						//2分钟打印一次
	PrintString("2 Minutes Test\n");	
#endif

#ifdef TEST_5MINUTE						//5分钟打印一次
	PrintString("5 Minutes Test\n");	
#endif

#ifdef TEST_20MINUTE					//20分钟打印一次
	PrintString("20 Minutes Test\n");
#endif

#ifdef TEST_50KM							//50公里测试
	PrintString("50Km Test\n");
#endif

#ifdef TEST_CUTTER_LIFE			//切刀测试
	PrintString("Cutter Life Test\n");
#endif
}

static	void	PrintReliabitityResultTotalNumber(void)
{
	uint8_t	bBuf[16];
	
	//打印次数转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	LongToStr(m_iReliableTestNumber, bBuf);
	PrintString("累计测试次数：");
	PrintString(bBuf);					//打印次数
	PrintString("  \n");
}

static	void	PrintReliabitityResultTotalTime(void)
{
	uint8_t	cHour;
	uint8_t	cMinute;
	uint8_t	bBuf[16];
	uint16_t	iDay;
	uint32_t	iTime;
	
	iTime = m_iReliableTestNumber * RELIABLE_TEST_INTERVAL_S;
	
	iDay = iTime / SECOND_PER_DAY;					//天数
	iTime -= (iDay * SECOND_PER_DAY);
	cHour = iTime / SECOND_PER_HOUR;				//小时数
	iTime -= (cHour * SECOND_PER_HOUR);
	cMinute = iTime / SECOND_PER_MINUTE;		//分钟数
	iTime -= (cMinute * SECOND_PER_MINUTE);	//剩余秒数
	PrintString("累计测试时间：");
	LongToStr(iDay, bBuf);			//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);					//打印天数
	PrintString(" 天 ");
	LongToStr(cHour, bBuf);			//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);					//打印小时数
	PrintString(" 小时 ");
	LongToStr(cMinute, bBuf);		//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);					//打印分钟数
	PrintString(" 分钟 ");
	LongToStr(iTime, bBuf);			//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);					//打印秒数
	PrintString(" 秒 ");
	PrintString("\n");
}

static	void	PrintReliabitityResultTotalLen(uint32_t	iMicrometer)
{
	uint8_t	cKilometer;
	uint8_t	cCentimeter;	
	uint8_t	bBuf[16];
	uint16_t	Meter;
	
	cKilometer = iMicrometer / MM_PER_KM;			//Km
	iMicrometer -= (cKilometer * MM_PER_KM);
	Meter = iMicrometer / MM_PER_M;						//M
	iMicrometer -= (Meter * MM_PER_M);
	cCentimeter = iMicrometer / MM_PER_CM;		//Cm
	iMicrometer -= (cCentimeter * MM_PER_CM);	//剩余mm
	PrintString("累计测试长度：");
	LongToStr(cKilometer, bBuf);	//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);						//打印Km
	PrintString(" 公里 ");
	LongToStr(Meter, bBuf);				//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);						//打印M
	PrintString(" 米 ");
	LongToStr(cCentimeter, bBuf);	//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置	
	PrintString(bBuf);						//打印Cm
	PrintString(" 厘米 ");
	LongToStr(iMicrometer, bBuf);	//转换为各位数的ASCII码,按照bBuf[0]存储数据最高开始放置
	PrintString(bBuf);						//打印mm
	PrintString(" 毫米 ");
	PrintString("\n");
}

static	void	ReliableTestSaveParameter(void)
{
	uint8_t	bBuf[16];
	uint32_t	iCount;
	uint8_t	* pbBuf;
	
	if (!(m_iReliableTestNumber % RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM))
	{
		ReliableStoreWarnOn();	//存储操作开始
		
		pbBuf = bBuf;						//按照小端存储分拆
		for (iCount = 0; iCount < sizeof(int); iCount++)
		{
			*pbBuf = (uint8_t)(m_iReliableTestNumber >> (iCount * 8));
			pbBuf++;
		}
		
		//当最开始和超过存储区时先擦除,然后直接从原始地址0存储,否则先直接存储再递增地址
		if ((m_ReliableStoreIndex == 0x00) || \
				(m_ReliableStoreIndex >= (sFLASH_SPI_SECTORSIZE / sizeof(int))))
		{
			sFLASH_EraseSector(RELIABLE_TEST_PARA_BASE_ADDRESS);	//重新擦除扇区,从头开始使用
			m_ReliableStoreIndex = 0;
		}		//如果不是首尾地址则可以直接存储
		iCount = RELIABLE_TEST_PARA_BASE_ADDRESS + m_ReliableStoreIndex * sizeof(int);
		pbBuf = bBuf;
		sFLASH_WriteBuffer(pbBuf, iCount, sizeof(int));	//存入存储区
		m_ReliableStoreIndex++;	//指向下个存储起始地址
		ReliableStoreWarnOff();	//存储操作结束
	}	//是否到达存储次数
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 
	统计次数的值保存到字库FLASH中.算法如下:
	值按照四字节保存,保存在首地址开始的4K字节的一个扇区内容.
	从首地址开始依次保存值的最低0~8位,高8~16位,高16~24位,高24~32位.
	初始化时根据首地址和最后地址的四个字节值是否全为0xFF,判断该扇区是否需要擦除.
	保存时从首地址开始,依次保存.直到最后四个字节也被使用时,重新擦除,再从头保存.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
//得到合法存储参数的当前序号的下一个序号,从0开始,到1024结束.
//从首地址开始逐个读取,直到遇到全为FF的地址结束.
//返回值为合法数据的有效起始编号的下一个编号,每4个字节存储1个值
//如果起始到最后均为FF,则返回0.
//如果起始为0xFF,返回0;
//如果最后四个字节不都是FF,则返回1024.
//如果是其他位置,返回该数存储开始地址所在编号的下一个编号.
static	uint32_t	GetReliableTestParaStoreIndex(void)
{
	uint8_t	cTemp;
	uint8_t	bBuf[sizeof(int)];
	uint32_t	iAddress;
	uint32_t	iIndex;
	uint32_t	iCount;
	uint8_t	* pBuf = NULL;
	
	iAddress = RELIABLE_TEST_PARA_BASE_ADDRESS;
	for (iIndex = 0; iIndex < (sFLASH_SPI_SECTORSIZE / sizeof(int)); iIndex++)
	{
		pBuf = bBuf;
		sFLASH_ReadBuffer(pBuf, iAddress, sizeof(int));	//读取1个int的存储值
		iAddress += sizeof(int);
		cTemp = 0xFF;
		for (iCount = 0; iCount < sizeof(int); iCount++)
		{
			cTemp &= *pBuf++;		//得到读回值的四个字节的值相与结果
		}
		if (cTemp == 0xFF)		//到达最末有效存储地址的一个存储区
		{
			pBuf = NULL;
			return (iIndex);
		}
	}
	pBuf = NULL;
	
	return (iIndex);
}

//读回指定存储序号的值,int存储时以小端方式
static	uint32_t	GetReliableTestStoreValue(uint32_t iIndex)
{
	uint8_t	bBuf[sizeof(int)];
	uint32_t	iAddress;
	uint32_t	iNumber;
	uint32_t	iCount;
	uint8_t	* pBuf = NULL;
	
	if (iIndex > (sFLASH_SPI_SECTORSIZE / sizeof(int)))	//最后的存储区
	{
		iIndex = (sFLASH_SPI_SECTORSIZE / sizeof(int));		//合法参数范围应该为[0~1024]
	}
	if (iIndex)
		iIndex--;
	iAddress = RELIABLE_TEST_PARA_BASE_ADDRESS + iIndex * sizeof(int);
	pBuf = bBuf;
	sFLASH_ReadBuffer(pBuf, iAddress, sizeof(int));
	for (iNumber = 0, iCount = 0; iCount < sizeof(int); iCount++)
	{
		iNumber |= ((uint32_t)(*pBuf) << (iCount * 8));
		pBuf++;
	}
	if (iNumber == 0xFFFFFFFF)	//擦除后全部为高
	{
		iNumber = 0;
	}
	pBuf = NULL;
	
	return (iNumber);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 可靠性打印的初始化,初始化定时器和标记变量
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static	void InitReliableTest(void)
{
	giReliableTimerCount = 0;
	gcReliableRoutineFlag = 0;
	
	//读取参数保存的序号值
	m_ReliableStoreIndex = GetReliableTestParaStoreIndex();
	//读取保存到FLASH中的已处理的次数
	m_iReliableTestNumber = GetReliableTestStoreValue(m_ReliableStoreIndex);
	
 	SetTimerCounterWithAutoReload(BACKUP_TIMER, 10*1000);	//定时器定时时间为1ms*1000 = 1s
	StartTimer(BACKUP_TIMER);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 根据测试内容不同生成不同的打印内容,并处理打印.
	
	使用BACKUP_TIMER.
	其他正常功能全部具备.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
static	void ReliableTest(void)
{
#if defined	(TEST_2MINUTE) || defined	(TEST_5MINUTE) || defined	(TEST_20MINUTE)
	ReliabilityTestMinute();
#elif defined	(TEST_50KM)
	ReliabilityTestKm50();
#elif defined	(TEST_CUTTER_LIFE)
	ReliabilityTestCutterLife();
#endif
}

#endif	//全部可靠性测试处理程序结束 


#ifdef	VERSION_RELIABILITY
/*******************************************************************************
* Function Name  : 函数名
* Description    : 可靠性测试的程序.包括2分钟,20分钟,
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
int main(void)
{
	Sys_Init();		//初始化时钟,GPIO端口,全局变量,外部设备
	
#ifdef WITHBOOT
	CheckBootLegality();	//检验BOOTLOAD程序合法性
#endif
	
	ProcessModeType();
	ClearComInBufBusy();

#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
	InitReliableTest();
#endif
	
	while (1)
	{
#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
		if (gcReliableRoutineFlag)
		{
			gcReliableRoutineFlag = 0;
			ReliableTest();
		}
#endif
		CheckFeedButton();						//进行检查按键等工作
		CheckMaintainRoutin();				//检查周期性维护参数存储
		ProcessCharMaskReadBuf();			//处理字符点阵读取缓冲区
	}
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
