/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   EAN一维条码相关的程序.
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
	* 本文件处理的编码序列符合GB12904-2008国家标准,处理的编码序列中包括前置码,
	*	处理的最大编码序列长度为13.接收到的参数序列最大不超过13.
	* 前置码不包括在左侧数据符内, 不用条码字符表示.
	*	ENA13包括 起始符 + 左侧数据符6个 + 中间间隔符 + 右侧数据5个 + 校验符1个 + 结束符
	*	其中的前置码不包括在条码编码内, 但是可以打印出来,1位.
	* 根据前置码确定其余内容的编码方式.
	* EAN13编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
	*		d13			d12	~	d7	 	d6	~	d2		d1
	*	前置符		左侧数据		右侧数据		校验符
	*	EAN13左侧数据根据前置符确定左侧数据编码集A或者B
	*	EAN13右侧数据及校验符均采用编码集C
	
	*	ENA8包括 起始符 + 左侧数据符4个 + 中间间隔符 + 右侧数据3个 + 校验符1个 + 结束符
	*	EAN8左侧数据采用数据编码集A
	*	EAN8右侧数据及校验符均采用编码集C
	
	*	ENA13校验码计算方法如下:
	*	a)	从代码位置序号2开始, 所有偶数位的数字代码求和
	*	b)	将a)的和乘以3
	*	c)	从代码位置序号3开始, 所有奇数位的数字代码求和
	*	d)	将b)和c)的结果相加
	*	e)	用大于或者等于d)所得结果且为10的最小整数倍的数减去d)所得结果,其差即为校验值
	
	*	UPC-A条码即前置码为0的ENA13码.其共有12位数字组成.
	*	各个位的编码排序完全与EAN13相同.将UPC-A编码序列前面加上 0 即成为标准的EAN13编码.
	* UPC-A编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
	*	d13(无此位)			d12	~	d7	 	d6	~	d2		d1
	*	前置符(无此位)	左侧数据		右侧数据		校验符
	*	UPC-A校验码的计算方式相当于前置码为0的ENA13编码.
	*	UPC-A包括 起始符 + 左侧数据符6个 + 中间间隔符 + 右侧数据5个 + 校验符1个 + 结束符
	
	也可以按照以下方法计算,效果等同于上述算法.
	从国别码开始自左至右取数，设UPC-A各码代号如下：
	N1	N2	N3	N4	N5	N6	N7	N8	N9	N10	N11	C
	则检查码之计算步骤如下：
	C1 = N1+N3+N5+N7+N9+N11
	C2 = (N2+N4+N6+N8+N10)×3
	CC = (C1+C2)　取个位数
	C (检查码) = 10 - CC　 (若值为10，则取0)
	
	*	UPC-E条码由8位数字组成,是将系统字符为0的UPC-12编码压缩而成.
	*	UPC-E首字符恒为0,中间为6位数字,最后为1位校验码,
	*	UPC-E包括 起始符 + 左侧数据符3个 + 右侧数据3个 + 结束符,无中间间隔符和校验符
	
		UPC-A消零压缩方法如下:
											UPC-A编码																					UPC-E编码
						厂商识别代码						商品项目代码		校验码			商品项目识别代码			校验码
		X12(系统字符)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 0  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 1  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 2  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					
					0				X11 X10 3  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 ...0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 9  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					
					0				X11 X10 X9 1  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 ...0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 9  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					
					0				无零结尾X7 != 0   0  0  0  0	5       X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				无零结尾X7 != 0   0  0  0  0	...     X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				无零结尾X7 != 0   0  0  0  0	9       X1			0 X11 X10 X9 X8 X7 X2  	X1
		
	*	UPC-E编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
	*		d8	~		d7	~	d2	 				d1
	*	系统字符0		商品项目代码		校验码
	*	校验码为消零前的UPC-A编码计算得到的校验码.
	*	UPC-E无中间分隔符.
	* UPC-E终止符不同于EAN8, 为010101.
	*	UPC-E数据编码根据校验位值确定选用字符集A或者B.与EAN8编码不同.
	*	UPC-E中系统字符X8和校验码X1不用条码字符表示.
	* 
	*	总结:
	*	EAN13编码序列:前置符1 + 左侧数据符6 + 右侧数据符5 + 校验符1,共 13 字节
	*	EAN13打印内容序列:起始符b101 + 左侧数据 + 中间分隔符b01010 + 右侧数据 + 校验 + 终止符b101
	*	前置符只用来确定左侧数据符编码时的编码方式,不编码到实际的打印内容序列中
	*	EAN13左侧数据根据前置符确定左侧数据编码集A或者B,EAN13右侧数据及校验符均采用编码集C
	
	*	UPC-A编码序列:左侧数据符6 + 右侧数据符5 + 校验符1,共 12 字节
	*	UPC-A打印内容序列:起始符b101 + 左侧数据 + 中间分隔符b01010 + 右侧数据 + 校验 + 终止符b101
	*	无前置符,相当于前置符为0的EAN13编码.编码方式完全同EAN13.
	
	*	EAN8编码序列:左侧数据符4 + 右侧数据符3 + 校验符1,共 8 字节
	*	EAN8打印内容序列:起始符b101 + 左侧数据 + 中间分隔符b01010 + 右侧数据 + 校验 + 终止符b101
	*	左侧数据符编码时的用编码集A,右侧数据及校验用编码集C
	
	*	UPC-E编码序列:系统字符1 + 数据符6 + 校验符1,共 8 字节
	*	UPC-E打印内容序列:起始符b101 + 数据6 + 终止符b010101
	*	前置符为0,但是不包括在可打印的编码之内,校验符为压缩前的UPC-A的校验符,不包括在可打印编码内.
	
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include	"ean.h"
#include	"defconstant.h"

/*  */
/********** 一维条码参数全局变量 ***********/
extern	TypeDef_StructBarCtrl	g_tBarCodeCtrl;
extern	TypeDef_StructPrtCtrl	g_tPrtCtrlData;
extern	uint8_t	g_tUnderlineBuf[PRT_DOT_NUMBERS];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**********EAN13 A字符集数字编码模式 *******/
/* 数组中每一个成员依次分别代表数字字符0~9的条码编码序列,用于EAN-左资料码A */
const uint8_t EANContentCodeType_A[] = 
{
	0x0D, 0x19, 0x13, 0x3D, 0x23, 0x31, 0x2F, 0x3B, 0x37, 0x0B
};

/**********EAN13 B字符集数字编码模式 *******/
/* 数组中每一个成员依次分别代表数字字符0~9的条码编码序列,用于EAN-左资料码B */
const uint8_t EANContentCodeType_B[] = 
{
	0x27, 0x33, 0x1B, 0x21, 0x1D, 0x39, 0x05, 0x11, 0x09, 0x17
};

/**********EAN13 C字符集数字编码模式 *******/
/* 数组中每一个成员依次分别代表数字字符0~9的条码编码序列,用于EAN-右资料码,
	编码规则等同于UPC-A右资料编码 */
const uint8_t EANContentCodeType_C[] = 
{
	0x72, 0x66, 0x6C, 0x42, 0x5C, 0x4E, 0x50, 0x44, 0x48, 0x74
};

/**********EAN13 根据前置符确定的左侧字符选择字符集 *******/
/*****************************************************************************
*EAN 用于确定左侧数据字符的子集,"0"表示A子集,"1"表示B子集 
*本数组中每个数据的值从D0～D5分别表示EAN编码序列从右到左的第7～12位(左侧数据)
*根据国家码的确定左资料符的编码方式,数组中值的低6位有效
******************************************************************************/
const uint8_t EANLeftCharCodeTypeSel[] =
{
	0x00, 0x0B, 0x0D, 0x0E, 0x13, 0x19, 0x1C, 0x15, 0x16, 0x1A
};

const uint8_t	UPCECharCodeTypeSel[] =
{
	0x38, 0x34, 0x32, 0x31, 0x2C, 0x26, 0x23, 0x2A, 0x29, 0x25 
};


/* EAN起始符编码, 101 */
const uint8_t	EANStartCode = 0x05;

/* EAN结束符编码, 101 */
const uint8_t	EANStopCode = 0x05;

/* EAN中间符编码, 01010 */
const uint8_t	EANMiddleCode = 0x0A;

/* UPCE起始符编码, 101 */
const uint8_t	UPCStartCode = 0x05;

/* UPCE结束符编码, 010101 */
const uint8_t	UPCEStopCode = 0x15;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
*******************************************************************************/
/*******************************************************************************
* Function Name  : ErrorStatus CheckEANUPCCodeValid(uint8_t	bBuf[], uint8_t	bCodeLen)
* Description    : 检查指定长度的EAN_UPC编码是否合法
* Input          : 输入参数:编码字符流, 编码长度
* EAN13编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*		d13			d12	~	d7	 	d6	~	d2		d1
*	前置符		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	前置符		左侧数据		右侧数据		校验符
* Output         : 输出参数:状态值
*******************************************************************************/
ErrorStatus	CheckEANUPCCodeValid(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	ErrorStatus	Result;
	uint8_t	i;
	
	if (bCodeLen)
	{
		Result = SUCCESS;
		for (i = 0; i < bCodeLen; i++)
		{
			if (!IS_EAN_UPC_CODE( bBuf[i] ))
			{	
				Result = ERROR;
				break;
			}
		}
	}
	else
	{
		Result = ERROR;
	}
	
	return	(Result);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 计算指定字符序列EAN13的校验码
* Input          : 输入参数:已经标准化了的EAN13字符序列,共12或者13字节长度
* EAN13编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*		d13			d12	~	d7	 	d6	~	d2		d1
*	前置符		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	前置符		左侧数据		右侧数据		校验符
* Output         : 输出参数:计算后的校验码, 范围'0'~'9'
*******************************************************************************/
uint8_t	CalEAN13CheckCode(uint8_t	bBuf[])
{
	uint8_t	bResult;
	uint16_t	OddCodeSum;			/* 奇数序号编码和,按照从右到左方向计数 */
	uint16_t	EvenCodeSum;		/* 偶数序号编码和,按照从右到左方向计数 */
	uint16_t	nCount;
	uint8_t	* pbBuf = NULL;
	
	OddCodeSum = 0;
	EvenCodeSum = 0;
	pbBuf = bBuf;
	for (nCount = 0; nCount < (EAN13_DATA_MAX_LEN - 1); nCount++)
	{
		if (nCount % 2)
		{
			EvenCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* 除了校验字符外其他偶数位置字符和 */
		}
		else
		{
			OddCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* 除了校验字符外其他奇数位置字符和 */
		}
		pbBuf++;
	}
	pbBuf = NULL;
	
	EvenCodeSum *= 3;						/* 用不小于偶数*3+奇数和的10的整数倍的数减加和值*/
	EvenCodeSum += OddCodeSum;
	bResult = EvenCodeSum % 10;
	if (bResult != 0)
	{
		bResult = 10 - bResult;
	}
	bResult += EAN_UPC_CODE_MIN;
	
	return	(bResult);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将校验码放到指定字符序列EAN13的校验码位置
* Input          : 输入参数:已经标准化了的EAN13字符序列,共12或者13字节长度
* EAN13编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*		d13			d12	~	d7	 	d6	~	d2		d1
*	前置符		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	前置符		左侧数据		右侧数据		校验符
* Output         : 输出参数:
*******************************************************************************/
void	SetEAN13CheckCode(uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE(bCode));
	bBuf[ EAN13_DATA_MAX_LEN - 1 ] = bCode;
	bBuf[ EAN13_DATA_MAX_LEN ] = '\0';	 //加入字符串结束符
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 取得指定字符序列EAN13的校验码
* Input          : 输入参数:已经标准化了的EAN13字符序列,共12或者13字节长度
* EAN13编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*		d13			d12	~	d7	 	d6	~	d2		d1
*	前置符		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	前置符		左侧数据		右侧数据		校验符
* Output         : 输出参数:校验码
*******************************************************************************/
uint8_t	GetEAN13CheckCode(uint8_t	bBuf[])
{
	assert_param(IS_EAN_UPC_CODE(bBuf[ EAN13_DATA_MAX_LEN - 1 ]));
	
	return	(bBuf[ EAN13_DATA_MAX_LEN - 1 ]);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 计算指定字符序列EAN8的校验码
* Input          : 输入参数:已经标准化了的EAN8字符序列,共7或者8字节长度
* EAN8编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	~	d5	 	d4	~	d2		d1
*	左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d1	~	d4	 	d5	~	d7		d8
*	左侧数据		右侧数据		校验符
* Output         : 输出参数:计算后的校验码, 范围'0'~'9'
*******************************************************************************/
uint8_t	CalEAN8CheckCode(uint8_t	bBuf[])
{
	uint8_t	bResult;
	uint16_t	OddCodeSum;			/* 奇数序号编码和,按照从右到左方向计数 */
	uint16_t	EvenCodeSum;		/* 偶数序号编码和,按照从右到左方向计数 */
	uint16_t	nCount;
	uint8_t	* pbBuf = NULL;
	
	OddCodeSum = 0;
	EvenCodeSum = 0;
	pbBuf = bBuf;
	for (nCount = 0; nCount < (EAN8_DATA_MAX_LEN - 1); nCount++)
	{
		if (nCount % 2)
		{
			OddCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* 除了校验字符外其他奇数位置字符和 */
		}
		else
		{
			EvenCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* 除了校验字符外其他偶数位置字符和 */
		}
		pbBuf++;
	}
	pbBuf = NULL;
	
	EvenCodeSum *= 3;						/* 用不小于偶数*3+奇数和的10的整数倍的数减加和值*/
	EvenCodeSum += OddCodeSum;
	bResult = EvenCodeSum % 10;
	if (bResult != 0)
	{
		bResult = 10 - bResult;
	}
	bResult += EAN_UPC_CODE_MIN;
	
	return	(bResult);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将校验码放到指定字符序列EAN8的校验码位置
* Input          : 输入参数:已经标准化了的EAN8字符序列,共7或者8字节长度
* EAN8编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	~	d5	 	d4	~	d2		d1
*	左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d1	~	d4	 	d5	~	d7		d8
*	左侧数据		右侧数据		校验符

* Output         : 输出参数:
*******************************************************************************/
void	SetEAN8CheckCode(uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE(bCode));
	bBuf[ EAN8_DATA_MAX_LEN - 1 ] = bCode;
	bBuf[ EAN8_DATA_MAX_LEN ] = '\0';	 //加入字符串结束符
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 取得指定字符序列EAN8的校验码
* Input          : 输入参数:已经标准化了的EAN8字符序列,共7或者8字节长度
* EAN8编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	~	d5	 	d4	~	d2		d1
*	左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d1	~	d4	 	d5	~	d7		d8
*	左侧数据		右侧数据		校验符
* Output         : 输出参数:校验码
*******************************************************************************/
uint8_t	GetEAN8CheckCode(uint8_t	bBuf[])
{
	assert_param(IS_EAN_UPC_CODE(bBuf[ EAN8_DATA_MAX_LEN - 1 ]));
	
	return	(bBuf[ EAN8_DATA_MAX_LEN - 1 ]);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 根据指定条码种类及条码内容长度计算剩余打印空间是否能够放下条码
* Input          : 
* Output         : 输出参数:不能放下返回0, 能够放下返回1
*******************************************************************************/
uint8_t	CheckPrtSpace(uint8_t bType, uint8_t bCodeLen)
{
	uint16_t	nPrtWidth;					/* 打印宽度, 点数 */
	uint8_t		Result;
	
	if (bType == TYPE_EAN13)
	{
		nPrtWidth = \
			EAN_CODE_CONTENT_VALID_BIT_LEN * (EAN13_DATA_MAX_LEN - 1) + \
			EAN_START_VALID_BIT_LEN + \
			EAN_MIDDLE_VALID_BIT_LEN + \
			EAN_STOP_VALID_BIT_LEN;
		nPrtWidth	*= g_tBarCodeCtrl.BarCodeThinWidth;
		if (nPrtWidth > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
			Result = 0;			/* 打印缓冲区放不下, 不打印 */
		else
			Result = 1;
	}
	else if (bType == TYPE_EAN8)
	{
		nPrtWidth = \
			EAN_CODE_CONTENT_VALID_BIT_LEN * (EAN8_DATA_MAX_LEN - 1) + \
			EAN_START_VALID_BIT_LEN + \
			EAN_MIDDLE_VALID_BIT_LEN + \
			EAN_STOP_VALID_BIT_LEN;
		nPrtWidth	*= g_tBarCodeCtrl.BarCodeThinWidth;
		if (nPrtWidth > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
			Result = 0;			/* 打印缓冲区放不下, 不打印 */
		else
			Result = 1;
	}
	else if (bType == TYPE_UPCE)
	{
		nPrtWidth = \
			UPC_CODE_CONTENT_VALID_BIT_LEN * (UPCE_DATA_MAX_LEN - 2) + \
			UPC_START_VALID_BIT_LEN + \
			UPCE_STOP_VALID_BIT_LEN;
		nPrtWidth	*= g_tBarCodeCtrl.BarCodeThinWidth;
		if (nPrtWidth > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
			Result = 0;			/* 打印缓冲区放不下, 不打印 */
		else
			Result = 1;
	}
	else
	{
		Result = 0;		/* 未知编码方式 */
	}
	
	return	Result;
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将标准EAN13字符序列转换为可打印编码序列
* Input          : 输入参数:已经标准化了的EAN13字符序列,共13字节长度
*	转换结果存储到输入参数bPrtBuf, 每个加热点对应bPrtBuf[]的一个字节
*	数据为FF表示打印为黑,数据为0表示打印为空白.
* EAN13编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*		d13			d12	~	d7	 	d6	~	d2		d1
*	前置符		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	前置符		左侧数据		右侧数据		校验符
* 前置码不包括在左侧数据符内, 不用条码字符表示.
* 根据前置码确定其余内容的编码方式.
*	EAN13左侧数据根据前置符确定左侧数据编码集A或者B
*	EAN13右侧数据及校验符均采用编码集C
*	ENA13包括 起始符 + 左侧数据符6个 + 中间间隔符 + 右侧数据5个 + 校验符1个 + 结束符
*	处理时先根据前置符判断左侧数据的编码方式, 编码左侧数据, 再逐个编码其余值
* Output         : 输出参数:总可打印条码编码的宽度, 以点为单位计数, 
*	如果打印缓冲区空间放不下或者出现错误, 返回0
*	需要根据条码定义宽度计算填充和点数宽度值
*******************************************************************************/
uint16_t	EAN13ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
{
	uint8_t	bCodeTypeSel;					/* 左侧数据编码种类集选择*/
	/* 可打印编码值先取出, 后集中处理填充 */
	uint8_t	bContentBuf[EAN13_DATA_MAX_LEN];
	uint16_t	nCount;
	uint16_t	nWidth;							/* 打印宽度, 点数 */
	uint16_t	nPrtWidth;					/* 打印宽度, 点数 */
	uint8_t	* pbSrcBuf = NULL;
	uint8_t	* pbDstBuf = NULL;
	const uint8_t	* pbCodeContentBuf = NULL;
	
	/* 先判断当前可打印区域是否能够放下本次待填充的编码打印内容 */
	/* 必须处理, 否则可能内存溢出 */
	if (CheckPrtSpace(TYPE_EAN13, EAN13_DATA_MAX_LEN) == 0)
		return 0;			/* 打印缓冲区放不下, 不打印 */
	
	memset(bContentBuf, sizeof(bContentBuf) / sizeof(bContentBuf[0]), 0x00);
	pbSrcBuf = bBuf;
	assert_param(IS_EAN13_PRE_CODE( *pbSrcBuf ));
	bCodeTypeSel = EANLeftCharCodeTypeSel[ *pbSrcBuf - EAN13_PRE_CODE_MIN ];
	pbSrcBuf++;
	
	/* 将左侧所有数据先取出对应的可打印内容编码值 */
	for (nCount = 0; nCount < EAN13_LEFT_DATA_LEN; nCount++)
	{
		if ((bCodeTypeSel & (1 << (EAN13_LEFT_DATA_LEN - 1 - nCount))))
		{
			pbCodeContentBuf = EANContentCodeType_B;	/* 从编码字符集B中选择 */
		}
		else
		{
			pbCodeContentBuf = EANContentCodeType_A;	/* 从编码字符集A中选择 */
		}
		/* 将当前字符的可打印内容编码值取回 */
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	/* 继续将剩余右侧及校验码数据取出对应的可打印内容编码值 */
	pbCodeContentBuf = EANContentCodeType_C;	/* 从编码字符集C中选择 */
	for (; nCount < (EAN13_DATA_MAX_LEN - 1); nCount++)
	{
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	
	/* 下面开始填充可打印数据内容 */
	nPrtWidth = 0;								/* 统计总填充打印点数 */
	pbDstBuf = bPrtBuf;
	/* 先填充左起始符 b101 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStartCode, \
																EAN_START_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* 再填充左侧数据 */
	/* 每个数据的条码编码占用7位,根据前置符的值从A,B两个编码集中选择 */
	/* 编码最高位0无效,其余7位有效. 按照从MSB->LSB的顺序填充*/
	/* 前置符的低6位值从MSB->LSB依次对应6个左侧数据 */
	for (nCount = 0; nCount < EAN13_LEFT_DATA_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* 填充中间分隔符 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANMiddleCode, \
																EAN_MIDDLE_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* 填充剩余字符和校验符 */
	for (; nCount < (EAN13_DATA_MAX_LEN - 1); nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* 最后填充终止符 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStopCode, \
																EAN_STOP_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	
	pbSrcBuf = NULL;
	pbDstBuf = NULL;
	pbCodeContentBuf = NULL;
	
	return	(nPrtWidth);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将标准EAN8字符序列转换为可打印编码序列
* Input          : 输入参数:已经标准化了的EAN8字符序列,共8字节长度
*	转换结果存储到输入参数bPrtBuf, 每个加热点对应bPrtBuf[]的一个字节
*	数据为FF表示打印为黑,数据为0表示打印为空白.
* EAN8编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	~	d5	 	d4	~	d2		d1
*	左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d1	~	d4	 	d5	~	d7		d8
*	左侧数据		右侧数据		校验符
* 无前置码.
*	EAN8左侧数据采用编码集A
*	EAN8右侧数据及校验符均采用编码集C
*	ENA8包括 起始符 + 左侧数据符4个 + 中间间隔符 + 右侧数据3个 + 校验符1个 + 结束符
* Output         : 输出参数:总可打印条码编码的宽度, 以点为单位计数, 
*	如果打印缓冲区空间放不下或者出现错误, 返回0
*	需要根据条码定义宽度计算填充和点数宽度值
*******************************************************************************/
uint16_t	EAN8ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
{
	/* 可打印编码值先取出, 后集中处理填充 */
	uint8_t	bContentBuf[EAN13_DATA_MAX_LEN];
	uint16_t	nCount;
	uint16_t	nWidth;							/* 打印宽度, 点数 */
	uint16_t	nPrtWidth;					/* 打印宽度, 点数 */
	uint8_t	* pbSrcBuf = NULL;
	uint8_t	* pbDstBuf = NULL;
	const uint8_t	* pbCodeContentBuf = NULL;
	
	/* 先判断当前可打印区域是否能够放下本次待填充的编码打印内容 */
	/* 必须处理, 否则可能内存溢出 */
	if (CheckPrtSpace(TYPE_EAN8, EAN8_DATA_MAX_LEN) == 0)
		return 0;			/* 打印缓冲区放不下, 不打印 */
	
	memset(bContentBuf, sizeof(bContentBuf) / sizeof(bContentBuf[0]), 0x00);
	pbSrcBuf = bBuf;
	/* 将左侧所有数据先取出对应的可打印内容编码值 */
	pbCodeContentBuf = EANContentCodeType_A;	/* 从编码字符集A中选择 */
	for (nCount = 0; nCount < EAN8_LEFT_DATA_LEN; nCount++)
	{
		/* 将当前字符的可打印内容编码值取回 */
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	/* 继续将剩余右侧及校验码数据取出对应的可打印内容编码值 */
	pbCodeContentBuf = EANContentCodeType_C;	/* 从编码字符集C中选择 */
	for (; nCount < EAN8_DATA_MAX_LEN; nCount++)
	{
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	
	/* 下面开始填充可打印数据内容 */
	nPrtWidth = 0;								/* 统计总填充打印点数 */
	pbDstBuf = bPrtBuf;
	/* 先填充左起始符 b101 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStartCode, \
																EAN_START_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* 再填充左侧数据 */
	/* 每个数据的条码编码占用7位,根据前置符的值从A,B两个编码集中选择 */
	/* 编码最高位0无效,其余7位有效. 按照从MSB->LSB的顺序填充*/
	/* 前置符的低6位值从MSB->LSB依次对应6个左侧数据 */
	for (nCount = 0; nCount < EAN8_LEFT_DATA_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* 填充中间分隔符 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANMiddleCode, \
																EAN_MIDDLE_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* 填充剩余字符和校验符 */
	for (; nCount < EAN8_DATA_MAX_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* 最后填充终止符 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStopCode, \
																EAN_STOP_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	
	pbSrcBuf = NULL;
	pbDstBuf = NULL;
	pbCodeContentBuf = NULL;
	
	return	(nPrtWidth);
}

uint8_t	GetPrtDataFromCodeContentBuf(const uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE( bCode ));
	
	return	(bBuf[ bCode - EAN_UPC_CODE_MIN]);
}

/*******************************************************************************
* Function Name  : 
* Description    : 向指定的缓冲区写入一个位, 重复写入的次数由输入参数变量确定
* Input          : 
* Output         : 
*******************************************************************************/
void	SetOneBitToPrtBuf(uint8_t	bBuf[], uint8_t	bCode, uint32_t	nWidth)
{
	uint8_t	* pbBuf = NULL;
	
	pbBuf = bBuf;
	while (nWidth)
	{
		*pbBuf = bCode;
		pbBuf++;
		nWidth--;
	}
	pbBuf = NULL;
}

/*******************************************************************************
* Function Name  : 
* Description    : 	向指定的缓冲区写入一个字节, 写入的内容由输入参数变量确定
* Input          : 	bCode指定写入数据, 
										bBitNum指定要写入的位数,最大8,最小1,
*										位数以LSB开始计数, 但是写入时按照从MSB->LSB方向写入.
*										如果位数不足8, 则那些高位的内容不写入.
*										nWidth表示数据的每个位填充的次数.
* Output         :	返回写入内容占用的总点数宽度值.
*******************************************************************************/
uint32_t	SetOneByteToPrtBuf(uint8_t bBuf[], uint8_t bCode, uint8_t bBitNum, uint32_t nWidth)
{
	uint8_t	i;
	uint8_t	bFillData;
	uint32_t	nCount;				/* 写入内容占用的点数宽度值 */
	uint8_t	* pbBuf = NULL;
	
	if (bBitNum == 0)
		return	(0);
	
	nCount = 0;
	pbBuf = bBuf;
	for (i = 0; i < bBitNum; i++)
	{
		if ((bCode & (1 << (bBitNum - 1 - i))))
		{
			bFillData = 0xFF;
		}
		else
		{
			bFillData = 0x0;
		}
		SetOneBitToPrtBuf(pbBuf, bFillData, nWidth);
		pbBuf += nWidth;
		nCount += nWidth;
	}
	pbBuf = NULL;
	
	return	(nCount);
}

/*******************************************************************************
* Function Name  : 
* Description    : 根据指定数据长度将ean13的数据编码成可打印数据
* Input          : bBuf为完整的EAN13数据序列, 包括前置符, 但是不一定包括校验符
*										bCodeLen为输入数据序列的长度,12或者13.
*										如果长度为12, 自动添加校验位; 否则直接使用校验位而不检查
* Output         : 如果有错误返回0, 否则返回实际编辑结果点数宽度
*******************************************************************************/
uint16_t	FormatCodeEAN13(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint16_t Width;
	
	if (bCodeLen == 12)		/* 需要自动添加校验字符 */
	{
		Temp = CalEAN13CheckCode(bBuf);
		SetEAN13CheckCode(bBuf, Temp);
		Width = EAN13ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else if (bCodeLen == 13)	/* 直接使用接收到的校验字符而不再计算检查 */
	{
		Width = EAN13ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}

/*******************************************************************************
* Function Name  : 
* Description    : 根据指定数据长度将UPCA的数据编码成可打印数据. UPCA相当于
*										前置符0的EAN13.所以只需要添加前置符0,然后按照EAN13处理即可.
* Input          : bBuf为完整的UPCA数据序列, 但是不包括前置符, 不一定包括校验符
*										bCodeLen为输入数据序列的长度,11或者12.
*										如果长度为11, 自动添加校验位; 否则直接使用校验位而不检查
* Output         : 如果有错误返回0, 否则返回实际编辑结果点数宽度
*******************************************************************************/
uint16_t	FormatCodeUPCA(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint8_t	bEAN13[EAN13_DATA_MAX_LEN + 1];	/* 需要加入结束符, 防止下标越界 */
	uint16_t Width;
	uint8_t	* pbBuf = NULL;
	uint8_t	* pbEAN = NULL;
	
	if (bCodeLen > UPCA_DATA_MAX_LEN)	/* 指定内容超长,参数错误 */
		return	0;
	
	pbBuf = bBuf;
	pbEAN = bEAN13;
	*pbEAN++ = EAN13_PRE_CODE_MIN;		/* 添加前置符0 */
	for (Temp = 0; Temp < bCodeLen; Temp++)		/* 复制其余内容 */
		*pbEAN++ = *pbBuf++;
	
	if (bCodeLen == 11)		/* 需要自动添加校验字符 */
	{
		Temp = CalEAN13CheckCode(bEAN13);
		SetEAN13CheckCode(bEAN13, Temp);
		Width = EAN13ChangeToPrintData(bEAN13, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
		/* 添加上打印字符的校验码,否则打印字符无校验码 */
		pbBuf = bBuf;
		pbBuf += (UPCA_DATA_MAX_LEN - 1);
		*pbBuf++ = Temp;
		*pbBuf = '\0';		/* 需要加入结束符 */
	}
	else if (bCodeLen == 12)	/* 直接使用接收到的校验字符而不再计算检查 */
	{
		Width = EAN13ChangeToPrintData(bEAN13, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}

/*******************************************************************************
* Function Name  : 
* Description    : 根据指定数据长度将ean8的数据编码成可打印数据
* Input          : bBuf为完整的EAN8数据序列, 无前置符, 但是不一定包括校验符
*										bCodeLen为输入数据序列的长度,7或者8.
*										如果长度为7, 自动添加校验位; 否则直接使用校验位而不检查
* Output         : 如果有错误返回0, 否则返回实际编辑结果点数宽度
*******************************************************************************/
uint16_t	FormatCodeEAN8(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint16_t Width;
	
	if (bCodeLen == 7)		/* 需要自动添加校验字符 */
	{
		Temp = CalEAN8CheckCode(bBuf);
		SetEAN8CheckCode(bBuf, Temp);
		Width = EAN8ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else if (bCodeLen == 8)	/* 直接使用接收到的校验字符而不再计算检查 */
	{
		Width = EAN8ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}

/*******************************************************************************
* Function Name  : 
* Description    : 将UPCE的编码序列复原为UPCA编码序列以用来求校验码
* Input          : UPC的编码序列, 无校验码
* Output         : 复原为UPCA的编码序列, 注意UPE编码序列的长度要足够放下UPCA编码
*										返回值不包括校验码, 校验码需要接下来计算得到
	*	UPC-E条码由8位数字组成,是将系统字符为0的UPC-12编码压缩而成.
	*	UPC-E首字符恒为0,中间为6位数字,最后为1位校验码,
	*	UPC-E包括 起始符 + 左侧数据符3个 + 右侧数据3个 + 结束符,无中间间隔符和校验符
	
		UPC-A消零压缩方法如下:
											UPC-A编码																					UPC-E编码
						厂商识别代码						商品项目代码		校验码			商品项目识别代码			校验码
		X12(系统字符)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 0  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 1  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 2  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					
					0				X11 X10 3  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 ...0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 9  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					
					0				X11 X10 X9 1  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 ...0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 9  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					
					0				无零结尾X7 != 0   0  0  0  0	5       X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				无零结尾X7 != 0   0  0  0  0	...     X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				无零结尾X7 != 0   0  0  0  0	9       X1			0 X11 X10 X9 X8 X7 X2  	X1
		
	*	UPC-E编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
	*		d8	~		d7	~	d2	 				d1
	*	系统字符0		商品项目代码		校验码
	*	校验码为消零前的UPC-A编码计算得到的校验码.
	*	UPC-E无中间分隔符.
	* UPC-E终止符不同于EAN8, 为010101.
	*	UPC-E数据编码根据校验位值确定选用字符集A或者B.与EAN8编码不同.
	*	UPC-E中系统字符X8和校验码X1不用条码字符表示.
	UPC-E恢复为UPC-A的算法:
	UPCE码排序
	d8 d7 d6 d5 d4 d3 d2 d1
*******************************************************************************/
void	UPCERestoreUPCA(uint8_t	bBuf[])
{
	uint8_t	bTemp;
	uint8_t	bUPCA[EAN13_DATA_MAX_LEN];
	uint32_t	nCount;
	uint8_t	* pbBuf = NULL;
	
	pbBuf = bBuf;
	pbBuf += (UPCE_DATA_MAX_LEN - 2);
	bTemp = *pbBuf;		/* 得到d2 */
	
	/* 先按照UPCA编码顺序将其放入 bUPCA[] */
	if ((bTemp == '0') || (bTemp == '1') || (bTemp == '2'))
	{
	/*
									UPC-A编码																					UPC-E编码
				厂商识别代码						商品项目代码		校验码		商品项目识别代码		校验码
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2	d1
	X12(系统字符)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
				0				X11 X10 0  0  0   0  0  X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
				0				X11 X10 1  0  0   0  0  X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
				0				X11 X10 2  0  0   0  0  X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
	*/
		for (nCount = 0; nCount < 3; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		bUPCA[3] = bTemp;
		for (nCount = 4; nCount < 8; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 3;
		for (nCount = 8; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	else if (bTemp == '3')
	{
	/*
									UPC-A编码																					UPC-E编码
				厂商识别代码						商品项目代码		校验码		商品项目识别代码			校验码
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2		d1
	X12(系统字符)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9		X1
				0				X11 X10 3  0  0   0  0  0  X3 X2	X1			0 X11 X10 X9 X3 X2 3  	X1
				0				X11 X10 ...0  0   0  0  0  X3 X2	X1			0 X11 X10 X9 X3 X2 3  	X1
				0				X11 X10 9  0  0   0  0  0  X3 X2	X1			0 X11 X10 X9 X3 X2 3  	X1
	*/
		for (nCount = 0; nCount < 4; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		for (nCount = 4; nCount < 9; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 4;
		for (nCount = 9; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	else if (bTemp == '4')
	{
		/*
									UPC-A编码																					UPC-E编码
				厂商识别代码						商品项目代码		校验码		商品项目识别代码			校验码
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2		d1
	X12(系统字符)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9		X1
				0				X11 X10 X9 1  0   0  0  0  0  X2	X1			0 X11 X10 X9 X8 X2 4  	X1
				0				X11 X10 X9 ...0   0  0  0  0  X2	X1			0 X11 X10 X9 X8 X2 4  	X1
				0				X11 X10 X9 9  0   0  0  0  0  X2	X1			0 X11 X10 X9 X8 X2 4  	X1
	*/
		for (nCount = 0; nCount < 5; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		for (nCount = 5; nCount < 10; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 5;
		for (nCount = 10; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	else
	{
		/*
									UPC-A编码																					UPC-E编码
				厂商识别代码						商品项目代码		校验码		商品项目识别代码			校验码
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2		d1
	X12(系统字符)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9		X1
				0				无零结尾X7 != 0   0  0  0  0	5   X1			0 X11 X10 X9 X8 X7 X2  	X1
				0				无零结尾X7 != 0   0  0  0  0	... X1			0 X11 X10 X9 X8 X7 X2  	X1
				0				无零结尾X7 != 0   0  0  0  0	9   X1			0 X11 X10 X9 X8 X7 X2  	X1
	*/
		for (nCount = 0; nCount < 6; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		for (nCount = 6; nCount < 10; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 6;
		for (nCount = 10; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	pbBuf = NULL;
	
	/* 再将其返回,返回的是UPCA,不包括校验码 */
	for (nCount = 0; nCount < UPCA_DATA_MAX_LEN; nCount++)
	{
		bBuf[nCount] = bUPCA[nCount];
	}
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将校验码放到指定字符序列UPCE的校验码位置
* Input          : 输入参数:已经标准化了的UPCE字符序列,共7或者8字节长度
* UPCE编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	d7	~	d5	 	d4	~	d2		d1
*	0		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d0	d1	~	d3	 	d4	~	d6		d7
*	0		左侧数据		右侧数据		校验符
* Output         : 输出参数:
*******************************************************************************/
void	SetUPCECheckCode(uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE(bCode));
	bBuf[ UPCE_DATA_MAX_LEN - 1 ] = bCode;
	bBuf[ UPCE_DATA_MAX_LEN ] = '\0';	 //加入字符串结束符
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 取得指定字符序列UPCE的校验码
* Input          : 输入参数:已经标准化了的UPCE字符序列,共7或者8字节长度
* UPCE编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	d7	~	d5	 	d4	~	d2		d1
*	0		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d0	d1	~	d3	 	d4	~	d6		d7
*	0		左侧数据		右侧数据		校验符
* Output         : 输出参数:校验码
*******************************************************************************/
uint8_t	GetUPCECheckCode(uint8_t	bBuf[])
{
	assert_param(IS_EAN_UPC_CODE(bBuf[ UPCE_DATA_MAX_LEN - 1 ]));
	
	return	(bBuf[ UPCE_DATA_MAX_LEN - 1 ]);
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 将标准UPCE字符序列转换为可打印编码序列
* Input          : 输入参数:已经标准化了的UPCE字符序列,共8字节长度
*	转换结果存储到输入参数bPrtBuf, 每个加热点对应bPrtBuf[]的一个字节
*	数据为FF表示打印为黑,数据为0表示打印为空白.
* UPCE编码序列各个位置的位次命名如下:包括校验码在内,从右到左的顺序号
*	d8	d7	~	d5	 	d4	~	d2		d1
*	0		左侧数据		右侧数据		校验符
*	实际输入参数的排列如下:
*	d0	d1	~	d3	 	d4	~	d6		d7
*	0		左侧数据		右侧数据		校验符
* 前置码, 校验码不包括在数据符内, 不用条码字符表示.
* 根据校验码确定数据内容的编码方式,数据编码集A或者B.
*	UPCE包括 起始符 + 数据6个 + 结束符
* Output         : 输出参数:总可打印条码编码的宽度, 以点为单位计数, 
*	如果打印缓冲区空间放不下或者出现错误, 返回0
*	需要根据条码定义宽度计算填充和点数宽度值
*******************************************************************************/
uint16_t	UPCEChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
{
	uint8_t	bCodeTypeSel;					/* 数据编码种类集选择*/
	/* 可打印编码值先取出, 后集中处理填充 */
	uint8_t	bContentBuf[EAN13_DATA_MAX_LEN];
	uint16_t	nCount;
	uint16_t	nWidth;							/* 打印宽度, 点数 */
	uint16_t	nPrtWidth;					/* 打印宽度, 点数 */
	uint8_t	* pbSrcBuf = NULL;
	uint8_t	* pbDstBuf = NULL;
	const uint8_t	* pbCodeContentBuf = NULL;
	
	/* 先判断当前可打印区域是否能够放下本次待填充的编码打印内容 */
	/* 必须处理, 否则可能内存溢出 */
	if (CheckPrtSpace(TYPE_UPCE, UPCE_DATA_MAX_LEN) == 0)
		return 0;			/* 打印缓冲区放不下, 不打印 */
	
	memset(bContentBuf, sizeof(bContentBuf) / sizeof(bContentBuf[0]), 0x00);
	bCodeTypeSel = UPCECharCodeTypeSel[ GetUPCECheckCode(bBuf) - EAN_UPC_CODE_MIN ];
	
	/* 将所有数据先取出对应的可打印内容编码值 */
	pbSrcBuf = bBuf;
	assert_param(IS_UPCE_PRE_CODE( *pbSrcBuf ));
	pbSrcBuf++;		/* 不处理前置符 */
	for (nCount = 0; nCount < UPCE_PRT_DATA_LEN; nCount++)
	{
		if ((bCodeTypeSel & (1 << (UPCE_PRT_DATA_LEN - 1 - nCount))))
		{
			pbCodeContentBuf = EANContentCodeType_B;	/* 从编码字符集B中选择 */
		}
		else
		{
			pbCodeContentBuf = EANContentCodeType_A;	/* 从编码字符集A中选择 */
		}
		/* 将当前字符的可打印内容编码值取回 */
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	
	/* 下面开始填充可打印数据内容 */
	nPrtWidth = 0;								/* 统计总填充打印点数 */
	pbDstBuf = bPrtBuf;
	/* 先填充左起始符 b101 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																UPCStartCode, \
																UPC_START_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* 再填充数据 */
	/* 每个数据的条码编码占用7位,根据校验符的值从A,B两个编码集中选择 */
	/* 编码最高位0无效,其余7位有效. 按照从MSB->LSB的顺序填充*/
	for (nCount = 0; nCount < UPCE_PRT_DATA_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	UPC_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* 最后填充终止符 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																UPCEStopCode, \
																UPCE_STOP_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	
	pbSrcBuf = NULL;
	pbDstBuf = NULL;
	pbCodeContentBuf = NULL;
	
	return	(nPrtWidth);
}

/*******************************************************************************
* Function Name  : 
* Description    : 根据指定数据长度将UPCE的数据编码成可打印数据. UPCE相当于压缩
*										前置符0的UPCA.所以需要根据不同编码序列恢复为原压缩前的UPC-A,
*										并可能需要计算校验码,然后再处理.
* Input          :  bBuf为UPCE数据序列, 但是不一定包括前置符, 不一定包括校验符
*										bCodeLen为输入数据序列的长度,6,7,8,11或者12.
*										如果长度为6, 先自动添加前置符0,
*										如果长度为7,8,11或者12, 前置符必须为设置为0,
*										如果长度为6,7,11, 自动添加校验位,
*										如果长度为8或者12, 直接使用校验位而不检查
* Output         : 如果有错误返回0, 否则返回实际编辑结果点数宽度
*******************************************************************************/
uint16_t	FormatCodeUPCE(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint8_t	bEAN13[EAN13_DATA_MAX_LEN + 1];	/* 需要加入结束符, 防止下标越界 */
	uint8_t	bUPCA[EAN13_DATA_MAX_LEN + 1];	/* 需要加入结束符, 防止下标越界 */
	uint16_t Width;
	uint8_t	* pbBuf = NULL;
	uint8_t	* pbEAN = NULL;
	
	/* 先检查参数及数据是否合法 */
	if (bCodeLen > UPCA_DATA_MAX_LEN)	/* 指定内容超长,参数错误 */
		return	0;
	
	if ((bCodeLen == 7) || (bCodeLen == 8) || (bCodeLen == 11) || (bCodeLen == 12))
	{	/* 如果长度为7,8,11或者12, 前置符必须为设置为0 */
		if (bBuf[0] != UPCE_DEFAULT_PRE_CODE)
			return	0;
	}
	if (bCodeLen == 6)
	{
		/* 如果长度为6, 先自动添加前置符0 */
		pbBuf = bBuf;
		pbEAN = bUPCA;
		*pbEAN++ = UPCE_DEFAULT_PRE_CODE;		/* 添加前置符0 */
		for (Temp = 0; Temp < bCodeLen; Temp++)		/* 复制其余内容 */
		{
			*pbEAN++ = *pbBuf++;
		}
		bCodeLen += 1;	/* 数据长度加1 */
		
		/* 修改对应输入数据内容 */
		pbBuf = bBuf;
		pbEAN = bUPCA;
		for (Temp = 0; Temp < bCodeLen; Temp++)
		{
			*pbBuf++ = *pbEAN++;
		}
	}
	
	if ((bCodeLen == 11) || (bCodeLen == 12))	/* 作为UPCA打印, 自动处理校验码 */
	{
		Width = FormatCodeUPCA(bBuf, bCodeLen);
	}
	else if (bCodeLen == 7)	/* 作为UPCE打印, 先添加校验字符 */
	{
		pbBuf = bBuf;
		pbEAN = bUPCA;
		for (Temp = 0; Temp < bCodeLen; Temp++)		/* 复制其余内容 */
		{
			*pbEAN++ = *pbBuf++;
		}
		UPCERestoreUPCA(bUPCA);	/* 先恢复为原压缩前UPCA编码序列 */
		
		pbBuf = bUPCA;
		pbEAN = bEAN13;
		*pbEAN++ = EAN13_PRE_CODE_MIN;		/* 添加前置符0 */
		for (Temp = 0; Temp < UPCA_DATA_MAX_LEN; Temp++)	/* 复制其余内容 */
			*pbEAN++ = *pbBuf++;
		Temp = CalEAN13CheckCode(bEAN13);	/* 计算校验字符 */
		SetUPCECheckCode(bBuf, Temp);			/* 设置UPC-E校验字符,包括字符串结束符 */
		/* 根据校验字符选择左右数据编码集并编码 */
		Width = UPCEChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else if (bCodeLen == 8)	/* 作为UPCE打印, 直接使用接收到的校验字符而不再计算检查 */
	{
		/* 根据校验字符选择左右数据编码集并编码 */
		Width = UPCEChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
