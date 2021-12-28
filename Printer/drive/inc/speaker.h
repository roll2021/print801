#ifndef SPEAKER_H
#define SPEAKER_H


#ifdef __cplusplus
 extern "C" {
#endif
	 
#include	"stm32f10x.h"

#define SPEAKERFLAG	 
	 

#define SPEAKER_GPIO_PORT								   GPIOC
#define SPEAKER_RCC_APB2Periph_GPIO		     RCC_APB2Periph_GPIOC
#define SPEAKER_SDA									       GPIO_Pin_6
#define SPEAKER_SCL									       GPIO_Pin_7
#define SPEAKER_CTRL_PORT								   GPIOA
#define SPEAKER_CTRL_RCC_APB2Periph_GPIO   RCC_APB2Periph_GPIOA
#define SPEAKER_CTRL                       GPIO_Pin_10		//≈–∂œ”Ô“Ùƒ£øÈ «∑Ò∫∏Ω”
	 
	 
#define	CLEAR_SDA()					GPIO_ResetBits(SPEAKER_GPIO_PORT, SPEAKER_SDA)
#define	SET_SDA()				    GPIO_SetBits(SPEAKER_GPIO_PORT, SPEAKER_SDA)
	 
#define	CLEAR_SCL()					GPIO_ResetBits(SPEAKER_GPIO_PORT, SPEAKER_SCL)
#define	SET_SCL()				    GPIO_SetBits(SPEAKER_GPIO_PORT, SPEAKER_SCL)
extern void InitSpeakerPort(void);	 
extern void Line_2A_WTN5_Dly(unsigned char SB_DATA);
extern void Line_2A_WTN5_Tim(unsigned char SB_DATA);
// void SpeakerHandle(unsigned char * statement);
	 
#ifdef __cplusplus
}
#endif

#endif

