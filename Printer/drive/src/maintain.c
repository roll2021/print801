/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-30
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

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"maintain.h"
#include	"spiflash.h"
#include	"extgvar.h"

#ifdef	MAINTAIN_ENABLE

extern	uint8_t	GetPrintBusyStatus(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static	uint8_t	m_bStoreIndex = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
/*******************************************************************************
* Function Name  : 函数名
* Description    : 得到合法存储参数的当前序号的下一个序号,从0开始,到127结束.
	统计次数的值保存到字库FLASH中.算法如下:
	扇区为4K,一次需要存储总字节数为32,所以最多可以存储128次的结果.
	每个int值按照四字节保存,保存在首地址开始的4K字节的一个扇区内容.
	从首地址开始依次保存值的最低0~8位,高8~16位,高16~24位,高24~32位.
	初始化时根据首地址和最后地址的四个字节值是否全为0xFF,判断该扇区是否需要擦除.
	保存时从首地址开始,依次保存.直到最后四个字节也被使用时,重新擦除,再从头保存.

	从首地址开始逐个读取32个字节,直到遇到32个字节全为FF的地址结束.
	返回值为合法数据的有效起始编号,
	如果起始到最后均为FF,则返回0.
	如果起始32个字节均为0xFF,返回0;
	如果最后32个字节不都是FF,则返回128.
	如果是其他位置,返回该数存储开始地址所在编号的下一个编号.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
uint8_t	GetMaintainParaStoreIndex(void)
{

}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 读回指定存储序号处的保存值,int存储时以小端方式
	赋值给两个维护变量结构体.
	当存储序号值大于0时需要先递减1个.因为该变量意味着全为FF的序列号.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/


/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化维护变量值及存储序列号,赋值给两个维护变量结构体和本地变量.
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitMaintainParameter(void)
{
	m_bStoreIndex = GetMaintainParaStoreIndex();
}

/*******************************************************************************
* Function Name  : void	ReadFlashParameter(void)
* Description    : 从字库Flash中读维护计数器参数,
	扇区为4K,一次需要存储总字节数为32,所以最多可以存储128次的结果.
	处理的算法与可靠性测试参数存储算法相同.
	同于初始化部分的操作.
* Input          : 无
* Output         : 如果有有效数据,则将参数读入维护计数器变量中
* Return         : 
*******************************************************************************/
void ReadFlashParameter(void)
{
    uint32_t *p, *p1;
	uint8_t i;

	WriteFlashFlag =1;
	p= (u32*)TIMECOUNTADDR;

	//查找没有写入参数的地址
	while( (p <(u32*)(TIMECOUNTADDR +PAGE_SIZE)) && (*p != 0xffffffff))	//修正BUG，先比较地址，再比较其值	
	{
		p +=8;
	}

	//如果芯片是第一次运行，则初始化各计数器
	if(p == (u32*)TIMECOUNTADDR)
	{
		R_Counter.LineCounter=0;
		R_Counter.HeatCounter=0;
		R_Counter.CutCounter =0;
		R_Counter.TimeCounter=0;
		C_Counter.LineCounter=0;
		C_Counter.HeatCounter=0;
		C_Counter.CutCounter =0;
		C_Counter.TimeCounter=0;

	}
	else
	{
		p1 = p;
		
		//校验上次写入的数据是否完整有效
		while(p != (u32*)TIMECOUNTADDR)					
		{
			p -=8;
			for(i=0; i<8; i++)
			{
				if( *(p+i) ==0xffffffff)	   	//数据是否有效 
					break;
			}
			if(i == 8)							//数据有效
				break;
		}
		
		R_Counter.LineCounter =*p++;
		R_Counter.HeatCounter =*p++;
		R_Counter.CutCounter  =*p++;
		R_Counter.TimeCounter =*p++;
		C_Counter.LineCounter =*p++;
		C_Counter.HeatCounter =*p++;
		C_Counter.CutCounter  =*p++;
		C_Counter.TimeCounter =*p++;

		//如果上次写入有错或到了页尾则擦除页
		if(p !=	p1 || p >= (u32*)(TIMECOUNTADDR +PAGE_SIZE)) 	
		{								//
			FLASH_Unlock();
			FLASH_ErasePage(TIMECOUNTADDR);
			FLASH_Lock();
			WriteFlashParameter(2);		//将当前内容保存，防止掉电写入时错误
		}
	}	
	R_CounterBK = R_Counter;
	WriteFlashFlag =0;
}

/*******************************************************************************
* Function Name  : void	WriteFlashParameter(uint8_t Flag)
* Description    : 写字库Flash中的维护计数器
* Input          : Flag：0正常定时写,1关机（低压）时写,2指令写
* Output         : 
* Return         : None
*******************************************************************************/
void	WriteFlashParameter(uint8_t Flag)
{
	uint8_t	cTemp1;
	uint8_t	cTemp2;
	uint8_t	cTemp3;
	
	union 				
	{
		Maintenance_Counter	Counter[2];
		u32 Buff[8];
	} ui;

    uint32_t i, j, Addr;
	
	//先关闭实时指令执行,关闭ASB回传,写入完毕再开启
	cTemp1 = g_tRealTimeCmd.RealTimeEnable;
	g_tRealTimeCmd.RealTimeEnable = 0;
	cTemp2 = g_tError.ASBAllowFlag;
	g_tError.ASBAllowFlag = 0;
	cTemp3 = g_tFeedButton.KeyEnable;
	g_tFeedButton.KeyEnable = 1;
	


	//每30分钟写一次或低压时
	if((R_Counter.TimeCounter > R_CounterBK.TimeCounter + 30 ) || Flag)	
	{
	
		WriteFlashFlag =1;

	 	Addr = TIMECOUNTADDR;
		
		//查找没有写入过参数的地址
		while( Addr <(TIMECOUNTADDR +PAGE_SIZE) && (*((u32*)Addr) != 0xffffffff))	//修正BUG，先比较地址，再比较其值						
		{
			Addr +=32;
		}
		
		ui.Counter[0]=  R_Counter;		//可修改的
		ui.Counter[1]=  C_Counter;
	
		FLASH_Unlock();

		//如果是定时或指令写，检查是否是页的最后一次，如是则擦除页
		//为了保证写入时间，掉电写入时不会进行擦除操作，是每次写入后，保证至少还能写入一次
		if(Flag !=1 && (Addr+32) == (TIMECOUNTADDR +PAGE_SIZE))	
		{
			FLASH_ErasePage(TIMECOUNTADDR);
			Addr = TIMECOUNTADDR;
		}

		//写入校验，如果重复写入3次还是不正确则设置错误，
		for(j=0; j<3; j++)		
		{
			for(i=0; i<8; i++)
			{
				FLASH_ProgramWord(Addr, ui.Buff[i]);
				if(( *((u32*)Addr) != ui.Buff[i]))
					break;
				Addr +=4;
			}
			if(Flag ==1 || i ==8 )		//掉电写入时不校验是否正确，由上电时校验
				break;
			else
			{
				FLASH_ErasePage(TIMECOUNTADDR);
				Addr = TIMECOUNTADDR;
			}
		}
		FLASH_Lock();
		
		//错误报警
		if(j==3)
    {    
			g_tError.UR_ErrorFlag |= 0x20;		//bit5 CPU中Flash写入错误
    }    
		else
    {    
			g_tError.UR_ErrorFlag &= ~0x20;
		}	
		R_CounterBK = R_Counter;
		WriteFlashFlag =0;
	}
	
	//先关闭实时指令执行,关闭ASB回传,写入完毕再开启
	g_tRealTimeCmd.RealTimeEnable = cTemp1;
	g_tError.ASBAllowFlag = cTemp2;
	g_tFeedButton.KeyEnable = cTemp3;
	
}

void	CheckMaintainRoutin(void)
{
	if (g_bMaintainRountinWriteFlag)
	{
		if (GetPrintBusyStatus() == PRINTER_STATUS_ILDE)
		{
			WriteFlashParameter(0);
			g_bMaintainRountinWriteFlag = 0;
		}
	}
}

#endif	/*允许支持维护计数器宏定义结束*/

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
