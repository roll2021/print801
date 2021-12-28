/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06/19/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eth.h"
#include <stdio.h>
#include "uip.h"
#include "ETH_config.h"
#include "power.h"

/** @addtogroup 
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ETH_RXBUFNB        2 
#define ETH_TXBUFNB        2 
#define ETH_HPY_IRQHandler 		EXTI10_IRQHandler

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ETH_InitTypeDef ETH_InitStructure;
/* Ethernet Rx & Tx DMA Descriptors */
ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];
/* Ethernet buffers */
__align(4) uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE] ;
__align(4) uint8_t Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE] ; 

extern ErrorStatus HSEStartUpStatus;
uint8_t PHY_INT_Status;
 

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration_eth(void);

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void ETH_HardwareConfig(void)
{
	
	/* Enable ETHERNET clock  */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                        RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);
	/* ETHERNET pins remapp RX_DV and RxD[3:0] */
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE); 
		
	/* MII/RMII Media interface selection */
	#ifdef MII_MODE 
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);
	  
	/* Get HSE clock = 25MHz on PA8 pin(MCO) */
	RCC_MCOConfig(RCC_MCO_HSE); 
	  
	#elif defined RMII_MODE  
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
	  
	/* Get HSE clock = 25MHz on PA8 pin(MCO) */
	/* set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
	RCC_PLL3Config(RCC_PLL3Mul_10);
	/* Enable PLL3 */
	RCC_PLL3Cmd(ENABLE);
	/* Wait till PLL3 is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
	{}
	 
	/* Get clock PLL3 clock on PA8 pin */
	RCC_MCOConfig(RCC_MCO_PLL3CLK);    
	#endif
	  
	/* Configure the GPIO ports */
	GPIO_Configuration_eth();
}
/**
  * @brief  初始化以太网MAC、HPY.
  * @param  None
  * @retval None
  */
void Init_ETH(void)
{
	/* Reset ETHERNET on AHB Bus */
	ETH_DeInit();

	//2017.08.16
	DelayMs(20);
	PHYResetHigh();
	DelayMs(20);
	
	/* Software reset */ 
	ETH_SoftwareReset();
	  
	/* Wait for software reset */
	while(ETH_GetSoftwareResetStatus()==SET);  //2014.05.29

	/* ETHERNET Configuration ------------------------------------------------------*/
	/* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	ETH_StructInit(&ETH_InitStructure);
	  
	/* Fill ETH_InitStructure parametrs */
	/*------------------------   MAC   -----------------------------------*/
//	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;  
	ETH_InitStructure.ETH_Speed = ETH_Speed_100M;                                      
	ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;                                                                                  
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;              
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;                                                                                  
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;                                                                                                                                                                        
//	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;                                                                                                       
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;  			//MAC地址过滤
//	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;      
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;	//使能广播地址      
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;                                                             
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;      
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;                        

//2017.07.28
  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                                                          
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;                                                                 
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
//
	/* Configure ETHERNET */
	ETH_Init(&ETH_InitStructure, PHY_ADDRESS);
	
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;  	//允许自动协商
	  
	/* Initialize Tx Descriptors list: Chain Mode */
	ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
	/* Initialize Rx Descriptors list: Chain Mode  */
	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

   /* Enable the Ethernet Rx Interrupt */   //2017.07.28
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
	/* Enable MAC and DMA transmission and reception */
//	ETH_Start(); 
  
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration_eth(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

/* ETHERNET pins configuration */
/* AF Output Push Pull: 
- ETH_MII_MDIO / ETH_RMII_MDIO: PA2 
- ETH_MII_MDC / ETH_RMII_MDC: PC1
- ETH_MII_TXD2: PC2 
- ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11  
- ETH_MII_TXD0 / ETH_RMII_TXD0: PB12 
- ETH_MII_TXD1 / ETH_RMII_TXD1: PB13  
- ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5
- ETH_MII_TXD3: PB8 */ 

  /* Configure PA2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = MII_MDIO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(MII_PORTA, &GPIO_InitStructure);

  /* Configure PC1, PC2 and PC3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = MII_MDC | MII_TXD2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(MII_PORTC, &GPIO_InitStructure);

  /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = MII_TXD3 | MII_TXEN | 
                                MII_TXD0 | MII_TXD1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(MII_PORTB, &GPIO_InitStructure);
  
/**************************************************************/
/*               For Remapped Ethernet pins                   */
/*************************************************************/
/* Input (Reset Value):  
- ETH_MII_CRS CRS: PA0 
- ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1   
- ETH_MII_COL: PA3  
- ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
- ETH_MII_TX_CLK: PC3
- ETH_MII_RXD0 / ETH_RMII_RXD0: PD9 
- ETH_MII_RXD1 / ETH_RMII_RXD1: PD10 
- ETH_MII_RXD2: PD11
- ETH_MII_RXD3: PD12
- ETH_MII_RX_ER: PB10 */
 
  /* Configure PA0, PA1 and PA3 as input */
  GPIO_InitStructure.GPIO_Pin = MII_CRS | MII_RX_CLK | MII_COL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MII_PORTA, &GPIO_InitStructure);

  /* Configure PB10 as input */
  GPIO_InitStructure.GPIO_Pin = MII_RX_ER;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MII_PORTB, &GPIO_InitStructure);

  /* Configure PC3 as input */
  GPIO_InitStructure.GPIO_Pin = MII_TX_CLK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MII_PORTC, &GPIO_InitStructure); 

  /* Configure PD8, PD9, PD10, PD11 and PD12 as input */
  GPIO_InitStructure.GPIO_Pin = MII_CRS_DV | MII_RXD0 | MII_RXD1 | MII_RXD2 | MII_RXD3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MII_PORTD, &GPIO_InitStructure); /**/    

  /* MCO pin configuration------------------------------------------------- */
  /* Configure MCO (PA8) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =MII_MCO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(MII_PORTA, &GPIO_InitStructure);
 
}
/*******************************************************************************
* Function Name  : ETH_HPY_IRQHandler
* Description    : 以太网HPY中断服务程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_HPY_IRQHandler(void)
{
	PHY_INT_Status=1;						//设置中断产生标志
	EXTI_ClearITPendingBit(PHY_EXTI_LINE);	//清中断标志
}	

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
