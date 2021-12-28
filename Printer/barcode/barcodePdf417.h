/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   二维条码相关参数定义
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
  * <h2><center>&copy; COPYRIGHT 2012</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BARCODEPDF417_H
#define BARCODEPDF417_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"ean.h"

/* Macro Definition ----------------------------------------------------------*/
//二维码参数定义



//PDF417  相关结构定义


#define PDF417_START_FLAG	0xfea8
#define PDF417_STOP_FLAG	0xfd14
#define PDF417_MODE_WIDE	17		//每条符宽
#define PDF417_WIDE_MAX		30		//90-583模块宽-34字宽，30有效内容宽
#define PDF417_HIGH_MAX		90		//条码最大高度度，3-90行
#define MOD					929		//最大有效代码

//压缩模式
#define TEXT_MODE			900		//文本压缩模式
#define BYTE_MODE_6			924		//字节压缩模式2
#define BYTE_MODE			901		//字节压缩模式1
#define NUME_MODE			902		//数字压缩模式
#define BYTESHIFT			913		//模式转移

//文本压缩子模式
#define PL					25
#define LL					27
#define AS					27
#define ML					28
#define AL					28
#define PS					29
#define PAL					29
#define SPACE				26

//模式标志
#define UPPER				0x80		//大写
#define LOWER				0x40		//小写
#define MIXED				0x20		//混合
#define PUNCT				0x10		//符号


const char* MIXED_TBL = "0123456789&\r\t,:#-.$/+%*=^";
const char* PUNCT_TBL = ";<>@[\\]_`~!\r\t,:\n-.$/\"|*()?{}'";

typedef struct {
	u16 *err;			   		//纠错多项式参数地址
	u16 *buf;
	u16 len;
	u16 wide;
	u16 high;
	u16 err_n;					//纠错字
	u16 err_l;					//纠错等级
}PDF417_EN;

static PDF417_EN pdf_en;
static int pdf_mode;			//编码模式

//----------------------------------------------------------------------------------
//文本模式
#define text_get(m_n,ch)		\
{								\
	if(ch>='A'&&ch<='Z')		\
	{							\
		m_n=UPPER;				\
		ch-='A';				\
	}							\
	else if(ch>='a'&&ch<='z')	\
	{							\
		m_n=LOWER;				\
		ch-='a';				\
	}							\
	else if(ch==' ')			\
	{							\
		m_n=UPPER|LOWER|MIXED;	/*该字符在多种子模式下均存在*/  \
		ch=SPACE;				\
	}							\
	else						\
	{							\
		char *p;				\
		u8    c;				\
							\
		m_n=0;					\
		p=strchr(MIXED_TBL,ch);	\
		if(p)					\
		{						\
			m_n|=MIXED;			\
			c=p-MIXED_TBL;		\
		}						\
		p=strchr(PUNCT_TBL,ch);	\
		if(p)					\
		{	/*若字符在混合模式或字符模式下都存在,也没问题,因为同一字符在多种模式下值相同*/\
			m_n|=PUNCT;			\
			c=p-PUNCT_TBL;		\
		}						\
		ch=c;					\
	}							\
}

#define text_begin()	tital=0

#define text_add(ch)	\
{						\
	if(total&1)			\
		*dst++=dat+ch;	\
	else				\
		dat=ch*30;		\
	total=!total;		\
}

#define text_end()		\
{						\
	if(total)			\
		text_add(PS);	\
}

typedef struct {
	unsigned char *out;			//编码结果
	int wide;					//条码宽度
	int high;					//条码高度
	unsigned char *in;			//待编码数据
	int len;					//待编码数据长度
	int max;					//宽度限制
	char level;					//纠错等级
} pdf417;

u8 pdf417_encode ( pdf417 *p );

#ifdef EXTFLASH

	#define PDF417_CLUSTERS_ADDR		0x1f6500							//PDF417条码字模在外部FLASH中的首地址
	
	#define PDF417_ERROR_LEVEL_ADDR 	(PDF417_CLUSTERS_ADDR + 929*2*3)	//PDF417条码纠错在外部FLASH中的首地址
#else
u16	const	CLUSTERS[3][929]={
	{
		///0
		0xd5c0, 0xeaf0, 0xf57c, 0xd4e0, 0xea78, 0xf53e, 0xa8c0, 0xd470,
		0xa860, 0x5040, 0xa830, 0x5020, 0xadc0, 0xd6f0, 0xeb7c, 0xace0,
		0xd678, 0xeb3e, 0x58c0, 0xac70, 0x5860, 0x5dc0, 0xaef0, 0xd77c,
		0x5ce0, 0xae78, 0xd73e, 0x5c70, 0xae3c, 0x5ef0, 0xaf7c, 0x5e78,
		0xaf3e, 0x5f7c, 0xf5fa, 0xd2e0, 0xe978, 0xf4be, 0xa4c0, 0xd270,
		0xe93c, 0xa460, 0xd238, 0x4840, 0xa430, 0xd21c, 0x4820, 0xa418,
		0x4810, 0xa6e0, 0xd378, 0xe9be, 0x4cc0, 0xa670, 0xd33c, 0x4c60,
		0xa638, 0xd31e, 0x4c30, 0xa61c, 0x4ee0, 0xa778, 0xd3be, 0x4e70,
		0xa73c, 0x4e38, 0xa71e, 0x4f78, 0xa7be, 0x4f3c, 0x4f1e, 0xa2c0,
		0xd170, 0xe8bc, 0xa260, 0xd138, 0xe89e, 0x4440, 0xa230, 0xd11c,
		0x4420, 0xa218, 0x4410, 0x4408, 0x46c0, 0xa370, 0xd1bc, 0x4660,
		0xa338, 0xd19e, 0x4630, 0xa31c, 0x4618, 0x460c, 0x4770, 0xa3bc,
		0x4738, 0xa39e, 0x471c, 0x47bc, 0xa160, 0xd0b8, 0xe85e, 0x4240,
		0xa130, 0xd09c, 0x4220, 0xa118, 0xd08e, 0x4210, 0xa10c, 0x4208,
		0xa106, 0x4360, 0xa1b8, 0xd0de, 0x4330, 0xa19c, 0x4318, 0xa18e,
		0x430c, 0x4306, 0xa1de, 0x438e, 0x4140, 0xa0b0, 0xd05c, 0x4120,
		0xa098, 0xd04e, 0x4110, 0xa08c, 0x4108, 0xa086, 0x4104, 0x41b0,
		0x4198, 0x418c, 0x40a0, 0xd02e, 0xa04c, 0xa046, 0x4082, 0xcae0,
		0xe578, 0xf2be, 0x94c0, 0xca70, 0xe53c, 0x9460, 0xca38, 0xe51e,
		0x2840, 0x9430, 0x2820, 0x96e0, 0xcb78, 0xe5be, 0x2cc0, 0x9670,
		0xcb3c, 0x2c60, 0x9638, 0x2c30, 0x2c18, 0x2ee0, 0x9778, 0xcbbe,
		0x2e70, 0x973c, 0x2e38, 0x2e1c, 0x2f78, 0x97be, 0x2f3c, 0x2fbe,
		0xdac0, 0xed70, 0xf6bc, 0xda60, 0xed38, 0xf69e, 0xb440, 0xda30,
		0xed1c, 0xb420, 0xda18, 0xed0e, 0xb410, 0xda0c, 0x92c0, 0xc970,
		0xe4bc, 0xb6c0, 0x9260, 0xc938, 0xe49e, 0xb660, 0xdb38, 0xed9e,
		0x6c40, 0x2420, 0x9218, 0xc90e, 0x6c20, 0xb618, 0x6c10, 0x26c0,
		0x9370, 0xc9bc, 0x6ec0, 0x2660, 0x9338, 0xc99e, 0x6e60, 0xb738,
		0xdb9e, 0x6e30, 0x2618, 0x6e18, 0x2770, 0x93bc, 0x6f70, 0x2738,
		0x939e, 0x6f38, 0xb79e, 0x6f1c, 0x27bc, 0x6fbc, 0x279e, 0x6f9e,
		0xd960, 0xecb8, 0xf65e, 0xb240, 0xd930, 0xec9c, 0xb220, 0xd918,
		0xec8e, 0xb210, 0xd90c, 0xb208, 0xb204, 0x9160, 0xc8b8, 0xe45e,
		0xb360, 0x9130, 0xc89c, 0x6640, 0x2220, 0xd99c, 0xc88e, 0x6620,
		0x2210, 0x910c, 0x6610, 0xb30c, 0x9106, 0x2204, 0x2360, 0x91b8,
		0xc8de, 0x6760, 0x2330, 0x919c, 0x6730, 0xb39c, 0x918e, 0x6718,
		0x230c, 0x2306, 0x23b8, 0x91de, 0x67b8, 0x239c, 0x679c, 0x238e,
		0x678e, 0x67de, 0xb140, 0xd8b0, 0xec5c, 0xb120, 0xd898, 0xec4e,
		0xb110, 0xd88c, 0xb108, 0xd886, 0xb104, 0xb102, 0x2140, 0x90b0,
		0xc85c, 0x6340, 0x2120, 0x9098, 0xc84e, 0x6320, 0xb198, 0xd8ce,
		0x6310, 0x2108, 0x9086, 0x6308, 0xb186, 0x6304, 0x21b0, 0x90dc,
		0x63b0, 0x2198, 0x90ce, 0x6398, 0xb1ce, 0x638c, 0x2186, 0x6386,
		0x63dc, 0x63ce, 0xb0a0, 0xd858, 0xec2e, 0xb090, 0xd84c, 0xb088,
		0xd846, 0xb084, 0xb082, 0x20a0, 0x9058, 0xc82e, 0x61a0, 0x2090,
		0x904c, 0x6190, 0xb0cc, 0x9046, 0x6188, 0x2084, 0x6184, 0x2082,
		0x20d8, 0x61d8, 0x61cc, 0x61c6, 0xd82c, 0xd826, 0xb042, 0x902c,
		0x2048, 0x60c8, 0x60c4, 0x60c2, 0x8ac0, 0xc570, 0xe2bc, 0x8a60,
		0xc538, 0x1440, 0x8a30, 0xc51c, 0x1420, 0x8a18, 0x1410, 0x1408,
		0x16c0, 0x8b70, 0xc5bc, 0x1660, 0x8b38, 0xc59e, 0x1630, 0x8b1c,
		0x1618, 0x160c, 0x1770, 0x8bbc, 0x1738, 0x8b9e, 0x171c, 0x17bc,
		0x179e, 0xcd60, 0xe6b8, 0xf35e, 0x9a40, 0xcd30, 0xe69c, 0x9a20,
		0xcd18, 0xe68e, 0x9a10, 0xcd0c, 0x9a08, 0xcd06, 0x8960, 0xc4b8,
		0xe25e, 0x9b60, 0x8930, 0xc49c, 0x3640, 0x1220, 0xcd9c, 0xc48e,
		0x3620, 0x9b18, 0x890c, 0x3610, 0x1208, 0x3608, 0x1360, 0x89b8,
		0xc4de, 0x3760, 0x1330, 0xcdde, 0x3730, 0x9b9c, 0x898e, 0x3718,
		0x130c, 0x370c, 0x13b8, 0x89de, 0x37b8, 0x139c, 0x379c, 0x138e,
		0x13de, 0x37de, 0xdd40, 0xeeb0, 0xf75c, 0xdd20, 0xee98, 0xf74e,
		0xdd10, 0xee8c, 0xdd08, 0xee86, 0xdd04, 0x9940, 0xccb0, 0xe65c,
		0xbb40, 0x9920, 0xeedc, 0xe64e, 0xbb20, 0xdd98, 0xeece, 0xbb10,
		0x9908, 0xcc86, 0xbb08, 0xdd86, 0x9902, 0x1140, 0x88b0, 0xc45c,
		0x3340, 0x1120, 0x8898, 0xc44e, 0x7740, 0x3320, 0x9998, 0xccce,
		0x7720, 0xbb98, 0xddce, 0x8886, 0x7710, 0x3308, 0x9986, 0x7708,
		0x1102, 0x11b0, 0x88dc, 0x33b0, 0x1198, 0x88ce, 0x77b0, 0x3398,
		0x99ce, 0x7798, 0xbbce, 0x1186, 0x3386, 0x11dc, 0x33dc, 0x11ce,
		0x77dc, 0x33ce, 0xdca0, 0xee58, 0xf72e, 0xdc90, 0xee4c, 0xdc88,
		0xee46, 0xdc84, 0xdc82, 0x98a0, 0xcc58, 0xe62e, 0xb9a0, 0x9890,
		0xee6e, 0xb990, 0xdccc, 0xcc46, 0xb988, 0x9884, 0xb984, 0x9882,
		0xb982, 0x10a0, 0x8858, 0xc42e, 0x31a0, 0x1090, 0x884c, 0x73a0,
		0x3190, 0x98cc, 0x8846, 0x7390, 0xb9cc, 0x1084, 0x7388, 0x3184,
		0x1082, 0x3182, 0x10d8, 0x886e, 0x31d8, 0x10cc, 0x73d8, 0x31cc,
		0x10c6, 0x73cc, 0x31c6, 0x10ee, 0x73ee, 0xdc50, 0xee2c, 0xdc48,
		0xee26, 0xdc44, 0xdc42, 0x9850, 0xcc2c, 0xb8d0, 0x9848, 0xcc26,
		0xb8c8, 0xdc66, 0xb8c4, 0x9842, 0xb8c2, 0x1050, 0x882c, 0x30d0,
		0x1048, 0x8826, 0x71d0, 0x30c8, 0x9866, 0x71c8, 0xb8e6, 0x1042,
		0x71c4, 0x30c2, 0x71c2, 0x30ec, 0x71ec, 0x71e6, 0xee16, 0xdc22,
		0xcc16, 0x9824, 0x9822, 0x1028, 0x3068, 0x70e8, 0x1022, 0x3062,
		0x8560, 0x0a40, 0x8530, 0x0a20, 0x8518, 0xc28e, 0x0a10, 0x850c,
		0x0a08, 0x8506, 0x0b60, 0x85b8, 0xc2de, 0x0b30, 0x859c, 0x0b18,
		0x858e, 0x0b0c, 0x0b06, 0x0bb8, 0x85de, 0x0b9c, 0x0b8e, 0x0bde,
		0x8d40, 0xc6b0, 0xe35c, 0x8d20, 0xc698, 0x8d10, 0xc68c, 0x8d08,
		0xc686, 0x8d04, 0x0940, 0x84b0, 0xc25c, 0x1b40, 0x0920, 0xc6dc,
		0xc24e, 0x1b20, 0x8d98, 0xc6ce, 0x1b10, 0x0908, 0x8486, 0x1b08,
		0x8d86, 0x0902, 0x09b0, 0x84dc, 0x1bb0, 0x0998, 0x84ce, 0x1b98,
		0x8dce, 0x1b8c, 0x0986, 0x09dc, 0x1bdc, 0x09ce, 0x1bce, 0xcea0,
		0xe758, 0xf3ae, 0xce90, 0xe74c, 0xce88, 0xe746, 0xce84, 0xce82,
		0x8ca0, 0xc658, 0x9da0, 0x8c90, 0xc64c, 0x9d90, 0xcecc, 0xc646,
		0x9d88, 0x8c84, 0x9d84, 0x8c82, 0x9d82, 0x08a0, 0x8458, 0x19a0,
		0x0890, 0xc66e, 0x3ba0, 0x1990, 0x8ccc, 0x8446, 0x3b90, 0x9dcc,
		0x0884, 0x3b88, 0x1984, 0x0882, 0x1982, 0x08d8, 0x846e, 0x19d8,
		0x08cc, 0x3bd8, 0x19cc, 0x08c6, 0x3bcc, 0x19c6, 0x08ee, 0x19ee,
		0x3bee, 0xef50, 0xf7ac, 0xef48, 0xf7a6, 0xef44, 0xef42, 0xce50,
		0xe72c, 0xded0, 0xef6c, 0xe726, 0xdec8, 0xef66, 0xdec4, 0xce42,
		0xdec2, 0x8c50, 0xc62c, 0x9cd0, 0x8c48, 0xc626, 0xbdd0, 0x9cc8,
		0xce66, 0xbdc8, 0xdee6, 0x8c42, 0xbdc4, 0x9cc2, 0xbdc2, 0x0850,
		0x842c, 0x18d0, 0x0848, 0x8426, 0x39d0, 0x18c8, 0x8c66, 0x7bd0,
		0x39c8, 0x9ce6, 0x0842, 0x7bc8, 0xbde6, 0x18c2, 0x7bc4, 0x086c,
		0x18ec, 0x0866, 0x39ec, 0x18e6, 0x7bec, 0x39e6, 0x7be6, 0xef28,
		0xf796, 0xef24, 0xef22, 0xce28, 0xe716, 0xde68, 0xef36, 0xde64,
		0xce22, 0xde62, 0x8c28, 0xc616, 0x9c68, 0x8c24, 0xbce8, 0x9c64,
		0x8c22, 0xbce4, 0x9c62, 0xbce2, 0x0828, 0x8416, 0x1868, 0x8c36,
		0x38e8, 0x1864, 0x0822, 0x79e8, 0x38e4, 0x1862, 0x79e4, 0x38e2,
		0x79e2, 0x1876, 0x79f6, 0xef12, 0xde34, 0xde32, 0x9c34, 0xbc74,
		0xbc72, 0x1834, 0x3874, 0x78f4, 0x78f2, 0x0540, 0x0520, 0x8298,
		0x0510, 0x0508, 0x0504, 0x05b0, 0x0598, 0x058c, 0x0586, 0x05dc,
		0x05ce, 0x86a0, 0x8690, 0xc34c, 0x8688, 0xc346, 0x8684, 0x8682,
		0x04a0, 0x8258, 0x0da0, 0x86d8, 0x824c, 0x0d90, 0x86cc, 0x0d88,
		0x86c6, 0x0d84, 0x0482, 0x0d82, 0x04d8, 0x826e, 0x0dd8, 0x86ee,
		0x0dcc, 0x04c6, 0x0dc6, 0x04ee, 0x0dee, 0xc750, 0xc748, 0xc744,
		0xc742, 0x8650, 0x8ed0, 0xc76c, 0xc326, 0x8ec8, 0xc766, 0x8ec4,
		0x8642, 0x8ec2, 0x0450, 0x0cd0, 0x0448, 0x8226, 0x1dd0, 0x0cc8,
		0x0444, 0x1dc8, 0x0cc4, 0x0442, 0x1dc4, 0x0cc2, 0x046c, 0x0cec,
		0x0466, 0x1dec, 0x0ce6, 0x1de6, 0xe7a8, 0xe7a4, 0xe7a2, 0xc728,
		0xcf68, 0xe7b6, 0xcf64, 0xc722, 0xcf62, 0x8628, 0xc316, 0x8e68,
		0xc736, 0x9ee8, 0x8e64, 0x8622, 0x9ee4, 0x8e62, 0x9ee2, 0x0428,
		0x8216, 0x0c68, 0x8636, 0x1ce8, 0x0c64, 0x0422, 0x3de8, 0x1ce4,
		0x0c62, 0x3de4, 0x1ce2, 0x0436, 0x0c76, 0x1cf6, 0x3df6, 0xf7d4,
		0xf7d2, 0xe794, 0xefb4, 0xe792, 0xefb2, 0xc714, 0xcf34, 0xc712,
		0xdf74, 0xcf32, 0xdf72, 0x8614, 0x8e34, 0x8612, 0x9e74, 0x8e32,
		0xbef4
	},
	{
	///3
		0xf560, 0xfab8, 0xea40, 0xf530, 0xfa9c, 0xea20, 0xf518, 0xfa8e,
		0xea10, 0xf50c, 0xea08, 0xf506, 0xea04, 0xeb60, 0xf5b8, 0xfade,
		0xd640, 0xeb30, 0xf59c, 0xd620, 0xeb18, 0xf58e, 0xd610, 0xeb0c,
		0xd608, 0xeb06, 0xd604, 0xd760, 0xebb8, 0xf5de, 0xae40, 0xd730,
		0xeb9c, 0xae20, 0xd718, 0xeb8e, 0xae10, 0xd70c, 0xae08, 0xd706,
		0xae04, 0xaf60, 0xd7b8, 0xebde, 0x5e40, 0xaf30, 0xd79c, 0x5e20,
		0xaf18, 0xd78e, 0x5e10, 0xaf0c, 0x5e08, 0xaf06, 0x5f60, 0xafb8,
		0xd7de, 0x5f30, 0xaf9c, 0x5f18, 0xaf8e, 0x5f0c, 0x5fb8, 0xafde,
		0x5f9c, 0x5f8e, 0xe940, 0xf4b0, 0xfa5c, 0xe920, 0xf498, 0xfa4e,
		0xe910, 0xf48c, 0xe908, 0xf486, 0xe904, 0xe902, 0xd340, 0xe9b0,
		0xf4dc, 0xd320, 0xe998, 0xf4ce, 0xd310, 0xe98c, 0xd308, 0xe986,
		0xd304, 0xd302, 0xa740, 0xd3b0, 0xe9dc, 0xa720, 0xd398, 0xe9ce,
		0xa710, 0xd38c, 0xa708, 0xd386, 0xa704, 0xa702, 0x4f40, 0xa7b0,
		0xd3dc, 0x4f20, 0xa798, 0xd3ce, 0x4f10, 0xa78c, 0x4f08, 0xa786,
		0x4f04, 0x4fb0, 0xa7dc, 0x4f98, 0xa7ce, 0x4f8c, 0x4f86, 0x4fdc,
		0x4fce, 0xe8a0, 0xf458, 0xfa2e, 0xe890, 0xf44c, 0xe888, 0xf446,
		0xe884, 0xe882, 0xd1a0, 0xe8d8, 0xf46e, 0xd190, 0xe8cc, 0xd188,
		0xe8c6, 0xd184, 0xd182, 0xa3a0, 0xd1d8, 0xe8ee, 0xa390, 0xd1cc,
		0xa388, 0xd1c6, 0xa384, 0xa382, 0x47a0, 0xa3d8, 0xd1ee, 0x4790,
		0xa3cc, 0x4788, 0xa3c6, 0x4784, 0x4782, 0x47d8, 0xa3ee, 0x47cc,
		0x47c6, 0x47ee, 0xe850, 0xf42c, 0xe848, 0xf426, 0xe844, 0xe842,
		0xd0d0, 0xe86c, 0xd0c8, 0xe866, 0xd0c4, 0xd0c2, 0xa1d0, 0xd0ec,
		0xa1c8, 0xd0e6, 0xa1c4, 0xa1c2, 0x43d0, 0xa1ec, 0x43c8, 0xa1e6,
		0x43c4, 0x43c2, 0x43ec, 0x43e6, 0xe828, 0xf416, 0xe824, 0xe822,
		0xd068, 0xe836, 0xd064, 0xd062, 0xa0e8, 0xd076, 0xa0e4, 0xa0e2,
		0x41e8, 0xa0f6, 0x41e4, 0x41e2, 0xe814, 0xe812, 0xd034, 0xd032,
		0xa074, 0xa072, 0xe540, 0xf2b0, 0xf95c, 0xe520, 0xf298, 0xf94e,
		0xe510, 0xf28c, 0xe508, 0xf286, 0xe504, 0xe502, 0xcb40, 0xe5b0,
		0xf2dc, 0xcb20, 0xe598, 0xf2ce, 0xcb10, 0xe58c, 0xcb08, 0xe586,
		0xcb04, 0xcb02, 0x9740, 0xcbb0, 0xe5dc, 0x9720, 0xcb98, 0xe5ce,
		0x9710, 0xcb8c, 0x9708, 0xcb86, 0x9704, 0x9702, 0x2f40, 0x97b0,
		0xcbdc, 0x2f20, 0x9798, 0xcbce, 0x2f10, 0x978c, 0x2f08, 0x9786,
		0x2f04, 0x2fb0, 0x97dc, 0x2f98, 0x97ce, 0x2f8c, 0x2f86, 0x2fdc,
		0x2fce, 0xf6a0, 0xfb58, 0x6bf0, 0xf690, 0xfb4c, 0x69f8, 0xf688,
		0xfb46, 0x68fc, 0xf684, 0xf682, 0xe4a0, 0xf258, 0xf92e, 0xeda0,
		0xe490, 0xfb6e, 0xed90, 0xf6cc, 0xf246, 0xed88, 0xe484, 0xed84,
		0xe482, 0xed82, 0xc9a0, 0xe4d8, 0xf26e, 0xdba0, 0xc990, 0xe4cc,
		0xdb90, 0xedcc, 0xe4c6, 0xdb88, 0xc984, 0xdb84, 0xc982, 0xdb82,
		0x93a0, 0xc9d8, 0xe4ee, 0xb7a0, 0x9390, 0xc9cc, 0xb790, 0xdbcc,
		0xc9c6, 0xb788, 0x9384, 0xb784, 0x9382, 0xb782, 0x27a0, 0x93d8,
		0xc9ee, 0x6fa0, 0x2790, 0x93cc, 0x6f90, 0xb7cc, 0x93c6, 0x6f88,
		0x2784, 0x6f84, 0x2782, 0x27d8, 0x93ee, 0x6fd8, 0x27cc, 0x6fcc,
		0x27c6, 0x6fc6, 0x27ee, 0xf650, 0xfb2c, 0x65f8, 0xf648, 0xfb26,
		0x64fc, 0xf644, 0x647e, 0xf642, 0xe450, 0xf22c, 0xecd0, 0xe448,
		0xf226, 0xecc8, 0xf666, 0xecc4, 0xe442, 0xecc2, 0xc8d0, 0xe46c,
		0xd9d0, 0xc8c8, 0xe466, 0xd9c8, 0xece6, 0xd9c4, 0xc8c2, 0xd9c2,
		0x91d0, 0xc8ec, 0xb3d0, 0x91c8, 0xc8e6, 0xb3c8, 0xd9e6, 0xb3c4,
		0x91c2, 0xb3c2, 0x23d0, 0x91ec, 0x67d0, 0x23c8, 0x91e6, 0x67c8,
		0xb3e6, 0x67c4, 0x23c2, 0x67c2, 0x23ec, 0x67ec, 0x23e6, 0x67e6,
		0xf628, 0xfb16, 0x62fc, 0xf624, 0x627e, 0xf622, 0xe428, 0xf216,
		0xec68, 0xf636, 0xec64, 0xe422, 0xec62, 0xc868, 0xe436, 0xd8e8,
		0xc864, 0xd8e4, 0xc862, 0xd8e2, 0x90e8, 0xc876, 0xb1e8, 0xd8f6,
		0xb1e4, 0x90e2, 0xb1e2, 0x21e8, 0x90f6, 0x63e8, 0x21e4, 0x63e4,
		0x21e2, 0x63e2, 0x21f6, 0x63f6, 0xf614, 0x617e, 0xf612, 0xe414,
		0xec34, 0xe412, 0xec32, 0xc834, 0xd874, 0xc832, 0xd872, 0x9074,
		0xb0f4, 0x9072, 0xb0f2, 0x20f4, 0x61f4, 0x20f2, 0x61f2, 0xf60a,
		0xe40a, 0xec1a, 0xc81a, 0xd83a, 0x903a, 0xb07a, 0xe2a0, 0xf158,
		0xf8ae, 0xe290, 0xf14c, 0xe288, 0xf146, 0xe284, 0xe282, 0xc5a0,
		0xe2d8, 0xf16e, 0xc590, 0xe2cc, 0xc588, 0xe2c6, 0xc584, 0xc582,
		0x8ba0, 0xc5d8, 0xe2ee, 0x8b90, 0xc5cc, 0x8b88, 0xc5c6, 0x8b84,
		0x8b82, 0x17a0, 0x8bd8, 0xc5ee, 0x1790, 0x8bcc, 0x1788, 0x8bc6,
		0x1784, 0x1782, 0x17d8, 0x8bee, 0x17cc, 0x17c6, 0x17ee, 0xf350,
		0xf9ac, 0x35f8, 0xf348, 0xf9a6, 0x34fc, 0xf344, 0x347e, 0xf342,
		0xe250, 0xf12c, 0xe6d0, 0xe248, 0xf126, 0xe6c8, 0xf366, 0xe6c4,
		0xe242, 0xe6c2, 0xc4d0, 0xe26c, 0xcdd0, 0xc4c8, 0xe266, 0xcdc8,
		0xe6e6, 0xcdc4, 0xc4c2, 0xcdc2, 0x89d0, 0xc4ec, 0x9bd0, 0x89c8,
		0xc4e6, 0x9bc8, 0xcde6, 0x9bc4, 0x89c2, 0x9bc2, 0x13d0, 0x89ec,
		0x37d0, 0x13c8, 0x89e6, 0x37c8, 0x9be6, 0x37c4, 0x13c2, 0x37c2,
		0x13ec, 0x37ec, 0x13e6, 0x37e6, 0xfba8, 0x75f0, 0xbafc, 0xfba4,
		0x74f8, 0xba7e, 0xfba2, 0x747c, 0x743e, 0xf328, 0xf996, 0x32fc,
		0xf768, 0xfbb6, 0x76fc, 0x327e, 0xf764, 0xf322, 0x767e, 0xf762,
		0xe228, 0xf116, 0xe668, 0xe224, 0xeee8, 0xf776, 0xe222, 0xeee4,
		0xe662, 0xeee2, 0xc468, 0xe236, 0xcce8, 0xc464, 0xdde8, 0xcce4,
		0xc462, 0xdde4, 0xcce2, 0xdde2, 0x88e8, 0xc476, 0x99e8, 0x88e4,
		0xbbe8, 0x99e4, 0x88e2, 0xbbe4, 0x99e2, 0xbbe2, 0x11e8, 0x88f6,
		0x33e8, 0x11e4, 0x77e8, 0x33e4, 0x11e2, 0x77e4, 0x33e2, 0x77e2,
		0x11f6, 0x33f6, 0xfb94, 0x72f8, 0xb97e, 0xfb92, 0x727c, 0x723e,
		0xf314, 0x317e, 0xf734, 0xf312, 0x737e, 0xf732, 0xe214, 0xe634,
		0xe212, 0xee74, 0xe632, 0xee72, 0xc434, 0xcc74, 0xc432, 0xdcf4,
		0xcc72, 0xdcf2, 0x8874, 0x98f4, 0x8872, 0xb9f4, 0x98f2, 0xb9f2,
		0x10f4, 0x31f4, 0x10f2, 0x73f4, 0x31f2, 0x73f2, 0xfb8a, 0x717c,
		0x713e, 0xf30a, 0xf71a, 0xe20a, 0xe61a, 0xee3a, 0xc41a, 0xcc3a,
		0xdc7a, 0x883a, 0x987a, 0xb8fa, 0x107a, 0x30fa, 0x71fa, 0x70be,
		0xe150, 0xf0ac, 0xe148, 0xf0a6, 0xe144, 0xe142, 0xc2d0, 0xe16c,
		0xc2c8, 0xe166, 0xc2c4, 0xc2c2, 0x85d0, 0xc2ec, 0x85c8, 0xc2e6,
		0x85c4, 0x85c2, 0x0bd0, 0x85ec, 0x0bc8, 0x85e6, 0x0bc4, 0x0bc2,
		0x0bec, 0x0be6, 0xf1a8, 0xf8d6, 0x1afc, 0xf1a4, 0x1a7e, 0xf1a2,
		0xe128, 0xf096, 0xe368, 0xe124, 0xe364, 0xe122, 0xe362, 0xc268,
		0xe136, 0xc6e8, 0xc264, 0xc6e4, 0xc262, 0xc6e2, 0x84e8, 0xc276,
		0x8de8, 0x84e4, 0x8de4, 0x84e2, 0x8de2, 0x09e8, 0x84f6, 0x1be8,
		0x09e4, 0x1be4, 0x09e2, 0x1be2, 0x09f6, 0x1bf6, 0xf9d4, 0x3af8,
		0x9d7e, 0xf9d2, 0x3a7c, 0x3a3e, 0xf194, 0x197e, 0xf3b4, 0xf192,
		0x3b7e, 0xf3b2, 0xe114, 0xe334, 0xe112, 0xe774, 0xe332, 0xe772,
		0xc234, 0xc674, 0xc232, 0xcef4, 0xc672, 0xcef2, 0x8474, 0x8cf4,
		0x8472, 0x9df4, 0x8cf2, 0x9df2, 0x08f4, 0x19f4, 0x08f2, 0x3bf4,
		0x19f2, 0x3bf2, 0x7af0, 0xbd7c, 0x7a78, 0xbd3e, 0x7a3c, 0x7a1e,
		0xf9ca, 0x397c, 0xfbda, 0x7b7c, 0x393e, 0x7b3e, 0xf18a, 0xf39a,
		0xf7ba, 0xe10a, 0xe31a, 0xe73a, 0xef7a, 0xc21a, 0xc63a, 0xce7a,
		0xdefa, 0x843a, 0x8c7a, 0x9cfa, 0xbdfa, 0x087a, 0x18fa, 0x39fa,
		0x7978, 0xbcbe, 0x793c, 0x791e, 0x38be, 0x79be, 0x78bc, 0x789e,
		0x785e, 0xe0a8, 0xe0a4, 0xe0a2, 0xc168, 0xe0b6, 0xc164, 0xc162,
		0x82e8, 0xc176, 0x82e4, 0x82e2, 0x05e8, 0x82f6, 0x05e4, 0x05e2,
		0x05f6, 0xf0d4, 0x0d7e, 0xf0d2, 0xe094, 0xe1b4, 0xe092, 0xe1b2,
		0xc134, 0xc374, 0xc132, 0xc372, 0x8274, 0x86f4, 0x8272, 0x86f2,
		0x04f4, 0x0df4, 0x04f2, 0x0df2, 0xf8ea, 0x1d7c, 0x1d3e, 0xf0ca,
		0xf1da, 0xe08a, 0xe19a, 0xe3ba, 0xc11a, 0xc33a, 0xc77a, 0x823a,
		0x867a, 0x8efa, 0x047a, 0x0cfa, 0x1dfa, 0x3d78, 0x9ebe, 0x3d3c,
		0x3d1e, 0x1cbe, 0x3dbe, 0x7d70, 0xbebc, 0x7d38, 0xbe9e, 0x7d1c,
		0x7d0e, 0x3cbc, 0x7dbc, 0x3c9e, 0x7d9e, 0x7cb8, 0xbe5e, 0x7c9c,
		0x7c8e, 0x3c5e, 0x7cde, 0x7c5c, 0x7c4e, 0x7c2e, 0xc0b4, 0xc0b2,
		0x8174, 0x8172, 0x02f4, 0x02f2, 0xe0da, 0xc09a, 0xc1ba, 0x813a,
		0x837a, 0x027a, 0x06fa, 0x0ebe, 0x1ebc, 0x1e9e, 0x3eb8, 0x9f5e,
		0x3e9c, 0x3e8e, 0x1e5e, 0x3ede, 0x7eb0, 0xbf5c, 0x7e98, 0xbf4e,
		0x7e8c, 0x7e86, 0x3e5c, 0x7edc, 0x3e4e, 0x7ece, 0x7e58, 0xbf2e,
		0x7e4c, 0x7e46, 0x3e2e, 0x7e6e, 0x7e2c, 0x7e26, 0x0f5e, 0x1f5c,
		0x1f4e, 0x3f58, 0x9fae, 0x3f4c, 0x3f46, 0x1f2e, 0x3f6e, 0x3f2c,
		0x3f26
	},
	{
	///6
		0xabe0, 0xd5f8, 0x53c0, 0xa9f0, 0xd4fc, 0x51e0, 0xa8f8, 0xd47e,
		0x50f0, 0xa87c, 0x5078, 0xfad0, 0x5be0, 0xadf8, 0xfac8, 0x59f0,
		0xacfc, 0xfac4, 0x58f8, 0xac7e, 0xfac2, 0x587c, 0xf5d0, 0xfaec,
		0x5df8, 0xf5c8, 0xfae6, 0x5cfc, 0xf5c4, 0x5c7e, 0xf5c2, 0xebd0,
		0xf5ec, 0xebc8, 0xf5e6, 0xebc4, 0xebc2, 0xd7d0, 0xebec, 0xd7c8,
		0xebe6, 0xd7c4, 0xd7c2, 0xafd0, 0xd7ec, 0xafc8, 0xd7e6, 0xafc4,
		0x4bc0, 0xa5f0, 0xd2fc, 0x49e0, 0xa4f8, 0xd27e, 0x48f0, 0xa47c,
		0x4878, 0xa43e, 0x483c, 0xfa68, 0x4df0, 0xa6fc, 0xfa64, 0x4cf8,
		0xa67e, 0xfa62, 0x4c7c, 0x4c3e, 0xf4e8, 0xfa76, 0x4efc, 0xf4e4,
		0x4e7e, 0xf4e2, 0xe9e8, 0xf4f6, 0xe9e4, 0xe9e2, 0xd3e8, 0xe9f6,
		0xd3e4, 0xd3e2, 0xa7e8, 0xd3f6, 0xa7e4, 0xa7e2, 0x45e0, 0xa2f8,
		0xd17e, 0x44f0, 0xa27c, 0x4478, 0xa23e, 0x443c, 0x441e, 0xfa34,
		0x46f8, 0xa37e, 0xfa32, 0x467c, 0x463e, 0xf474, 0x477e, 0xf472,
		0xe8f4, 0xe8f2, 0xd1f4, 0xd1f2, 0xa3f4, 0xa3f2, 0x42f0, 0xa17c,
		0x4278, 0xa13e, 0x423c, 0x421e, 0xfa1a, 0x437c, 0x433e, 0xf43a,
		0xe87a, 0xd0fa, 0x4178, 0xa0be, 0x413c, 0x411e, 0x41be, 0x40bc,
		0x409e, 0x2bc0, 0x95f0, 0xcafc, 0x29e0, 0x94f8, 0xca7e, 0x28f0,
		0x947c, 0x2878, 0x943e, 0x283c, 0xf968, 0x2df0, 0x96fc, 0xf964,
		0x2cf8, 0x967e, 0xf962, 0x2c7c, 0x2c3e, 0xf2e8, 0xf976, 0x2efc,
		0xf2e4, 0x2e7e, 0xf2e2, 0xe5e8, 0xf2f6, 0xe5e4, 0xe5e2, 0xcbe8,
		0xe5f6, 0xcbe4, 0xcbe2, 0x97e8, 0xcbf6, 0x97e4, 0x97e2, 0xb5e0,
		0xdaf8, 0xed7e, 0x69c0, 0xb4f0, 0xda7c, 0x68e0, 0xb478, 0xda3e,
		0x6870, 0xb43c, 0x6838, 0xb41e, 0x681c, 0x25e0, 0x92f8, 0xc97e,
		0x6de0, 0x24f0, 0x927c, 0x6cf0, 0xb67c, 0x923e, 0x6c78, 0x243c,
		0x6c3c, 0x241e, 0x6c1e, 0xf934, 0x26f8, 0x937e, 0xfb74, 0xf932,
		0x6ef8, 0x267c, 0xfb72, 0x6e7c, 0x263e, 0x6e3e, 0xf274, 0x277e,
		0xf6f4, 0xf272, 0x6f7e, 0xf6f2, 0xe4f4, 0xedf4, 0xe4f2, 0xedf2,
		0xc9f4, 0xdbf4, 0xc9f2, 0xdbf2, 0x93f4, 0x93f2, 0x65c0, 0xb2f0,
		0xd97c, 0x64e0, 0xb278, 0xd93e, 0x6470, 0xb23c, 0x6438, 0xb21e,
		0x641c, 0x640e, 0x22f0, 0x917c, 0x66f0, 0x2278, 0x913e, 0x6678,
		0xb33e, 0x663c, 0x221e, 0x661e, 0xf91a, 0x237c, 0xfb3a, 0x677c,
		0x233e, 0x673e, 0xf23a, 0xf67a, 0xe47a, 0xecfa, 0xc8fa, 0xd9fa,
		0x91fa, 0x62e0, 0xb178, 0xd8be, 0x6270, 0xb13c, 0x6238, 0xb11e,
		0x621c, 0x620e, 0x2178, 0x90be, 0x6378, 0x213c, 0x633c, 0x211e,
		0x631e, 0x21be, 0x63be, 0x6170, 0xb0bc, 0x6138, 0xb09e, 0x611c,
		0x610e, 0x20bc, 0x61bc, 0x209e, 0x619e, 0x60b8, 0xb05e, 0x609c,
		0x608e, 0x205e, 0x60de, 0x605c, 0x604e, 0x15e0, 0x8af8, 0xc57e,
		0x14f0, 0x8a7c, 0x1478, 0x8a3e, 0x143c, 0x141e, 0xf8b4, 0x16f8,
		0x8b7e, 0xf8b2, 0x167c, 0x163e, 0xf174, 0x177e, 0xf172, 0xe2f4,
		0xe2f2, 0xc5f4, 0xc5f2, 0x8bf4, 0x8bf2, 0x35c0, 0x9af0, 0xcd7c,
		0x34e0, 0x9a78, 0xcd3e, 0x3470, 0x9a3c, 0x3438, 0x9a1e, 0x341c,
		0x340e, 0x12f0, 0x897c, 0x36f0, 0x1278, 0x893e, 0x3678, 0x9b3e,
		0x363c, 0x121e, 0x361e, 0xf89a, 0x137c, 0xf9ba, 0x377c, 0x133e,
		0x373e, 0xf13a, 0xf37a, 0xe27a, 0xe6fa, 0xc4fa, 0xcdfa, 0x89fa,
		0xbae0, 0xdd78, 0xeebe, 0x74c0, 0xba70, 0xdd3c, 0x7460, 0xba38,
		0xdd1e, 0x7430, 0xba1c, 0x7418, 0xba0e, 0x740c, 0x32e0, 0x9978,
		0xccbe, 0x76e0, 0x3270, 0x993c, 0x7670, 0xbb3c, 0x991e, 0x7638,
		0x321c, 0x761c, 0x320e, 0x760e, 0x1178, 0x88be, 0x3378, 0x113c,
		0x7778, 0x333c, 0x111e, 0x773c, 0x331e, 0x771e, 0x11be, 0x33be,
		0x77be, 0x72c0, 0xb970, 0xdcbc, 0x7260, 0xb938, 0xdc9e, 0x7230,
		0xb91c, 0x7218, 0xb90e, 0x720c, 0x7206, 0x3170, 0x98bc, 0x7370,
		0x3138, 0x989e, 0x7338, 0xb99e, 0x731c, 0x310e, 0x730e, 0x10bc,
		0x31bc, 0x109e, 0x73bc, 0x319e, 0x739e, 0x7160, 0xb8b8, 0xdc5e,
		0x7130, 0xb89c, 0x7118, 0xb88e, 0x710c, 0x7106, 0x30b8, 0x985e,
		0x71b8, 0x309c, 0x719c, 0x308e, 0x718e, 0x105e, 0x30de, 0x71de,
		0x70b0, 0xb85c, 0x7098, 0xb84e, 0x708c, 0x7086, 0x305c, 0x70dc,
		0x304e, 0x70ce, 0x7058, 0xb82e, 0x704c, 0x7046, 0x302e, 0x706e,
		0x702c, 0x7026, 0x0af0, 0x857c, 0x0a78, 0x853e, 0x0a3c, 0x0a1e,
		0x0b7c, 0x0b3e, 0xf0ba, 0xe17a, 0xc2fa, 0x85fa, 0x1ae0, 0x8d78,
		0xc6be, 0x1a70, 0x8d3c, 0x1a38, 0x8d1e, 0x1a1c, 0x1a0e, 0x0978,
		0x84be, 0x1b78, 0x093c, 0x1b3c, 0x091e, 0x1b1e, 0x09be, 0x1bbe,
		0x3ac0, 0x9d70, 0xcebc, 0x3a60, 0x9d38, 0xce9e, 0x3a30, 0x9d1c,
		0x3a18, 0x9d0e, 0x3a0c, 0x3a06, 0x1970, 0x8cbc, 0x3b70, 0x1938,
		0x8c9e, 0x3b38, 0x191c, 0x3b1c, 0x190e, 0x3b0e, 0x08bc, 0x19bc,
		0x089e, 0x3bbc, 0x199e, 0x3b9e, 0xbd60, 0xdeb8, 0xef5e, 0x7a40,
		0xbd30, 0xde9c, 0x7a20, 0xbd18, 0xde8e, 0x7a10, 0xbd0c, 0x7a08,
		0xbd06, 0x7a04, 0x3960, 0x9cb8, 0xce5e, 0x7b60, 0x3930, 0x9c9c,
		0x7b30, 0xbd9c, 0x9c8e, 0x7b18, 0x390c, 0x7b0c, 0x3906, 0x7b06,
		0x18b8, 0x8c5e, 0x39b8, 0x189c, 0x7bb8, 0x399c, 0x188e, 0x7b9c,
		0x398e, 0x7b8e, 0x085e, 0x18de, 0x39de, 0x7bde, 0x7940, 0xbcb0,
		0xde5c, 0x7920, 0xbc98, 0xde4e, 0x7910, 0xbc8c, 0x7908, 0xbc86,
		0x7904, 0x7902, 0x38b0, 0x9c5c, 0x79b0, 0x3898, 0x9c4e, 0x7998,
		0xbcce, 0x798c, 0x3886, 0x7986, 0x185c, 0x38dc, 0x184e, 0x79dc,
		0x38ce, 0x79ce, 0x78a0, 0xbc58, 0xde2e, 0x7890, 0xbc4c, 0x7888,
		0xbc46, 0x7884, 0x7882, 0x3858, 0x9c2e, 0x78d8, 0x384c, 0x78cc,
		0x3846, 0x78c6, 0x182e, 0x386e, 0x78ee, 0x7850, 0xbc2c, 0x7848,
		0xbc26, 0x7844, 0x7842, 0x382c, 0x786c, 0x3826, 0x7866, 0x7828,
		0xbc16, 0x7824, 0x7822, 0x3816, 0x7836, 0x0578, 0x82be, 0x053c,
		0x051e, 0x05be, 0x0d70, 0x86bc, 0x0d38, 0x869e, 0x0d1c, 0x0d0e,
		0x04bc, 0x0dbc, 0x049e, 0x0d9e, 0x1d60, 0x8eb8, 0xc75e, 0x1d30,
		0x8e9c, 0x1d18, 0x8e8e, 0x1d0c, 0x1d06, 0x0cb8, 0x865e, 0x1db8,
		0x0c9c, 0x1d9c, 0x0c8e, 0x1d8e, 0x045e, 0x0cde, 0x1dde, 0x3d40,
		0x9eb0, 0xcf5c, 0x3d20, 0x9e98, 0xcf4e, 0x3d10, 0x9e8c, 0x3d08,
		0x9e86, 0x3d04, 0x3d02, 0x1cb0, 0x8e5c, 0x3db0, 0x1c98, 0x8e4e,
		0x3d98, 0x9ece, 0x3d8c, 0x1c86, 0x3d86, 0x0c5c, 0x1cdc, 0x0c4e,
		0x3ddc, 0x1cce, 0x3dce, 0xbea0, 0xdf58, 0xefae, 0xbe90, 0xdf4c,
		0xbe88, 0xdf46, 0xbe84, 0xbe82, 0x3ca0, 0x9e58, 0xcf2e, 0x7da0,
		0x3c90, 0x9e4c, 0x7d90, 0xbecc, 0x9e46, 0x7d88, 0x3c84, 0x7d84,
		0x3c82, 0x7d82, 0x1c58, 0x8e2e, 0x3cd8, 0x1c4c, 0x7dd8, 0x3ccc,
		0x1c46, 0x7dcc, 0x3cc6, 0x7dc6, 0x0c2e, 0x1c6e, 0x3cee, 0x7dee,
		0xbe50, 0xdf2c, 0xbe48, 0xdf26, 0xbe44, 0xbe42, 0x3c50, 0x9e2c,
		0x7cd0, 0x3c48, 0x9e26, 0x7cc8, 0xbe66, 0x7cc4, 0x3c42, 0x7cc2,
		0x1c2c, 0x3c6c, 0x1c26, 0x7cec, 0x3c66, 0x7ce6, 0xbe28, 0xdf16,
		0xbe24, 0xbe22, 0x3c28, 0x9e16, 0x7c68, 0x3c24, 0x7c64, 0x3c22,
		0x7c62, 0x1c16, 0x3c36, 0x7c76, 0xbe14, 0xbe12, 0x3c14, 0x7c34,
		0x3c12, 0x7c32, 0x02bc, 0x029e, 0x06b8, 0x835e, 0x069c, 0x068e,
		0x025e, 0x06de, 0x0eb0, 0x875c, 0x0e98, 0x874e, 0x0e8c, 0x0e86,
		0x065c, 0x0edc, 0x064e, 0x0ece, 0x1ea0, 0x8f58, 0xc7ae, 0x1e90,
		0x8f4c, 0x1e88, 0x8f46, 0x1e84, 0x1e82, 0x0e58, 0x872e, 0x1ed8,
		0x8f6e, 0x1ecc, 0x0e46, 0x1ec6, 0x062e, 0x0e6e, 0x1eee, 0x9f50,
		0xcfac, 0x9f48, 0xcfa6, 0x9f44, 0x9f42, 0x1e50, 0x8f2c, 0x3ed0,
		0x9f6c, 0x8f26, 0x3ec8, 0x1e44, 0x3ec4, 0x1e42, 0x3ec2, 0x0e2c,
		0x1e6c, 0x0e26, 0x3eec, 0x1e66, 0x3ee6, 0xdfa8, 0xefd6, 0xdfa4,
		0xdfa2, 0x9f28, 0xcf96, 0xbf68, 0x9f24, 0xbf64, 0x9f22, 0xbf62,
		0x1e28, 0x8f16, 0x3e68, 0x1e24, 0x7ee8, 0x3e64, 0x1e22, 0x7ee4,
		0x3e62, 0x7ee2, 0x0e16, 0x1e36, 0x3e76, 0x7ef6, 0xdf94, 0xdf92,
		0x9f14, 0xbf34, 0x9f12, 0xbf32, 0x1e14, 0x3e34, 0x1e12, 0x7e74,
		0x3e32, 0x7e72, 0xdf8a, 0x9f0a, 0xbf1a, 0x1e0a, 0x3e1a, 0x7e3a,
		0x035c, 0x034e, 0x0758, 0x83ae, 0x074c, 0x0746, 0x032e, 0x076e,
		0x0f50, 0x87ac, 0x0f48, 0x87a6, 0x0f44, 0x0f42, 0x072c, 0x0f6c,
		0x0726, 0x0f66, 0x8fa8, 0xc7d6, 0x8fa4, 0x8fa2, 0x0f28, 0x8796,
		0x1f68, 0x8fb6, 0x1f64, 0x0f22, 0x1f62, 0x0716, 0x0f36, 0x1f76,
		0xcfd4, 0xcfd2, 0x8f94, 0x9fb4, 0x8f92, 0x9fb2, 0x0f14, 0x1f34,
		0x0f12, 0x3f74, 0x1f32, 0x3f72, 0xcfca, 0x8f8a, 0x9f9a, 0x0f0a,
		0x1f1a, 0x3f3a, 0x03ac, 0x03a6, 0x07a8, 0x83d6, 0x07a4, 0x07a2,
		0x0396, 0x07b6, 0x87d4, 0x87d2, 0x0794, 0x0fb4, 0x0792, 0x0fb2,
		0xc7ea
	}
};

u16	const	ERROR_LEVEL0[2]={
	27,917
};

u16	const	ERROR_LEVEL1[4]={
	522, 568, 723, 809
};

u16	const	ERROR_LEVEL2[8]={
	237, 308, 436, 284, 646, 653, 428, 379
};

u16	const	ERROR_LEVEL3[16]={
	274, 562, 232, 755, 599, 524, 801, 132, 295, 116, 442, 428, 295,  42, 176,  65
};

u16	const	ERROR_LEVEL4[32]={
	361, 575, 922, 525, 176, 586, 640, 321, 536, 742, 677, 742, 687, 284, 193, 517,
	273, 494, 263, 147, 593, 800, 571, 320, 803, 133, 231, 390, 685, 330,  63, 410
};

u16	const	ERROR_LEVEL5[64]={
	539, 422,   6,  93, 862, 771, 453, 106, 610, 287, 107, 505, 733, 877, 381, 612,
	723, 476, 462, 172, 430, 609, 858, 822, 543, 376, 511, 400, 672, 762, 283, 184,
	440,  35, 519,  31, 460, 594, 225, 535, 517, 352, 605, 158, 651, 201, 488, 502,
	648, 733, 717,  83, 404,  97, 280, 771, 840, 629,   4, 381, 843, 623, 264, 543
};

u16	const	ERROR_LEVEL6[128]={
	521, 310, 864, 547, 858, 580, 296, 379,  53, 779, 897, 444, 400, 925, 749, 415,
	822,  93, 217, 208, 928, 244, 583, 620, 246, 148, 447, 631, 292, 908, 490, 704,
	516, 258, 457, 907, 594, 723, 674, 292, 272,  96, 684, 432, 686, 606, 860, 569,
	193, 219, 129, 186, 236, 287, 192, 775, 278, 173,  40, 379, 712, 463, 646, 776,
	171, 491, 297, 763, 156, 732,  95, 270, 447,  90, 507,  48, 228, 821, 808, 898,
	784, 663, 627, 378, 382, 262, 380, 602, 754, 336,  89, 614,  87, 432, 670, 616,
	157, 374, 242, 726, 600, 269, 375, 898, 845, 454, 354, 130, 814, 587, 804,  34,
	211, 330, 539, 297, 827, 865,  37, 517, 834, 315, 550,  86, 801,   4, 108, 539
};

u16	const	ERROR_LEVEL7[256]={
	524, 894,  75, 766, 882, 857,  74, 204,  82, 586, 708, 250, 905, 786, 138, 720,
	858, 194, 311, 913, 275, 190, 375, 850, 438, 733, 194, 280, 201, 280, 828, 757,
	710, 814, 919,  89,  68, 569,  11, 204, 796, 605, 540, 913, 801, 700, 799, 137,
	439, 418, 592, 668, 353, 859, 370, 694, 325, 240, 216, 257, 284, 549, 209, 884,
	315,  70, 329, 793, 490, 274, 877, 162, 749, 812, 684, 461, 334, 376, 849, 521,
	307, 291, 803, 712,  19, 358, 399, 908, 103, 511,  51,   8, 517, 225, 289, 470,
	637, 731,  66, 255, 917, 269, 463, 830, 730, 433, 848, 585, 136, 538, 906,  90,
	2, 290, 743, 199, 655, 903, 329,  49, 802, 580, 355, 588, 188, 462,  10, 134,
	628, 320, 479, 130, 739,  71, 263, 318, 374, 601, 192, 605, 142, 673, 687, 234,
	722, 384, 177, 752, 607, 640, 455, 193, 689, 707, 805, 641,  48,  60, 732, 621,
	895, 544, 261, 852, 655, 309, 697, 755, 756,  60, 231, 773, 434, 421, 726, 528,
	503, 118,  49, 795,  32, 144, 500, 238, 836, 394, 280, 566, 319,   9, 647, 550,
	73, 914, 342, 126,  32, 681, 331, 792, 620,  60, 609, 441, 180, 791, 893, 754,
	605, 383, 228, 749, 760, 213,  54, 297, 134,  54, 834, 299, 922, 191, 910, 532,
	609, 829, 189,  20, 167,  29, 872, 449,  83, 402,  41, 656, 505, 579, 481, 173,
	404, 251, 688,  95, 497, 555, 642, 543, 307, 159, 924, 558, 648,  55, 497,  10
};

u16	const	ERROR_LEVEL8[512]={
	352,  77, 373, 504,  35, 599, 428, 207, 409, 574, 118, 498, 285, 380, 350, 492,
	197, 265, 920, 155, 914, 299, 229, 643, 294, 871, 306,  88,  87, 193, 352, 781,
	846,  75, 327, 520, 435, 543, 203, 666, 249, 346, 781, 621, 640, 268, 794, 534,
	539, 781, 408, 390, 644, 102, 476, 499, 290, 632, 545,  37, 858, 916, 552,  41,
	542, 289, 122, 272, 383, 800, 485,  98, 752, 472, 761, 107, 784, 860, 658, 741,
	290, 204, 681, 407, 855,  85,  99,  62, 482, 180,  20, 297, 451, 593, 913, 142,
	808, 684, 287, 536, 561,  76, 653, 899, 729, 567, 744, 390, 513, 192, 516, 258,
	240, 518, 794, 395, 768, 848,  51, 610, 384, 168, 190, 826, 328, 596, 786, 303,
	570, 381, 415, 641, 156, 237, 151, 429, 531, 207, 676, 710,  89, 168, 304, 402,
	40, 708, 575, 162, 864, 229,  65, 861, 841, 512, 164, 477, 221,  92, 358, 785,
	288, 357, 850, 836, 827, 736, 707,  94,   8, 494, 114, 521,   2, 499, 851, 543,
	152, 729, 771,  95, 248, 361, 578, 323, 856, 797, 289,  51, 684, 466, 533, 820,
	669,  45, 902, 452, 167, 342, 244, 173,  35, 463, 651,  51, 699, 591, 452, 578,
	37, 124, 298, 332, 552,  43, 427, 119, 662, 777, 475, 850, 764, 364, 578, 911,
	283, 711, 472, 420, 245, 288, 594, 394, 511, 327, 589, 777, 699, 688,  43, 408,
	842, 383, 721, 521, 560, 644, 714, 559,  62, 145, 873, 663, 713, 159, 672, 729,
	624,  59, 193, 417, 158, 209, 563, 564, 343, 693, 109, 608, 563, 365, 181, 772,
	677, 310, 248, 353, 708, 410, 579, 870, 617, 841, 632, 860, 289, 536,  35, 777,
	618, 586, 424, 833,  77, 597, 346, 269, 757, 632, 695, 751, 331, 247, 184,  45,
	787, 680,  18,  66, 407, 369,  54, 492, 228, 613, 830, 922, 437, 519, 644, 905,
	789, 420, 305, 441, 207, 300, 892, 827, 141, 537, 381, 662, 513,  56, 252, 341,
	242, 797, 838, 837, 720, 224, 307, 631,  61,  87, 560, 310, 756, 665, 397, 808,
	851, 309, 473, 795, 378,  31, 647, 915, 459, 806, 590, 731, 425, 216, 548, 249,
	321, 881, 699, 535, 673, 782, 210, 815, 905, 303, 843, 922, 281,  73, 469, 791,
	660, 162, 498, 308, 155, 422, 907, 817, 187,  62,  16, 425, 535, 336, 286, 437,
	375, 273, 610, 296, 183, 923, 116, 667, 751, 353,  62, 366, 691, 379, 687, 842,
	37, 357, 720, 742, 330,   5,  39, 923, 311, 424, 242, 749, 321,  54, 669, 316,
	342, 299, 534, 105, 667, 488, 640, 672, 576, 540, 316, 486, 721, 610,  46, 656,
	447, 171, 616, 464, 190, 531, 297, 321, 762, 752, 533, 175, 134,  14, 381, 433,
	717,  45, 111,  20, 596, 284, 736, 138, 646, 411, 877, 669, 141, 919,  45, 780,
	407, 164, 332, 899, 165, 726, 600, 325, 498, 655, 357, 752, 768, 223, 849, 647,
	63, 310, 863, 251, 366, 304, 282, 738, 675, 410, 389, 244,  31, 121, 303, 263
};
#endif

#endif /* BARCODE_H定义结束 */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
