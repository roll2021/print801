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
#include  "stm32f10x_conf.h"
#include	"update.h"
#include	"led.h"
#include	"cominbuf.h"
#include	"extgvar.h"
extern uint32_t __Vectors_End, __Vectors;  
extern TypeDef_StructInBuffer volatile g_tInBuf;

/* Private typedef -----------------------------------------------------------*/
typedef uint32_t (*pFunction)(uint8_t * Str, uint32_t Length);

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define BKP_DR_NUMBER 			10

/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t  JumpAddress;
uint16_t BKPDataReg[BKP_DR_NUMBER] = 
{
	BKP_DR1, BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5,
	BKP_DR6, BKP_DR7, BKP_DR8, BKP_DR9, BKP_DR10
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
/**
  * @brief  Writes data Backup DRx registers.
  * @param FirstBackupData: data to be written to Backup data registers.
  * @retval : None
  */
void	WriteToBackupReg(uint16_t FirstBackupData)
{
  uint32_t index;
	
  for (index = 0; index < BKP_DR_NUMBER; index++)
  {
    BKP_WriteBackupRegister(BKPDataReg[index], FirstBackupData + (index * 0x5A));
  }
}

uint8_t	CheckBackupReg(uint16_t FirstBackupData)
{
  uint8_t 	Str[10];
  uint16_t	Temp;
	uint32_t	index;
	
  for (index = 0; index < 5; index++)
  {
    Temp = (BKP_ReadBackupRegister(BKPDataReg[index]));
    Str[(index << 1)]			= Temp >> 8;
    Str[(index << 1) + 1]	= Temp & 0xFF;
  }
  if (memcmp(Str, "UPDATE_APP", 10) == 0)
  {
		return ((uint8_t)BKP_ReadBackupRegister(BKPDataReg[(index + 1)]));   // <<改为+1防止越界
  }
  else
  {
	  return (0xFF);
  }
}

/*******************************************************************************
* Function Name  : System_Upgrade
* Description    : 进入系统升级方式
* Input          : Interface:=0,=1,=2,=3
* Output         : None
* Return         : None
*******************************************************************************/
void System_Upgrade(uint8_t Interface)
{
	uint32_t	i;
	uc8	* Str = "UPDATE_APP";
	uint16_t	Temp;
	
  /* Enable PWR and BKP clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);
  for (i = 0; i < 5; i++)
  {
  	Temp = Str[(i << 1)];
		Temp = Temp << 8 | Str[(i << 1) + 1];
		BKP_WriteBackupRegister(BKPDataReg[i], Temp);
  }
  BKP_WriteBackupRegister(BKPDataReg[(i + 1)], Interface);
	
  ErrorLedOn();
	PELedOn();
	NVIC_SystemReset();			  //产生系统复位
}

/*******************************************************************************
* Function Name  : Erase_Flash_Inside
* Description    : 清除NVIC_VectTab_FLASH所在的FLASH扇区
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Erase_Flash_Inside(void)
{
  FLASH_Unlock();
	FLASH_ErasePage(NVIC_VectTab_FLASH);
	FLASH_Lock();
  NVIC_SystemReset();			  //产生系统复位
}

void	Update(void)
{
	uint8_t	Interface_Type;
	
	//while (g_tInBuf.BytesNumber == 0);	//等待接收到外部数据  不需要有外部数据
	
	if (g_tSystemCtrl.PrtSourceIndex == LPT)				//当前数据源为LPT
	{
		Interface_Type = LPT;
		//禁用其他接口中断
	}
	else if (g_tSystemCtrl.PrtSourceIndex == COM)	//当前数据源为COM
	{
		Interface_Type = COM;
		//禁用其他接口中断
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)	//当前数据源为USB
	{
		Interface_Type = USB;
		//禁用其他接口中断
	}
	else if (g_tSystemCtrl.PrtSourceIndex == ETHERNET)	//当前数据源为ETHERNET
	{
		Interface_Type = ETHERNET;
		//禁用其他接口中断
	}
	
	System_Upgrade(Interface_Type);
}

/*******************************************************************************
* Function Name  : CheckBootLegality
* Description    : 检测boot的合法性
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    CheckBootLegality(void)
{
	uint32_t i,checkSum =0,*p;
	uint8_t	*Str;

	Str =(uint8_t *) (NVIC_VectTab_FLASH+0x90);       //任意地址A
    
	p =(uint32_t*)(0x8000000 + 0x154);	             //boot版本地址 0x13c 2016.07.13
  BootVerStr = (uint8_t *)(*p);	
	if(strncmp(BootVerStr,"WBV",3) != 0)
	{    
			BootVerStr = "WBV01";      
	}
	else
	{
			BootVerStr = BootVerStr+2;                //读取保存的为V01 前面的WB去掉2016.07.13
	}  
    
	for(i=0; i<0x10; i++)
	{
		checkSum += Str[i];
	}
// 	if(FLASH_GetUserOptionByte()>>8 !=0x4159)	//AY  不验证此处
// 	{
// 		while(1);								//参数错误
// 	}

	JumpAddress = *(__IO uint32_t*) (0x8000000 + 0x150);	  //CheckSumFun地址 指向boot程序中的求和函数
	Jump_To_Application = (pFunction)JumpAddress;		      	//取校验程序入口地址	
	if(Jump_To_Application(Str, i) != checkSum)	            //给求和函数参数赋值地址A和个数i 使用boot中的求和函数计算累加和，比较相等则正常；
	{
		while(1);								//校验错误
	}

}

/******************************
  * @brief  配置Flash选项字节,使能读保护，IAP程序空间写保护，标志字
  * @param  None
  * @retval : None
  *************************/
void ConfigOptionByte(void)        //2017.10.18
{
	uint32_t BlockNbr = 0, UserMemoryMask = 0;
	
  /* Get the number of block (4 or 2 pages) from where the user program will be loaded */
  BlockNbr = ((USER_FLASH_FIRST_PAGE_ADDRESS - 0x08000000) >> 12)-2;	//2020.01.09 保留BootLoad最后4保护区不受保护，用于维护读数器

  /* Compute the mask to test if the Flash memory, where the user program will be
     loaded, is write protected */
#ifdef STM32F10X_MD
  UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* STM32F10X_HD */
  
  if (BlockNbr < 62)
  {
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
  }
	
  else
  {
    UserMemoryMask = ((uint32_t)0x80000000);
  }
#endif /* STM32F10X_MD */

  //如果没有设置读出保护，则自动设置，
  if(!FLASH_GetReadOutProtectionStatus())	
  {
  	FLASH_Unlock();

	  FLASH_ReadOutProtection(ENABLE);				//设置读出保护
	  FLASH_EnableWriteProtection(~UserMemoryMask);	//设置IAP程序空间为写保护
  	FLASH_Lock();
  	NVIC_SystemReset();			  					//产生系统复位
  }
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
