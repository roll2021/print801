/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： spi.c
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年07月29日
  * @摘要       ： SPI源文件
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2016-07-29 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "spi.h"


/************************************************
函数名称 ： SPI_Delay
功    能 ： SPI延时
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void SPI_Delay(void)
{
  uint16_t cnt = 5;

  while(cnt--);
}

/************************************************
函数名称 ： SPI_GPIO_Configuration
功    能 ： SPI引脚配置
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void SPI_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* CS */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_CS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_SPI_CS, &GPIO_InitStructure);

  /* SCK */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_SCK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_SPI_SCK, &GPIO_InitStructure);

  /* MISO */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(PORT_SPI_MISO, &GPIO_InitStructure);

  /* MOSI */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_SPI_MOSI, &GPIO_InitStructure);
}

/************************************************
函数名称 ： SPI_Initializes
功    能 ： SPI初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void SPI_Initializes(void)
{
  SPI_GPIO_Configuration();

  SPI_CS_DISABLE;
  SPI_SCK_HIGH;
  SPI_MOSI_HIGH;
}

/************************************************
函数名称 ： SPI_WriteByte
功    能 ： SPI写一字节数据
参    数 ： TxData --- 发送的字节数据
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void SPI_WriteByte(uint8_t TxData)
{
  uint8_t cnt;

  for(cnt=0; cnt<8; cnt++)
  {
    SPI_SCK_LOW;                                 //时钟 - 低
    SPI_Delay();

    if(TxData & 0x80)                            //发送数据
      SPI_MOSI_HIGH;
    else
      SPI_MOSI_LOW;
    TxData <<= 1;

    SPI_Delay();
    SPI_SCK_HIGH;                                //时钟 - 高
    SPI_Delay();
  }
}

/************************************************
函数名称 ： SPI_ReadByte
功    能 ： SPI读一字节数据
参    数 ： 无
返 回 值 ： 读回来的字节数据
作    者 ： strongerHuang
*************************************************/
uint8_t SPI_ReadByte(void)
{
  uint8_t cnt;
  uint8_t RxData = 0;

  for(cnt=0; cnt<8; cnt++)
  {
    SPI_SCK_LOW;                                 //时钟 - 低
    SPI_Delay();

    RxData <<= 1;
    if(SPI_MISO_READ)                            //读取数据
    {
      RxData |= 0x01;
    }

    SPI_SCK_HIGH;                                //时钟 - 高
    SPI_Delay();
  }

  return RxData;
}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
