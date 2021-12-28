/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   一维条码相关的程序.
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
#include	"barcode.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"timconfig.h"
#include	"feedpaper.h"
#include	"print.h"
#include	"heat.h"
#include	"charmaskbuf.h"
#include	"ean.h"

extern TypeDef_StructHeatCtrl	g_tHeatCtrl;
//extern TypeDef_StructLinePrtBuf	g_tLinePrtBuf;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : 函数名
* Description    : 描述
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
/*****************************
*EAN 用于确定左侧数据字符的子集,"0"表示A子集,"1"表示B子集 
*从D0～D5分别表示第从右到左的7～12位
*根据国家码的头一位确定左资料符的编码方式,数组中值的低6位有效
*******************************/
const uint8_t EANCharSel[] =
{
	0x00,0x0B,0x0D,0x0E,0x13,0x19,0x1C,0x15,0x16,0x1A,
};

/***用于确定数据字符的子集,"0"表示A子集,"1"表示B子集 
*从D0～D5分别表示从右到左的第2～7位
*******************************/
const uint8_t UPCCharSel[] =
{
	0x38,0x34,0x32,0x31,0x2C,0x26,0x23,0x2A,0x29,0x25 
};

/***用于确定UPC-E的6位数据字符的子集,"0"表示A子集,"1"表示B子集 
*从D5～D0分别表示从左到右的6位资料码的5～0位,编码方式同UPC-A
*******************************/
const uint8_t UPCECharSel[] =
{
	0x38,0x34,0x32,0x31,0x2C,0x26,0x23,0x2A,0x29,0x25 
};

/**********EAN13 A字符集字模式 *******/
/*ENA-左资料码A*/
const uint8_t EAN13_A[] = 
{
	0x0d,0x19,0x13,0x3d,0x23,0x31,0x2f,0x3b,0x37,0x0b
};

/**********EAN13 B字符集字模式 *******/
/*ENA-左资料码B*/
const uint8_t EAN13_B[] = 
{
	0x27,0x33,0x1b,0x21,0x1d,0x39,0x05,0x11,0x09,0x17
};

/**********EAN13 C字符集字模式 *******/
/*ENA-右资料码,编码规则等同于UPC-A右资料编码*/
const uint8_t EAN13_C[] = 
{
	0x72,0x66,0x6c,0x42,0x5c,0x4e,0x50,0x44,0x48,0x74
};

/* ENA起始符编码 */
const uint8_t	ENAStartCode = 0x05;

/* ENA结束符编码 */
const uint8_t	ENAStopCode = 0x05;

/* ENA中间符编码 */
const uint8_t	ENADivideCode = 0x0A;

/********* Code39字符集 ********************/
const uint8_t *codebuf39 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%*";

/***********CODE39 条码条模***************************
*条模数据低10位有效始,每位交替表示条、空,共10位(包括分隔符),
*0表示窄条,1表示宽条.分隔符为一窄空
*************************************************************/
const uint16_t Code39buf[] = 
{
	0x068,0x242,0x0C2,0x2C0,0x062,0x260,0x0E0,0x04A,0x248,0x0C8,	//0～9
	0x212,0x092,0x290,0x032,0x230,0x0B0,0x01A,0x218,0x098,0x038,	//A～J
	0x206,0x086,0x284,0x026,0x224,0x0A4,0x00E,0x20C,0x08C,0x02C,	//K～T
	0x302,0x182,0x380,0x122,0x320,0x1A0,							//U~Z
	0x10A,0x308,0x188,0x150,0x144,0x114,0x054,				//-. $/+%*
	0x128,															//起始、结束符
};

/***********ITF 条码条模*************************************
*条模数据低5位有效，0表示窄条，1表示宽条。
*************************************************************/
const uint8_t CodeItf[] =
{
	0x06,0x11,0x09,0x18,0x05,0x14,0x0c,0x03,0x12,0x0a,
};

/*********** CODABAR 字符集********************/
const uint8_t * codabar = "0123456789$-:/.+ABCD";

/*********** CODABAR 条码字模***********************************
*条模数据从最高位开始,每位交替表示条、空,共8位(包括分隔符),
*0表示窄条,1表示宽条.
*************************************************************/
const uint8_t Codabarbuf[] =
{
	0x07,0x0d,0x13,0xc1,0x25,0x85,0x43,0x49,0x61,0x91,
	0x31,0x19,0x8b,0xa3,0xa9,0x2b,0x35,0x53,0x17,0x1d,
};

/************ Code93 字符集 ********************/
const uint8_t * codebuf93 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%";

/*********** CODE93 条码字模***********************************
*条模数据低9位(结束符10位)有效,0表示空,1表示条.均为窄条宽
*用多个连续的0或1来表示宽条
*************************************************************/
const uint16_t code93buf[] =
{
	0x114,0x148,0x144,0x142,0x128,0x124,0x122,0x150,0x112,0x10A,	//0～9				 
	0x1A8,0x1A4,0x1A2,0x194,0x192,0x18A,0x168,0x164,0x162,0x134,	//A～J				
	0x11A,0x158,0x14C,0x146,0x12C,0x116,0x1B4,0x1B2,0x1AC,0x1A6,	//K~T			
	0x196,0x19A,0x16C,0x166,0x136,0x13A,		 		//U~Z
	0x12E,0x1D4,0x1D2,0x1CA,0x16E,0x176,0x1AE,	//-. $/+%
	0x126,0x1DA,0x1D6,0x132,	//转换码($)(%)(/)(+)
	0x15E,0x2BD,							//起始符、结束符														
};

/*********Code128 条码条模 *****
*条模数据低11位（结束符13位）有效,每位交替表示条、空,0表示窄条,1表示宽条.
***************************/
const uint16_t CODE128[]=
{
	0x6CC,0x66C,0x666,0x498,0x48C,
	0x44C,0x4C8,0x4C4,0x464,0x648,
	0x644,0x624,0x59C,0x4DC,0x4CE,
	0x5CC,0x4EC,0x4E6,0x672,0x65C,
	0x64E,0x6E4,0x674,0x76E,0x74C,
	0x72C,0x726,0x764,0x734,0x732,
	0x6D8,0x6C6,0x636,0x518,0x458,
	0x446,0x588,0x468,0x462,0x688,
	0x628,0x622,0x5B8,0x58E,0x46E,
	0x5D8,0x5C6,0x476,0x776,0x68E,
	0x62E,0x6E8,0x6E2,0x6EE,0x758,
	0x746,0x716,0x768,0x762,0x71A,
	0x77A,0x642,0x78A,0x530,0x50C,
	0x4B0,0x486,0x42C,0x426,0x590,
	0x584,0x4D0,0x4C2,0x434,0x432,
	0x612,0x650,0x7BA,0x614,0x47A,
	0x53C,0x4BC,0x49E,0x5E4,0x4F4,
	0x4F2,0x7A4,0x794,0x792,0x6DE,
	0x6F6,0x7B6,0x578,0x51E,0x45E,
	0x5E8,0x5E2,0x7A8,0x7A2,0x5DE,
	0x5EE,0x75E,0x7AE,0x684,0x690,
	0x69C,0x18EB,
};

//		0		1		2		3		4		5		6		7		8		9		10	11
enum {M6, M5, M4, M3, M2, M1, S5, S4, S3, S2, S1, CD};

/*******************************************************************************
* Function Name  : EanCheckCode(uint8_t *Inbuf, uint8_t length)
* Description    : 计算Inbuf里指定长度的校验码,并将校验码加在Inbuf里
* Input          : inbuf：输入数据指针，length：数据长度
* Output         :
* Return		 : 	
*******************************************************************************/
void	EanCheckCode(uint8_t * Inbuf, uint8_t Length)
{
	uint8_t	i;
	uint16_t	lsum;
	uint16_t	rsum;
	uint16_t	tsum;
	
	lsum = 0;
	rsum = 0;
	//计算校验和,Inbuf[0]不参与计算校验和
	for (i = 1; i <= Length; i++)
	{
		if (i % 2)
		{
			lsum += (*(Inbuf + Length - i) - '0');		//偶数位加和,需要转换为0~9间的数字
		}
		else
		{
			rsum += (*(Inbuf + Length - i) - '0');		//奇数位加和,需要转换为0~9间的数字
		}
	}
	
	tsum = 3 * lsum + rsum;
	i--;
	*(Inbuf + i) = ((10 - tsum % 10) % 10) + '0';	//如果tsum的个位数为0,则10-tsum个位数=10,此时取值为0
	i++;
	*(Inbuf + i) = '\0';
}

/*******************************************************************************
* Function Name  : FormatOne(uint8_t *Dest, uint16_t Data , uint8_t Blength)
* Description    : 将1个只有窄条的条码符号的条模数据放入指定的缓冲区中
* Input          : Dest：数据存入指针，Data：条模数据，Blength：有效数据位长度
* Output         : 
* Return		 : 	
*******************************************************************************/
uint16_t FormatOne(uint8_t * Dest, uint16_t Data ,uint8_t Blength)
{
	uint8_t	i, j, Temp;
	uint16_t n = 0;
	
	//填充时从指定的最高位开始到最低位结束
	for (i = 1; i <= Blength; i++)
	{
		if (Data & (uint16_t)(1 << (Blength - i)))
			Temp = 0xFF;
		else
			Temp = 0x00;
		for (j = 0; j < g_tBarCodeCtrl.BarCodeThinWidth; j++)
			Dest[n++] = Temp;
	}
	
	return n;
}

/*******************************************************************************
* Function Name  : EnaFillToBuf(uint8_t *Inbuf, uint8_t length,uint8_t PreCode)
* Description    : Inbuf里数据置的为条码数据,存放于g_tUnderlineBuf中
* Input          : inbuf：输入数据指针,
*						length：数据长度
*				   	PreCode：前置码,=0时表示是美国和加拿大
*						Tpye:条码类型,=1,UPC_E;=0,EAN-13,EAN-8,UPC-A
* Output         : 
* Return		 : 	
*******************************************************************************/
uint16_t EnaFillToBuf(uint8_t * inbuf, uint8_t Length, uint8_t PreCode, uint8_t Tpye)
{
	uint8_t i, Temp;
	uint8_t	outbuf[13];
	uint8_t	* p = NULL;
	uint16_t j;
	
	if (Tpye)		//UPC-E
	{
		Temp = UPCCharSel[PreCode];
		//计算条码总宽度,左护线010101,右护线101,无中间线
		j = (7 * Length + 3 + 6) * g_tBarCodeCtrl.BarCodeThinWidth;
	}
	else				//EAN
	{
		Temp = EANCharSel[PreCode];		//决定左资料码的AB排列类别
		//计算条码总宽度,左右护线各3,中间线5,资料码7
		j = (7 * Length + 3 + 5 + 3) * g_tBarCodeCtrl.BarCodeThinWidth;
	}
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
		return 0;		//超过打印区域,不能放下完整的条码,返回0
	
	//for (i = 0; i < (Length / 2); i++)	//左侧资料码,最多7位
	for (i = 0; i < (Length >> 1); i++)	//左侧资料码,最多7位
	{
		if ((uint8_t)(Temp << i) & 0x20)	//D5表示字符集选择,编码为00xx xxxx,位值为0表示A类型编码,位值为1表示B类型编码
		{
			outbuf[i] = EAN13_B[*(inbuf + i) - 0x30];	//接收到的数据为ASCII码值0~9,需要转换成数字值0~9
		}
		else
		{
			outbuf[i] = EAN13_A[*(inbuf + i) - 0x30];
		}
	}
	if (Tpye == 0)		//EAN
	{
		for (; i < Length; i++)		//剩余右侧资料码
		{
		  outbuf[i] = EAN13_C[*(inbuf + i) - 0x30];
		}
	}
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	j = FormatOne(p, 0x05, 0x03);	//起始符,左护线101,长度3,返回值j表示填充点数值
	p += j;		//准备填充左资料码
	
	//for (i = 0; i < (Length / 2); i++)
	for (i = 0; i < (Length >> 1); i++)
	{
		j = FormatOne(p, outbuf[i], 0x07);	//填充左资料码,每个码长度7,
		p += j;
	}
	
	if (Tpye == 0)	//继续ENA填充,UPC_E没有中间分隔符和右侧数据
	{
		j = FormatOne(p, 0x0A, 0x05);	//填充ENA中间分隔符,01010,长度5
		p += j;
		for ( ; i < Length; i++)
		{
			j = FormatOne(p, outbuf[i], 0x07);	//填充右资料码,每个码长度7,
			p += j;
		}
		j = FormatOne(p, 0x05, 0x03);	//填充结束符,右护线101,长度3
		p += j;
	}
	else
	{
		j = FormatOne(p, 0x15, 0x06);	//结束符,UPC_E的结束符不同
		p += j;
	}
	//填充完毕p指向为最后一个条码图形的下一个位置
	j = p - g_tUnderlineBuf;	//j = 原g_tPrtCtrlData.PrtDataDotIndex + 填充的所有码线的长度
	
	return j;		//返回值为从左到右需要打印的下划线的列数
}

/*******************************************************************************
* Function Name  : FormatCodeUpc_A(uint8_t *inbuf)
* Description    : 把inbuf里输入进来的数据转化为UPC_A条码数据,数据输出到g_tUnderlineBuf里.
* Input          : inbuf：输入条码数据
* Output         : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
*******************************************************************************/
uint16_t FormatCodeUpc_A(uint8_t * inbuf)
{
 	EanCheckCode(inbuf, 11);	//计算Inbuf里指定长度的校验码,并将校验码加在Inbuf里
 	return (EnaFillToBuf((inbuf), 12, 0, 0));
}

/*******************************************************************************
* Function Name  : FormatCodeUpc_E(uint8_t *inbuf)
* Description    : 把inbuf里输入进来的数据转化为UPC_A条码数据,数据输出到g_tUnderlineBuf里.
* Input          : inbuf：输入条码数据
* Output         : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
* UPC-E是UPC-A码的简化型式，其编码方式是将UPC-A码整体压缩成短码，以方便使用，
*	因此其编码形式须经由UPC-A码来转换。UPC-E由6位数码与左右护线组成，无中间线。
*	6位数字码的排列为3奇3偶，其排列方法取决於检查码的值
*	UPC-E码只用於国别码为0的商品。
*******************************************************************************/
#if	0   //2016.07.02
//此为修改后的程序,按照8位参数处理
uint16_t FormatCodeUpc_E(uint8_t * inbuf)
{	
	uint8_t i;	
	uint8_t	cCheckValue;
	uint8_t rule;
	uint8_t	outbuf[13];	
	uint8_t	* p = NULL;
	uint16_t j;
	
	if (*inbuf != '0')	//首位必须是0表示国家是美国
		return 0;     // illegal; can't compress
	
	cCheckValue = *(inbuf + 7) - '0';	//检查码无需计算,直接读取,检查码的值决定资料码6位的排列
	rule = UPCCharSel[cCheckValue];		//6位资料码的编码方式规则
	
	//计算条码总宽度,左护线101,右护线010101,无中间线,6位数据,每位数据占用7位
	j = (7 * 6 + 3 + 6) * g_tBarCodeCtrl.BarCodeThinWidth;
	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
		return 0;		//超过打印区域,不能放下完整的条码,返回0
		
	for (i = 0; i < 6; i++)	//6个资料码
	{
		if ((uint8_t)(rule << i) & 0x20)	//D5表示字符集选择,编码为00xx xxxx,位值为0表示A类型编码,位值为1表示B类型编码
		{
			outbuf[i + 1] = EAN13_B[*(inbuf + 1 + i) - 0x30];	//接收到的数据为ASCII码值0~9,需要转换成数字值0~9
		}
		else
		{
			outbuf[i + 1] = EAN13_A[*(inbuf + 1 + i) - 0x30];
		}
	}
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;	

	j = FormatOne(p, 0x05, 0x03);	//起始符,左护线UPC_E的左护线与UPC-A的相同,101,长度3,返回值j表示填充点数值
	p += j;		//准备填充资料码
	
	for (i = 0; i < 6; i++)
	{
		j = FormatOne(p, outbuf[i + 1], 0x07);	//填充资料码,每个码长度7,
		p += j;
	}
	
	j = FormatOne(p, 0x15, 0x06);		//结束符,右护线010101,长度6,返回值j表示填充点数值
	p += j;
	
	j = p - g_tUnderlineBuf;	//j = 原g_tPrtCtrlData.PrtDataDotIndex + 填充的所有码线的长度
	
	return j;		//返回值为从左到右需要打印的下划线的列数
}
#else
//此为原程序,按照11或者12位参数处理
uint16_t FormatCodeUpc_E(uint8_t * inbuf)
{
	uint8_t rule, i;
	uint8_t	outbuf[13];
	
	// general check; m6, s5, and s4 must always be 0
	if ((inbuf[M6] | inbuf[S5] | inbuf[S4]) != '0')
		// mfr# not of form 0mmmmmm and/or sku not of form 00sss
		return 0;     // illegal; can't compress
	
	// else calculate the Rule key:
	/* 计算校验和 */
 	EanCheckCode(inbuf,11);

	/* 开始压缩计算 */
	rule = 0;
	// if m1 is not 0 ...
	if ( inbuf[M1] != '0' )
	{ // ... then s1 must be 5..9, and s2, s3 must both be 0
		if  ( (inbuf[S1] >= '5') && ( (inbuf[S2] | inbuf[S3]) == '0' ) )
			rule = 4;
		// else not valid
	}
	else // m1 is 0, so ...
	{   // ... if m2 is not 0, ...
		if ( inbuf[M2] != '0' )
		{   // ... then s3 must also be 0
				if ( inbuf[S3] == '0' )
						rule = 3;
				// else not valid
		}
		else // m2, m1 are 0, so ...
		{    // ... if m3 is > 2 ...
				if ( inbuf[M3] >= '3' )
						rule = 2;
				else
						rule = 1;
		}
	}
	// quit here if there's no valid rule key
	if (rule == 0)
		return 0;     // illegal; can't compress

	i = 0;
	outbuf[i++] = inbuf[M6];  // always 0
	outbuf[i++] = inbuf[M5];
	outbuf[i++] = inbuf[M4];

	switch (rule)
	{
		case 1:     // Rule E.1: mfr# = 0mmx00 (x = 0..2), sku = 00sss
			outbuf[i++] = inbuf[S3];
			outbuf[i++] = inbuf[S2];
			outbuf[i++] = inbuf[S1];	
			outbuf[i++] = inbuf[M3];	
			outbuf[i++] = inbuf[CD];
			break;
		case 2:     // Rule E.2: mfr# = 0mmx00 (x = 3..9), sku = 000ss
			outbuf[i++] = inbuf[M3];	
			outbuf[i++] = inbuf[S2];
			outbuf[i++] = inbuf[S1];
			outbuf[i++] = '3';	
			outbuf[i++] = inbuf[CD];
			break;
		case 3:     // Rule E.3: mfr# = 0mmmm0, sku = 0000s
			outbuf[i++] = inbuf[M3];
			outbuf[i++] = inbuf[M2];	
			outbuf[i++] = inbuf[S1];	
			outbuf[i++] = '4';	
			outbuf[i++] = inbuf[CD];
			break;
		case 4:     // Rule E.4: mfr# = 0mmmmx (x = 1..9), sku = 0000y (y = 5..9)
			outbuf[i++] = inbuf[M3];
			outbuf[i++] = inbuf[M2];
			outbuf[i++] = inbuf[M1];	
			outbuf[i++] = inbuf[S1];	
			outbuf[i++] = inbuf[CD];
			break;
	}
	outbuf[i++] = '\0';
	strncpy(inbuf,outbuf,9);		//返回HRI字符

 	return (EnaFillToBuf((outbuf + 1), 2*6, (*(outbuf + 7) - '0'), 1));		//2*6为了子函数中方便实现,
}
#endif

/*******************************************************************************
* Function Name  : FormatCodeEan13(uint8_t *inbuf)
* Description    : 把inbuf里输入进来的数据转化为Ean13条码数据,数据输出到g_tUnderlineBuf里.
* Input          : inbuf：输入条码数据
* Output         : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
* ENA-13的国别码的第一位是用来确定左资料码编码方式的,本身并不参与编码,国别码的其余码值参与编码
*	所以左资料码为6位,包括国别码其余码2位+厂商代码4位.右资料码5位,外加检察码1位.
*	所以打印出来的条码中间线左侧为6位,右侧为6位共12位,
*	但是接收到的数据包括国别码的第一位,所以是13位.
*	如果不包括检查位,此时接收到的是12位,自行计算检查码.
*	解码时需要根据国别码第一位确定左资料码的解码方式,所以国别码第一位数据必不可少.
*	在填充条码的时候不需要填充国别码第一位.打印HRI时需要打印出接收到的全部数字.
*	EAN-8码共8位数,包括国别码2位,产品代码5位,及检查码1位.
*	EAN-8码左右资料码编码规则与EAN-13码相同.
*	EAN-8左资料码包括国别码2位+产品代码2位,右资料码包括产品代码3位+检查码1位
*******************************************************************************/
uint16_t FormatCodeEan13(uint8_t * inbuf)
{
 	uint8_t Temp;

	EanCheckCode(inbuf, 12);	//计算校验码并赋值
	Temp = (*inbuf -'0');			//取前置字符,根据前置字符值决定左资料码的类别
	
	//导入值为EAN-13的最左边第一个数字,即国家代码的第一码,是不用条码符号表示的,
	//其功能仅做为左资料码的编码设定之用
	//返回值为从左到右需要打印的下划线的列数
 	return (EnaFillToBuf((inbuf + 1), 12, Temp, 0));
}

/*******************************************************************************
* Function Name : FormatCodeEan8(uint8_t *inbuf)
* Description   : 把inbuf里输入进来的数据转化为Ean8条码数据,数据输出到g_tUnderlineBuf里.
* Input         : inbuf：输入条码数据
* Output        : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
*******************************************************************************/
uint16_t FormatCodeEan8(uint8_t *inbuf)
{
	EanCheckCode(inbuf, 7);
	return (EnaFillToBuf(inbuf, 8, 0, 0));
}

/*******************************************************************************
* Function Name : FormatTwo(uint8_t *Dest, uint16_t Data , uint8_t Blength)
* Description   : 将1个有宽、窄条的条码符号的条模数据放入指定的缓冲区中
* Input         : Dest：数据存入指针,Data：条模数据,Blength：有效数据位长度
* Output        :
* Return		 : 	
*******************************************************************************/
uint16_t FormatTwo(uint8_t *Dest, uint16_t Data, uint8_t Blength)
{
	uint8_t i, j, Temp, Width;
	uint16_t n = 0;
	
	for (i = 1; i <= Blength; i++)
	{	
		if (i % 2)
			Temp = 0xFF;	//条,条码符号从左到右,表示奇数位字符的条码数据符由条组成,
		else
			Temp = 0x00;	//空,表示偶数位字符的条码数据符由空组成
		
		if (Data & (uint16_t)(1 << (Blength - i)))
		{
			Width = g_tBarCodeCtrl.BarCodeThickWidth;	//宽条宽
		}
		else
		{
			Width = g_tBarCodeCtrl.BarCodeThinWidth;		//窄条宽
		}
		for (j = 0; j < Width; j++)
			Dest[n++] = Temp;
	}
	
	return n;
}

/*******************************************************************************
* Function Name  : FormatCode39(uint8_t *inbuf)
* Description    : 把inbuf里输入进来的数据转化为code39条码数据,数据输出到g_tUnderlineBuf里.
* Input          : inbuf：输入条码数据
* Output         : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
*******************************************************************************/
uint16_t FormatCode39(uint8_t *inbuf)
{
	uint8_t codebuf[255];
	uint8_t * p = '\0';
	uint16_t i, j, Number, Zimo, tsum;
	
	tsum = 0;
	
	/* 查找是否有终止符,并计算个数*/ 
	for (Number = 0; ; )
	{
		if ((inbuf[Number] == '*') || (inbuf[Number] == 0x00))
		{
			Number++;
			if (Number != 1)
				break;
		}
		else
		{
			p = strchr((const char *)codebuf39, inbuf[Number]);
			tsum += (p - codebuf39);		/* 计算校验和 */
			Number++;
		}
	}
	
	if (inbuf[0] != '*')
	{
		codebuf[0] = '*';			//如果没有起始符,则加入
		strncpy(&codebuf[1], (const char *)inbuf, Number);
		Number++;
	}
	else
		strncpy(&codebuf[0], (const char *)inbuf, Number);	//有开始结束符
	
	if (codebuf[Number - 1] != '*')
	{
		codebuf[Number - 1] = '*';		   //加入结束符
	}
	
	strncpy(inbuf, (const char *)codebuf, Number);	   //返回HRI
	*(inbuf + Number) = 0x00;
	
/* 加入校验和 因为校验和是可选的,参考EPSON,WOOSIM等机型,去掉校验和 */
/*
	osum = tsum%43;
	code[j-1]=*(codebuf39+osum);
	code[j]='*';
	j++;
*/
	j = Number * (((6 + 1)) * g_tBarCodeCtrl.BarCodeThinWidth + 3 * g_tBarCodeCtrl.BarCodeThickWidth);
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//超过打印区域,不能放下完整的条码,返回0
	
	//代码转换
	for (i = 0; i < Number; i++)
	{
		p = strchr((const char *)codebuf39, codebuf[i]);
		codebuf[i] = (p - codebuf39);
	}	
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < Number; i++)
	{
		Zimo = Code39buf[codebuf[i]];
		j = FormatTwo(p, Zimo, 10);
		p += j;
	}
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name: FormatItf25(uint8_t *inbuf)
* Description  : 把inbuf里输入进来的数据转化为TIF25条码数据,数据输出到g_tUnderlineBuf里.
* Input        : inbuf：输入条码数据
* Output       : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
*******************************************************************************/
uint16_t FormatItf25(uint8_t *inbuf)
{
	uint8_t Zimo1, Zimo2;
	uint8_t * p = NULL;
	uint16_t i, j, m, n, Number;	
	
	Number = strlen((const char *)inbuf);
	
	if ((Number % 2) == 1)	//检查输入字节数是否为偶数
	{
		Number--;
		inbuf[Number] = 0x00;	//如果输入数不为偶数,则去掉最后一位数
	}
	
/* 计算校验码,参考EPSON WOOSIM机型,不计算校验码,去掉 */

/*
	for (i=0;i<m;i++){
		if (i%2)
			oddsum +=*(inbuf+i)-'0';
		else
			evensum+=*(inbuf+i)-'0';

	}
	check = (oddsum+evensum*3)%10+'0';
	strcat(newinbuf,inbuf);
//	strncat(inbuf,&check,1);
	strncat(newinbuf,&check,1);
*/
	/*交叉25码每个数据由5个单元组成,2宽3窄;起始符2窄条2窄空;终止符1宽条1窄条1窄空*/
	j = (Number * 3 + 4 + 2) * g_tBarCodeCtrl.BarCodeThinWidth + 
			(Number * 2 + 1) * g_tBarCodeCtrl.BarCodeThickWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//超过打印区域,不能放下完整的条码,返回0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	j = FormatOne(p, 0x0a, 4);		//起始符2窄条2窄空
	
	p += j;
	for (i = 0; i < Number; i++)
	{
		Zimo1 = CodeItf[inbuf[i] - 0x30];	//两字节合成交叉的1字节
		i++;
		Zimo2 = CodeItf[inbuf[i] - 0x30];	
		n = 0;
		for (m = 0; m < 5; m++)
		{
			if ((Zimo1 >> m) & 0x01)
				n |= (uint16_t)(1 << (2 * m + 1));
			if ((Zimo2 >> m) & 0x01)
				n |= (uint16_t)(1 << (2 * m));
		}
		j = FormatTwo(p, n, 10);	//一次插入10个位置,2个数据位
		p += j;
	}
	
	j = FormatTwo(p, 0x04, 3);	//终止符
	p += j;
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name : FormatCodabar(uint8_t *inbuf)
* Description   : 把inbuf里输入进来的数据转化为codabar条码数据,数据输出到g_tUnderlineBuf里.
* Input         : inbuf：输入条码数据
* Output        : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
*******************************************************************************/
uint16_t FormatCodabar(uint8_t *inbuf)
{
	uint16_t i, j, Number;
	uint8_t Zimo, codebuf[255];
	uint8_t	 * p = '\0';
	
	for (Number = 1; ; )
	{
		if (((inbuf[Number] >= 'A') && (inbuf[Number] <= 'D')) || (inbuf[Number] == 0x00))
		{
			Number++;
			break;
		}
		else
			Number++;
	}
	
	if ((inbuf[0] < 'A') || (inbuf[0] > 'D'))
	{
		codebuf[0] = 'A';					//如果没有起始符,则加入
		strncpy(&codebuf[1], (const char *)inbuf, Number);
		Number++;
	}
	else
		strncpy(&codebuf[0], (const char *)inbuf, Number);	//有开始结束符
	
	if ((codebuf[Number - 1] < 'A') || (codebuf[Number - 1] > 'D'))
	{
		codebuf[Number - 1] = 'B';		   			//加入结束符
	}
	strncpy(inbuf, (const char *)codebuf, Number);				//返回
	*(inbuf + Number) = 0x00;
	
	j = 0;
	for (i = 0; i < Number; i++)
	{									//进行代码转换
		p = strchr((const char *)codabar, codebuf[i]);
		codebuf[i] = (p - codabar);
		if (codebuf[i] > 12)
			j++;						//计算3个宽条的字符数量
	}
	
	j = (Number * 5 - j) * g_tBarCodeCtrl.BarCodeThinWidth + \
			(Number * 3 + j) * g_tBarCodeCtrl.BarCodeThickWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//超过打印区域,不能放下完整的条码,返回0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < Number; i++)
	{
		Zimo = Codabarbuf[*(codebuf + i)];
		j = FormatTwo(p, Zimo, 8);
		p += j;
	}
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name : FormatCode93(uint8_t *Inbuf, uint8_t Length)
* Description   : 把inbuf里输入进来的数据转化为code93条码数据,数据输出到g_tUnderlineBuf里.
* Input         : inbuf：输入条码数据,Length：数据长度
* Output        : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0.
************************************************************************************/
uint16_t FormatCode93(uint8_t *inbuf, uint8_t Length)	 
{
	uint8_t m, n, Temp;
	uint8_t	* p = '\0';	
	uint8_t codebuf[PRT_DOT_NUMBERS / (2 * 9)], Hribuf[PRT_DOT_NUMBERS / (2 * 9)];
	uint16_t i, j, Number, osum, tsum, Zimo;
	
 	codebuf[0] = 47;	//加入起始符
	i = 1;
	m = 0;
	for (Number = 0; Number < Length; Number++)
	{
		Temp = *(inbuf + Number);
		p = strchr((const char *)codebuf93, Temp);
		if ((p == 0x00) || (Temp == 0))	/* P＝＝0,不在基本字符集中,则进行代码转换*/
		{
			n = Temp & 0x1F;
			if (Temp < 0x20)    /* 0x00=<vartemp<0x20 */
			{
				Hribuf[m] = 0xFE;	/*不可打印字符处理 ,不可打印字符在其前字符“0xfe”表示*/
				m++;
				if ((n > 0) && (n < 0x1B))
				{
					codebuf[i] = 43;	/*控制符($) */
					i++;
					Temp = n - 1 + 'A';
				}
				else
				{
					codebuf[i] = 44;	/*控制符(%) */
					i++;
					if (n == 0)
					{
						Temp = 'U';
					}
					else
					{
						Temp = n - 0x1B + 'A';
					}
				}
				Hribuf[m] = Temp;	/*HRI打印字符处理*/
			}
			else if	(Temp < 0x40)	/* 0x20=<vartemp<0x40 */
			{
				Hribuf[m] = Temp;	/*HRI打印字符处理 */
				if ((n > 0) && (n < 0x1B))
				{
					codebuf[i] = 45;	/*控制符(/) */
					i++;
					Temp = n - 1 + 'A';
				}
				else
				{
					codebuf[i] = 44;	/*控制符(%) */
					i++;
					Temp = n - 0x1B + 'F';
			 	}
			}
			else if	(Temp < 0x60)	/* 0x40=<vartemp<0x60 */
			{
				Hribuf[m] = Temp;	/*HRI打印字符处理 */
				codebuf[i] = 44;	/*控制符(%) */
				i++;
				if (n == 0)
				{
					Temp = 'V';
				}
				else			  
				{
					Temp = n - 0x1B + 'K';
				}
			}
			else 	 	 /* 0x60=<vartemp<0x80 */
			{
				if (Temp == 0x7F)
				{
					Hribuf[m] = 0xFE;	/*不可打印字符处理 */
					m++;
					Hribuf[m] = 'T';
				}
				else 
					Hribuf[m] = Temp;	/*HRI打印字符处理 */
				if ((n > 0) && (n < 0x1B))
				{
					codebuf[i] = 46;	/*控制符(+) */
					i++;
					Temp = n - 1 + 'A';
				}
				else
				{
					codebuf[i] = 44;	/*控制符(%) */
					i++;
					if (n == 0)
					{
						Temp = 'W';
					}
					else
					{
						Temp = n - 0x1B + 'P';
					}
				}
			}
			p = strchr((const char *)codebuf93, Temp);
		}
		else
			Hribuf[m] = Temp;	/*HRI打印字符处理 */
		m++;
		codebuf[i] = (p - codebuf93);
		i++;
	}
	
	*(Hribuf + m) = 0x00;
	m++;
	strncpy(inbuf, (const char *)Hribuf, m);	//有开始结束符
	
	/*转换为CODE93的基本码值,并计算第一校验码 */
	tsum = 0;
	j = i - 1;
	for (i = 0; i < j; i++)
	{
		tsum += codebuf[j - i] * (i % 20 + 1);
	}
	tsum = tsum % 47;
	j++;
	codebuf[j] = tsum;
	
	/*计算第=校验码 C */
	tsum = 0;
	for (i = 0; i < j; i++)
		tsum += codebuf[j - i] * (i % 15 + 1);
	osum = tsum % 47;
	j++;
	codebuf[j] = osum;
	j++;
	codebuf[j] = 48;		//加入结束符
	
	Number = j + 1;

	j = (Number * 9 + 1) * g_tBarCodeCtrl.BarCodeThinWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;														//超过打印区域,不能放下完整的条码,返回0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < (Number - 1); i++)
	{
		Zimo = code93buf[*(codebuf + i)];
		j = FormatOne(p, Zimo, 9);
		p += j;
	}
	Zimo = code93buf[*(codebuf + i)];	//结束符
	j = FormatOne(p, Zimo, 10);
	p += j;
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name  : FormatCode128(uint8_t *Inbuf, uint8_t Length)
* Description    : 把inbuf里输入进来的数据转化为code93条码数据,数据输出到g_tUnderlineBuf里.
* Input          : inbuf：输入条码数据,Length：数据长度
* Output         : g_tUnderlineBuf缓冲区为条码数据,inbuf：中为HRI字符串
* Return		 : 数据格式正确：条码宽度. 数据太多或数据出错：0
************************************************************************************/
uint16_t FormatCode128(uint8_t *Inbuf, uint8_t Length)
{
	uint8_t * p = NULL;
	uint16_t i, j, Zimo;
	
	j = (Length * 11 + 2) * g_tBarCodeCtrl.BarCodeThinWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//超过打印区域,不能放下完整的条码,返回0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < (Length - 1); i++)
	{
		Zimo = CODE128[*(Inbuf + i)];
		j = FormatOne(p, Zimo, 11);
		p += j;
	}
	
	Zimo = CODE128[*(Inbuf + i)];
	j = FormatOne(p, Zimo, 13);		//结束符
	p += j;	
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name : GetCode128Str(uint8_t *Codebuf, uint8_t *Hribuf, uint8_t Length,))
* Description   : 接收指定长度的字符,判断字符是否为有效字符,并转换为相应的条码数据
* Input         : Codebuf:输入数据转换后条码字符存放指针,Length：需要输入数据长度,Hirbuf：转换后的HIR存放指针
* Output        : None
* Return        : 接收字符非法：0,否则返回实际接收的字符长度
*******************************************************************************/
uint8_t GetCode128Str(uint8_t *Codebuf, uint8_t *Hribuf, uint8_t Length)
{
	uint8_t code, shift, Temp, TempCode;
	uint8_t	* p = NULL;
	uint8_t GetChar;
	uint16_t i, k, m, sum;
	
	p = Codebuf;
	k = 1;			//K校验和计算权数
	code = 0;		//字符集
	shift = 0;	//转换字符标志
	sum	= 0;		//校验和
	m = 0;			//转换后HRI长度
	for (i = 0; i < Length; i++)
	{
		GetChar = ReadInBuffer();
		if ((i == 0) && (GetChar != '{'))	//如果没有字符集选择就退出,字符集选择通过{A,{B,{C实现
		{
			return 0;
		}
		if (GetChar == '{')		 //字符集选择符号
		{
			GetChar = ReadInBuffer();
			i++;
			switch (GetChar)
			{
				case 65:   			//CODE A 
				{
					if (code != 1)
					{
						code = 1;
						if (i == 1)
						{
							Temp = 103;	//校验和
						}
						else
						{
							Temp = 101;	 
						}
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 66:            //CODE B 
				{
					if (code != 2)	   //
					{
						code = 2;
						if (i == 1)
						{
							Temp = 104;	 
						}
						else
						{
							Temp = 100;	 
						}
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 67:               //CODE C
				{
					if (code != 3)	   //
					{
						code = 3;
						if (i == 1)
						{
							Temp = 105;	 
						}
						else
						{
							Temp = 99;	 
						}
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 83:              //SHIFT 
				{
					if (code != 3)
					{
						Temp = 98;
						shift = 1;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 123:		   	 //”｛“字符处理
				{
					if (code == 2)
					{
						Temp = (GetChar - 32);
						Hribuf[m++] = GetChar;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 49:              //FNC1 
				{
					Temp = 102;	 
					break;
				}
				case 50:              //FNC2 
				{
					if (code != 3)
					{
						Temp = 97;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 51:              //FNC3 
				{
					if (code != 3)
					{
						Temp = 96;	 
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 52:              //FNC4 
				{
					if (code == 1)
					{
						Temp = 101;	 
					}
					else if (code == 2)
					{
						Temp = 100;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				default:
					return 0;
			}
			sum += Temp * k;
			*(p++) = Temp;
			Hribuf[m++] = 0x20;	   //HRI为空格符
			if (i > 1)
			{
			 	k++;
			}
		}	 // End  *(DST+i)==123
		else 
		{
			TempCode = code;
			if (shift)
			{
			 	if (code == 1)
					TempCode = 2;
				else if (code == 2)
					TempCode = 1;
				shift = 0;
			}
			switch (TempCode)
			{
				case 1:				//A字符集的字符转换为条码代码
				{
					if ((GetChar >= 32) && (GetChar <= 95))
					{
						Temp = GetChar - 32;
						Hribuf[m++] = GetChar;
					}
					else if (GetChar < 32)
					{
						Temp = GetChar + 64;
						Hribuf[m++] = GetChar;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 2:			//B字符集的字符转换为条码代码
				{
					if ((GetChar >= 32) && (GetChar <= 127))
					{
						Temp = GetChar - 32;
						Hribuf[m++] = GetChar;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 3:			//C字符集的字符转换为条码代码
				{
					if (GetChar <= 99)
					{
						Temp = GetChar;
						Hribuf[m++] = GetChar / 10 + '0';
						Hribuf[m++] = GetChar % 10 + '0';
					}
					else
					{
						return 0;
					}
					
					break;
				}
			}
			sum += Temp * k;
			*(p++) = Temp;
			k++;
		}
	}
	*(p++) = sum % 103;
	k++;	//校验位

	*(p++) = 106;	 //stop bit
	k++;		  	//实际条码字符数

 	Hribuf[m++] = '\0';		//加入结束符
	
	return k;
}

/*******************************************************************************
* Function Name  : void GetCodeBarStr(uint8_t *Inbuf, uint8_t Number, uint8_t Type)
* Description    : 接收指定数量的字符,如果指定数据0xFF,则接收到0x00,返回
* Input          : Inbuf:输入数据存放指针,Number：需要输入数据长度,Type：条码类型
* Output         : None
* Return         : 接收字符非法：0,否则返回实际接收的字符长度
*******************************************************************************/
uint8_t GetCodeBarStr(uint8_t *Inbuf, uint8_t Number, uint8_t Type)
{
	uint8_t GetChar,i;

	for (i = 0; i < Number; i++)
	{
		GetChar = ReadInBuffer();
		Inbuf[i] = GetChar;
		
		switch (Type)
		{
			case	0:		//UPC-A
			case	1:		//UPC-E
			case	2:		//ENA-13
			case	3:		//ENA-8
			case	5:		//ITF
			case	65:		//UPC-A
			case	66:		//UPC-E
			case	67:		//ENA-13
			case	68:		//ENA-8
			case	70:		//ITF
			{
				if (Inbuf[i] == 0)	//如果是NUL,表示数据参数结束
					//return (i + 1);
					return i;
				else if ((Inbuf[i] < '0') || (Inbuf[i] > '9'))
					return 0;			//非法字符返回
				break;
			}
			case	4:		//CODE39
			case	69:		//CODE39
			{
				if ((Inbuf[i] == 0) || ((Inbuf[i] == '*') && (i != 0)))
					return (i + 1);
				else if (strchr((const char *)codebuf39, Inbuf[i]) == 0)
					return 0;			//非法字符返回
				break;
			}
			case	6:		     //CODEBAR
			case	71:		     //CODEBAR
			{
				if ((Inbuf[i] == 0) || ((Inbuf[i] >= 'A') && (Inbuf[i] <= 'D') && (i != 0)))
					return (i + 1);
				else if (strchr((const char *)codabar,Inbuf[i]) == 0)
					return 0;			//非法字符返回
				break;
			}
			case	72:		      //CODE93
			{
				if (Inbuf[i] >= 0x80)
					return 0;			//非法字符返回
				break;
			}
			default:
				break;
		}
	}
	
	Inbuf[i] = 0;			//加入结束符
	
	return i;
}

/*******************************************************************************
* Function Name  : void PrintBar(uint8_t *HriStr, uint16_t Width)
* Description    : 打印条码
* Input          : *HriStr:HRI地址,存放可阅读内容,NUL结束
	Width:条码总宽,包括从g_tPrtCtrlData.PrtLeftLimit在内的从0到所有条码图形宽度在内
	修改为只包括条码图形宽度值
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBar(uint8_t * HriStr, uint16_t Width)
{
	if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_NORMAL)
	{
		if (g_tBarCodeCtrl.HRIPosition)
		{
			if ( g_tBarCodeCtrl.HRIPosition & 0x01 )
			{	//如果上面需要打印HRI
				PrintBarHRI(HriStr, Width);
			}
			
			PrintBarBitmap(HriStr, Width);
			
			if (g_tBarCodeCtrl.HRIPosition & 0x02)
			{	//如果下面需要打印HRI
				PrintBarHRI(HriStr, Width);
			}
		}
		else	//无需打印HRI
		{
			PrintBarBitmap(HriStr, Width);
		}
	}
	else
	{
		if (g_tBarCodeCtrl.HRIPosition)
		{
			if ( g_tBarCodeCtrl.HRIPosition & 0x02 )
			{	//如果下面需要打印HRI
				PrintBarHRI(HriStr, Width);
			}
			
			PrintBarBitmap(HriStr, Width);
			
			if ( g_tBarCodeCtrl.HRIPosition & 0x01 )
			{	//如果上面需要打印HRI
				PrintBarHRI(HriStr, Width);
			}
		}
		else	//无需打印HRI
		{
			PrintBarBitmap(HriStr, Width);
		}
	}
	g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;   //2017.10.14
}

/*******************************************************************************
* Function Name  : void PrintBarHRI(uint8_t *HriStr, uint16_t Width)
* Description    : 打印条码文字
* Input          : *HriStr:HRI地址,Width:条码总宽,检查全局变量
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBarHRI(uint8_t * HriStr, uint16_t Width)
{
	uint8_t	i;
	uint8_t Language;
	uint8_t SYS_FontSize;
	uint8_t Underline;
	uint8_t CharWidth;
	uint8_t CharHigh;
	uint8_t MaxRatioX;
	uint8_t MaxRatioY;
	uint8_t CodeLineHigh;
	uint8_t HZUnderline;
	uint8_t CharNot;
	uint8_t CharCycle90;
	uint16_t LeftSpace;
	uint16_t RightSpace;
	uint32_t temp1;                 //2017.10.14
	
	//保护参数
	temp1 = g_tPrtCtrlData.PrtDataDotIndex;       //2017.10.14
	Language			= g_tSysConfig.SysLanguage;			//语言
	SYS_FontSize	= g_tSysConfig.SysFontSize;			//字体
	HZUnderline		= g_tPrtModeHZ.Underline;				//下划线
	MaxRatioY			= g_tPrtModeChar.HeightMagnification;	//纵向放大倍数
	MaxRatioX			= g_tPrtModeChar.WidthMagnification;	//横向放大倍数
	Underline			= g_tPrtModeChar.Underline;			//下划线
	CharWidth			=	g_tPrtModeChar.CharWidth;			//字符宽度
	CharHigh			= g_tPrtModeChar.CharHigh;			//字符高度
	LeftSpace			=	g_tPrtModeChar.LeftSpace;			//字符左间距
	RightSpace		= g_tPrtModeChar.RightSpace;		//字符右间距
	CodeLineHigh	= g_tPrtCtrlData.CodeLineHigh;	//行间距
	CharNot				= g_tPrtCtrlData.CharNotFlag;		//反显打印
	CharCycle90		= g_tPrtCtrlData.CharCycle90;		//旋转90度
	
	//设置参数
	g_tSysConfig.SysLanguage = LANGUAGE_ENGLISH;
	if (g_tBarCodeCtrl.HRIFont == ASCII_FONT_A)
	{
		g_tSysConfig.SysFontSize	= ASCII_FONT_A;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh		= ASCII_FONT_A_HEIGHT;
		g_tPrtCtrlData.LineHigh		= ASCII_FONT_A_HEIGHT;
	}
	else if (g_tBarCodeCtrl.HRIFont == ASCII_FONT_B)
	{
		g_tSysConfig.SysFontSize	= ASCII_FONT_B;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_B_WIDTH;
		g_tPrtModeChar.CharHigh		= ASCII_FONT_B_HEIGHT;
		g_tPrtCtrlData.LineHigh		= ASCII_FONT_B_HEIGHT;
	}
	else
	{
		g_tSysConfig.SysFontSize	= ASCII_FONT_A;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh		= ASCII_FONT_A_HEIGHT;
		g_tPrtCtrlData.LineHigh		= ASCII_FONT_A_HEIGHT;
	}
	
	g_tPrtModeChar.Underline =	0;
	g_tPrtModeChar.HeightMagnification = 1;
	g_tPrtModeChar.WidthMagnification = 1;
	g_tPrtModeChar.LeftSpace = 0;
	i = strlen((const char *)HriStr);
	if ((Width / i) > g_tPrtModeChar.CharWidth)
		g_tPrtModeChar.RightSpace = (Width / i - g_tPrtModeChar.CharWidth);
	else
		g_tPrtModeChar.RightSpace = 0;
	g_tPrtModeHZ.Underline					= 0;
	g_tPrtCtrlData.CodeLineHigh 		= (g_tPrtModeChar.CharHigh + 8);
	g_tPrtCtrlData.DoUnderlineFlag	= 0;	//确保如果先打印HRI字符,不会清除掉下划线缓冲区
	g_tPrtCtrlData.OneLineWidthFlag	= 0;
	g_tPrtCtrlData.MaxRatioY		= 1;			//行内最大纵向倍数
	g_tPrtCtrlData.CharNotFlag	= 0;			//反显打印
	g_tPrtCtrlData.CharCycle90	= 0;			//旋转90度
	PrintString(HriStr);
	PrintString("\n");
	#if	1
	//恢复参数
	g_tPrtCtrlData.PrtDataDotIndex = temp1;       //2017.10.14
	g_tSysConfig.SysLanguage	= Language;				//语言
	g_tSysConfig.SysFontSize	= SYS_FontSize;		//字体
	g_tPrtModeHZ.Underline		= HZUnderline;
	g_tPrtModeChar.HeightMagnification	= MaxRatioY;			//纵向放大倍数
	g_tPrtModeChar.WidthMagnification		= MaxRatioX;			//横向放大倍数
	g_tPrtModeChar.Underline						= Underline;			//下划线
	g_tPrtModeChar.LeftSpace						= LeftSpace;			//字符左间距
	g_tPrtModeChar.RightSpace						= RightSpace;			//字符右间距
	g_tPrtModeChar.CharWidth						= CharWidth;			//字符宽度
	g_tPrtModeChar.CharHigh							= CharHigh;				//字符高度
	g_tPrtCtrlData.CodeLineHigh					= CodeLineHigh;		//行间距
	g_tPrtCtrlData.CharNotFlag					= CharNot;				//反显打印
	g_tPrtCtrlData.CharCycle90					= CharCycle90;		//旋转90度
	#endif
}

#ifdef	BARCODE_PRT_AS_BITMAP_ENABLE
/*******************************************************************************
* Function Name  : void PrintBarBitmap(uint8_t *HriStr, uint16_t Width)
* Description    : 打印条码位图,将下划线缓冲区内容根据条码高度填充到打印缓冲区并打印
* Input          : *HriStr:HRI地址,Width:条码总宽,检查全局变量
*	下划线缓冲区有效数据:g_tPrtCtrlData.PrtDataDotIndex开始,到.PrtDataDotIndex + Width结束.
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBarBitmap(uint8_t *HriStr, uint16_t Width)
{
	uint8_t	i;
	uint8_t LineHigh, CodeLineHigh;
	uint8_t cByteHigh, cMaxByte;
	uint16_t	j;
	uint8_t * psrc = NULL;
	uint8_t * pdst = NULL;
	uint8_t * ptemp = NULL;
	
	CodeLineHigh = g_tPrtCtrlData.CodeLineHigh;
	LineHigh = g_tPrtCtrlData.LineHigh;
	
	if (Width > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))	//避免超过最大值
	{
		return;
	}
	
	cMaxByte = g_tBarCodeCtrl.BarCodeHigh >> 3;	//计算填充的字节数
	if ((g_tBarCodeCtrl.BarCodeHigh % 8) > 4)		//四舍五入
		cMaxByte += 1;
	
	if (cMaxByte >= MAX_PRT_HEIGHT_BYTE)	//防止高度过高超过内存缓冲区
	{
		cByteHigh = MAX_PRT_HEIGHT_BYTE;		//分成两次打印
		//填充首地址,填充内容为g_tUnderlineBuf中从LeftLimit开始Width宽度的值,0xFF或者0x00
		ptemp = g_tLineEditBuf + g_tPrtCtrlData.PrtDataDotIndex;		//当前编辑缓冲区空白起始位置
		g_tPrtCtrlData.PrtDataDotIndex += Width;		//加上条码宽度
		if (g_tPrtCtrlData.PrtDataDotIndex > g_tPrtCtrlData.PrtLength)	//超出一行的部分不打印
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		
		for (i = 0; i < cByteHigh; i++)		//逐字节行填充
		{
			psrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;
			pdst = ptemp + i * PRT_DOT_NUMBERS;
			for (j = 0; j < Width; j++)			//将下划线缓冲区中对应位置的内容填充到编辑缓冲区
			{
				*pdst++ = *psrc++;
			}
		}
		
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)cByteHigh;	//纵向总点数
		g_tPrtCtrlData.MaxPrtDotHigh <<= 3;
		g_tPrtCtrlData.BitImageFlag = 1;				//以图形方式打印
// 		PrintOneLine();													//打印该条形码
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		cByteHigh = cMaxByte - MAX_PRT_HEIGHT_BYTE;		//分成两次打印
		//填充首地址,填充内容为g_tUnderlineBuf中从LeftLimit开始Width宽度的值,0xFF或者0x00
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - cByteHigh) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
		g_tPrtCtrlData.PrtDataDotIndex += Width;	//加上条码宽度
		if (g_tPrtCtrlData.PrtDataDotIndex > g_tPrtCtrlData.PrtLength)	//超出一行的部分不打印
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		
		for (i = 0; i < cByteHigh; i++)		//逐字节行填充
		{
			psrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;
			pdst = ptemp + i * PRT_DOT_NUMBERS;
			for (j = 0; j < Width; j++)			//将下划线缓冲区中对应位置的内容填充到编辑缓冲区
			{
				*pdst++ = *psrc++;
			}
		}
		
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)cByteHigh;	//纵向总点数
		g_tPrtCtrlData.MaxPrtDotHigh <<= 3;
		g_tPrtCtrlData.BitImageFlag = 1;				//以图形方式打印
		g_tPrtCtrlData.DoUnderlineFlag = 0;			//清除下划线
		g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
		//memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS); //清缓冲区
		memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
// 		PrintOneLine();													//打印该条形码
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	else
	{
		cByteHigh = cMaxByte;						//直接一次打印完毕
		//填充首地址,填充内容为g_tUnderlineBuf中从LeftLimit开始Width宽度的值,0xFF或者0x00
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - cByteHigh) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
		g_tPrtCtrlData.PrtDataDotIndex += Width;		//加上条码宽度
		if (g_tPrtCtrlData.PrtDataDotIndex > g_tPrtCtrlData.PrtLength)	//超出一行的部分不打印
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		
		for (i = 0; i < cByteHigh; i++)		//逐字节行填充
		{
			psrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;
			pdst = ptemp + i * PRT_DOT_NUMBERS;
			for (j = 0; j < Width; j++)			//将下划线缓冲区中对应位置的内容填充到编辑缓冲区
			{
				*pdst++ = *psrc++;
			}
		}
		
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)cByteHigh;	//纵向总点数
		g_tPrtCtrlData.MaxPrtDotHigh <<= 3;
		g_tPrtCtrlData.BitImageFlag = 1;					//以图形方式打印
		g_tPrtCtrlData.DoUnderlineFlag = 0;				//清除下划线		
		g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
		//memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS); //清缓冲区
		memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
    //PrintOneLine();													//打印该条形码
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	
	GoDotLine(8);	   //走纸点行
	
	g_tPrtCtrlData.LineHigh 		= LineHigh;		//恢复参数
	g_tPrtCtrlData.CodeLineHigh	= CodeLineHigh;
}		//图形方式打印条码结束

#else
extern	uint8_t	g_tbHeatElementBuf[];
static uint16_t	GetStartColumnIndex(void)
{
	uint16_t	ColumnIndex;	//横向列计数索引
	
	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_LEFT)		//左对齐
	{
		ColumnIndex = g_tPrtCtrlData.PrtLeftLimit;
	}
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)	//中间对齐
	{	// = 有效打印区域 - 打印进程列计数器索引(一次行打印打印点阵计数器)
		ColumnIndex = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		ColumnIndex >>= 1;
		ColumnIndex += g_tPrtCtrlData.PrtLeftLimit;
	}
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//右对齐
	{
		ColumnIndex = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		ColumnIndex += g_tPrtCtrlData.PrtLeftLimit;
	}
	
	return (ColumnIndex);
}

static void	FillCodebarBufToHeatElementBuf(uint8_t * bBuf, uint8_t Lines)
{
	if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_NORMAL)	//正向打印开始
	{
		CodebarBufToHeatElementNormalDir(bBuf, Lines);
	}	//正向打印处理结束
	else if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)	//反向打印开始
	{
		CodebarBufToHeatElementReverseDir(bBuf, Lines);
	}	//反向打印处理结束
}

static void	CodebarBufToHeatElementNormalDir(uint8_t * bBuf, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//横向加热处理结果
	uint8_t	CurrentBit;		//当前字节填充位的位置,从8到1,为0时一个字节填充完毕
	uint16_t	ColumnIndex;	//横向列计数索引,根据起始值补充空白位置和字节位
	uint16_t	i;
	uint16_t	CopyIndex;
	uint8_t	* pBuf = NULL;
	
	//memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetStartColumnIndex();	//根据对齐方式得到起始列值
	//整数倍数的空白位置填充完毕,因为前面已经全部清空了
	if (ColumnIndex % 8)				//ColumnIndex不是8的整倍数,前后需要衔接
	{
		CurrentBit = 8 - (ColumnIndex % 8);	//当前字节填充位的位置,从8到1,为0		
	}
	else
	{
		CurrentBit = 8;		//一个字节最高有8位
	}
	i = (ColumnIndex >> 3);	//准备填充该字节,该字节的后面不足的部分需要在下面程序中补齐
	CopyIndex = i;
	//以上为处理空白部分
	
	//从起始列从左往右处理待转换字符
	for (ColumnIndex = g_tPrtCtrlData.PrtLeftLimit; 
		ColumnIndex < g_tPrtCtrlData.PrtDataDotIndex; ColumnIndex++)
	{
		//判断该字节对应位的值是否为1,Lines需要转换为范围0~7
		if ((*bBuf & (0x80 >> (Lines & 0x07))))	//如果对应位置为1
		{
			Buf[i] |= (uint8_t)(1 << (CurrentBit - 1));	//填充一个横向字节
		}
		CurrentBit--;					//字节位数每次不论处理字节对应位是否为1都需要进行处理
		
		if (CurrentBit == 0)	//已经填满一个横向加热字节
		{
			CurrentBit = 8;			//重新开始准备填充下一个字节,从MSB开始填充
			i++;								//只有满字节后才执行
		}
		bBuf++;								//准备检测下一个字节
	}		//待打印字节处理完毕
#if	0
	if (!CurrentBit)		//说明有转换到不足一个字节而没有完成的,需要递增1个
		i++;
	if (i > BYTES_OF_HEAT_ELEMENT)		//防止溢出
		i = BYTES_OF_HEAT_ELEMENT;
#endif
	
	//以下将处理结果填充到横向加热缓冲区
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//跳过左侧32位空白区和加热区中为0的内容,右侧32位空白区已经清零
	pBuf = g_tbHeatElementBuf;
	pBuf += (BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT + CopyIndex);
	for (; CopyIndex < BYTES_OF_HEAT_ELEMENT; CopyIndex++)
	{
		*pBuf++ = Buf[CopyIndex];		//中间实际打印区
	}
}

static void	CodebarBufToHeatElementReverseDir(uint8_t * bBuf, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//横向加热处理结果
	uint8_t	CurrentBit;		//当前字节填充位的位置,从8到1,为8时一个字节填充完毕
	uint16_t	ColumnIndex;	//横向列计数索引,根据起始值补充空白位置和字节位
	uint16_t	i;
	uint8_t	* pBuf = NULL;
	
	//memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetStartColumnIndex();	//根据对齐方式得到起始列值
	i = g_tPrtCtrlData.PrtLength - ColumnIndex;	//指向横向转换后加热缓冲区开始位置
	if (i % 8)
	{
		CurrentBit = 8 - (i % 8);	//可能出现不在8位整数倍位置,从右往左从最低位开始组合起
	}
	else
	{
		CurrentBit = 0;		//如果是8的整数倍则从0开始
	}
	
	i >>= 3;													//转化为字节
	if (i >= BYTES_OF_HEAT_ELEMENT)		//如果超过最后一个字节
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//防止下标越界
	}
	
	pBuf = Buf;
	pBuf += i;
	//需要处理的列(字节)数目
	for (i = g_tPrtCtrlData.PrtLeftLimit; i < g_tPrtCtrlData.PrtDataDotIndex; i++)
	{
		//判断该字节对应位的值是否为1,Lines需要转换为范围0~7
		if (*bBuf & (0x80 >> (Lines & 0x07)))	//如果对应位置为1
		{
			*pBuf |= (uint8_t)(1 << CurrentBit);	//填充一个横向字节
		}
		CurrentBit++;					//字节位数每次不论处理字节对应位是否为1都需要进行处理
		
		if (CurrentBit == 8)	//已经填满一个横向加热字节
		{
			CurrentBit = 0;			//重新开始准备填充下一个字节,从LSB开始填充
			pBuf--;							//只有满一个字节后才减少
		}
		bBuf++;								//准备检测下一列的字节,每次循环都增加
	}
	
	//以下将处理结果填充到横向加热缓冲区
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;		//跳过左侧32位空白区,右侧32位空白区已经清零
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pBuf++ = Buf[i];		//中间实际打印区
	}
}

/*******************************************************************************
* Function Name  : void PrintBarBitmap(uint8_t *HriStr, uint16_t Width)
* Description    : 打印条码位图,将下划线缓冲区内容根据条码高度做为循环点行次数打印
*	处理方式:先确定打印起始和结束范围;再据此从下划线缓冲区找出一点行的条码内容填充到
*	加热缓冲区;再根据条码高度循环打印点行
*	下划线缓冲区有效数据:g_tPrtCtrlData.PrtDataDotIndex开始,到.PrtDataDotIndex + Width结束.
* Input          : *HriStr:HRI地址,Width:条码总宽,检查全局变量
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBarBitmap(uint8_t * HriStr, uint16_t Width)
{
	uint16_t	HeatTime;
	uint16_t	totalheatus;
	uint32_t	temp,temp1;
	uint32_t	i;
	uint8_t * psrc = NULL;
	
	WaitPreviousPrintFinish();	//等待缓冲区为空白
	WaitHeatFinish();						//等待加热完成
	
	//将下划线缓冲区数据从开始到结束读取到编辑缓冲区
	temp1 = g_tPrtCtrlData.PrtDataDotIndex;
	g_tPrtCtrlData.PrtDataDotIndex += Width;		//索引值调整到结束位置 2017.10.14
	//将下划线缓冲区的指定高度的内容转换为横向加热的格式
	//将转换后结果保存到全局加热数组变量中
	psrc = g_tUnderlineBuf;          // + temp;		//从最开始的有效数据开始  2017.10.14 更改是得左边距有效
// 	FillCodebarBufToHeatElementBuf(psrc, 0x00);	//根据最低位提取,从0到7任意一位都可以
// 	SendHeatElementBufToCore();		//发送加热数据到机芯
// 	LatchHeatData();							//锁存机芯数据
// 	WaitFeedPaperFinish();				//等待走纸完成
	
	for (temp = 0; temp < g_tBarCodeCtrl.BarCodeHigh; temp++)	//按照条码高度循环打印每点行
	{
			MovDotLine( psrc, 0);
	}
	
	//处理结束,走一定间距,恢复保存的全局变量
	GoDotLine(8);	   //走纸点行
	
	g_tPrtCtrlData.PrtDataDotIndex = temp1;       //g_tPrtCtrlData.PrtLeftLimit;    //2017.10.14
	g_tPrtCtrlData.DoUnderlineFlag = 0;						//清除下划线
	g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
	//memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS);	//清缓冲区
	memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
