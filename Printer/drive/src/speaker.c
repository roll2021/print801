#include "speaker.h"
#include "extgvar.h"
#include	"spiflash.h"
#include	"charmaskbuf.h"

void Line_2A_WTN588H_Dly(unsigned int ddata);
/*******************************************************************************
* Function Name  : 函数名
* Description    : 语音端口初始化
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void InitSpeakerPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(SPEAKER_RCC_APB2Periph_GPIO, ENABLE);
  RCC_APB2PeriphClockCmd(SPEAKER_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin		= SPEAKER_SDA | SPEAKER_SCL;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(SPEAKER_GPIO_PORT, &GPIO_InitStructure);
  SET_SCL();
	SET_SDA();
	
	GPIO_InitStructure.GPIO_Pin		= SPEAKER_CTRL;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPD;   //下拉输入
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(SPEAKER_CTRL_PORT, &GPIO_InitStructure);
	DelayMs(5);
	if(GPIO_ReadInputDataBit(SPEAKER_CTRL_PORT,SPEAKER_CTRL))   //2018.01.22
	{
		g_tSpeaker.SpeakCtrl = 1;   //语音电路存在
		g_tSpeaker.SpeakElecCtrl = 1;
	}
	else 
	{
		g_tSpeaker.SpeakCtrl = 0;
		g_tSpeaker.SpeakElecCtrl = 0;
	}

}
	 
/*******************************************************************************
* Function Name  : 函数名
* Description    : 发送一个字节
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void Line_2A_WTN5_Dly(unsigned char SB_DATA)
{
	unsigned char S_DATA,B_DATA,i;
  SET_SCL();
	SET_SDA();
	
	S_DATA = SB_DATA;
	CLEAR_SCL();
	DelayMs(5);

	B_DATA = S_DATA & 0x01;
	
	for(i=0; i<8; i++)
	{
		CLEAR_SCL();
		if(B_DATA)
			SET_SDA();
		else
			CLEAR_SDA();
		
		DelayUs(300);
		SET_SCL();
		DelayUs(300);
		
		S_DATA = S_DATA>>1;
		B_DATA = S_DATA & 0x01;
  }
	
  SET_SCL();
	SET_SDA();
}
/*******************************************************************************
* Function Name  : 函数名
* Description    : 发送一个字节
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void Line_2A_WTN588H_Dly(unsigned int ddata)
{
	unsigned char j,num_temp;
	unsigned int ddata_temp,pdata_temp;
  SET_SCL();
	SET_SDA();
	ddata_temp = ddata;
	pdata_temp = ddata_temp&0x00FF;
	ddata_temp = ddata_temp>>8;
	pdata_temp = pdata_temp<<8;
	ddata_temp |= pdata_temp;
	num_temp = 16;
	CLEAR_SCL();
// 	DelayMs(5);
	DelayMs(6);
	for(j=0; j<num_temp; j++)
	{
		if(j == 8)
		{
			SET_SCL();
			SET_SDA();
// 			DelayMs(2);
			DelayMs(3);
			CLEAR_SCL();
// 			DelayMs(5);
			DelayMs(6);
		}
		CLEAR_SCL();
		if(ddata_temp&0x0001)
			SET_SDA();
		else 
			CLEAR_SDA();
// 		DelayUs(200);
		DelayUs(300);
		SET_SCL();
// 		DelayUs(200);
		DelayUs(300);
		ddata_temp = ddata_temp >> 1;
  }
	
  SET_SCL();
	SET_SDA();
}


void Line_2A_WTN588H_Tim(unsigned int ddata)
{
	unsigned char j,num_temp;
	unsigned int pdata_temp;
	static unsigned int ddata_temp;
	static unsigned char SpeakTempCnt = 0;

	num_temp = 16;
	SpeakTempCnt = g_tSpeaker.SpeakTempCntBuf;
	
	SpeakTempCnt++;
	if(SpeakTempCnt == 1)
	{
		SET_SDA();
		SET_SCL();
		ddata_temp = ddata;
		pdata_temp = ddata_temp&0x00FF;
		ddata_temp = ddata_temp>>8;
		pdata_temp = pdata_temp<<8;
		ddata_temp |= pdata_temp;
	}
	else if(SpeakTempCnt == 4)
	{
		CLEAR_SCL();
	}	
	else if(SpeakTempCnt >= 9 && SpeakTempCnt <= 24)
	{
		if(SpeakTempCnt%2)
		{	
			CLEAR_SCL();
	  	if(ddata_temp&0x0001)
			{	
			  SET_SDA();
			}	
		  else
			{	
			  CLEAR_SDA();
			}	
		}
		else
		{
			SET_SCL();
			ddata_temp = ddata_temp >> 1;
		}		
	}
	else if(SpeakTempCnt == 25)
	{
			SET_SCL();
			SET_SDA();
	}
	else if(SpeakTempCnt == 28)
	{
		CLEAR_SCL();
	}
	else if(SpeakTempCnt >= 35 && SpeakTempCnt <= 50)
	{
		if(SpeakTempCnt%2)
		{	
			CLEAR_SCL();
	  	if(ddata_temp&0x0001)
			{	
			  SET_SDA();
			}	
		  else
			{	
			  CLEAR_SDA();
			}	
		}
		else
		{
			SET_SCL();
			ddata_temp = ddata_temp >> 1;
		}		
	}
	else if(SpeakTempCnt == 51)
	{

		SpeakTempCnt = 0;

		g_tSystemCtrl.CutLedFlagBuf1 = 1;
		g_tSpeaker.SpeakCnt = 0;  //延时时间从此刻计时
		g_tSpeaker.SpeakBufFlg = 0;
		if(VoiceOverflag == 0)
		{	
			VoiceOverflag = 1;
		}	
		else
		{	
			VoiceOverflag = 0;
			g_tSpeaker.SpeakFlag = 1;
		}	
		
	}
	
		g_tSpeaker.SpeakTempCntBuf = SpeakTempCnt;
}




void Line_2A_WTN5_Tim(unsigned char SB_DATA)
{
  unsigned char S_DATA,B_DATA,i;
	static unsigned char SpeakDlyCnt = 0;
	static unsigned char SpeakAllCnt = 0;
	static unsigned char SpeakTempCnt = 0;
	static unsigned char SpeakTempNum = 0;
	
	SpeakTempCnt++;
	if(SpeakTempCnt == 1)
	{
		SET_SDA();
		SET_SCL();
	}	
	else if(SpeakTempCnt == 4)
	{
		CLEAR_SCL();
	}	
	else if(SpeakTempCnt >= 9 && SpeakTempCnt <= 24)
	{
		if(SpeakTempCnt%2)
		{	
			S_DATA = SB_DATA;
			S_DATA = S_DATA>>(SpeakTempNum);
			B_DATA = S_DATA & 0x01;
			SpeakTempNum++;
			CLEAR_SCL();
	  	if(B_DATA)
			{	
			  SET_SDA();
			}	
		  else
			{	
			  CLEAR_SDA();
			}	
		}
		else
		{
			SET_SCL();
		}		
	}	
	else if(SpeakTempCnt == 25)
	{
		SET_SCL();
   	SET_SDA();
	}	
	else if(SpeakTempCnt == 26)
	{
		SpeakTempNum = 0;
		SpeakTempCnt = 0;
		
		g_tSystemCtrl.CutLedFlagBuf1 = 1;
		g_tSpeaker.SpeakCnt = 0;  //延时时间从此刻计时
		if(VoiceOverflag == 0)
		{	
			VoiceOverflag = 1;
		}	
		else
		{	
			VoiceOverflag = 0;
			g_tSpeaker.SpeakFlag = 1;
		}	
	}		
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 发送一个字节
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
// void SpeakerHandle(unsigned char * statement)
// {
// 	static unsigned char buff = 0;
// 	if(*statement == PRINTERLIST)//打印完成
// 	{
// 		*statement = 0;
// // 		Line_2A_WTN5(0xFE);//停止播放当前语音
// 		Line_2A_WTN5(0x00);//播放 来单了，请取单
//   }
// // 	if(*statement == PEERROR)    //语言报缺纸
// // 	{
// // 		buff = 1;
// // // 		Line_2A_WTN5(0xFE);
// // // 		Line_2A_WTN5(0xF3);
// // 		Line_2A_WTN5(0x01);
// // 		Line_2A_WTN5(0xF8);//
// // 		Line_2A_WTN5(30);
// // // 		Line_2A_WTN5(0xF3);
// // // 		Line_2A_WTN5(0x00);
// //   }
// // 	else if(buff == 1)
// // 	{
// // 		buff = 0;
// // 		Line_2A_WTN5(0xFE);//停止播放当前语音
// //   }
// 	
// 	if(*statement == PEERROR)    //语言报缺纸
// 	{
// 		buff = 1;
// 		if(SpeakerStart == 0)
// 		{
// 			SpeakerStart = 1;
// 			Line_2A_WTN5(0x01);
// 		}
//   }
// 	else if(buff == 1)
// 	{
// 		buff = 0;
// 		Line_2A_WTN5(0xFE);//停止播放当前语音
// 		SpeakerStart = 0;
//   }


// }


